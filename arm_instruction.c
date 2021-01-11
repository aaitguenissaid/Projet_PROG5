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
	uint8_t cpsr = arm_read_cpsr(p);
	uint8_t n = get_bit(cpsr, 31);
	uint8_t z = get_bit(cpsr, 30);
	uint8_t c = get_bit(cpsr, 29);
	uint8_t v = get_bit(cpsr, 28);
	switch (cond){
		case 0x00:
			return z == 0x01; //0000  EQ       Equal                               Z set
		case 0x01:
			return z == 0x00; //0001  NE       Not equal                           Z clear
		case 0x02:
			return c == 0x01; //0010  CS/HS    Carry set/unsigned higher or same   C set
		case 0x03:
			return c == 0x00; //0011  CC/LO    Carry clear/unsigned lower          C clear
		case 0x04:
			return n == 0x01; //0100  MI       Minus/negative                      N set
		case 0x05:
			return n == 0x00; //0101  PL       Plus/positive or zero               N clear
		case 0x06:
			return v == 0x01; //0110  VS       Overflow                            V set
		case 0x07:
			return v == 0x00; //0111  VC       No overflow                         V clear
		case 0x08:
			return z == 0x00 && c == 0x01; //1000  HI       Unsigned higher                     C set and Z clear
		case 0x09:
			return z == 0x01 || c == 0x00; //1001  LS       Unsigned lower or same              C clear or Z set
		case 0x0A:
			return n == v; //1010  GE       Signed greater than or equal        N set and V set, orN clear and V clear (N == V)
		case 0x0B:
			return n != v; //1011  LT       Signed less than                    N set and V clear, or N clear and V set (N != V)
		case 0x0C:
			return z == 0x00 && n == v; //1100  GT       Signed greater than                 Z clear, and either N set and V set, or N clear and V clear (Z == 0,N == V)
		case 0x0D:
			return z == 0x01 || n != v; //1101  LE       Signed less than or Equal 			 Z set, or N set and V clear, or N clear and V set (Z == 1 or N != V)
		case 0x0E:
			return 1; //1110  AL       Always (unconditional)
		case 0x0F:
			return 0; //1111  -        See Condition code 0b1111
		default:
			return 0;
	}
}

static int arm_execute_instruction(arm_core p) {

    //fetch
    // ri Registre d'instruction (Variable local)
    uint32_t ri;
    int exception = arm_fetch(p, &ri);
    if(exception != 0){
      return PREFETCH_ABORT;
    }
    //decode 27 - 25
     uint8_t opcode = (uint8_t) get_bits(ri, 27, 25);

    if(condition_passed(p, ri)){
      switch(opcode){
        case 0x00:
          if(get_bit(ri, 4)==0) {  // bit_4 = 0
              if(get_bits(ri, 24, 23)==2 && get_bit(ri, 20)==0){ //  Miscellaneous instructions
                return arm_miscellaneous(p, ri);
              } else { //  Data processing immediate shift
                return arm_data_processing_shift(p, ri);
              }
          } else { // bit_4 = 1
              if(get_bit(ri, 7)==0){
                  if(get_bits(ri, 24, 23)==2 && get_bit(ri, 20)==1){  //  Miscellaneous instructions
                    return arm_miscellaneous(p, ri);
                  } else { // Data processing register shift [2]
                    return arm_data_processing_shift(p, ri);
                  }
              } else { // bit_7 = 1 Multiplies: See Figure A3-3
                      //Extra load/stores: See Figure A3-5
                      return arm_coprocessor_load_store(p, ri);
              }
          }
          break;
        case 0x01:
          if(get_bits(ri, 24, 23)==2) {
            if(get_bits(ri, 22, 21)==2) { // Move immediate to status register page 145
  // TODO
              return UNDEFINED_INSTRUCTION;
            } else { // Undefined instruction
              return UNDEFINED_INSTRUCTION;
            }
          } else { // Data processing immediate [2]
            return arm_data_processing_shift(p, ri);
          }
          break;
        // Load/store immediate offset
        case 0x02:
          return arm_load_store(p, ri);
        break;
        case 0x03:
          if(get_bit(ri, 4)==0) {  //  Load/store register offset
            return arm_load_store(p, ri);
          } else {
            if(get_bits(ri, 24, 20)==0x1F && get_bits(ri, 7, 5)==0x07){ // Architecturally undefined
              return UNDEFINED_INSTRUCTION;
            } else { // Media instructions [4]: See Figure A3-2 page 142
// TODO
              return UNDEFINED_INSTRUCTION;
            }
          }
          break;
        //Load/store multiple
        case 0x04:
          return arm_load_store_multiple(p, ri);
        break;
        //Branch and branch with link
        case 0x05:
          return arm_branch(p, ri);
        break;
        //Coprocessor load/store and double register transfers
        case 0x06:
          return arm_coprocessor_load_store(p, ri);
        break;
        case 0x07:
          if(get_bit(ri,24)==0){
            if(get_bit(ri,4)==0){ // Coprocessor data processing
              return arm_data_processing_shift(p, ri);
            } else { // Coprocessor register transfers
              return UNDEFINED_INSTRUCTION;
            }
          } else { // Software interrupt
            return arm_coprocessor_others_swi(p, ri);
          }
        break;
        default:
        return UNDEFINED_INSTRUCTION;
        break;
      }
  }
return 0;
}


int arm_step(arm_core p) {
    int result;

    result = arm_execute_instruction(p);
    if (result)
        arm_exception(p, result);
    return result;
}
