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
#include "arm_load_store.h"
#include "arm_branch_other.h"
#include "util.h"
#include "debug.h"

/*-------------------------- Fonctions auxiliaires --------------------------*/

uint8_t get_opcode(uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bits(ins, 24, 21);
	return valeur;
}

uint8_t bit_I(uint32_t ins){
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

uint8_t bit_S(uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bit(ins, 20);
	return valeur;
}

uint32_t get_C_flag(arm_core p){
	uint32_t valeur = 0;
	valeur = get_bit(arm_read_cpsr(p), C);
	return valeur;
}

uint8_t get_shift_imm(uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bits(ins, 11, 7);
	return valeur;
}

uint8_t get_shift(uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bits(ins, 6, 5);
	return valeur;
}

uint8_t get_rm(uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bits(ins, 3, 0);
	return valeur;
}

uint8_t get_rn(uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bits(ins, 19, 16);
	return valeur;
}

uint8_t get_rd(uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bits(ins, 15, 12);
	return valeur;
}

uint8_t get_rm_31(arm_core p, uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bit(arm_read_register(p, get_rm(ins)), 31);
	return valeur;
}

uint8_t get_rs(uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bits(ins, 11, 8);
	return valeur;
}

uint8_t get_data_rs_7_0(arm_core p, uint32_t ins){
	uint8_t valeur = 0;
	valeur = get_bits(arm_read_register(p, get_rs(ins)), 7, 0);
	return valeur;
}

void update_flags_cpsr(arm_core p, uint8_t N_Flag, uint8_t Z_Flag, uint8_t C_Flag, uint8_t V_Flag){
	uint32_t val_CPSR = arm_read_cpsr(p);
	uint32_t new_val_CPSR = (N_Flag << N) | (Z_Flag << Z) | (C_Flag << C) | (V_Flag << V) | val_CPSR;
	arm_write_cpsr(p, new_val_CPSR);
}

uint32_t two_complement(uint32_t operand){
	 uint32_t valeur  = operand;
	 valeur = ~(valeur) + 1;
	 return valeur;
}

uint64_t operation(uint64_t operand1, uint64_t operand2, int op){
	uint64_t valeur = 0;
	if (op == 1){	// addition
		valeur = operand1 + operand2;
	}
	else if (op == 2){	// soustraction
		uint32_t ca2_operand2 = two_complement(operand2);
		valeur = operand1 + (uint64_t)ca2_operand2;
	}
	return valeur;
}

int borrow_from(uint32_t operand1, uint32_t operand2){
	if ( operand1 >= operand2){
		return 0;
	}
	else{
		return 1;
	}
}

int overflow_from(uint32_t operand1, uint32_t operand2, int op){
	uint64_t valeur = operation((uint64_t)operand1, (uint64_t)operand2, op);
	int signe_operand1 = get_bit(operand1, 31);
	int signe_operand2 = get_bit(operand2, 31);
	int signe_valeur = get_bit(valeur, 31);
	if (signe_operand1 == 0 && signe_operand2 == 0){
		if (signe_valeur == 1)
			return 1;
		else{
			return 0;
		}	
	}
	else if (signe_operand1 == 1 && signe_operand2 == 1){
		if (signe_valeur == 0)
			return 1;
		else{
			return 0;
		}
	}
	else{
		return 0;
	}
}

int carry_from(uint32_t operand1, uint32_t operand2, int op){
	uint64_t valeur = operation((uint64_t)operand1, (uint64_t)operand2, op);
	int signe_valeur = get_bit(valeur, 32);
	if ( op == 1){	// addition
		if (signe_valeur == 1)
			return 1;
		else{
			return 0;
		}
	}
	else if ( op == 2){	// soustraction
		if (signe_valeur == 0)
			return 1;
		else{
			return 0;
		}
	}
	else 
		return 0;
}

/*-------------------------- addressing modes --------------------------*/

void imm_32_bit(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	uint8_t immed_8 = get_bits(ins, 7, 0);
	uint8_t rotate_imm = get_bits(ins, 11, 8);
	*shifter_operand = ror((uint32_t)immed_8, (rotate_imm <<1));
	if ( rotate_imm == 0 )
		*shifter_carry_out = get_C_flag(p);
	else{
		*shifter_carry_out = get_bit(*shifter_operand, 31);
	}
}

void reg_shift(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	*shifter_operand = get_rm(ins);
	*shifter_carry_out = get_C_flag(p);
}

void lsl_imm(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	if (get_shift_imm(ins) == 0){
		*shifter_operand = get_rm(ins);
		*shifter_carry_out = get_C_flag(p);
	}
	else{
		*shifter_operand = get_rm(ins) << get_shift_imm(ins); 
		*shifter_carry_out = get_bit(arm_read_register(p, get_rm(ins)), (32 - get_shift_imm(ins))); 
	}
}

void lsl_reg(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	if ( get_data_rs_7_0(p, ins) == 0 ){
		*shifter_operand = (arm_read_register(p, get_rm(ins)));
		*shifter_carry_out = get_C_flag(p);
	}
	else if ( get_data_rs_7_0(p, ins) < 32 ){
		*shifter_operand = (arm_read_register(p, get_rm(ins))) << get_data_rs_7_0(p, ins);
		*shifter_carry_out = get_bit(arm_read_register(p, get_rm(ins)), (32 - get_data_rs_7_0(p, ins)));
	}
	else if ( get_data_rs_7_0(p, ins) == 32 ){
		*shifter_operand = 0;
		*shifter_carry_out = get_bit(arm_read_register(p, get_rm(ins)), 0);
	}
	else{
		*shifter_operand = 0;
		*shifter_carry_out = 0;
	}
}

void lsr_imm(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	if ( get_shift_imm(ins) == 0 ){
		*shifter_operand = 0;
		*shifter_carry_out = get_rm_31(p, ins);
	}
	else{
		*shifter_operand = arm_read_register(p, get_rm(ins)) >> get_shift_imm(ins);
		*shifter_carry_out = get_bit(arm_read_register(p, get_rm(ins)), (get_shift_imm(ins) - 1));
	}	
}

void lsr_reg(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	if ( get_data_rs_7_0(p, ins) == 0 ){
		*shifter_operand = (arm_read_register(p, get_rm(ins)));
		*shifter_carry_out = get_C_flag(p);
	}
	else if ( get_data_rs_7_0(p, ins) < 32 ){
		*shifter_operand = (arm_read_register(p, get_rm(ins))) >> get_data_rs_7_0(p, ins);
		*shifter_carry_out = get_bit(arm_read_register(p, get_rm(ins)), (get_data_rs_7_0(p, ins) - 1));
	}
	else if ( get_data_rs_7_0(p, ins) == 32 ){
		*shifter_operand = 0;
		*shifter_carry_out = get_rm_31(p, ins);
	}
	else{
		*shifter_operand = 0;
		*shifter_carry_out = 0;
	}
}

void asr_imm(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	if ( get_shift_imm(ins) == 0 ){
		if ( get_bit(arm_read_register(p, get_rm(ins)), 31) == 0){
			*shifter_operand = 0;
			*shifter_carry_out = get_rm_31(p, ins);
		}
		else{
			*shifter_operand = 0xFFFFFFFF;
			*shifter_carry_out = get_rm_31(p, ins);
		}
	}
	else{
		*shifter_operand = asr(arm_read_register(p, get_rm(ins)), get_shift_imm(ins)); 
		*shifter_carry_out = get_bit(arm_read_register(p, get_rm(ins)), (get_shift_imm(ins) - 1));
	}
}

void asr_reg(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	if ( get_data_rs_7_0(p, ins) == 0 ){
		*shifter_operand = (arm_read_register(p, get_rm(ins)));
		*shifter_carry_out = get_C_flag(p);
	}
	else if ( get_data_rs_7_0(p, ins) < 32 ){
		*shifter_operand = asr(arm_read_register(p, get_rm(ins)), get_data_rs_7_0(p, ins));
		*shifter_carry_out = get_bit(arm_read_register(p, get_rm(ins)), (get_data_rs_7_0(p, ins) - 1));
	}
	else{
		if ( get_rm_31(p, ins) == 0){
			*shifter_operand = 0;
			*shifter_carry_out = get_rm_31(p, ins);
		}
		else{
			*shifter_operand = 0xFFFFFFFF;
			*shifter_carry_out = get_rm_31(p, ins);
		}
	}
}

void ror_imm(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	if ( get_shift_imm(ins) == 0 ){	// mode RXR
		*shifter_operand = (get_C_flag(p) << 31) || (arm_read_register(p, get_rm(ins)) >> 1);
		*shifter_carry_out = get_bit(arm_read_register(p, get_rm(ins)), 0);
	}
	else{
		*shifter_operand = ror(arm_read_register(p, get_rm(ins)), get_shift_imm(ins));
		*shifter_carry_out = get_bit(arm_read_register(p, get_rm(ins)), (get_shift_imm(ins) - 1));
	}
}

void ror_reg(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	uint8_t Rs_4_0 = get_bits(arm_read_register(p, get_rs(ins)), 4, 0);
	if (get_data_rs_7_0(p, ins) == 0){ 
		*shifter_operand = arm_read_register(p, get_rm(ins));
		*shifter_carry_out = get_C_flag(p);
	}
	else if (Rs_4_0 == 0){
		*shifter_operand = arm_read_register(p, get_rm(ins));
		*shifter_carry_out = get_rm_31(p, ins);
	}
	else{
		*shifter_operand = ror(arm_read_register(p, get_rm(ins)), Rs_4_0);
		*shifter_carry_out = get_bit(arm_read_register(p, get_rm(ins)), (Rs_4_0 - 1));
	}
}

void get_shifter_operand_carry_out(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out){
	if (bit_I(ins) == 1)
		imm_32_bit(p, ins, shifter_operand, shifter_carry_out);
	else{
		if (get_shift(ins) == LSL){
			if ( bit_4(ins) == 0)
				lsl_imm(p, ins, shifter_operand, shifter_carry_out);
			if ( bit_4(ins) == 1)
				lsl_reg(p, ins, shifter_operand, shifter_carry_out);
		}
		if (get_shift(ins) == LSR){
			if ( bit_4(ins) == 0)
				lsr_imm(p, ins, shifter_operand, shifter_carry_out);
			if ( bit_4(ins) == 1)
				lsr_reg(p, ins, shifter_operand, shifter_carry_out);
		}
		if (get_shift(ins) == ASR){
			if ( bit_4(ins) == 0)
				asr_imm(p, ins, shifter_operand, shifter_carry_out);
			if ( bit_4(ins) == 1)
				asr_reg(p, ins, shifter_operand, shifter_carry_out);
		}
		if (get_shift(ins) == ROR){
			if ( bit_4(ins) == 0)
				ror_imm(p, ins, shifter_operand, shifter_carry_out);
			if ( bit_4(ins) == 1)
				ror_reg(p, ins, shifter_operand, shifter_carry_out);
		}
	}
}

/*-------------------------- Instructions  de traitement de données --------------------------*/

int ins_AND(arm_core p, uint32_t ins){
	uint32_t shifter_operand = 0;
	uint8_t shifter_carry_out = 0;
	
	if(condition_passed(p, ins)){
		
		get_shifter_operand_carry_out(p, ins, &shifter_operand, &shifter_carry_out);
		uint32_t valeur = shifter_operand & arm_read_register(p, get_rn(ins));
		arm_write_register(p, get_rd(ins), valeur);
		
		if (bit_S(ins) == 1 && get_rd(ins) == 15){
			if (arm_current_mode_has_spsr(p))
				arm_write_cpsr(p, arm_read_spsr(p));
			else{
				return 0; // pas une instruction data-processing
			}
		} 
		else if (bit_S(ins) == 1){
			uint8_t N_Flag = get_bit(arm_read_register(p, get_rd(ins)),31);
			uint8_t Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
			uint8_t C_Flag = shifter_carry_out;
			uint8_t V_Flag = (arm_read_cpsr(p) >> 28) & 1;
			update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
		}
	}
	return 1;
}

int ins_EOR(arm_core p, uint32_t ins){ 
	uint32_t shifter_operand = 0;
	uint8_t shifter_carry_out = 0;
	if (condition_passed(p, ins)){
		get_shifter_operand_carry_out(p, ins, &shifter_operand, &shifter_carry_out);
		int32_t valeur = arm_read_register(p, get_rn(ins)) ^ shifter_operand;
		arm_write_register(p, get_rd(ins), valeur);
		
		if (bit_S(ins) == 1 && get_rd(ins) == 15){
			if (arm_current_mode_has_spsr(p))
				arm_write_cpsr(p, arm_read_spsr(p));
			else{
				return 0; // pas une instruction data-processing
			}
		} 
		else if (bit_S(ins) == 1){
			uint8_t N_Flag = get_bit(arm_read_register(p, get_rd(ins)),31);
			uint8_t Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
			uint8_t C_Flag = shifter_carry_out;
			uint8_t V_Flag = get_bit(arm_read_cpsr(p), V);  //unaffected
			update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
		}
	}
	return 1;
}

int ins_SUB(arm_core p, uint32_t ins){
	uint32_t shifter_operand = 0;
	uint8_t shifter_carry_out = 0;
	
	if (condition_passed(p, ins)){
		get_shifter_operand_carry_out(p, ins, &shifter_operand, &shifter_carry_out);
		uint32_t valeur = arm_read_register(p, get_rn(ins)) - shifter_operand ;
		arm_write_register(p, get_rd(ins), valeur);
		
		if (bit_S(ins) == 1 && get_rd(ins) == 15){
			if (arm_current_mode_has_spsr(p))
				arm_write_cpsr(p, arm_read_spsr(p));
			else{
				return 0; // pas une instruction data-processing
			}
		} 
		else if (bit_S(ins) == 1){
			uint8_t N_Flag = get_bit(arm_read_register(p, get_rd(ins)),31);
			uint8_t Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
			uint8_t C_Flag = ~borrow_from(arm_read_register(p, get_rn(ins)), shifter_operand)&1;
			uint8_t V_Flag = overflow_from(arm_read_register(p, get_rn(ins)), shifter_operand,2);
			update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
		}
	}
	return 1;
}

int ins_RSB(arm_core p, uint32_t ins){
	uint32_t shifter_operand = 0;
	uint8_t shifter_carry_out = 0;
	
	if (condition_passed(p, ins)){
		get_shifter_operand_carry_out(p, ins, &shifter_operand, &shifter_carry_out);
		uint32_t valeur = shifter_operand - arm_read_register(p, get_rn(ins));
		arm_write_register(p, get_rd(ins), valeur);
		
		if (bit_S(ins) == 1 && get_rd(ins) == 15){
			if (arm_current_mode_has_spsr(p))
				arm_write_cpsr(p, arm_read_spsr(p));
			else{
				return 0; // pas une instruction data-processing
			}
		} 
		else if (bit_S(ins) == 1){
			uint8_t N_Flag = get_bit(arm_read_register(p, get_rd(ins)),31);
			uint8_t Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
			uint8_t C_Flag = ~borrow_from(shifter_operand, arm_read_register(p, get_rn(ins)))&1;
			uint8_t V_Flag = overflow_from(shifter_operand, arm_read_register(p, get_rn(ins)),2);
			update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
		}
	}
	return 1;
}

int ins_ADD(arm_core p, uint32_t ins){
	uint32_t shifter_operand = 0;
	uint8_t shifter_carry_out = 0;
	
	if (condition_passed(p, ins)){
		
		get_shifter_operand_carry_out(p, ins, &shifter_operand, &shifter_carry_out);
		uint32_t valeur = shifter_operand + arm_read_register(p, get_rn(ins));
		arm_write_register(p, get_rd(ins), valeur);
		
		if (bit_S(ins) == 1 && get_rd(ins) == 15){
			if (arm_current_mode_has_spsr(p))
				arm_write_cpsr(p, arm_read_spsr(p));
			else{
				return 0; // pas une instruction data-processing
			}
		} 
		else if (bit_S(ins) == 1){
			uint8_t N_Flag = get_bit(arm_read_register(p, get_rd(ins)),31);
			uint8_t Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
			uint8_t C_Flag = carry_from(shifter_operand, arm_read_register(p, get_rn(ins)),1);
			uint8_t V_Flag = overflow_from(arm_read_register(p, get_rn(ins)), shifter_operand, 1);
			update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
		}
	}
	return 1;
}

int ins_ADC(arm_core p, uint32_t ins){
	uint32_t shifter_operand = 0;
	uint8_t shifter_carry_out = 0;
	
	if (condition_passed(p, ins)){
		get_shifter_operand_carry_out(p, ins, &shifter_operand, &shifter_carry_out);
		uint32_t valeur = arm_read_register(p, get_rn(ins)) + shifter_operand + get_C_flag(p);
		arm_write_register(p, get_rd(ins), valeur);
		if (bit_S(ins) == 1 && get_rd(ins) == 15){
			if (arm_current_mode_has_spsr(p))
				arm_write_cpsr(p, arm_read_spsr(p));
			else{
				return 0; // pas une instruction data-processing
			}
		} 
		else if (bit_S(ins) == 1){
			uint8_t N_Flag = get_bit(arm_read_register(p, get_rd(ins)),31);
			uint8_t Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
			uint8_t C_Flag = carry_from(arm_read_register(p, get_rn(ins)), (shifter_operand + get_C_flag(p)),1);
			uint8_t V_Flag = overflow_from(arm_read_register(p, get_rn(ins)), (shifter_operand + get_C_flag(p)), 1);
			update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
		}
	}
	return 1;
}

int ins_SBC(arm_core p, uint32_t ins){
	return 0;
	uint32_t shifter_operand = 0;
	uint8_t shifter_carry_out = 0;

	if ( condition_passed(p, ins)){
		get_shifter_operand_carry_out(p, ins, &shifter_operand, &shifter_carry_out);
		uint32_t valeur = operation(arm_read_register(p, get_rn(ins)), shifter_operand, 2);
		valeur = operation(valeur, ~(get_C_flag(p))&1, 2);
		arm_write_register(p, get_rd(ins), valeur);
		if (bit_S(ins) == 1 && get_rd(ins) == 15){
			if (arm_current_mode_has_spsr(p))
				arm_write_cpsr(p, arm_read_spsr(p));
			else{
				return 0; // pas une instruction data-processing
			}
		} 
		else if (bit_S(ins) == 1){
			uint8_t N_Flag = get_bit(arm_read_register(p, get_rd(ins)),31);
			uint8_t Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
			uint8_t C_Flag = ~borrow_from(arm_read_register(p, get_rn(ins)), shifter_operand + get_C_flag(p))&1;
			uint8_t V_Flag = overflow_from(arm_read_register(p, get_rn(ins)), shifter_operand + get_C_flag(p), 2);
			update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
		}
	}
	return 1;
}

int ins_RSC(arm_core p, uint32_t ins){
	uint32_t shifter_operand = 0;
	uint8_t shifter_carry_out = 0;

	if (condition_passed(p, ins)){
		get_shifter_operand_carry_out(p, ins, &shifter_operand, &shifter_carry_out);
		uint32_t valeur = operation(shifter_operand, arm_read_register(p, get_rn(ins)), 2);
		valeur = operation(valeur, ~(get_C_flag(p))&1, 2);
		arm_write_register(p, get_rd(ins), valeur);
		if (bit_S(ins) == 1 && get_rd(ins) == 15){
			if (arm_current_mode_has_spsr(p))
				arm_write_cpsr(p, arm_read_spsr(p));
			else{
				return 0; // pas une instruction data-processing
			}
		} 
		else if (bit_S(ins) == 1){
			uint8_t N_Flag = get_bit(arm_read_register(p, get_rd(ins)),31);
			uint8_t Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
			uint8_t C_Flag = ~borrow_from(shifter_operand, (arm_read_register(p, get_rn(ins))+ get_C_flag(p)))&1;
			uint8_t V_Flag = overflow_from(shifter_operand, (arm_read_register(p, get_rn(ins))+ get_C_flag(p)), 2);
			update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
		}
	}
	return 1;
}

int ins_TST(arm_core p, uint32_t ins){
	uint32_t shifter_operand = 0;
	uint8_t shifter_carry_out = 0;
	
	if ( condition_passed(p, ins) ){ 
		get_shifter_operand_carry_out(p, ins, &shifter_operand, &shifter_carry_out);
		uint32_t alu_out = arm_read_register(p, get_rn(ins)) & shifter_operand;
		uint8_t N_Flag = get_bit(alu_out,31);
		uint8_t Z_Flag = alu_out == 0 ? 1 : 0;
		uint8_t C_Flag = shifter_carry_out;
		uint8_t V_Flag = get_bit(arm_read_cpsr(p), V);	//unaffected
		update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
	}
	return 1;
}

int ins_TEQ(arm_core p, uint32_t ins){
	uint32_t shifter_operand = 0;
	uint8_t shifter_carry_out = 0;

	if ( condition_passed(p, ins) ){ 
		get_shifter_operand_carry_out(p, ins, &shifter_operand, &shifter_carry_out);
		uint32_t alu_out = arm_read_register(p, get_rn(ins)) ^ shifter_operand;
		uint8_t N_Flag = get_bit(alu_out,31);
		uint8_t Z_Flag = alu_out == 0 ? 1 : 0;
		uint8_t C_Flag = shifter_carry_out;
		uint8_t V_Flag = get_bit(arm_read_cpsr(p), V);	//unaffected
		update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
	}
	return 1;
}

int ins_CMP(arm_core p, uint32_t ins){
	uint32_t shifter_operand = 0;
	uint8_t shifter_carry_out = 0;

	if ( condition_passed(p, ins)){ 
		get_shifter_operand_carry_out(p, ins, &shifter_operand, &shifter_carry_out);
		uint32_t alu_out = arm_read_register(p, get_rn(ins)) - shifter_operand;
		uint8_t N_Flag = get_bit(alu_out,31);
		uint8_t Z_Flag = alu_out == 0 ? 1 : 0;
		uint8_t C_Flag = carry_from(arm_read_register(p, get_rn(ins)), shifter_operand, 1);
		uint8_t V_Flag = overflow_from(arm_read_register(p, get_rn(ins)), shifter_operand, 0);
		update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
	}
	return 1;
}

int ins_CMN(arm_core p, uint32_t ins){
	uint32_t shifter_operand = 0;
	uint8_t shifter_carry_out = 0;

	if ( condition_passed(p, ins)){ 
		get_shifter_operand_carry_out(p, ins, &shifter_operand, &shifter_carry_out);
		uint32_t alu_out = arm_read_register(p, get_rn(ins)) + shifter_operand;
		uint8_t N_Flag = get_bit(alu_out,31);
		uint8_t Z_Flag = alu_out == 0 ? 1 : 0;
		uint8_t C_Flag = carry_from(arm_read_register(p, get_rn(ins)), shifter_operand, 1);
		uint8_t V_Flag = overflow_from(arm_read_register(p, get_rn(ins)), shifter_operand,1);
		update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
	}
	return 1;
}

int ins_ORR(arm_core p, uint32_t ins){
	uint32_t shifter_operand = 0;
	uint8_t shifter_carry_out = 0;

	if ( condition_passed(p, ins)){
		get_shifter_operand_carry_out(p, ins, &shifter_operand, &shifter_carry_out);
		int32_t valeur = arm_read_register(p, get_rn(ins)) | shifter_operand ;
		arm_write_register(p, get_rd(ins), valeur);
		if (bit_S(ins) == 1 && get_rd(ins) == 15){
			if (arm_current_mode_has_spsr(p))
				arm_write_cpsr(p, arm_read_spsr(p));
			else{
				return 0; // pas une instruction data-processing
			}
		} 
		else if (bit_S(ins) == 1){
			uint8_t N_Flag = get_bit(arm_read_register(p, get_rd(ins)),31);
			uint8_t Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
			uint8_t C_Flag = shifter_carry_out;
			uint8_t V_Flag = get_bit(arm_read_cpsr(p), V);	//unaffected
			update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
		}
	} 
	return 1;
}

int ins_MOV(arm_core p, uint32_t ins){
	uint32_t shifter_operand = 0;
	uint8_t shifter_carry_out = 0;

	if ( condition_passed(p, ins)){
		get_shifter_operand_carry_out(p, ins, &shifter_operand, &shifter_carry_out);
		arm_write_register(p, get_rd(ins), shifter_operand);
		if (bit_S(ins) == 1 && get_rd(ins) == 15){
			if (arm_current_mode_has_spsr(p))
				arm_write_cpsr(p, arm_read_spsr(p));
			else{
				return 0; // pas une instruction data-processing
			}
		} 
		else if (bit_S(ins) == 1){
			uint8_t N_Flag = get_bit(arm_read_register(p, get_rd(ins)),31);
			uint8_t Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
			uint8_t C_Flag = shifter_carry_out;
			uint8_t V_Flag = get_bit(arm_read_cpsr(p), V);	//unaffected
			update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
		}
	} 
	return 1;
}

int ins_BIC(arm_core p, uint32_t ins){
	uint32_t shifter_operand = 0;
	uint8_t shifter_carry_out = 0;

	if (condition_passed(p, ins)){
		get_shifter_operand_carry_out(p, ins, &shifter_operand, &shifter_carry_out);
		int32_t valeur = arm_read_register(p, get_rn(ins)) & ~shifter_operand;
		arm_write_register(p, get_rd(ins), valeur);
		if (bit_S(ins) == 1 && get_rd(ins) == 15){
			if (arm_current_mode_has_spsr(p))
				arm_write_cpsr(p, arm_read_spsr(p));
			else{
				return 0; // pas une instruction data-processing
			}
		} 
		else if (bit_S(ins) == 1){
			uint8_t N_Flag = get_bit(arm_read_register(p, get_rd(ins)),31);
			uint8_t Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
			uint8_t C_Flag = shifter_carry_out;
			uint8_t V_Flag = get_bit(arm_read_cpsr(p), V);	//unaffected
			update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
		}
	} 
	return 1;
}

int ins_MVN(arm_core p, uint32_t ins){
	uint32_t shifter_operand = 0;
	uint8_t shifter_carry_out = 0;

	if ( condition_passed(p, ins)){
		get_shifter_operand_carry_out(p, ins, &shifter_operand, &shifter_carry_out);
		arm_write_register(p, get_rd(ins), ~shifter_operand);
		if (bit_S(ins) == 1 && get_rd(ins) == 15){
			if (arm_current_mode_has_spsr(p))
				arm_write_cpsr(p, arm_read_spsr(p));
			else{
				return 0; // pas une instruction data-processing
			}
		} 
		else if (bit_S(ins) == 1){
			uint8_t N_Flag = get_bit(arm_read_register(p, get_rd(ins)),31);
			uint8_t Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
			uint8_t C_Flag = shifter_carry_out;
			uint8_t V_Flag = get_bit(arm_read_cpsr(p), V);	//unaffected
			update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
		}
	} 
	return 1;
}

/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins) {
	uint8_t opcode = get_opcode(ins);
	switch (opcode){
	case 0 : return ins_AND(p, ins); break;
	case 1 : return ins_EOR(p, ins); break;
	case 2 : return ins_SUB(p, ins); break;
	case 3 : return ins_RSB(p, ins); break;
	case 4 : return ins_ADD(p, ins); break;
	case 5 : return ins_ADC(p, ins); break;
	case 6 : return ins_SBC(p, ins); break;
	case 7 : return ins_RSC(p, ins); break;
	case 8 : return ins_TST(p, ins); break;
	case 9 : return ins_TEQ(p, ins); break;
	case 10: return ins_CMP(p, ins); break;
	case 11: return ins_CMN(p, ins); break;
	case 12: return ins_ORR(p, ins); break;
	case 13: return ins_MOV(p, ins); break;
	case 14: return ins_BIC(p, ins); break;
	case 15: return ins_MVN(p, ins); break;
	default: return UNDEFINED_INSTRUCTION; break;
	}
}

