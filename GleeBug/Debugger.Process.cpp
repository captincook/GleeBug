#include "Debugger.Process.h"

namespace GleeBug
{
	bool ProcessInfo::AddBreakpoint(LPVOID address, uint32_t type)
	{
		breakpoint_id bp_id = breakpoint_id((uint32_t)address, type);
		breakpoint newbp = breakpoint(this->hProcess, address, type);
		bpManager[bp_id] = newbp;
		return newbp.enabled;
	}
	bool ProcessInfo::DelBreakpoint(LPVOID address, uint32_t type)
	{
		breakpoint_id bp_id = breakpoint_id((uint32_t)address, type);
		breakpoint target_bp = breakpoint();
		try{
			target_bp = bpManager.at(bp_id);
		}
		catch (std::out_of_range& oor)
		{
			return false;
		}
		if (target_bp.DeleteBP(this->hProcess) == true)
		{
			bpManager.erase(bp_id);
			return true;
		}
	}
	ProcessInfo::ProcessInfo()
	{
		this->bpManager = bpmap_addrbp_bp{};
		this->curThread = nullptr;
		this->systemBreakpoint = false;
		this->hProcess = INVALID_HANDLE_VALUE;
	}

	ProcessInfo::ProcessInfo(DWORD dwProcessId, DWORD dwMainThreadId)
	{
		this->bpManager = bpmap_addrbp_bp{};
		this->systemBreakpoint = false;
		this->hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
		this->dwProcessId = dwProcessId;
		this->dwMainThreadId = dwMainThreadId;
		this->threads.clear();
	}

	ProcessInfo::~ProcessInfo()
	{
		if (this->hProcess != INVALID_HANDLE_VALUE)
			CloseHandle(hProcess);
	}
};