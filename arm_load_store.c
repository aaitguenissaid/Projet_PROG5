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
typedef enum NomsDeFunc { LDR, LDRB, LDRH, STR, STRB, STRH, LDM, STM}


uint32_t get_index(uint32_t ins){
	uint32_t rm = arm_read_register(p, (uint8_t) get_bits(ins, 3, 0));
	uint8_t shift_imm = get_bits(ins, 11, 7);
	uint8_t shift_ind = get_bits(ins, 6, 5);
	uint32_t index;
	switch (shift_ind)
	    {
		case 0:
			index = rm << shift_imm;
			break;
		case 1:
			if (shift_imm == 0)
			    index = 0;
			else
			    index = rm >> shift_imm;
				break;
	    case 2:
			{
			if (shift_imm == 0){
			    if (get_bit(rm, 31) == 1){
					index = 0xFFFFFFFF;
				} else{
					index = 0; 
				}						
			} else {
			    if (rm >= 0){
					index = rm >> shift_imm;
				} else {   
					uint32_t masque = ((0xFFFFFFFF >> (32-shift_imm)) << (32-shift_imm));
			        index = (rm >> shift_imm) | masque;
			        }
			    }
			    break;
			}
		case 3:
			{
			if (shift_imm == 0){
				uint8_t cpsr = arm_read_cpsr(p);
				uint32_t c = get_bit(cpsr,29);
				index = c | (rm << 1);
			} else {
				index = (rm << shift_imm) | (rm >> (32-shift_imm));
				}					
			break;
	        }
		default:
			break;
		}
	return index;
	
}

void operation_1(int u, uint32_t *adr, uint32_t op_gauche, uint32_t op_droit){
    if(u){
        *adr = op_gauche + op_droit;
    }else{
        *adr = op_gauche - op_droit;
    }	
}


void condtionPassFonc(arm_core p, uint32_t ins, uint32_t ins *adr){
	if (ConditionPassed(p, ins)){
		arm_write_register(p, (uint8_t) get_bits(ins, 19, 16), *adr);
	}
}

void condtionPassFonc_modifieRn(arm_core p, uint32_t ins, uint32_t op_droit){
		if ConditionPassed(p, ins){
			uint32_t Rn_write;
			int u = get_bit(ins, 23);
			uint32_t rn = arm_read_register(p, (uint8_t) get_bits(ins, 19, 16));
			if(u){
	        	Rn_write = rn + op_droit;
	    	}else {
	        	Rn_write = rn - op_droit;
	    	}
			arm_write_register(p,(uint8_t) get_bits(ins, 19, 16), Rn_write);	
		}
}

uint32_t get_adres(arm_core p, uint32_t ins){
	int u = get_bit(ins, 23);
	uint32_t rm = arm_read_register(p, (uint8_t) get_bits(ins, 3, 0));
	uint32_t rn = arm_read_register(p, (uint8_t) get_bits(ins, 19, 16));
	uint32_t offset_12 = get_bits(ins, 11, 0);
	uint8_t bits_2724 = get_bits(ins,27, 24);
	uint8_t bits_21 = get_bit(ins, 21);
	uint32_t adresse = 0;
	if ((bits_2724 == 0x5) && (bits_21 == 0)){
		// 1.Load and Store Word or Unsigned Byte - Immediate offset
		operation_1(u, &adresse, rn, offset_12);
		return adresse;
	}
	else if ((bits_2724==0x07) && (bits_21 == 0) && (get_bits(ins,11,4) == 0)){
		// 2.Load and Store Word or Unsigned Byte - Register offset
		operation_1(u, &adresse, rn, rm);
		return adresse;
	}
	else if ((bits_2724 == 0x7) && (bits_21 == 0) && (get_bit(ins, 4) == 0)){
   		// 3.Load and Store Word or Unsigned Byte - Scaled register offset
		uint32_t index = get_index(ins);
		operation_1(u, &adresse, rn, index);
		return adresse;
			
	}
	else if ((bits_2724 == 0x05) && (bits_21 == 1)){
		// 4.Load and Store Word or Unsigned Byte - Immediate pre-indexed
		operation_1(u, &adresse, rn, offset_12);
		condtionPassFonc(p, ins, &adresse);
		return address;
	}
	else if ((bits_2724 == 0x07) && (bits_21 == 1) && (get_bits(ins,11,4) == 0)){
		// 5.Load and Store Word or Unsigned Byte - Register pre-indexed
		operation_1(u, &adresse, rn, rm);
		condtionPassFonc(p, ins, &adresse);
		return address;
	}
	else if ((bits_2724 == 0x07) && (bits_21 == 1) && !get_bit(ins, 4)){
		// 6.Load and Store Word or Unsigned Byte - Scaled register pre-indexed

	    uint32_t index = get_index(ins);
		operation_1(u, &adresse, rn, index);
		condtionPassFonc(p, ins, &adresse);
		return address;		
	}
	else if ((bits_2724 == 0x04) && (bits_21 == 0)){
		// 7.Load and Store Word or Unsigned Byte - Immediate post-indexed
		address = rn;
		condtionPassFonc_modifieRn(p, ins, offset_12)
		return address;
	} 
	else if ((bits_2724 == 0x06) && (bits_21 == 0) && (get_bits(ins,11,4) == 0)){
		// 8.Load and Store Word or Unsigned Byte - Register post-indexed
		address = rn;
		condtionPassFonc_modifieRn(p, ins, rm);
		return address;
	}
	else if ((bits_2724 == 0x06) && (bits_21 == 0) && !get_bit(ins,4)){
		// 9.Load and Store Word or Unsigned Byte - Scaled register post-indexed
		address = rn;
	    uint32_t index = get_index(ins);
		condtionPassFonc_modifieRn(p, ins, index);
		return address;	
	}
}

