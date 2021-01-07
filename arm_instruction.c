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
la lecture de la prochaine instruction,
l’incrémentation du compteur ordinal
l’interprétation du champ d’exécution conditionnelle.

Ensuite, cette fonction devra sélectionner la classe d’instructions concernée par l’instruction courante et
appeler la fonction appropriée, chargée de la suite du décodage.

L’interprétation du champ d’exécution conditionnelle et la détermination de la classe de l’instruction
courante peuvent être implémentées principalement selon deux approches :
bloc switch ou table depo inteurs de fonctions.

Les listings (de pseudo-code) donnés en Figure 1 illustrent ces deux techniques
de décodage, en considérant un champ de 3 bits extrait des bits 15 à 17 d’un mot de 32 bits.
*/
static int arm_execute_instruction(arm_core p) {
    return 0;
}

int arm_step(arm_core p) {
    int result;
    //fetch
    //decode
    //execute

    result = arm_execute_instruction(p);
    if (result)
        arm_exception(p, result);
    return result;
}
