/*
Armator - simulateur de jeu d'instruction ARMv5T � but p�dagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique G�n�rale GNU publi�e par la Free Software
Foundation (version 2 ou bien toute autre version ult�rieure choisie par vous).

Ce programme est distribu� car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but sp�cifique. Reportez-vous � la
Licence Publique G�n�rale GNU pour plus de d�tails.

Vous devez avoir re�u une copie de la Licence Publique G�n�rale GNU en m�me
temps que ce programme ; si ce n'est pas le cas, �crivez � la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
�tats-Unis.

Contact: Guillaume.Huard@imag.fr
	 B�timent IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'H�res
*/
#include "arm_load_store.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "util.h"
#include "debug.h"
#include <assert.h>

typedef enum name_of_function
{
	LDR,
	LDRB,
	LDRH,
	STR,
	STRB,
	STRH,
	LDM,
	STM
} name_of_function;

uint32_t get_index(arm_core p, uint32_t ins)
{	uint8_t bit_3_0 = get_bits(ins, 3, 0);
	uint32_t rm = arm_read_register(p, bit_3_0);
	uint8_t shift_imm = get_bits(ins, 11, 7);
	uint8_t shift_ind = get_bits(ins, 6, 5);
	uint32_t index;
	switch (shift_ind){
	case 0: /* LSL */
		index = rm << shift_imm;
		break;
	case 1: /* LSR */
		if (shift_imm == 0)
			index = 0;
		else
			index = rm >> shift_imm;
		break;
	case 2: /* ASR */
	{
		if (shift_imm == 0){
			if (get_bit(rm, 31) == 1)
				index = 0xFFFFFFFF;
			else
				index = 0;
		}else{
			index = asr(rm, shift_imm);
		}
		break;
	}
	case 3: /* ROR or RRX */
	{
		if (shift_imm == 0)
		{
			uint8_t cpsr = arm_read_cpsr(p);
			uint32_t c = get_bit(cpsr, 29);
			index = (c << 31) | (rm >> 1);
		}else{
			index = ror(rm, shift_imm);
		}
		break;
	}
	default:
		break;
	}
	return index;
}

void add_substract(int u, uint32_t *adr, uint32_t op_gauche, uint32_t op_droit)
{
	if(u) {
		*adr = op_gauche + op_droit;
	}else{
		*adr = op_gauche - op_droit;
	}
}

void condtion_pass_modify(arm_core p, uint32_t ins, uint32_t op_droit)
{
	uint32_t Rn_write;
	int u = get_bit(ins, 23);
	uint32_t rn = arm_read_register(p, (uint8_t)get_bits(ins, 19, 16));
	if (u) {
		Rn_write = rn + op_droit;
	}else{
		Rn_write = rn - op_droit;
	}
	arm_write_register(p, (uint8_t)get_bits(ins, 19, 16), Rn_write);

}

uint32_t get_address(arm_core p, uint32_t ins)
{
	int u = get_bit(ins, 23);
	uint32_t rm = arm_read_register(p, (uint8_t)get_bits(ins, 3, 0));
	uint32_t rn = arm_read_register(p, (uint8_t)get_bits(ins, 19, 16));
	uint32_t offset_12 = get_bits(ins, 11, 0);
	uint8_t bits_2724 = get_bits(ins, 27, 24);
	uint8_t bits_21 = get_bit(ins, 21);
	uint32_t address = 0;
	uint32_t index = get_index(p, ins);
	if ((bits_2724 == 0x5) && (bits_21 == 0))
	{
		// 1.Load and Store Word or Unsigned Byte - Immediate offset
		add_substract(u, &address, rn, offset_12);
		return address;
	}
	else if ((bits_2724 == 0x07) && (bits_21 == 0) && (get_bits(ins, 11, 4) == 0))
	{
		// 2.Load and Store Word or Unsigned Byte - Register offset
		add_substract(u, &address, rn, rm);
		return address;
	}
	else if ((bits_2724 == 0x7) && (bits_21 == 0) && (get_bit(ins, 4) == 0))
	{
		// 3.Load and Store Word or Unsigned Byte - Scaled register offset
		add_substract(u, &address, rn, index);
		return address;
	}
	else if ((bits_2724 == 0x05) && (bits_21 == 1))
	{
		// 4.Load and Store Word or Unsigned Byte - Immediate pre-indexed
		add_substract(u, &address, rn, offset_12);
		arm_write_register(p, (uint8_t)get_bits(ins, 19, 16), address);
		return address;
	}
	else if ((bits_2724 == 0x07) && (bits_21 == 1) && (get_bits(ins, 11, 4) == 0))
	{
		// 5.Load and Store Word or Unsigned Byte - Register pre-indexed
		add_substract(u, &address, rn, rm);
		arm_write_register(p, (uint8_t)get_bits(ins, 19, 16), address);
		return address;
	}
	else if ((bits_2724 == 0x07) && (bits_21 == 1) && !get_bit(ins, 4))
	{
		// 6.Load and Store Word or Unsigned Byte - Scaled register pre-indexed
		add_substract(u, &address, rn, index);
		arm_write_register(p, (uint8_t)get_bits(ins, 19, 16), address);
		return address;
	}
	else if ((bits_2724 == 0x04) && (bits_21 == 0))
	{
		// 7.Load and Store Word or Unsigned Byte - Immediate post-indexed
		address = rn;
		condtion_pass_modify(p, ins, offset_12);
		return address;
	}
	else if ((bits_2724 == 0x06) && (bits_21 == 0) && (get_bits(ins, 11, 4) == 0))
	{
		// 8.Load and Store Word or Unsigned Byte - Register post-indexed
		address = rn;
		condtion_pass_modify(p, ins, rm);
		return address;
	}
	else if ((bits_2724 == 0x06) && (bits_21 == 0) && !get_bit(ins, 4))
	{
		// 9.Load and Store Word or Unsigned Byte - Scaled register post-indexed
		address = rn;
		condtion_pass_modify(p, ins, index);
		return address;
	}
	else
		return -1;
}

