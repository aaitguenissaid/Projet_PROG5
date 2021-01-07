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

int arm_load_store(arm_core p, uint32_t ins);
int arm_load_store_multiple(arm_core p, uint32_t ins);
int arm_coprocessor_load_store(arm_core p, uint32_t ins);

/*
   get_index
   description : obtenir le résultat de command shfit
   parametres : instruction ins.
   valeur de retour : index.
   effets de bord : 
*/
uint32_t get_index(uint32_t ins);


/*
   operation_1
   description : modifier l'adresse selon le U
   parametres : U, poiteur de adresse, deux valeur qu'on doit faire l'opération.
   valeur de retour : rien
   effets de bord : adresse a été modifié
*/
void operation_1(int u, uint32_t *adr, uint32_t op_gauche, uint32_t op_droit);



/*
   condtionPassFonc
   description : modifier le contenue de registre Rn = adresse
   parametres : arm core p, instruction ins, poiteur de adresse.
   valeur de retour : rien
   effets de bord : Rn a été modifié
*/
void condtionPassFonc(arm_core p, uint32_t ins, uint32_t ins *adr);

/*
   condtionPassFonc_modifieRn
   description : modifier le contenue de registre Rn = Rn +/- op_droit
   parametres : arm core p, instruction ins, op_droit.
   valeur de retour : rien
   effets de bord : Rn a été modifié
*/
void condtionPassFonc_modifieRn(arm_core p, uint32_t ins, uint32_t op_droit)

/*
   get_adres
   description : décoder l'instruction pour LDR|STR{<cond>}{B}{T} 
   parametres : arm p et instruction ins.
   valeur de retour : adresse.
   effets de bord : si la condition satisfait, on écrit valeur dans le registre Rn.
*/
uint32_t get_adres(arm_core p,uint32_t ins);

/*
   get_offset
   description : otenir l'offet 
   parametres : instruction ins.
   valeur de retour : offset.
   effets de bord : 
*/
uint32_t get_offset(uint32_t ins)

/*
   get_adres_h
   description : décoder l'instruction pour LDR|STR{<cond>}H|SH|SB|D 
   parametres : arm p et instruction ins.
   valeur de retour : adresse.
   effets de bord : si la condition satisfait, on écrit valeur dans le registre Rn.
*/
uint32_t get_adres_h(arm_core p,uint32_t ins);

#endif
