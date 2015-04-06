#ifndef DEBUGGER_BREAKPOINTS_H
#define DEBUGGER_BREAKPOINTS_H

#include "Debugger.Global.h"


/*
Incomplete Job. I'll Continue Later. 
*/
namespace GleeBug{


	typedef std::tuple<uint32_t, uint32_t> breakpoint_id; //I told you
	
	

	struct breakpoint {

		
		uint32_t bp_type;
		LPVOID bp_address;
		uint8_t original_opcode;
		bool enabled;
	
		breakpoint()
		{
			bp_type = NULL;
			bp_address = NULL;
			original_opcode = NULL;
			enabled = false;
		}
		breakpoint(HANDLE hProcess, LPVOID _address, uint32_t _bp_type)
		{
			SIZE_T nbytes_written = 0;
			bp_address = _address;
			bp_type = _bp_type;
			enabled = true;
			

			switch (bp_type)
			{
			case SOFT_BP:
				bp_type = 0xcc;
				break;
			default:
				enabled = false;
				break;
			}

			if(ReadProcessMemory(hProcess, bp_address, &original_opcode, 1, &nbytes_written) == 0)
			{
				printf("Cant Read Process at %X\n an the process handle is %X", bp_address, hProcess);
				enabled = false;
			}

			if (nbytes_written != 1){
				printf("could not reaad the number of bytes!\n");
				enabled = false;
			}


			if (WriteProcessMemory(hProcess, bp_address, &bp_type, 1, &nbytes_written) == 0)
			{
				printf("could not Write the process\n");
				enabled= false;
			}

			if (nbytes_written != 1){
				printf("didnt write the bytes\n");
				enabled= false;
			}
			

			
		}

		bool DeleteBP(HANDLE hProcess)
		{

			
			SIZE_T nbytes_written = 0;
			
			if (WriteProcessMemory(hProcess, bp_address, &original_opcode, 1, &nbytes_written)==0)
			{
				return false;
			}
			if (nbytes_written != 1){
				return false;
			}
			return true;
		}
		

		void disable(){
			enabled = false;
		}
		void enable(){
			enabled = true;
		}

		const uint32_t GetType(){
			return bp_type;
		}

		LPVOID GetAddress(){
			return bp_address;
		}

	};

	struct Hasher{
		Hasher()
		{

		}

		uint32_t operator()(breakpoint_id bpid){
			std::hash<uint32_t> hasher;
			return hasher(std::get<0>(bpid)) ^ hasher(std::get<1>(bpid));
		}
	};

	typedef std::unordered_map<breakpoint_id, breakpoint, Hasher> bpmap_addrbp_bp;
	
}
#endif