uint32_t get_offset(uint32_t ins){
	uint32_t immedH = get_bits(ins, 11, 8);
	uint32_t immedL = get_bits(ins, 3, 0);
	uint32_t offset_8 = (immedH << 4) | immedL;
	return offset_8;
}

uint32_t get_adres_h(arm_core p,uint32_t ins){
	int u = get_bit(ins, 23);
	uint32_t rn = arm_read_register(p, (uint8_t) get_bits(ins, 19, 16));
	uint8_t bits_2724 = get_bits(ins, 27, 24);
	uint8_t bits_2221 = get_bits(ins, 22, 21);
	uint32_t adresse = 0;
	if ((bits_2724 == 1) && (bits_2221 == 2)){
		// 1.Miscellaneous Loads and Stores - Immediate offset
		uint32_t offset_8 = get_offset(ins);
		operation_1(u, &adresse, rn, offset_8);
		return address;			
	}
	else if ((bits_2724 == 1) && (bits_2221 == 0))
	{  // 2.Miscellaneous Loads and Stores - Register offset
		uint32_t rm = arm_read_register(p, (uint8_t) get_bits(ins, 3, 0));
		operation_1(u, &adresse, rn, rm);
		return address;	
	}
	else if ((bits_2724 == 1) && (bits_2221 == 3))
	{  // 3.Miscellaneous Loads and Stores - Immediate pre-indexed 
		uint32_t offset_8 = get_offset(ins);
		operation_1(u, &adresse, rn, offset_8);
		condtionPassFonc(p, ins, &adresse);
		return address;				
	}
	else if ((bits_2724 == 1) && (bits_2221 == 1))
	{  // 4.Miscellaneous Loads and Stores - Immediate pre-indexed
		uint32_t rm = arm_read_register(p, (uint8_t) get_bits(ins, 3, 0));
		operation_1(u, &adresse, rn, rm);
		condtionPassFonc(p, ins, &adresse);
		return address;				
	}
	else if ((bits_2724 == 0) && (bits_2221 == 2))
	{  // 5.Miscellaneous Loads and Stores - Immediate post-indexed
		address = rn;
		uint32_t offset_8 = get_offset(ins);
		condtionPassFonc_modifieRn(p, ins, offset_8);
		return address;				
	}
	else if ((bits_2724 == 0) && (bits_2221 == 0))
	{  // 6.Miscellaneous Loads and Stores - Register post-indexed
		address = rn;
		uint32_t rm = arm_read_register(p, (uint8_t) get_bits(ins, 3, 0));
		condtionPassFonc_modifieRn(p, ins, rm);
		return address;				
	}
}