uint32_t get_offset(uint32_t ins)
{
	uint32_t immedH = get_bits(ins, 11, 8);
	uint32_t immedL = get_bits(ins, 3, 0);
	uint32_t offset_8 = (immedH << 4) | immedL;
	return offset_8;
}

uint32_t get_address_h(arm_core p, uint32_t ins)
{
	int u = get_bit(ins, 23);
	uint32_t rn = arm_read_register(p, (uint8_t)get_bits(ins, 19, 16));
	uint32_t rm = arm_read_register(p, (uint8_t)get_bits(ins, 3, 0));
	uint8_t bits_2724 = get_bits(ins, 27, 24);
	uint8_t bits_2221 = get_bits(ins, 22, 21);
	uint32_t address = 0;
	if ((bits_2724 == 1) && (bits_2221 == 2))
	{
		// 1.Miscellaneous Loads and Stores - Immediate offset
		uint32_t offset_8 = get_offset(ins);
		add_substract(u, &address, rn, offset_8);
		return address;
	}
	else if ((bits_2724 == 1) && (bits_2221 == 0))
	{ // 2.Miscellaneous Loads and Stores - Register offset
		add_substract(u, &address, rn, rm);
		return address;
	}
	else if ((bits_2724 == 1) && (bits_2221 == 3))
	{ // 3.Miscellaneous Loads and Stores - Immediate pre-indexed
		uint32_t offset_8 = get_offset(ins);
		add_substract(u, &address, rn, offset_8);
		arm_write_register(p, (uint8_t)get_bits(ins, 19, 16), address);
		return address;
	}
	else if ((bits_2724 == 1) && (bits_2221 == 1))
	{ // 4.Miscellaneous Loads and Stores - Immediate pre-indexed
		add_substract(u, &address, rn, rm);
		arm_write_register(p, (uint8_t)get_bits(ins, 19, 16), address);
		return address;
	}
	else if ((bits_2724 == 0) && (bits_2221 == 2))
	{ // 5.Miscellaneous Loads and Stores - Immediate post-indexed
		address = rn;
		uint32_t offset_8 = get_offset(ins);
		condtion_pass_modify(p, ins, offset_8);
		return address;
	}
	else if ((bits_2724 == 0) && (bits_2221 == 0))
	{ // 6.Miscellaneous Loads and Stores - Register post-indexed
		address = rn;
		condtion_pass_modify(p, ins, rm);
		return address;
	}
	else
		return -1;
}

void set_t_bit(arm_core p,uint8_t x){
	uint32_t cprs = arm_read_cpsr(p);
	if (x==0x01)
		cprs = set_bit(cprs, 5);
	else
		cprs = clr_bit(cprs, 5);
	arm_write_cpsr(p, cprs);
}

uint32_t get_address_from_name(arm_core p,uint32_t ins, enum name_of_function name){
	if(name == LDRH || name == STRH)
		return get_address_h(p, ins);
	else
		return get_address(p, ins);
}

