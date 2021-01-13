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
   bit_S
   description : Obtenir le S bit dans une instruction.
   parametres : uint32_t ins
   valeur de retour : le bit ins[20]
   effets de bord : 
*/
uint8_t bit_S(uint32_t ins);

/*
   get_C_flag
   description : Obtenir la valeur du C_Flag dans le registre CPSR.
   parametres : arm_core p
   valeur de retour : la valeur du C_Flag
   effets de bord : 
*/
uint32_t get_C_flag(arm_core p);

/*
   get_rd
   description : Obtenir la partie Rd d'une instruction.
   parametres : uint32_t ins
   valeur de retour : ins[11:8]
   effets de bord : 
*/
uint8_t get_rd(uint32_t ins);

/*
   get_rn
   description : Obtenir la partie Rn d'une instruction.
   parametres : uint32_t ins
   valeur de retour : ins[11:8]
   effets de bord : 
*/
uint8_t get_rn(uint32_t ins);

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
                op = 2, la soustraction ( l'addition en complément à deux)
   valeur de retour : un entier non-signé à 64 bits
   effets de bord : 
*/
uint64_t operation(uint64_t operand1, uint64_t operand2, int op);

/*
   carry_from
   description : vérifier si'il y a Carry pour une opération.
                 On transfère les deux opérandes à 64 bit.
                 Pour l'addition, si résultat[32] == 1, il y a Carry, sinon il y en n'a pas.
                 Pour la soustraction, si résultat[32] == 0, il y a Carry, sinon il y en n'a pas.
   parametres : uint32_t operand1, uint32_t operand2, int op
                op = 1, l'addition
                op = 2, la soustraction ( l'addition en complément à deux)
   valeur de retour : 1 si'il y a Carry, 0 sinon
   effets de bord : 
*/
int carry_from(uint32_t operand1, uint32_t operand2, int op);

/*
   overflow_from
   description : vérifier si'il y a Overflow pour une opération.
                 si op1[31] != op2[31], pas de overflow
                 si op1[31] == op2[31], et resultat[31] != op1[31], il y a overflow
   parametres : uint32_t operand1, uint32_t operand2, int op
                op = 1, l'addition
                op = 2, la soustraction ( l'addition en complément à deux)
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
   get_operand_carryout
   description : Obtenir shifter_operand et shifter_carry_out selon la addressing mode  
   parametres : arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out
   valeur de retour : 
   effets de bord : modifier les valeurs shifter_operand et shifter_carry_out
*/
void get_operand_carryout(arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out);

/*
   update_flags_cpsr
   description : Faire la mis à jour pour les N,Z,C,V flags dans le registre CPSR.  
   parametres : arm_core p, uint32_t ins, uint32_t *shifter_operand, uint8_t *shifter_carry_out
   valeur de retour : 
   effets de bord : modifier les valeurs shifter_operand et shifter_carry_out
*/
void update_flags_cpsr(arm_core p, int n, int z, int c, int v);

/*
   description : Les instructions de data-processing
   parametres : arm_core p, uint32_t ins
   valeur de retour : 
   effets de bord : modifier la valeur du registre de distination, modifier les flags dans le CPSR.
*/
int ins_AND(arm_core p, uint32_t ins);
int ins_EOR(arm_core p, uint32_t ins);
int ins_SUB(arm_core p, uint32_t ins);
int ins_RSB(arm_core p, uint32_t ins);
int ins_ADD(arm_core p, uint32_t ins);
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

/*
   arm_data_processing
   description : décodage spécialisé pour les instructions  de traitement de données AND, EOR, SUB, RSB, ADD, ADC, SBC, RSC, TST, TEQ, CMP, CMN, ORR, MOV, BIC, MVN
   parametres : arm_core p, uint32_t ins
   valeur de retour : 1 si succès, 0 sinon
   effets de bord : modifier la valeur du registre de distination, modifier les flags dans le CPSR.
*/
int arm_data_processing(arm_core p, uint32_t ins);

/*
   arm_data_processing_immediate_msr
   description : décodage spécialisé pour l'instruction MSR avec l'opérande immédiate
                 MSR{<cond>} CPSR_<fields>, #<immediate>
                 MSR{<cond>} SPSR_<fields>, #<immediate>
   parametres : arm_core p, uint32_t ins
   valeur de retour : 1 si succès, 0 sinon
   effets de bord : modifier la valeur du registre CPSR ou SPSR.
*/
int arm_data_processing_immediate_msr(arm_core p, uint32_t ins);

#endif
