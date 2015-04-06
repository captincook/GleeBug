#include "Debugger.h"
#include "Debugger.Breakpoints.h"
#include "Debugger.Global.h"

namespace GleeBug
{
	void Debugger::exceptionBreakpoint(const EXCEPTION_RECORD & exceptionRecord, const bool firstChance)
	{
		unsigned char bp_type = 0;
		breakpoint trigger_bp = breakpoint();


		if (!_process->systemBreakpoint) //handle system breakpoint
		{
			_process->systemBreakpoint = true;
			_continueStatus = DBG_CONTINUE;

			//call the callback
			cbSystemBreakpoint();
		}
		else //handle other breakpoint exceptions
		{
			
			
			
			ReadProcessMemory(_process->hProcess, exceptionRecord.ExceptionAddress, &bp_type, 1, NULL);
			
			printf("The breakpoint ocurred at location %p and the bp type is %X\n", exceptionRecord.ExceptionAddress, bp_type);
			breakpoint_id bp_id = breakpoint_id((uint32_t)exceptionRecord.ExceptionAddress, bp_type);
			try{
				trigger_bp = _process->bpManager.at(bp_id);
			}
			catch (std::out_of_range & oor){
				return;
			}
			
		
			cbOnUserBreakPointTrigger(trigger_bp);
		}

	}

	void Debugger::exceptionSingleStep(const EXCEPTION_RECORD & exceptionRecord, const bool firstChance)
	{
		if (_thread->isSingleStepping) //handle single step
		{
			_thread->isSingleStepping = false;
			_continueStatus = DBG_CONTINUE;

			//call the callbacks
			StepCallbackVector cbStepCopy = _thread->stepCallbacks;
			_thread->stepCallbacks.clear();
			for (auto cbStep : cbStepCopy)
				cbStep();
			cbStep();			
		}
		else //handle other single step exceptions
		{
		}
	}

	void Debugger::exceptionEvent(const EXCEPTION_DEBUG_INFO & exceptionInfo)
	{
		//let the debuggee handle exceptions per default
		_continueStatus = DBG_EXCEPTION_NOT_HANDLED;

		const EXCEPTION_RECORD & exceptionRecord = exceptionInfo.ExceptionRecord;
		const bool firstChance = exceptionInfo.dwFirstChance == 1;

		//dispatch the exception
		switch (exceptionInfo.ExceptionRecord.ExceptionCode)
		{
		case STATUS_BREAKPOINT:
			exceptionBreakpoint(exceptionRecord, firstChance);
			break;
		case STATUS_SINGLE_STEP:
			exceptionSingleStep(exceptionRecord, firstChance);
			break;
		}

		//call the unhandled exception callback
		if (_continueStatus == DBG_EXCEPTION_NOT_HANDLED)
			cbUnhandledException(exceptionRecord, firstChance);

		//call the debug event callback
		cbExceptionEvent(exceptionInfo);
	}
};