enum name_of_function get_func(uint32_t ins)
{
	if (get_bits(ins, 27, 26) == 0x1)
	{
		if (!get_bit(ins, 22))
		{
			if (get_bit(ins, 20))
				return LDR;
			else
				return STR;
		}else{ 
			if (get_bit(ins, 20))
				return LDRB;
			else
				return STRB;
		}
	}
	else if ((get_bits(ins, 27, 25) == 0x0) && (get_bits(ins, 7, 4) == 0xB))
	{
		if (get_bit(ins, 20))
			return LDRH;
		else
			return STRH;
	}
	else if ((get_bits(ins, 27, 25) == 0x4) && (!get_bit(ins, 22)))
	{
		if (get_bit(ins, 20))
			return LDM;
		else
			return STM;
	}
	else
		return UNDEFINED_INSTRUCTION;
}

int condition_passed(arm_core p, uint32_t ins)
{
	uint8_t cond = get_bits(ins, 31, 28);
	uint8_t cpsr = arm_read_cpsr(p);
	uint8_t n = get_bit(cpsr, 31);
	uint8_t z = get_bit(cpsr, 30);
	uint8_t c = get_bit(cpsr, 29);
	uint8_t v = get_bit(cpsr, 28);
	switch (cond){
		case 0x00:
			return z == 0x01; //0000  EQ       Equal                               Z set
		case 0x01:
			return z == 0x00; //0001  NE       Not equal                           Z clear
		case 0x02:
			return c == 0x01; //0010  CS/HS    Carry set/unsigned higher or same   C set
		case 0x03:
			return c == 0x00; //0011  CC/LO    Carry clear/unsigned lower          C clear
		case 0x04:
			return n == 0x01; //0100  MI       Minus/negative                      N set
		case 0x05:
			return n == 0x00; //0101  PL       Plus/positive or zero               N clear
		case 0x06:
			return v == 0x01; //0110  VS       Overflow                            V set
		case 0x07:
			return v == 0x00; //0111  VC       No overflow                         V clear
		case 0x08:
			return z == 0x00 && c == 0x01; //1000  HI       Unsigned higher                     C set and Z clear
		case 0x09:
			return z == 0x01 || c == 0x00; //1001  LS       Unsigned lower or same              C clear or Z set
		case 0x0A:
			return n == v; //1010  GE       Signed greater than or equal        N set and V set, orN clear and V clear (N == V)
		case 0x0B:
			return n != v; //1011  LT       Signed less than                    N set and V clear, or N clear and V set (N != V)
		case 0x0C:
			return z == 0x00 && n == v; //1100  GT       Signed greater than                 Z clear, and either N set and V set, or N clear and V clear (Z == 0,N == V)
		case 0x0D:
			return z == 0x01 || n != v; //1101  LE       Signed less than or Equal 			 Z set, or N set and V clear, or N clear and V set (Z == 1 or N != V)
		case 0x0E:
			return 1; //1110  AL       Always (unconditional)
		case 0x0F:
			return 0; //1111  -        See Condition code 0b1111
		default:
			return 0;
	}
}

uint32_t number_of_set_bits(uint32_t ins){
	uint32_t number = 0;
	int i;
	for (i = 0; i < 16; i++)
	{
		if(((ins >> i) & 1 ) == 1)
			number++;
	}
	return number;
}

void condtion_pass_modify_w(arm_core p, uint32_t ins, uint32_t op_droit, int op)
{
	uint32_t Rn_write;
	uint32_t rn = arm_read_register(p, (uint8_t)get_bits(ins, 19, 16));
	if(op)
		Rn_write = rn + op_droit;
	else
		Rn_write = rn - op_droit;
	arm_write_register(p, (uint8_t)get_bits(ins, 19, 16), Rn_write);
}

void get_start_end_address(arm_core p, uint32_t ins, uint32_t *start_address, uint32_t *end_address){
	if (get_bits(ins, 27, 25) == 0x4)
	{
		int bits_24_23 = get_bits(ins, 24, 23);
		uint32_t rn = arm_read_register(p, (uint8_t)get_bits(ins, 19, 16));
		uint32_t register_list = number_of_set_bits(ins) * 4;
		switch (bits_24_23){
			case 0:
				/* Load and Store Multiple - Decrement after */
				*start_address = rn - register_list + 4;
				*end_address = rn;
				condtion_pass_modify_w(p, ins, register_list, 0);
				break;

			case 1:
				/* Load and Store Multiple - Increment after */
				*start_address = rn;
				*end_address = rn + register_list - 4;
				condtion_pass_modify_w(p, ins, register_list, 1);
				break;

			case 2:
				/* Load and Store Multiple - Decrement before */
				*start_address = rn - register_list;
				*end_address = rn - 4;
				condtion_pass_modify_w(p, ins, register_list, 0);
				break;

			case 3:
				/* Load and Store Multiple - Increment before */
				*start_address = rn + 4;
				*end_address = rn + register_list;
				condtion_pass_modify_w(p, ins, register_list, 1);
				break;
			default:
				break;
		}
	}
}