int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {
	if((get_bits(ins, 27, 26) == 0) && (get_bits(ins, 24, 23) == 2) && (get_bits(ins, 21, 20) == 2)){
		if (condition_passed(p, ins)){
			uint32_t operand;
			uint32_t mask;
			if(get_bit(ins, 25) == 1){
				uint32_t huit_bit_immediate = get_bits(ins, 7, 0);
				uint8_t rotate_imm = get_bits(ins, 11, 8);
				operand = ror(huit_bit_immediate, rotate_imm*2);
			}else{
				uint8_t bit_3_0 = get_bits(ins, 3, 0);
				operand = arm_read_register(p, bit_3_0);
			}
			if ((operand & UnallocMask ) != 0)   
				return 0;              // UNPREDICTABLE
			uint32_t byte_mask = (get_bit(ins, 16) ? 0x000000FF : 0x00000000) | \
								 (get_bit(ins, 17) ? 0x0000FF00 : 0x00000000) | \
								 (get_bit(ins, 18) ? 0x00FF0000 : 0x00000000) | \
								 (get_bit(ins, 19) ? 0xFF000000 : 0x00000000);
			if(get_bit(ins, 22) == 0){ // R == 0
				if(arm_in_a_privileged_mode(p)){                  // InAPrivilegedMode()
					if((operand & StateMask) != 0){
						return 0;      // UNPREDICTABLE
					}else{
						mask = byte_mask & (UserMask | PrivMask);
					}									
 				}else{
					mask = byte_mask & UserMask;
				}
				uint32_t cpsr_value = (arm_read_cpsr(p) & ~mask) | (operand & mask);				
				arm_write_cpsr(p, cpsr_value);

			}else{ // R == 1
				if(arm_current_mode_has_spsr(p)){
					mask = byte_mask & (UserMask | PrivMask | StateMask);
					uint32_t spsr_value = (arm_read_spsr(p) & ~mask) | (operand & mask);				
					arm_write_spsr(p, spsr_value);
				}else{
					return 0;
				}				
			}			
		}
	}
    return UNDEFINED_INSTRUCTION;
}