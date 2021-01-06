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
typedef enum NomsDeFunc { LDR, LDRB, LDRH, STR, STRB, STRH, LDM, STM}

uint32_t get_adres(arm_core p,uint32_t ins){
	if ((get_bits(ins,27, 24) == 0x5) && (get_bit(ins, 21) == 0)){
		//Load and Store Word or Unsigned Byte - Immediate offset
    	if(get_bit(ins, 23) == 1){
        	return arm_read_register(p,(uint8_t) get_bits(ins, 19, 16)) + (uint32_t) get_bits(ins, 11, 0);
    	}else {
        	return arm_read_register(p,(uint8_t) get_bits(ins, 19, 16)) - (uint32_t) get_bits(ins, 11, 0);
    	}     
	}else if ((get_bits(ins,27,24)==0x07)&&!get_bit(ins,21)&&(get_bits(ins,11,4)==0x00)){
		//Load and Store Word or Unsigned Byte - Register offset
		if(get_bit(ins,23)){	
			return arm_read_register(p,(uint8_t) get_bits(ins,19,16)) + arm_read_register(p,(uint8_t) get_bits(ins,3,0));
		}else{
			return arm_read_register(p,(uint8_t) get_bits(ins,19,16)) - arm_read_register(p,(uint8_t) get_bits(ins,3,0));
		}
	}else if ((get_bits(ins,27,24)==0x05)&&get_bit(ins,21)){
		//Load and Store Word or Unsigned Byte - Immediate pre-indexed
		uint32_t address;
		if(get_bit(ins,23)){	
			address =  arm_read_register(p,(uint8_t) get_bits(ins, 19, 16)) + (uint32_t) get_bits(ins, 11, 0);
    	}else {
        	address = arm_read_register(p,(uint8_t) get_bits(ins, 19, 16)) - (uint32_t) get_bits(ins, 11, 0);
    	}  
		if ConditionPassed(p,ins){
			arm_write_register(p,(uint8_t) get_bits(ins, 19, 16),address);
		}
		return address;
	}else if ((get_bits(ins,27,24)==0x07)&&get_bit(ins,21)&&(get_bits(ins,11,4)==0x00)){
		//Load and Store Word or Unsigned Byte - Register pre-indexed
		uint32_t address;
		if(get_bit(ins,23)){	
			address =  arm_read_register(p,(uint8_t) get_bits(ins,19,16)) + arm_read_register(p,(uint8_t) get_bits(ins,3,0));
    	}else {
        	address = arm_read_register(p,(uint8_t) get_bits(ins,19,16)) - arm_read_register(p,(uint8_t) get_bits(ins,3,0));
    	}  
		if ConditionPassed(p,ins){
			arm_write_register(p,(uint8_t) get_bits(ins, 19, 16),address);
		}
		return address;
	}else if ((get_bits(ins,27,24)==0x04)&&!get_bit(ins,21)){
		//Load and Store Word or Unsigned Byte - Immediate post-indexed
		if ConditionPassed(p,ins){
			if(get_bit(ins, 23) == 1){
	        	return arm_read_register(p,(uint8_t) get_bits(ins, 19, 16)) + (uint32_t) get_bits(ins, 11, 0);
	    	}else {
	        	return arm_read_register(p,(uint8_t) get_bits(ins, 19, 16)) - (uint32_t) get_bits(ins, 11, 0);
	    	} 
		}
	} else if ((get_bits(ins,27,24)==0x06)&&!get_bit(ins,21)&&(get_bits(ins,11,4)==0x00)){
		//Load and Store Word or Unsigned Byte - Register post-indexed
		if ConditionPassed(p,ins){
			if(get_bit(ins,23)){	
				return arm_read_register(p,(uint8_t) get_bits(ins,19,16)) + arm_read_register(p,(uint8_t) get_bits(ins,3,0));
			}else{
				return arm_read_register(p,(uint8_t) get_bits(ins,19,16)) - arm_read_register(p,(uint8_t) get_bits(ins,3,0));
			}
		}
	}else/*Addressing Mode 3 - Miscellaneous Loads and Stores*/ 
		if((get_bits(ins,27,24)==0x01)&&(get_bits(ins,22,21)==0x02)&&get_bit(ins,7)&&get_bit(ins,4)){
			// ??? Miscellaneous Loads and Stores - Immediate offset
			uint32_t offset_8 = 0x00;
			offset_8 = (uint32_t) ((get_bits(ins,11,8)<<4)|get_bits(ins,3,0));
			if(get_bit(ins, 23) == 1){
				return arm_read_register(p,(uint8_t) get_bits(ins, 19, 16)) +  offset_8
			}else{
				return arm_read_register(p,(uint8_t) get_bits(ins, 19, 16)) -  offset_8
			}
		}
}

NomsDeFunc get_func(uint32_t ins){
	if(!get_bit(ins,27)&&get_bit(ins,26)){
		if (!get_bit(ins,22)){
			if(get_bit(ins,20)){
				return LDR;
			}else{ // donc !get_bit(ins,20)
				return STR;
			}
		}else { //donc get_bit(ins,22)
			if(get_bit(ins,20)){
				return LDRB;
			}else { // donc !get_bit(ins,20) 
				return STRB;
			}
		}
	}else if(!get_bit(ins,27)&&!get_bit(ins,26)&&!get_bit(ins,25)&&(get_bits(ins,7,4)==0x0B)){
		if(get_bit(ins,20)){
				return LDRH;
			}else { // donc !get_bit(ins,20) 
				return STRH;
			}
	}
}

int ConditionPassed (arm_core p,uint32_t ins){
	uint8_t cond = get_bits(ins,31,28);
	uint8_t cpsr = arm_read_cpsr(p);
	uint8_t n = get_bit(cpsr,31);
	uint8_t z = get_bit(cpsr,30);
	uint8_t c = get_bit(cpsr,29);
	uint8_t v = get_bit(cpsr,28);
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
	NomsDeFunc name = get_func(ins);
	switch (name){
		case LDR : break;
		case STR : break;
		case LDRB : break;
		case STRB : break;
		case LDRH : break;
		case STRH : break;
		default: return UNDEFINED_INSTRUCTION;
	}
    return UNDEFINED_INSTRUCTION;
}
int arm_load_store_multiple(arm_core p, uint32_t ins){
	
	return UNDEFINED_INSTRUCTION;	
}
int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    if(get_bit(ins,27))
    	return arm_load_store_multiple(p,ins);
    else
		return arm_load_store(p,ins);
 
}
