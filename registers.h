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
#ifndef __REGISTERS_H__
#define __REGISTERS_H__
#include <stdint.h>


typedef struct registers_data *registers;


/*
   registers_create
   description : allocation des registres.
   parametres :
   valeur de retour : structure de registres , NULL sinon.
   effets de bord : alloue l'espace les registres.
*/
registers registers_create();

/*
   registers_destroy
   description : destruction des registres.
   parametres : registres r.
   valeur de retour :
   effets de bord : les registres sont detruit.
*/
void registers_destroy(registers r);


/*
   get_mode
   description : renvoies le mode M[4,0] de CPSR.
   parametres : registres r.
   valeur de retour : le mode du processeur.
   effets de bord :
*/
uint8_t get_mode(registers r);

/*
   current_mode_has_spsr
   description : verifie si le mode actuel utilise SPSR.
   parametres : registres r.
   valeur de retour : 1 si on a SPSR, 0 sinon.
   effets de bord :
*/
int current_mode_has_spsr(registers r);

/*
   in_a_privileged_mode
   description : verifie si le mode est privilégié.
   parametres : registres r.
   valeur de retour : renvoies 1 si le mode est privilégier, 0 sinon.
   effets de bord :
*/
int in_a_privileged_mode(registers r);


/*
   read_register
   description : lecture d'un registre.
   parametres : registres r et l'adresse du registre.
   valeur de retour : la valeur du registre.
   effets de bord :
*/
uint32_t read_register(registers r, uint8_t reg);

/*
   read_usr_register
   description : lecture du registre reg_usr.
   parametres : registres r et l'adresse du registre.
   valeur de retour : la valeur du registre user.
   effets de bord :
*/
uint32_t read_usr_register(registers r, uint8_t reg);

/*
   read_cpsr
   description : lecture du registre CPSR.
   parametres : registres r et l'adresse du registre.
   valeur de retour : la valeur du registre CPSR.
   effets de bord :
*/
uint32_t read_cpsr(registers r);

/*
   read_spsr
   description : lecture du registre SPSR.
   parametres : registres r et l'adresse du registre.
   valeur de retour : la valeur du registre SPSR.
   effets de bord :
*/
uint32_t read_spsr(registers r);

/*
   write_register
   description : ecrit la valeur value dans le registre reg.
   parametres : registers r, l'adresse reg, la valeur value.
   valeur de retour :
   effets de bord : modifie la valeur du registre.
*/
void write_register(registers r, uint8_t reg, uint32_t value);

/*
   write_register
   description : ecrit la valeur value dans le registre reg_usr
   parametres : registers r, l'adresse reg, la valeur value.
   valeur de retour :
   effets de bord : modifie la valeur du registre user.
*/
void write_usr_register(registers r, uint8_t reg, uint32_t value);

/*
   write_register
   description : ecrit la valeur value dans le registre CPSR.
   parametres : registers r, la valeur value.
   valeur de retour :
   effets de bord : modifie la valeur du registre CPSR.
*/
void write_cpsr(registers r, uint32_t value);

/*
   write_register
   description : ecrit la valeur value dans le registre SPSR.
   parametres : registers r, la valeur value.
   valeur de retour :
   effets de bord : modifie la valeur du registre SPSR.
*/
void write_spsr(registers r, uint32_t value);

#endif