NomsDeFunc get_func(uint32_t ins){
	if(get_bits(ins, 27, 26) == 0x1){
		if (!get_bit(ins, 22)){
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
	}else if((get_bits(ins, 27, 25) == 0x0) && (get_bits(ins, 7, 4) == 0xB)){
		if(get_bit(ins,20)){
				return LDRH;
			}else { // donc !get_bit(ins,20)
				return STRH;
			}
	}
	else if((get_bits(ins, 27, 25) == 0x4) && (!get_bit(ins, 22))){
		if(get_bit(ins,20)){
				return LDM;
		} else { // donc !get_bit(ins,20)
				return STM;
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
	uint8_t Rd = (uint8_t) get_bits(ins,15,12);
	uint32_t data;
	uint32_t address;
	uint32_t cprs=arm_read_cpsr(p);
	uint32_t RdVal;
	uint8_t byte;
	uint16_t half;
	switch (name){
		case LDR :{
			if(ConditionPassed(p,ins)){
				address = get_adres(p,ins);
				if(CP15_reg1_Ubit==0){
					if (arm_read_word(p,(uint32_t) address , &data)!=0){
						printf("ERROR");
					}
					data=reverse_4(data);//????Rotate_Right (8 * address[1:0])
				}else{
					if (arm_read_word(p,(uint32_t) address , &data)!=0){
						printf("ERROR");
					}
				}
				if (Rd==0x0f){
					arm_write_register(p,PC,(uint32_t) (data & 0xfffffffe))
					//TBit = data[0] 
					if(get_bit(data,0)){
						cprs=set_bit(cprs, 5);
					}else{
						cprs=clr_bit(cprs, 5);
					}
					arm_write_cpsr(p,cprs);
				}else{
					arm_write_register(p,Rd,data);
				}
			}
			return 0; //??????
			break;
		}
		case STR : {
			if(ConditionPassed(p,ins)){
				address = get_adres(p,ins);
				RdVal = arm_read_register(p,Rd);
				if(arm_write_word(p,adresse,RdVal)!=0){
					printf("ERROR");
				}
			}
			return 0; //??????
			break;
		}
		case LDRB :{ 
			if(ConditionPassed(p,ins)){
				address = get_adres(p,ins);
				if(arm_read_byte(p,(uint32_t)address,&byte)!=0){
					printf("ERROR");
				}
				arm_write_register(p,Rd,(uint32_t) byte)
			}
			return 0; //??????
			break;
		}
		case STRB : {
			if(ConditionPassed(p,ins)){
				address = get_adres(p,ins);
				RdVal = arm_read_register(p,Rd);
				if(arm_write_byte(p,adresse,(uint8_t)get_bits(RdVal,7,0))!=0){
					printf("ERROR");
				}
			return 0;
			break;
		}
		case LDRH : {
			if(ConditionPassed(p,ins)){
				adresse=get_adres_h(p,ins);
				if(CP15_reg1_Ubit==0){
					if(get_bit(adresse,0)==0x00){
						if(arm_read_half(p,(uint32_t)address,&half)!=0){
							printf("ERROR");
						}
					}else{
						data=0x00;
						return UNDEFINED_INSTRUCTION;//OU ERRROR
					}
				}else{
					if(arm_read_half(p,(uint32_t)address,&half)!=0){
						printf("ERROR");
					}
				}
				//Rd = ZeroExtend(data[15:0])
				arm_write_register(p,Rd,(uint16_t) half);//????????
			}
			return 0;
			break;
		}
		case STRH :{ 
			if(ConditionPassed(p,ins)){
				address = get_adres_h(p,ins);
				RdVal = arm_read_register(p,Rd);
				if(CP15_reg1_Ubit==0){
					if(get_bit(adresse,0)==0x00){
						if(arm_write_half(p,adresse,(uint16_t)get_bits(RdVal,15,0))!=0){
							printf("ERROR");
						}
					}else{
						if(arm_write_half(p,adresse,(uint16_t)0x00)!=0){
							printf("ERROR");
						}
						return UNDEFINED_INSTRUCTION;
					}
				}else{
					if(arm_write_half(p,adresse,(uint16_t)get_bits(RdVal,15,0))!=0){
						printf("ERROR");
					}
				}
			}
			return 0;
			break;
		}
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
