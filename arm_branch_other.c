/*
Armator - simulateur de jeu d'instruction ARMv5T ï¿½ but pï¿½dagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique Gï¿½nï¿½rale GNU publiï¿½e par la Free Software
Foundation (version 2 ou bien toute autre version ultï¿½rieure choisie par vous).

Ce programme est distribuï¿½ car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but spï¿½cifique. Reportez-vous ï¿½ la
Licence Publique Gï¿½nï¿½rale GNU pour plus de dï¿½tails.

Vous devez avoir reï¿½u une copie de la Licence Publique Gï¿½nï¿½rale GNU en mï¿½me
temps que ce programme ; si ce n'est pas le cas, ï¿½crivez ï¿½ la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
ï¿½tats-Unis.

Contact: Guillaume.Huard@imag.fr
	 Bï¿½timent IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'Hï¿½res
*/
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"
#include <debug.h>
#include <stdlib.h>

#define SUCCESS 0

static uint32_t sign_extend_30(int32_t value);
static void Link(arm_core p);
static void Exchange(arm_core p, uint8_t val);
static void Branch(arm_core p, uint32_t branch_addr);

int arm_branch(arm_core p, uint32_t ins) {
	uint32_t branch_addr = 0x0;
	int defined = 0;
	if(get_bits(ins, 27, 25) == 0x5) { //B, BL ou BLX
		defined = 1;
		uint32_t pc_value = arm_read_register(p, PC);
		uint32_t signed_immed_24 = get_bits(ins, 23, 0);
		branch_addr = pc_value + (sign_extend_30(signed_immed_24) << 2);
		if(get_bits(ins,31,28) == 0xF || get_bit(ins,24)) {
			Link(p);
		}
		if(get_bits(ins,31,28) == 0xF) {
			Exchange(p, 1);
			branch_addr += (get_bit(ins,24) << 1); //Adding H bit
		}
	} else if(get_bits(ins, 27, 6) == 0x04BFFC) { //BLX, BX, BXJ
		uint8_t Rm = get_bits(ins, 3, 0);
		uint8_t br = get_bits(ins, 5, 4);
		defined = (br!=0) ? 1 : 0;
		uint32_t Rm_value = arm_read_register(p, Rm);
		branch_addr = Rm_value & 0xFFFFFFFE;
		if(br==3 || br==1 || br==2 ) {
			//To simplify, we consider BXJ always exchanges the same way
			Exchange(p, get_bit(Rm_value, 0));
		}
		if(br==3) {
			Link(p);
		}
	}
	if(defined) {
		Branch(p, branch_addr);
		return SUCCESS;
	}
	return UNDEFINED_INSTRUCTION;
}

int arm_coprocessor_others_swi(arm_core p, uint32_t ins) {
	if (get_bits(ins, 27, 24) == 0xF) { //Condition initiale : get_bit(ins, 24)
        	/* Here we implement the end of the simulation as swi 0x123456 */
		if ((ins & 0xFFFFFF) == 0x123456)
			exit(SOFTWARE_INTERRUPT);
		else {
			//ENTERING SUPERVISOR MODE
			uint32_t cpsr_value = arm_read_register(p, CPSR);
			uint32_t cpsr_new_value = set_bits(cpsr_value, 4, 0, 0b10011);
			arm_write_register(p, CPSR, cpsr_new_value);
			//LINKING
			Link(p);
			//SAVING CPSR
			arm_write_register(p, SPSR, cpsr_value);
			//CHANGING OTHER CPSR FIELDS
			cpsr_new_value = clr_bit(cpsr_new_value, 5);
			cpsr_new_value = set_bit(cpsr_new_value, 7);
			//CP15 registers are not enabled yet, TODO :
			//cpsr_new_value = (get_bit(CP15_reg1, 25)) ? set_bit(cpsr_new_value, 9) : clr_bit(cpsr_new_value, 9);
			arm_write_register(p, CPSR, cpsr_new_value);
			//CHANGING PC
			//I consider high vectors are not configured
			uint32_t br_addr = 0x00000008; //Otherwise, we'd do : uint32_t br_addr = (get_bit(CP15_reg1, 13)) ? 0xFFFF0008 : 0x00000008;
			Branch(p, br_addr);
		}
		return SOFTWARE_INTERRUPT;
	}
	return UNDEFINED_INSTRUCTION;
}

int arm_miscellaneous(arm_core p, uint32_t ins) {
	if(get_bits(ins, 27, 20) == 0x16 && get_bits(ins, 7, 4) == 0x1) {
		//Instruction Count Leading Zeros CLZ
		uint8_t Rd = get_bits(ins, 15, 12);
		uint8_t Rm = get_bits(ins, 03, 00);
		if(Rd==15 || Rm==15)
			return UNDEFINED_INSTRUCTION;
		uint8_t val = arm_read_register(p,Rm), res;
		int cpt=32;
		while(get_bit(val,cpt)==0 && cpt>=0) {
			cpt--;
		}
		if(cpt == -1) {res = 0;}
		else {res = 32 - cpt;}
		arm_write_register(p,Rd,res);
		return SUCCESS;
	}
	return UNDEFINED_INSTRUCTION;
}



/* INTERN FUNCTIONS */
static uint32_t sign_extend_30(int32_t value) {
	return asr(value << 8, 8);
}

/*Sauvegarde de PC dans LR*/
static void Link(arm_core p){
	arm_write_register(p, LR, arm_read_register(p, PC) - 4);
}

/*CPSR_T_Bit (CPSR[5]) = val*/
static void Exchange(arm_core p, uint8_t val) { 
	uint32_t cpsr_value;
	if(val) cpsr_value = set_bit(arm_read_register(p, CPSR),5);
	else cpsr_value = clr_bit(arm_read_register(p, CPSR),5);
	arm_write_register(p, CPSR, cpsr_value);
}

/*Branchement à l'adresse branch_addr*/
static void Branch(arm_core p, uint32_t branch_addr) {
	arm_write_register(p, PC, branch_addr);
}




