#pragma once

#include <cstdint>
#include <bitset>
#include <intrin.h>
#include <array>
#include <vector>
#include <iostream>
#include <Windows.h>

class C_CPUID
{
	enum WHATREGISTER
	{
		EAX,
		EBX,
		ECX,
		EDX
	};
public:
	/* ctor */
	C_CPUID() 
	{
		this->init_cpu();
		this->init_vendor();
	}
	
	/* dtor */
	~C_CPUID() {  }

	const std::bitset<32>& get_register(enum WHATREGISTER&& r)
	{
		switch (r)
		{
		case EAX:
			return this->eax;
			break;
		case EDX:
			return this->edx;
			break;
		case EBX:
			return this->ebx;
			break;
		case ECX:
			return this->ecx;
			break;
		default:
			throw std::runtime_error("CPUID: FATAL ERROR COULDN'T GET REGISTER PROPERLY");
			break;
		}
	}

	const std::string& get_vendor()
	{
		return this->vendor_name;
	}

	const bool is_cpu_loaded()
	{
		return this->cpuinfo.data() != nullptr ? true : false;
	}
private:
	/* this function will grab each register
	 * eax, ecx, ebx, edx as it is used with 
	 * CPUID
	 */
	const void init_cpu()
	{
		/* calling with 0x0 for
		 * the 2nd arg will insert
		 * the manufacturer ID / vendor id
		 * inside of ebx edx ecx in that order

		 * the highest order entry is returned in eax
		 * into cpuinfo[0], we can then
		 * loop over that to grab the whole 
		 * amount of information that is there 
		 */ 
		__cpuid(cpuinfo.data(), 0);
		std::int32_t num_ids = cpuinfo[0];

		for (std::int32_t it = 0; it < num_ids; it++)
		{
			/* now we can load all the order 
			 * functions into our cpuinfo
			 * array at it
			 */
			__cpuid(cpuinfo.data(), it);
			this->reg_w_order_idx.push_back(cpuinfo);
		}

		this->init_registers();
	}

	const void init_registers()
	{
		if (this->reg_w_order_idx.data() != nullptr)
		{
			/* set registers initial value
			 * to what is stored at [][...]
			 * will change over time
			 */
			this->ebx = this->get_register(0, EBX);
			this->edx = this->get_register(0, EDX); // we jump ahead + 1 because of the way the vendor ascii string is setup in CPUID
			this->ecx = this->get_register(0, ECX);
		}
		else
		{
			throw std::runtime_error("CPUID: REGISTERS WERE NOT INITIALLY LOADED PROPERLY");
		}
	}
	
	/* this function will 
	 * get us a string to the vendor
	 * of the processor
	 */
	const void init_vendor()
	{
		char vendorarr[0x20];
		std::memset(vendorarr, 0, sizeof(vendorarr));

		*reinterpret_cast<std::uint32_t*>(vendorarr) = this->ebx.to_ulong();
		*reinterpret_cast<std::uint32_t*>(vendorarr + 4) = this->edx.to_ulong();
		*reinterpret_cast<std::uint32_t*>(vendorarr + 8) = this->ecx.to_ulong();

		this->vendor_name = std::string(vendorarr);
	}
	
	/* this function returns a pointer
	 * to the register that	 is inputted
	 */
	const std::bitset<32> get_register(std::int32_t order_entry, const WHATREGISTER&& r)
	{
		return std::bitset<32>(this->reg_w_order_idx[order_entry][r]);
	}

	/* this function sets a register
	 * based on the order_entry to the value
	 * given in val
	 */
	const void set_register(const WHATREGISTER&& r, std::uint32_t val)
	{
		switch (r)
		{
		case EAX:
			this->eax = val;
			break;
		case EDX:
			this->edx = val;
			break;
		case EBX:
			this->ebx = val;
			break;
		case ECX:
			this->ecx = val;
			break;
		}

		return;
	}

	std::string vendor_name;
	std::array<std::int32_t, 4> cpuinfo;
	std::vector<std::array<std::int32_t, 4>> reg_w_order_idx;
	std::bitset<32> eax;
	std::bitset<32> ebx; // 1 off
	std::bitset<32> ecx; // 2 off
	std::bitset<32> edx; // 3 off
protected:
};
