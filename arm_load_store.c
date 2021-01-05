/*
Armator - simulateur de jeu d'instruction ARMv5T à but pédagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique Générale GNU publiée par la Free Software
Foundation (version 2 ou bien toute autre version ultérieure choisie par vous).

Ce programme est distribué car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but spécifique. Reportez-vous à la
Licence Publique Générale GNU pour plus de détails.

Vous devez avoir reçu une copie de la Licence Publique Générale GNU en même
temps que ce programme ; si ce n'est pas le cas, écrivez à la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
États-Unis.

Contact: Guillaume.Huard@imag.fr
	 Bâtiment IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'Hères
*/
#include "arm_load_store.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "util.h"
#include "debug.h"

int ConditionPassed(arm_core p,uint32_t ins){
	uint8_t cond = ins>>28 & 0x0f;
	uint8_t cpsr = arm_read_cpsr(p);
	uint8_t n = cpsr>>31 & 0x01;
	uint8_t z = cpsr>>30 & 0x01;
	uint8_t c = cpsr>>29 & 0x01;
	uint8_t v = cpsr>>28 & 0x01;
	switch(cond){
		case 0x00: return z==0x01;				//0000  EQ       Equal                               Z set
		case 0x01: return z==0x00;				//0001  NE       Not equal                           Z clear
		case 0x02: return c==0x01;				//0010  CS/HS    Carry set/unsigned higher or same   C set
		case 0x03: return c==0x00;				//0011  CC/LO    Carry clear/unsigned lower          C clear
		case 0x04: return n==0x01;				//0100  MI       Minus/negative                      N set
		case 0x05: return n==0x00;				//0101  PL       Plus/positive or zero               N clear
		case 0x06: return v==0x01;				//0110  VS       Overflow                            V set
		case 0x07: return v==0x00;				//0111  VC       No overflow                         V clear
		case 0x08: return z==0x00 && c==0x01;	//1000  HI       Unsigned higher                     C set and Z clear
		case 0x09: return z==0x01 || c==0x00;	//1001  LS       Unsigned lower or same              C clear or Z set
		case 0x0A: return n==v;					//1010  GE       Signed greater than or equal        N set and V set, orN clear and V clear (N == V)
		case 0x0B: return n!=v;					//1011  LT       Signed less than                    N set and V clear, or N clear and V set (N != V)
		case 0x0C: return z==0x00 && n==v;		//1100  GT       Signed greater than                 Z clear, and either N set and V set, or N clear and V clear (Z == 0,N == V)
		case 0x0D: return z==0x01 || n!=v;		//1101  LE       Signed less than or Equal 			 Z set, or N set and V clear, or N clear and V set (Z == 1 or N != V)
		case 0x0E: return 1;					//1110  AL       Always (unconditional)
		case 0x0F: return 0;					//1111  -        See Condition code 0b1111
		default: return 0;
	}
}
int arm_load_store(arm_core p, uint32_t ins) {
	if(((ins>>27)&0x01==0X00)&&((ins>>26)&0x01==0x01)){
		if ((ins>>22)&0x01==0x00){
			if((ins>>20)&0x01==0x01){
				/* 			LDR 
				MemoryAccess(B-bit, E-bit)
				if ConditionPassed(cond) then
					if (CP15_reg1_Ubit == 0) then
						data = Memory[address,4] Rotate_Right (8 * address[1:0])
					else    / CP15_reg_Ubit == 1 /
						data = Memory[address,4]
					if (Rd is R15) then
						if (ARMv5 or above) then
							PC = data AND 0xFFFFFFFE
							T Bit = data[0]
						else
							PC = data AND 0xFFFFFFFC
					else
						Rd = data
				*/
				if (ConditionPassed(p,ins)){
					
				}
			}else{ // donc ((ins>>20)&0x01==0x00) 
				/*           STR
				MemoryAccess(B-bit, E-bit)
				processor_id = ExecutingProcessor()
				if ConditionPassed(cond) then
					Memory[address,4] = Rd
					if Shared(address) then    / from ARMv6 /
						physical_address = TLB(address)
						ClearExclusiveByAddress(physical_address,processor_id,4)
						/ See Summary of operation on page A2-49 /
				*/
				if (ConditionPassed(p,ins)){
					
				}
			}
		}else { //donc ((ins>>20)&0x01==0x01)
			if((ins>>20)&0x01==0x01){
				/*			LDRB
				MemoryAccess(B-bit, E-bit)
				if ConditionPassed(cond) then
					Rd = Memory[address,1]
				*/
				if (ConditionPassed(p,ins)){
					
				}
			}else { // donc ((ins>>20)&0x01==0x00) 
				/*           STRB
				processor_id = ExecutingProcessor()
				if ConditionPassed(cond) then
					Memory[address,1] = Rd[7:0]
					if Shared(address) then      / from ARMv6 /
						physical_address = TLB(address)
						ClearExclusiveByAddress(physical_address,processor_id,1)
						/ See Summary of operation on page A2-49 /
				*/
				if (ConditionPassed(p,ins)){
					
				}
			}
		}
	}else if(((ins>>27)&0x01==0X00)&&((ins>>26)&0x01==0x00)&&((ins>>25)&0x01==0x00)&&((ins>>4)&0x0F==0x0B)){
		if((ins>>20)&0x01==0x01){
				/*			LDRH
				MemoryAccess(B-bit, E-bit)
				if ConditionPassed(cond) then
					if (CP15_reg1_Ubit == 0) then
						if address[0] == 0 then
							data = Memory[address,2]
						else
							data = UNPREDICTABLE
					else    / CP15_reg1_Ubit == 1 /
						data = Memory[address,2]
					Rd = ZeroExtend(data[15:0])
				*/
				if (ConditionPassed(p,ins)){
					
				}
			}else { // donc ((ins>>20)&0x01==0x00) 
				/*           STRH 
				MemoryAccess(B-bit, E-bit)
				processor_id = ExecutingProcessor()
				if ConditionPassed(cond) then
					if (CP15_reg1_Ubit == 0) then
						if address[0] == 0b0 then
							Memory[address,2] = Rd[15:0]
						else
							Memory[address,2] = UNPREDICTABLE
					else    / CP15_reg1_Ubit ==1  /
						Memory[address,2] = Rd[15:0]
					if Shared(address) then    / ARMv6 /
						physical_address = TLB(address)
						ClearExclusiveByAddress(physical_address,processor_id,2)
						/ See Summary of operation on page A2-49 /
				*/
				if (ConditionPassed(p,ins)){
					
				}
			}
	}
    return UNDEFINED_INSTRUCTION;
}

int arm_load_store_multiple(arm_core p, uint32_t ins) {
	if(((ins>>27)&0x01==0X01)&&((ins>>26)&0x01==0x00)&&((ins>>25)&0x01==0x00)){
		if((ins>>20)&0x01==0x01){
				//LDM
			}else { // donc ((ins>>20)&0x01==0x00) 
				//STM
			}	
	}
    return UNDEFINED_INSTRUCTION;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    if((ins>>27)&0x01)
    	return arm_load_store_multiple(p,ins);
    else
		return arm_load_store(p,ins);
 
}
