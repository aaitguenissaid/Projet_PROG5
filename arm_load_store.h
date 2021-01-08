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
#ifndef __ARM_LOAD_STORE_H__
#define __ARM_LOAD_STORE_H__
#include <stdint.h>
#include "arm_core.h"
#define CP15_reg1_Ubit 1

/*
   get_index 
   description : obtenir le résultat de command shfit
   parametres : arm core p, instruction ins.
   valeur de retour : index.
   effets de bord : 
*/
uint32_t get_index(arm_core p, uint32_t ins);

/*
   add_substract
   description : modifier l'adresse selon le U
   parametres : U, poiteur de adresse, deux valeur qu'on doit faire l'opération.
   valeur de retour : rien
   effets de bord : adresse a été modifié
*/
void add_substract(int u, uint32_t *adr, uint32_t op_gauche, uint32_t op_droit);

/*
   condtion_pass_function
   description : modifier le contenue de registre Rn = adresse
   parametres : arm core p, instruction ins, poiteur de adresse.
   valeur de retour : rien
   effets de bord : Rn a été modifié
*/
void condtion_pass_function(arm_core p, uint32_t ins, uint32_t *adr);

/*
   condtion_pass_modify
   description : modifier le contenue de registre Rn = Rn +/- op_droit
   parametres : arm core p, instruction ins, op_droit.
   valeur de retour : rien
   effets de bord : Rn a été modifié
*/
void condtion_pass_modify(arm_core p, uint32_t ins, uint32_t op_droit);

/*
   get_address
   description : décoder l'addresse de l'instruction pour LDR|STR{<cond>}{B}{T} 
   parametres : arm p et instruction ins.
   valeur de retour : adresse.
   effets de bord : si la condition satisfait, on écrit valeur dans le registre Rn.
*/
uint32_t get_address(arm_core p, uint32_t ins);

/*
   get_offset
   description : otenir l'offet 
   parametres : instruction ins.
   valeur de retour : adresse.
   effets de bord : 
*/
uint32_t get_offset(uint32_t ins);

/*
   get_address_h
   description : décoder l'addresse de l'instruction pour LDR|STR{<cond>}H|SH|SB|D 
   parametres : arm p et instruction ins.
   valeur de retour : adresse.
   effets de bord : si la condition satisfait, on écrit valeur dans le registre Rn.
*/
uint32_t get_address_h(arm_core p,uint32_t ins);

/*
   get_func
   description : décoder l'instruction 
   parametres : instruction ins.
   valeur de retour : LDR, STR, LDRB, STRB, LDRH, STRH, LDM, STM.
   effets de bord : 
*/
name_of_function get_func(uint32_t ins);

/*
   condition_passed
   description : tester le condition de instruction satisfait ou pas 
   parametres : arm p et instruction ins.
   valeur de retour : 0 pas satisfait et 1 satisfait.
   effets de bord : 
*/
int condition_passed(arm_core p, uint32_t ins);

/*
   number_of_set_bits
   description : obtenir nombre de set_bits dans register_list
   parametres : instruction ins.
   valeur de retour : entier entre 0 et 16.
   effets de bord : 
*/
uint32_t number_of_set_bits(uint32_t ins);

/*
   condtion_pass_modify_add
   description : fontion en utilisant addition pour modifier le contenu de registre Rn
   parametres : arm p et instruction ins.
   valeur de retour : 
   effets de bord : Rn a été modifié
*/
void condtion_pass_modify_add(arm_core p, uint32_t ins, uint32_t op_droit);

/*
   condtion_pass_modify_subtract
   description : fontion en utilisant sustraction pour modifier le contenu de registre Rn
   parametres : arm p et instruction ins.
   valeur de retour : 
   effets de bord : Rn a été modifié
*/
void condtion_pass_modify_subtract(arm_core p, uint32_t ins, uint32_t op_droit);

/*
   get_start_end_address
   description : obtenir start_address et end_address pour LDM ou STM
   parametres : arm p et instruction ins, deux poiteurs start_address et end_address
   valeur de retour : 
   effets de bord : start_address et end_address ont été modifié
*/
void get_start_end_address(arm_core p, uint32_t ins, uint32_t *start_address, uint32_t *end_address);

/*
   set_t_bit
   description : set t bit le valeur x dans cpsr
   parametres : arm p et valeur x
   valeur de retour : 
   effets de bord : t bit a été modifié
*/
void set_t_bit(arm_core p,uint8_t x);

/*
   get_address_from_name
   description : obtenir l'addresse à partir de nom de l'instruction
   parametres : arm p et instruction ins
   valeur de retour : une addresse
   effets de bord : 
*/
uint32_t get_address_from_name(arm_core p, uint32_t ins, name_of_function name);

/*
   rotate_right
   description : rotation de valeur à droit 
   parametres : arm p et valeur x
   valeur de retour : une valeur a été déplacée
   effets de bord : 
*/
uint32_t rotate_right(uint32_t data, int x);


/*
   arm_load_store
   description : traite les instructions LDR, STR , LDRB ,STRB ,LDRH ,STRH
   parametres : arm core p, instruction ins.
   valeur de retour : ???
   effets de bord : applique les instructions de changement appropriées en mémoire et / ou change les registres
*/
int arm_load_store(arm_core p, uint32_t ins);
/*
   arm_load_store_multiple
   description : traite les instructions LDM , STM
   parametres : arm core p, instruction ins.
   valeur de retour : ???
   effets de bord : applique les instructions de changement appropriées en mémoire et / ou change les registres
*/

int arm_load_store_multiple(arm_core p, uint32_t ins);
/*
   arm_coprocessor_load_store
   description : recupere une instruction et envoie l'instruction pour traitement 
   parametres : arm core p, instruction ins.
   valeur de retour : ???
   effets de bord : 
*/
int arm_coprocessor_load_store(arm_core p, uint32_t ins);

















/*
   set_t_bit
   description :  mettre en place bit T
   parametres : arm p et x bit a deposer.
   valeur de retour : rien.
   effets de bord : Registr CPRS modifier.
*/
void set_t_bit(arm_core p,uint8_t x);
/*
   rotate_right   ???
   description :  tourner valeur a distance x
   parametres : valeur data et x distance à tourner.
   valeur de retour : valer tourner .
   effets de bord : 
*/
uint32_t rotate_right(uint32_t data, int x);
/*
   get_address_from_name
   description : décoder l'instruction pour LDR|STR{<cond>}H|SH|SB|D et LDR|STR{<cond>}{B}{T}
   parametres : arm p  , instruction ins et nom de function .
   valeur de retour : adresse.
   effets de bord : si la condition satisfait, on écrit valeur dans le registre Rn.
*/
uint32_t get_address_from_name(arm_core p,uint32_t ins,name_of_function name);
#endif