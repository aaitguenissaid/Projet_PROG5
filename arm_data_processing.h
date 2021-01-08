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
#ifndef __ARM_DATA_PROCESSING_H__
#define __ARM_DATA_PROCESSING_H__
#include <stdint.h>
#include "arm_core.h"

/*
Dans les instructions de data-processing, ins[11:0] est le shift_operand. La valeur de shift_operand est 32 bits.
Il y a 11 addressings modes:
1)Immediate,
2)Register,
3)Logical shift left by immediate, 
4)Logical shift left by register,
5)Logical shift right by immediate,
6)Logical shift right by register,
7)Arithmetic shift right by immediate,
8)Arithmetic shift right by register,
9)Rotate right by immediate,
10)Rotate right by register,
11)Rotate right with extend.
*/

/*
   get_opcode
   description : Obtenir la valeur de opcode dans une instruction.
   parametres : uint32_t ins
   valeur de retour : la valeur de opcode
   effets de bord : 
*/
uint8_t get_opcode(uint32_t ins);

/*
   bit_I
   description : Obtenir le I bit dans une instruction.
   parametres : uint32_t ins
   valeur de retour : le bit ins[25]
   effets de bord : 
*/
uint8_t bit_I(uint32_t ins);

uint8_t bit_4(uint32_t ins);

uint8_t bit_7(uint32_t ins);

uint8_t bit_S(uint32_t ins);

/*
   get_C_flag
   description : Obtenir la valeur du C_Flag dans le registre CPSR.
   parametres : arm_core p
   valeur de retour : la valeur du C_Flag
   effets de bord : 
*/
uint8_t get_C_flag(arm_core p);

/*
   shift_imm
   description : Obtenir la partie shift_imm d'une instruction.
   parametres : uint32_t ins
   valeur de retour : ins[11:7]
   effets de bord : 
*/
uint8_t shift_imm(uint32_t ins);

/*
   shift
   description : Obtenir la partie shift d'une instruction.
   parametres : uint32_t ins
   valeur de retour : ins[6:5]
   effets de bord : 
*/
uint8_t shift(uint32_t ins);

/*
   Rm
   description : Obtenir la valeur Rm d'une instruction.
   parametres : uint32_t ins
   valeur de retour : ins[3:0]
   effets de bord : 
*/
uint8_t get_rm(uint32_t ins);

/*
   Rm_31
   description : Obtenir le 31_bit de la valeur de Rm.
   parametres : arm_core p, uint32_t ins
   valeur de retour : Rm[31]
   effets de bord : 
*/
uint8_t get_rm_31(arm_core p, uint32_t ins);

/*
   Rs
   description : Obtenir la valeur Rs d'une instruction.
   parametres : uint32_t ins
   valeur de retour : ins[11:8]
   effets de bord : 
*/
uint8_t get_rs(uint32_t ins);


/*
   Rs_7_0
   description : Obtenir la partie de 0 à 7 bits de la valeur de Rs.
   parametres : arm_core p, uint32_t ins
   valeur de retour : Rs[7:0]
   effets de bord : 
*/
uint8_t get_rs_7_0(arm_core p, uint32_t ins);

/*
   two_complement
   description : Faire le complement à 2 pour un entier.
   parametres : uint32_t operand
   valeur de retour : un entier non-signé à 32 bits
   effets de bord : 
*/
uint32_t two_complement(uint32_t operand);

/*
   operation
   description : Faire une opération pour deux entiers à 64 bits.
   parametres : uint64_t operand1, uint64_t operand2, int op
                op = 1, l'addition
                op = 2, la soustraction
   valeur de retour : un entier non-signé à 64 bits
   effets de bord : 
*/
uint64_t operation(uint64_t operand1, uint64_t operand2, int op);

/*
   carry_from
   description : vérifier si'il y a Carry pour une opération.
   parametres : uint32_t operand1, uint32_t operand2, int op
                op = 1, l'addition
                op = 2, la soustraction
   valeur de retour : 1 si'il y a Carry, 0 sinon
   effets de bord : 
*/
int carry_from(uint32_t operand1, uint32_t operand2, int op);

/*
   overflow_from
   description : vérifier si'il y a Overflow pour une opération.
   parametres : uint32_t operand1, uint32_t operand2, int op
                op = 1, l'addition
                op = 2, la soustraction
   valeur de retour : 1 si'il y a Overflow, 0 sinon
   effets de bord : 
*/
int overflow_from(uint32_t operand1, uint32_t operand2, int op);

/*
   borrow_from
   description : vérifier si'il y a Borrow pour une soustraction.
                 pour (operand1 - operand2), si operand1 < operand2, il y a Borrow.
                 si operand1 > operand2, il y en n'a pas.
   parametres : uint32_t operand1, uint32_t operand2
   valeur de retour : 1 si'il y a Borrow, 0 sinon
   effets de bord : 
*/
int borrow_from(uint32_t operand1, uint32_t operand2);

