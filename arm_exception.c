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
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_core.h"
#include "util.h"
#include <stdlib.h>

// Not supported below ARMv6, should read as 0
#define CP15_reg1_EEbit 0
// Not implemented
#define VE 0

#define Exception_bit_9 (CP15_reg1_EEbit << 9)

static void change_mode(arm_core p, uint8_t mode);
static void save_cpsr(arm_core p, uint32_t cpsr);
static void update_cpsr(arm_core p, uint32_t cpsr, int8_t chg_6, int8_t chg_8);

void arm_exception(arm_core p, unsigned char exception) {
	uint32_t cpsr;
	switch(exception) {
		case RESET:
			arm_write_cpsr(p, 0x1d3 | Exception_bit_9);
			arm_write_usr_register(p, 15, 0);
			break;
		case UNDEFINED_INSTRUCTION:
			change_mode(p, (uint8_t) 0b11011);
			arm_write_register(p,LR,arm_read_register(p,PC) - 4); //Link
			cpsr = arm_read_cpsr(p);
			save_cpsr(p, cpsr);
			update_cpsr(p, cpsr, -1, -1);
			arm_write_register(p, PC, 0x4);
			break;
		case SOFTWARE_INTERRUPT:
			change_mode(p, (uint8_t) 0b10011);
			arm_write_register(p,LR,arm_read_register(p,PC) - 4); //Link
			cpsr = arm_read_cpsr(p);
			save_cpsr(p, cpsr);
			update_cpsr(p, cpsr, -1, -1);
			arm_write_register(p, PC, 0x8);
			break;
		case PREFETCH_ABORT:
			change_mode(p, (uint8_t) 0b10111);
			arm_write_register(p,LR,arm_read_register(p,PC) - 4); //Link
			cpsr = arm_read_cpsr(p);
			save_cpsr(p, cpsr);
			update_cpsr(p, cpsr, -1, 1);
			arm_write_register(p, PC, 0xC);
			break;
		case DATA_ABORT:
			change_mode(p, (uint8_t) 0b10111);
			arm_write_register(p,LR,arm_read_register(p,PC)); //Link
			cpsr = arm_read_cpsr(p);
			save_cpsr(p, cpsr);
			update_cpsr(p, cpsr, -1, 1);
			arm_write_register(p, PC, 0x10);
			break;
		case INTERRUPT:
			change_mode(p, (uint8_t) 0b10010);
			arm_write_register(p,LR,arm_read_register(p,PC)); //Link
			cpsr = arm_read_cpsr(p);
			save_cpsr(p, cpsr);
			update_cpsr(p, cpsr, -1, 1);
			if(!VE) {
				arm_write_register(p, PC, 0x18);
			}
			break;
		case FAST_INTERRUPT:
			change_mode(p, (uint8_t) 0b10001);
			arm_write_register(p,LR,arm_read_register(p,PC)); //Link
			cpsr = arm_read_cpsr(p);
			save_cpsr(p, cpsr);
			update_cpsr(p, cpsr, 1, 1);
			if(!VE) {
				arm_write_register(p, PC, 0x1C);
			}
			break;
		default:
			break;
	}
}

static void change_mode(arm_core p, uint8_t mode) {
	uint32_t val = arm_read_cpsr(p) & 0xFFFFFFE0; //Retire le mode
	val |= mode; //Ajoute le nouveau mode
	arm_write_cpsr(p,val); //Ecrit dans le registre CPSR
}

static void save_cpsr(arm_core p, uint32_t cpsr) {
	arm_write_spsr(p, cpsr);
}

//chg_i = -1 ==> do not change, chg_i = 0 ==> clear(i), chg_i = 1 ==> set(i)
static void update_cpsr(arm_core p, uint32_t cpsr, int8_t chg_6, int8_t chg_8) {
	cpsr = clr_bit(cpsr, 5); //Execute in ARM state
	cpsr = (chg_6==-1) ? cpsr : ((chg_6) ? set_bit(cpsr,6) : clr_bit(cpsr,6));
	cpsr = set_bit(cpsr, 7); //Disable normal interrupts
	cpsr = (chg_8==-1) ? cpsr : ((chg_8) ? set_bit(cpsr,8) : clr_bit(cpsr,8));
	cpsr = (CP15_reg1_EEbit) ? set_bit(cpsr,9):clr_bit(cpsr, 9);
	arm_write_cpsr(p, cpsr);
}
