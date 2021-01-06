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
#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "util.h"
#include "debug.h"


uint8_t opcode(uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bits(ins, 24, 21);
	return valeur;
}

uint8_t I_bit(uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bit(ins, 25);
	return valeur;
}

uint8_t bit_4(uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bit(ins, 4);
	return valeur;
}

uint8_t bit_7(uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bit(ins, 7);
	return valeur;
}

uint8_t S_bit(uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bit(ins, 20);
	return valeur;
}

uint8_t C_Flag(arm_core p){
	uint8_t valeur = 0;
	valeur = get_bit(arm_read_cpsr(p), 29);
	return valeur;
}

uint8_t shift_imm(uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bitsget_bits(ins, 11, 7);;
	return valeur;
}

uint8_t shift(uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bitsget_bits(ins, 6, 5);;
	return valeur;
}

uint8_t Rm(uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bits(ins, 3, 0);
	return valeur;
}

uint8_t Rm_31(arm_core p, uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bit(arm_read_register(p, Rm(ins)), 31);
	return valeur;
}

uint8_t Rs(uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bits(ins, 11, 8);
	return valeur;
}

uint8_t Rs_7_0(arm_core p, uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bits(arm_read_register(p, Rs(ins)), 7, 0);
	return valeur;
}

void imm_32_bit(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	uint8_t immed_8 = get_bits(ins, 7, 0);
	uint8_t rotate_imm = get_bits(ins, 11, 8);
	if (I_bit(ins) == 1){
		*shifter_operand = ror((uint32_t)immed_8, rotate_imm <<1);
		if ( rotate_imm == 0 )
			*shifter_carry_out = C_Flag;
		else{
			*shifter_carry_out = get_bit(*shifter_operand, 31);
		}
	}
}

void LSL_imm(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	if (shift_imm(ins) == 0 && shift(ins) == 0 && bit_4(ins) == 0){
		*shifter_operand = Rm(ins);
		*shifter_carry_out = C_Flag(p);
	}
	if ( shift_imm(ins) > 0 && shift(ins) == 0 && bit_4(ins) == 0 ){
		*shifter_operand = Rm(ins) << shift_imm(ins); 
		*shifter_carry_out = get_bit(arm_read_register(p, Rm(ins)), (32 - shift_imm(ins))); 
	}
}

void LSR_imm(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	if ( shift(ins) == 1 && bit_4(ins) == 0){
		if ( shift_imm(ins) == 0 ){
			*shifter_operand = 0;
			*shifter_carry_out = Rm_31(p, ins);
		}
		else{
			*shifter_operand = arm_read_register(p, Rm(ins)) >> shift_imm(ins);
			*shifter_carry_out = get_bit(arm_read_register(p, Rm(ins)), (shift_imm(ins) - 1));
		}
	}	
}

void ASR_imm(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	if ( shift(ins) == 2 && bit_4(ins) == 0){
		if ( shift_imm(ins) == 0 ){
			if ( get_bit(arm_read_register(p, Rm(ins)), 31) == 0){
				*shifter_operand = 0;
				*shifter_carry_out = Rm_31(p, ins);
			}
			else{
				*shifter_operand = 0xFFFFFFFF;
				*shifter_carry_out = Rm_31(p, ins);
			}
		}
		else{
			*shifter_operand = ars(arm_read_register(p, Rm(ins)), shift_imm(ins)); 
			*shifter_carry_out = get_bit(arm_read_register(p, Rm(ins)), (shift_imm(ins) - 1));
		}
	}
}

void ROR_imm(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	if ( shift(ins) == 3 && bit_4(ins) == 0){
		if ( shift_imm(ins) == 0 ){
			*shifter_operand = (C_Flag(p) << 31) || (arm_read_register(p, Rm(ins)) >> 1);
			*shifter_carry_out = get_bit(arm_read_register(p, Rm(ins)), 0);
		}
		else{
			*shifter_operand = ror(arm_read_register(p, Rm(ins)), shift_imm(ins));
			*shifter_carry_out = get_bit(arm_read_register(p, Rm(ins)), (shift_imm(ins) - 1));
		}
	}
}

void reg_shift(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	if ( shift_imm(ins) == 0 && shift(ins) == 0 && bit_4(ins) ==0 ){
		*shifter_operand = Rm(ins);
		*shifter_carry_out = C_Flag(p);
	}
}

void LSL_reg(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	if ( I_bit(ins) == 0 && shift(ins) ==0 && bit_4(ins) ==1 ){
		if ( Rs_7_0(p, ins) == 0 ){
			*shifter_operand = (arm_read_register(p, Rm(ins)));
			*shifter_carry_out = C_Flag;
		}
		else if ( Rs_7_0(p, ins) < 32 ){
			*shifter_operand = (arm_read_register(p, Rm(ins))) << Rs_7_0(p, ins);
			*shifter_carry_out = get_bit(arm_read_register(p, Rm(ins)), (32 - Rs_7_0(p, ins)));
		}
		else if ( Rs_7_0(p, ins) == 32 ){
			*shifter_operand = 0;
			*shifter_carry_out = get_bit(arm_read_register(p, Rm(ins)), 0);
		}
		else{
			*shifter_operand = 0;
			*shifter_carry_out = 0;
		}
	}
	
}