/*
   imm_32_bit
   description : Data-processing operands - Immediate. (page A5-6)
   parametres : arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out
   valeur de retour : 
   effets de bord : modifier les valeurs shifter_operand et shifter_carry_out
*/

void imm_32_bit(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out);

/*
   LSL_imm
   description : Data-processing operands - Logical shift left by immediate. (page A5-6)
   parametres : arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out
   valeur de retour : 
   effets de bord : modifier les valeurs shifter_operand et shifter_carry_out
*/

/*
   LSR_imm
   description : Data-processing operands - Immediate. (page A5-6)
   parametres : arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out
   valeur de retour : 
   effets de bord : modifier les valeurs shifter_operand et shifter_carry_out
*/

void lsl_imm(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out);

/*
   LSR_imm
   description : Data-processing operands - Immediate. (page A5-6)
   parametres : arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out
   valeur de retour : 
   effets de bord : modifier les valeurs shifter_operand et shifter_carry_out
*/
void lsr_imm(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out);

/*
   ASR_imm
   description : Data-processing operands - Immediate. (page A5-6)
   parametres : arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out
   valeur de retour : 
   effets de bord : modifier les valeurs shifter_operand et shifter_carry_out
*/
void asr_imm(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out);

/*
   ROR_imm
   description : Data-processing operands - Immediate. (page A5-6)
   parametres : arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out
   valeur de retour : 
   effets de bord : modifier les valeurs shifter_operand et shifter_carry_out
*/
void ror_imm(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out);

/*
   reg_shift
   description : Data-processing operands - Register. (page A5-6)
   parametres : arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out
   valeur de retour : 
   effets de bord : modifier les valeurs shifter_operand et shifter_carry_out
*/
void reg_shift(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out);

/*
   LSL_reg
   description : Data-processing operands - Logical shift left by register. (page A5-6)
   parametres : arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out
   valeur de retour : 
   effets de bord : modifier les valeurs shifter_operand et shifter_carry_out
*/
void lsl_reg(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out);

/*
   LSR_reg
   description : Data-processing operands - Immediate. (page A5-6)
   parametres : arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out
   valeur de retour : 
   effets de bord : modifier les valeurs shifter_operand et shifter_carry_out
*/
void lsr_reg(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out);

/*
   ASR_reg
   description : Data-processing operands - Immediate. (page A5-6)
   parametres : arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out
   valeur de retour : 
   effets de bord : modifier les valeurs shifter_operand et shifter_carry_out
*/
void asr_reg(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out);

/*
   ROR_reg
   description : Data-processing operands - Rotate right by register. (page A5-6)
   parametres : arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out
   valeur de retour : 
   effets de bord : modifier les valeurs shifter_operand et shifter_carry_out
*/
void ror_reg(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out);

/*
   ROR_extend
   description : Data-processing operands - Rotate right with extend. (page A5-17)
   parametres : arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out
   valeur de retour : 
   effets de bord : modifier les valeurs shifter_operand et shifter_carry_out
*/
void ror_extend(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out);

/*
   arm_data_processing_shift
   description : décodage spécialisé pour les instructions  de traitement de données AND, EOR, SUB, RSB, ADD, ADC, SBC, RSC, TST, TEQ, CMP, CMN, ORR, MOV, BIC, MVN
   parametres : arm_core p, uint32_t ins
   valeur de retour : 1 si succès, 0 sinon
   effets de bord : modifier la valeur du registre de distination, modifier les flags dans le CPSR.
*/

/*
   description : Les instructions de data-processing
   parametres : arm_core p, uint32_t ins
   valeur de retour : 
   effets de bord : modifier la valeur du registre de distination, modifier les flags dans le CPSR.
*/
int ins_EOR(arm_core p, uint32_t ins);
int ins_SUB(arm_core p, uint32_t ins);
int ins_RSB(arm_core p, uint32_t ins);
int ins_ADC(arm_core p, uint32_t ins);
int ins_SBC(arm_core p, uint32_t ins);
int ins_RSC(arm_core p, uint32_t ins);
int ins_TST(arm_core p, uint32_t ins);
int ins_TEQ(arm_core p, uint32_t ins);
int ins_CMP(arm_core p, uint32_t ins);
int ins_CMN(arm_core p, uint32_t ins);
int ins_ORR(arm_core p, uint32_t ins);
int ins_MOV(arm_core p, uint32_t ins);
int ins_BIC(arm_core p, uint32_t ins);
int ins_MVN(arm_core p, uint32_t ins);

int arm_data_processing_shift(arm_core p, uint32_t ins);

/*
   arm_data_processing_immediate_msr
   description : décodage spécialisé pour l'instruction diverse : MSR
   parametres : arm_core p, uint32_t ins
   valeur de retour : 1 si succès, 0 sinon
   effets de bord : modifier la valeur du registre de distination, modifier les flags dans le CPSR.
*/
int arm_data_processing_immediate_msr(arm_core p, uint32_t ins);

#endif
