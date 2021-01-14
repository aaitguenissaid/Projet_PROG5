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
#include "arm_instruction.h"
#include "arm_exception.h"
#include "arm_data_processing.h"
#include "arm_load_store.h"
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"

/*
La fonction arm_execute_instruction est la toute première partie de la simulation
d’un cycle d’exécution d’une instruction dans le processeur.
cette fonction devra contenir la partie commune à l’exécution de
toute instruction ARM :
DONE  la lecture de la prochaine instruction,
DONE  l’incrémentation du compteur ordinal
DONE  l’interprétation du champ d’exécution conditionnelle.

DONE  Ensuite, cette fonction devra sélectionner la classe d’instructions
      concernée par l’instruction courante et
      appeler la fonction appropriée, chargée de la suite du décodage.

*/

int condition_passed(arm_core p, uint32_t ins) {
   uint8_t cond = get_bits(ins, 31, 28);
   uint32_t cpsr = arm_read_cpsr(p);
   int n = get_bit(cpsr, 31);
   int z = get_bit(cpsr, 30);
   int c = get_bit(cpsr, 29);
   int v = get_bit(cpsr, 28);
   switch (cond) {
   case 0x0:
      return z == 1;
   case 0x1:
      return z == 0;
   case 0x2:
      return c == 1;
   case 0x3:
      return c == 0;
   case 0x4:
      return n == 1;
   case 0x5:
      return n == 0;
   case 0x6:
      return v == 1;
   case 0x7:
      return v == 0;
   case 0x8:
      return c == 1 && z == 0;
   case 0x9:
      return c == 0 || z == 1;
   case 0xA:
      return n == v;
   case 0xB:
      return n != v;
   case 0xC:
      return z == 0 && n == v;
   case 0xD:
      return z == 1 || n != v;
   case 0xF:
      return 0xF;
   default:
      return 1;
   }
}

static int arm_execute_instruction(arm_core p) {
   uint32_t ins;
   int exception = arm_fetch(p, & ins);
   if (exception != 0)
      return 1;

   if (!condition_passed(p, ins))
      return 1;

   uint8_t opcode = get_bits(ins, 27, 25);
   switch (opcode) {
   case 0x0:
      if (get_bit(ins, 4) == 0) {
         if (get_bits(ins, 24, 23) == 2 && get_bit(ins, 20) == 0)
            return arm_miscellaneous(p, ins);
         else
            return arm_data_processing(p, ins);
      } else {
         if (get_bit(ins, 7) == 0) {
            if (get_bits(ins, 24, 23) == 2 && get_bit(ins, 20) == 1)
               return arm_miscellaneous(p, ins);
            else
               return arm_data_processing(p, ins);
         } else {
            return arm_load_store(p, ins);
         }
      }
      break;
   case 0x1:
      return arm_data_processing(p, ins);

   case 0x2:
      return arm_load_store(p, ins);

   case 0x3:
      if (get_bit(ins, 4) == 0) {
         return arm_load_store(p, ins);
      } else {
         if (get_bits(ins, 24, 20) == 0x1F && get_bits(ins, 7, 5) == 0x07) {
            return UNDEFINED_INSTRUCTION;
         } else {
            return UNDEFINED_INSTRUCTION;
         }
      }

    case 0x4:
       return arm_load_store_multiple(p, ins);

    case 0x5:
       return arm_branch(p, ins);

    case 0x6:
       return arm_coprocessor_load_store(p, ins);

    case 0x7:
       if (get_bit(ins, 24) == 0) {
          if (get_bit(ins, 4) == 0) {
             return arm_data_processing(p, ins);
          } else {
             return UNDEFINED_INSTRUCTION;
          }
       } else {
          return arm_coprocessor_others_swi(p, ins);
       }

    default:
        return UNDEFINED_INSTRUCTION;
   }
}

int arm_step(arm_core p) {
   int result;

   result = arm_execute_instruction(p);
   if (result)
      arm_exception(p, result);
   return result;
}