void LSR_reg(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	if ( I_bit(ins) == 0 && shift(ins) ==1 && bit_4(ins) ==1 ){
		if ( Rs_7_0(p, ins) == 0 ){
			*shifter_operand = (arm_read_register(p, Rm(ins)));
			*shifter_carry_out = C_Flag(p);
		}
		else if ( Rs_7_0 < 32 ){
			*shifter_operand = (arm_read_register(p, Rm(ins))) >> Rs_7_0(p, ins);
			*shifter_carry_out = get_bit(arm_read_register(p, Rm(ins)), (Rs_7_0(p, ins) - 1));
		}
		else if ( Rs_7_0(p, ins) == 32 ){
			*shifter_operand = 0;
			*shifter_carry_out = Rm_31(p, ins);
		}
		else{
			*shifter_operand = 0;
			*shifter_carry_out = 0;
		}
	}
}

void ASR_reg(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	if ( I_bit(ins) == 0 && shift(ins) ==2 && bit_4(ins) ==1 ){
		if ( Rs_7_0(p, ins) == 0 ){
			*shifter_operand = (arm_read_register(p, Rm(ins)));
			*shifter_carry_out = C_Flag(p);
		}
		else if ( Rs_7_0(p, ins) < 32 ){
			*shifter_operand = ars(arm_read_register(p, Rm(ins)), Rs_7_0(p, ins));
			*shifter_carry_out = get_bit(arm_read_register(p, Rm(ins)), (Rs_7_0(p, ins) - 1));
		}
		else{
			if ( Rm_31(p, ins) == 0){
				*shifter_operand = 0;
				*shifter_carry_out = Rm_31(p, ins);
			}
			else{
				*shifter_operand = 0xFFFFFFFF;
				*shifter_carry_out = Rm_31(p, ins);
			}
		}
	}
}

void ROR_reg(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	uint8_t Rs_4_0 = get_bits(arm_read_register(p, Rs), 4, 0);
	if ( I_bit(ins) == 0 && shift(ins) ==3 && bit_4(ins) ==1 ){
		if (Rs_7_0(p, ins) == 0){ 
			*shifter_operand = arm_read_register(p, Rm(ins));
			*shifter_carry_out = C_Flag;
		}
		else if (Rs_4_0 == 0){
			*shifter_operand = arm_read_register(p, Rm(ins));
			*shifter_carry_out = Rm_31(p, ins);
		}
		else{
			*shifter_operand = ror(arm_read_register(p, Rm(ins)), Rs_4_0);
			*shifter_carry_out = get_bit(arm_read_register(p, Rm(ins)), (Rs_4_0 - 1));
		}
	}
}

void ROR_extend(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	if ( I_bit(ins) == 0 && shift(ins) ==3 && bit_4(ins) ==1 && shift_imm(ins) == 0){
		shifter_operand = (C_Flag(p) << 31) | (arm_read_register(p, Rm(ins)) >> 1);
		shifter_carry_out = get_bit(arm_read_register(p, Rm(ins)), 0);
	}
}

int ins_ADD(arm_core p, uint32_t ins){
	/* pseudocode 
	if ConditionPassed(cond) then
		Rd = Rn + shifter_operand
		if S == 1 and Rd == R15 then
			if CurrentModeHasSPSR() then 
				CPSR = SPSR
			else UNPREDICTABLE 
		else if S == 1 then
			N Flag = Rd[31]
			Z Flag = if Rd == 0 then 1 else 0
			C Flag = CarryFrom(Rn + shifter_operand)
			V Flag = OverflowFrom(Rn + shifter_operand)
	*/
	return 0;
}

int ins_EOR(arm_core p, uint32_t ins){
	/* pseudocode */
	return 0;
}

int ins_SUB(arm_core p, uint32_t ins){
	/* pseudocode */
	return 0;
}

int ins_RSB(arm_core p, uint32_t ins){
	/* pseudocode */
	return 0;
}

int ins_ADC(arm_core p, uint32_t ins){
	/* pseudocode */
	return 0;
}

int ins_SBC(arm_core p, uint32_t ins){
	/* pseudocode */
	return 0;
}

int ins_RSC(arm_core p, uint32_t ins){
	/* pseudocode */
	return 0;
}

int ins_TST(arm_core p, uint32_t ins){
	/* pseudocode */
	return 0;
}

int ins_TEQ(arm_core p, uint32_t ins){
	/* pseudocode */
	return 0;
}

int ins_CMP(arm_core p, uint32_t ins){
	/* pseudocode */
	return 0;
}

int ins_CMN(arm_core p, uint32_t ins){
	/* pseudocode */
	return 0;
}

int ins_ORR(arm_core p, uint32_t ins){
	/* pseudocode */
	return 0;
}

int ins_MOV(arm_core p, uint32_t ins){
	/* pseudocode */
	return 0;
}

int ins_BIC(arm_core p, uint32_t ins){
	/* pseudocode */
	return 0;
}

int ins_MVN(arm_core p, uint32_t ins){
	/* pseudocode */
	return 0;
}

/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins) {
	return UNDEFINED_INSTRUCTION;
}

int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}


