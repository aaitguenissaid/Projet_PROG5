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
#ifndef __ARM_BRANCH_OTHER_H__
#define __ARM_BRANCH_OTHER_H__
#include <stdint.h>
#include "arm_core.h"

/*
   arm_branch
   description : effectue le branchement d�crit par l'instruction ins dans le coeur p.
		 Dans la documentation ARMv5, regarder :
			- A4.1.5 - B, BL (page A4-10)
			- A4.1.8 - BLX(1) et A4.1.9 - BLX(2) (pages A4-16 et A4-18)
			- A4.1.10 - BX (page A4-20)
   parametres : Le coeur arm_core appel� p, l'instruction sur 32 bits.
   valeur de retour : //TODO si l'instruction a bien �t� ex�cut�e, UNDEFINED_INSTRUCTION si elle n'a pas �t� reconnue. (cf arm_constants.h pour les valeurs)
   effets de bord : p->registers est modifi� (peut modifier PC, CPSR, SPSR, LR et des registres g�n�raux)
*/
int arm_branch(arm_core p, uint32_t ins);

/*
   arm_coprocessor_others_swi
   description : ex�cute les instructions SWI et autres.
		 Dans la documentation ARMv5, regarder :
			- A3.14.1 - Instruction encodings (page A3-29)
   parametres : Le coeur arm_core appel� p, l'instruction sur 32 bits.
   valeur de retour : SOFTWARE_INTERRUPT si une instruction SWI a �t� ex�cut�e, //TODO si une autre instruction a �t� ex�cut�e, UNDEFINED_INSTRUCTION sinon. (cf arm_constants.h pour les valeurs)
   effets de bord :
*/
int arm_coprocessor_others_swi(arm_core p, uint32_t ins);

/*
   arm_miscellaneous
   description : ex�cute des instructions ins diverses dans le coeur p.
		 Dans la documentation ARMv5, regarder :
		 	- A3.8 - Miscellaneous arithmetic instructions (page A3-17)
		 	- A3.9 - Other miscellaneous instructions (page A3-18)
   parametres : Le coeur arm_core appel� p, l'instruction sur 32 bits.
   valeur de retour : //TODO si l'instruction a bien �t� ex�cut�e, UNDEFINED_INSTRUCTION si elle n'a pas �t� reconnue. (cf arm_constants.h pour les valeurs)
   effets de bord : Modification du coeur p (lecture / �criture de registres)
*/
int arm_miscellaneous(arm_core p, uint32_t ins); //miscellaneous = divers

#endif
