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
#ifndef __MEMORY_H__
#define __MEMORY_H__
#include <stdint.h>
#include <sys/types.h>

typedef struct memory_data *memory;

/*
   memory_create
   description : la memoire est crée avec une taille size et un boutisme is_big_endian.
   parametres : la taille et le boutisme.
   valeur de retour : la memoire si succes ou NULL sinon
   effets de bord : alloue de la memoire
*/
memory memory_create(size_t size, int is_big_endian);

/*
   memory_get_size
   description : renvoie la taille de la memoire ou -1 si erreur.
   parametres : on reçoit la memoire.
   valeur de retour : renvoie la taille de la memoire ou -1 si erreur.
   effets de bord : aucun.
*/
size_t memory_get_size(memory mem);

/*
   memory_destroy
   description : libere la memoire.
   parametres : on reçoit la memoir.
   valeur de retour : 0 succes ou -1 failure.
   effets de bord : libere de la memoire.
*/
void memory_destroy(memory mem);

/* All these functions perform a read/write access to a byte/half/word data at
 * address a in mem. The result is respectively taken from or stored to the
 * parameter value. w access is made using the given endianess (be == 1 for a
 * big endian access and be == 0 for a little endian access).
 * The return value indicates a succes (0) or a failure (-1).
 */

 /*
    memory_read_byte
    description : lis un octet (8 bit) de la memoire à l'address donnée. et le stock dans value. la valeur de be n'est pas importante
    parametres : on reçoit la memoir, l'address de lecture et un pointeur pour la valeur lu.
    valeur de retour : 0 succes ou -1 failure.
    effets de bord : la valeur lu en memoire est mise dans value.
 */
int memory_read_byte(memory mem, uint32_t address, uint8_t *value);

/*
   memory_read_half
   description : lis un demi-mot (16 bit) de la memoire à l'address donnée et le stock dans value
   parametres : on reçoit la memoir, l'address de lecture et un pointeur pour la valeur lu.
   valeur de retour : 0 succes ou -1 failure.
   effets de bord : la valeur lu en memoire est mise dans value.
*/
int memory_read_half(memory mem, uint32_t address, uint16_t *value);

/*
   memory_read_word
   description : lis un mot (32 bit) de la memoire à l'address donnée. et le stock dans value
   parametres : on reçoit la memoir, l'address de lecture et un pointeur pour la valeur lu.
   valeur de retour : 0 succes ou -1 failure.
   effets de bord : la valeur lu en memoire est mise dans value.
*/
int memory_read_word(memory mem, uint32_t address, uint32_t *value);

/*
   memory_write_byte
   description : ecrit l'octet value (8 bit) dans la memoire mem à l'address donnée.
   parametres : on reçoit la memoir, l'address de lecture et la valeur à ecrire.
   valeur de retour : 0 succes ou -1 failure.
   effets de bord : la memoire est modifiée.
*/
int memory_write_byte(memory mem, uint32_t address, uint8_t value);

/*
   memory_write_half
   description : ecrit le demi-mot value (16 bit) dans la memoire mem à l'address donnée.
   parametres : on reçoit la memoir, l'address de lecture et la valeur à ecrire.
   valeur de retour : 0 succes ou -1 failure.
   effets de bord : la memoire est modifiée.
*/
int memory_write_half(memory mem, uint32_t address, uint16_t value);

/*
   memory_write_word
   description : ecrit le mot value (32 bit) dans la memoire mem à l'address donnée.
   parametres : on reçoit la memoir, l'address de lecture et la valeur à ecrire.
   valeur de retour : 0 succes ou -1 failure.
   effets de bord : la memoire est modifiée.
*/
int memory_write_word(memory mem, uint32_t address, uint32_t value);

#endif
