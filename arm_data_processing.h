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
   arm_data_processing_shift
   description : décodage spécialisé pour les instructions de traitement de données.
				Pour shifter_operand, il y a 11 addressings modes:
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
   parametres : arm_core p, uint32_t ins
   valeur de retour : 1 si succès, 0 sinon
   effets de bord : modifier la valeur du registre de distination, modifier les flags dans le CPSR.
*/
int arm_data_processing_shift(arm_core p, uint32_t ins);

/*
   arm_data_processing_immediate_msr
   description : décodage spécialisé pour l'instruction diverse : MSR
   parametres : arm_core p, uint32_t ins
   valeur de retour : 1 si succès, 0 sinon
   effets de bord : modifier la valeur du registre de distination, modifier les flags dans le CPSR.
*/
int arm_data_processing_immediate_msr(arm_core p, uint32_t ins);

/*
   operation
   description : faire l'operation pour les deux opérandes.
   parametres : arm_core p, uint8_t Rn, uint8_t Rd, uint32_t shifter_operand, uint8_t opcode
   valeur de retour :
   effets de bord : stocker la valeur de l'opération dans le registre de distination.
*/
void operation(arm_core p, uint8_t Rn, uint8_t Rd, uint32_t shifter_operand, uint8_t opcode)

#endif
