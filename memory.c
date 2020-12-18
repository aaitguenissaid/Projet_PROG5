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
#include <stdlib.h>
#include "memory.h"
#include "util.h"
#include <assert.h>

struct memory_data {
  uint8_t *donnees;
  size_t size;
  int endian;
};

memory memory_create(size_t size, int is_big_endian) {
    memory mem = malloc(sizeof(struct memory_data));
    assert(mem != NULL);
    mem->donnees = malloc(sizeof(uint8_t)*size);
    assert(mem->donnees != NULL);
    mem->size = size;
    mem->endian = is_big_endian;
    return mem;
}

size_t memory_get_size(memory mem) {
    return mem->size;
    return 0;
}

void memory_destroy(memory mem) {
    free(mem->donnees);
    free(mem);
}

int memory_read_byte(memory mem, uint32_t address, uint8_t *value) {
    *value = *(mem->donnees + address);
    if (*value)
      return 0;
    else
      return -1;
}

int memory_read_half(memory mem, uint32_t address, uint16_t *value) {
    if(mem->endian){
      *value = (*(mem->donnees + address + 0))<< 8 | (*(mem->donnees + address +1));
    } else {
      *value = (*(mem->donnees + address + 1))<< 8 | (*(mem->donnees + address +0));   }
    if (*value)
      return 0;
    else
      return -1;
}

int memory_read_word(memory mem, uint32_t address, uint32_t *value) {
    if(mem->endian){
        *value = ((uint32_t)(*(mem->donnees + address + 0)))<< 24 | ((uint32_t)(*(mem->donnees + address + 1)))<< 16 \
        | ((uint32_t)(*(mem->donnees + address + 2)))<< 8 | ((uint32_t)(*(mem->donnees + address + 3)))<< 0;

    } else {
        *value = ((uint32_t)(*(mem->donnees + address + 3)))<< 24 | ((uint32_t)(*(mem->donnees + address + 2)))<< 16 \
        | ((uint32_t)(*(mem->donnees + address + 1)))<< 8 | ((uint32_t)(*(mem->donnees + address + 0)))<< 0;
       }
    if (*value)
      return 0;
    else
      return -1;
}

int memory_write_byte(memory mem, uint32_t address, uint8_t value) {
    *(mem->donnees + address) = value;
    if (*(mem->donnees + address))
      return 0;
    else
      return -1;
}

int memory_write_half(memory mem, uint32_t address, uint16_t value) {
  if(mem->endian){
      *(mem->donnees + address + 1) = value & 0XFF;
      *(mem->donnees + address + 0) = (value >> 8) & 0XFF;
  } else {
      *(mem->donnees + address + 0) = value & 0XFF;
      *(mem->donnees + address + 1) = (value >> 8) & 0XFF;
  }
  if (*(mem->donnees + address))
      return 0;
  else
      return -1;
}

int memory_write_word(memory mem, uint32_t address, uint32_t value) {
  if(mem->endian){
      *(mem->donnees + address + 0) = (uint8_t) ((value >> 24) & 0XFF);
      *(mem->donnees + address + 1) = (uint8_t) ((value >> 16) & 0XFF);
      *(mem->donnees + address + 2) = (uint8_t) ((value >> 8) & 0XFF);
      *(mem->donnees + address + 3) = (uint8_t) ((value >> 0) & 0XFF);
  } else {
      *(mem->donnees + address + 0) = (uint8_t) ((value >> 0) & 0XFF);
      *(mem->donnees + address + 1) = (uint8_t) ((value >> 8) & 0XFF);
      *(mem->donnees + address + 2) = (uint8_t) ((value >> 16) & 0XFF);
      *(mem->donnees + address + 3) = (uint8_t) ((value >> 24) & 0XFF);
       }
  if (*(mem->donnees + address))
      return 0;
  else
      return -1;
}
