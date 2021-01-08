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
      l’interprétation du champ d’exécution conditionnelle.

      Ensuite, cette fonction devra sélectionner la classe d’instructions
      concernée par l’instruction courante et
      appeler la fonction appropriée, chargée de la suite du décodage.

      L’interprétation du champ d’exécution conditionnelle et la détermination
      de la classe de l’instructio courante peuvent être implémentées principalement
      selon deux approches :
      bloc switch ou table depo inteurs de fonctions.

      Les listings (de pseudo-code) donnés en Figure 1 illustrent ces deux techniques
      de décodage, en considérant un champ de 3 bits extrait des bits 15 à 17 d’un mot de 32 bits.
*/
static int arm_execute_instruction(arm_core p) {
    return 0;
    //fetch
    uint32_t *accumulator;
    int interuption = arm_fetch(p, *accumulator);

    // par convention R0 est considéré comme l'accumulator
    arm_write_usr_register(p,0,accumulator);
    //decode 31 - 28
    //je lis opcode de accumulator la variable ou le registre R0 ?
    uint8_t opcode = (uint8_t) get_bits(accumulator, 31, 28);
    switch(opcode){
      case 0x00:  // 0000 AND Logical AND Rd := Rn AND shifter_operand

      break;
      case 0x01:  // 0001 EOR Logical Exclusive OR Rd := Rn EOR shifter_operand

      break;
      case 0x01:  // 0010 SUB Subtract Rd := Rn - shifter_operand

      break;
      case 0x03:  // 0011 RSB Reverse Subtract Rd := shifter_operand - Rn

      break;
      case 0x04:  // 0100 ADD Add Rd := Rn + shifter_operand

      break;
      case 0x05:  // 0101 ADC Add with Carry Rd := Rn + shifter_operand + Carry Flag

      break;
      case 0x06:  // 0110 SBC Subtract with Carry Rd := Rn - shifter_operand - NOT(Carry Flag)

      break;
      case 0x07:  // 0111 RSC Reverse Subtract with Carry Rd := shifter_operand - Rn - NOT(Carry Flag)

      break;
      case 0x08:  // 1000 TST Test Update flags after Rn AND shifter_operand

      break;
      case 0x09:  // 1001 TEQ Test Equivalence Update flags after Rn EOR shifter_operand

      break;
      case 0x0A:  // 1010 CMP Compare Update flags after Rn - shifter_operand

      break;
      case 0x0B:  // 1011 CMN Compare Negated Update flags after Rn + shifter_operand

      break;
      case 0x0C:  // 1100 ORR Logical (inclusive) OR Rd := Rn OR shifter_operand

      break;
      case 0x0D:  // 1101 MOV Move Rd := shifter_operand (no first operand)

      break;
      case 0x0E:  // 1110 BIC Bit Clear Rd := Rn AND NOT(shifter_operand)

      break;
      case 0x0F:  // 1111 MVN Move Not Rd := NOT shifter_operand (no first operand)

      break;
      default: // cas impossible
    }

}


int arm_step(arm_core p) {
    int result;

    result = arm_execute_instruction(p);
    if (result)
        arm_exception(p, result);
    return result;
}
