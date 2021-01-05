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
#include "registers.h"
#include "arm_constants.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*
  registerfrommode pour r0 -r7 + PC(r15)+Cpsr sulement 1 valeur 
  registerfrommode pour r8 -r12 avec 2 valeur 
  registerfrommode pour r13 -r14 avec 6 valeur
  registerfrommode pour SPSR avec 5 valeur 
PC CPSR toujour dans 
registerfrommode[0] -valeur dans mode USR SYS pour R0-R15  SPSR NULL 



CPSR - reg_data[16]
SPSR - reg_data[17]

*/
struct registers_data {
  uint32_t **reg_data;
};

registers registers_create() {
    registers r;
    r = malloc(sizeof(struct registers_data));
    r->reg_data=malloc(sizeof(uint32_t *)*18);
    for(int i = 0 ; i<18;i++){
      if((i<8)||(i==15)||(i==16)){
        r->reg_data[i]=malloc(sizeof(uint32_t));
        r->reg_data[i][0]=0x00;
      } else if((i>=8)&&(i<13)){
        r->reg_data[i]=malloc(sizeof(uint32_t)*2);
        r->reg_data[i][0]=0x00;
        r->reg_data[i][1]=0x00;
      }else if((i>=13)&&(i<15)){
        r->reg_data[i]=malloc(sizeof(uint32_t)*6);
        r->reg_data[i][0]=0x00;
        r->reg_data[i][1]=0x00;
        r->reg_data[i][2]=0x00;
        r->reg_data[i][3]=0x00;
        r->reg_data[i][4]=0x00;
        r->reg_data[i][5]=0x00;
      }else if((i==17)){
        r->reg_data[i]=malloc(sizeof(uint32_t)*5);
        r->reg_data[i][0]=0x00;
        r->reg_data[i][1]=0x00;
        r->reg_data[i][2]=0x00;
        r->reg_data[i][3]=0x00;
        r->reg_data[i][4]=0x00;
      }

    }
    return r;
}

void registers_destroy(registers r) {
  for (int i = 0;i<18;i++){
    free(r->reg_data[i]);
  }
  free(r->reg_data);
  free(r);
}

uint8_t get_mode(registers r) {
    uint32_t mask = 0x1f;
    return r->reg_data[16][0]&mask;
}

int current_mode_has_spsr(registers r) {
    uint8_t mode=get_mode(r);
    if ((mode == 0x10)||(mode == 0x1f))
      return 0;
    else
      return 1; 
}
//???
int in_a_privileged_mode(registers r) {
    uint8_t mode=get_mode(r);
    if (mode == 0x10)
      return 0;
    else
      return 1; 
}

uint32_t read_register(registers r, uint8_t reg) {
    uint32_t value;
    if(reg<8){
        value=r->reg_data[reg][0];
      } else if((reg>=8)&&(reg<13)){
        if (get_mode(r)==0x11){
          value=r->reg_data[reg][1];
        }else{
          value=r->reg_data[reg][0];
        }
      }else if((reg>=13)&&(reg<15)){
        if (get_mode(r)==0x11){//fiq
          value=r->reg_data[reg][1];
        } else if (get_mode(r)==0x12){//irq
          value=r->reg_data[reg][2];
        } else if (get_mode(r)==0x13){//supervisor
          value=r->reg_data[reg][5];
        } else if (get_mode(r)==0x17){//About
          value=r->reg_data[reg][4];
        } else if (get_mode(r)==0x1B){//undrefined
          value=r->reg_data[reg][3];
        } else{
          value=r->reg_data[reg][0];
        }
      }
    return value;
}
  
uint32_t read_usr_register(registers r, uint8_t reg) {
    uint32_t value=r->reg_data[reg][0];
    return value;
}

uint32_t read_cpsr(registers r) {
    uint32_t value=r->reg_data[16][0];
    return value;
}

uint32_t read_spsr(registers r) {
    uint32_t value;
    if (get_mode(r)==0x11){//fiq
        value=r->reg_data[17][0];
    }else if (get_mode(r)==0x12){//irq
      value=r->reg_data[17][1];
    }else if (get_mode(r)==0x13){//supervisor
      value=r->reg_data[17][4];
    }else if (get_mode(r)==0x17){//About
      value=r->reg_data[17][3];
    }else if (get_mode(r)==0x1B){//undrefined
      value=r->reg_data[17][2];
    }
    return value;
}

void write_register(registers r, uint8_t reg, uint32_t value) {
    if(reg<8){
        memcpy(&r->reg_data[reg][0],&value,sizeof(uint32_t));
      } else if((reg>=8)&&(reg<13)){
        if (get_mode(r)==0x11){
          memcpy(&r->reg_data[reg][1],&value,sizeof(uint32_t));
        }else{
          memcpy(&r->reg_data[reg][0],&value,sizeof(uint32_t));
        }
      }else if((reg>=13)&&(reg<15)){
        if (get_mode(r)==0x11){//fiq
          memcpy(&r->reg_data[reg][1],&value,sizeof(uint32_t));
        } else if (get_mode(r)==0x12){//irq
          memcpy(&r->reg_data[reg][2],&value,sizeof(uint32_t));
        } else if (get_mode(r)==0x13){//supervisor
          memcpy(&r->reg_data[reg][5],&value,sizeof(uint32_t));
        } else if (get_mode(r)==0x17){//About
          memcpy(&r->reg_data[reg][4],&value,sizeof(uint32_t));
        } else if (get_mode(r)==0x1B){//undrefined
          memcpy(&r->reg_data[reg][3],&value,sizeof(uint32_t));
        } else{
          memcpy(&r->reg_data[reg][0],&value,sizeof(uint32_t));
        }
      }
}

void write_usr_register(registers r, uint8_t reg, uint32_t value) {
    memcpy(&r->reg_data[reg][0],&value,sizeof(uint32_t));
}

void write_cpsr(registers r, uint32_t value) {
  memcpy(&r->reg_data[16][0],&value,sizeof(uint32_t));
}

void write_spsr(registers r, uint32_t value) {
    if (get_mode(r)==0x11){//fiq
      memcpy(&r->reg_data[17][0],&value,sizeof(uint32_t));
    }else if (get_mode(r)==0x12){//irq
      memcpy(&r->reg_data[17][1],&value,sizeof(uint32_t));
    }else if (get_mode(r)==0x13){//supervisor
      memcpy(&r->reg_data[17][4],&value,sizeof(uint32_t));
    }else if (get_mode(r)==0x17){//About
      memcpy(&r->reg_data[17][3],&value,sizeof(uint32_t));
    }else if (get_mode(r)==0x1B){//undrefined
      memcpy(&r->reg_data[17][2],&value,sizeof(uint32_t));
    }
}