int arm_load_store(arm_core p, uint32_t ins)
{
	name_of_function name = get_func(ins);
	uint8_t Rd = (uint8_t)get_bits(ins, 15, 12);
	uint32_t data = 0;
	uint8_t byte = 0;
	uint16_t half = 0;
	uint32_t RdVal = arm_read_register(p, Rd);
	uint32_t address = get_address_from_name(p, ins, name);
	switch (name){
		case LDR:
		{
			if (CP15_reg1_Ubit == 0) {
				if (arm_read_word(p, address, &data) != 0)
					return -1;
				uint8_t bits_1_0 = get_bits(address, 1, 0);
				data = ror(data, 8 * bits_1_0);
			}else{
				if (arm_read_word(p, address, &data) != 0)
					return -1;
			}
			if (Rd == 0x0F){
				arm_write_register(p, (uint8_t)PC, (data & 0xFFFFFFFE));
				set_t_bit(p,(uint8_t)get_bit(data, 0));
			}else{
				arm_write_register(p, Rd, data);
			}
			return 0;
		}
		case STR:
		{
			if (arm_write_word(p, address, RdVal) != 0)
				return -1;
			return 0;
		}
		case LDRB:
		{
			if (arm_read_byte(p, address, &byte) != 0)
				return -1;
			arm_write_register(p, Rd, (uint32_t)byte);
			return 0;
		}
		case STRB:
		{
			if (arm_write_byte(p, address, (uint8_t)get_bits(RdVal, 7, 0)) != 0)
				return -1;
			return 0;

		}
		case LDRH:
		{
			if (CP15_reg1_Ubit == 0){
				if (get_bit(address, 0) == 0x00){
					if (arm_read_half(p, address, &half) != 0)
						return -1;
				}else{
					half = 0x00; //UNPREDICTABLE
				}
			}else{
				if (arm_read_half(p, address, &half) != 0)
					return -1;
			}
			arm_write_register(p, Rd, (uint32_t)half);
			return 0;
		}
		case STRH:
		{
			if (CP15_reg1_Ubit == 0){
				if (get_bit(address, 0) == 0){
					if (arm_write_half(p, address, (uint16_t)get_bits(RdVal, 15, 0)) != 0)
						return -1;
				}else{
					if (arm_write_half(p, address, (uint16_t)0x00) != 0)
						return -1;
				}
			}else{
				if (arm_write_half(p, address, (uint16_t)get_bits(RdVal, 15, 0)) != 0)
					return -1;
			}
		return 0;
		}

		default:
			return UNDEFINED_INSTRUCTION;
	}

	return -1;
}

int arm_load_store_multiple(arm_core p, uint32_t ins)
{
	uint32_t end_address = 0;
	uint32_t start_address = 0;
	get_start_end_address(p, ins, &start_address, &end_address);
	uint32_t address;
	uint32_t data = 0;
	name_of_function name = get_func(ins);
	address = start_address;
	switch (name){
		case LDM:
		{
			uint8_t i;
			for ( i = 0; i < 15; i++)
			{
				if (get_bit(ins, i) == 1)
				{
					if (arm_read_word(p, address, &data) != 0)
						return -1;
					arm_write_register(p, i, data);
					address = address + 4;
				}
			}
			if (get_bit(ins, 15) == 1){
				uint32_t value = 0;
				if (arm_read_word(p, address, &value) != 0)
					return -1;
				arm_write_register(p, (uint8_t)PC, (value & 0xFFFFFFFE));
				set_t_bit(p,get_bit(data, 0));
				address = address + 4;
			}
			assert (end_address == (address - 4));
			return 0;
		}
		case STM:
		{
			uint8_t i;
			for (i = 0; i < 15; i++)
			{
				if (get_bit(ins, i) == 1)
				{
					uint32_t Ri_val = arm_read_register(p, i);
					if (arm_write_word(p, address, Ri_val) != 0)
						return -1;
					address = address + 4;
				}
			}
			assert (end_address == (address - 4));
			return 0;
		}
			default:
				return UNDEFINED_INSTRUCTION;
		}

	return -1;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins)
{
	return UNDEFINED_INSTRUCTION;
}
