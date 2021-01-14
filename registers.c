/*
Armator - simulateur de jeu d'instruction ARMv5T à but pédagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique Générale GNU publiée par la Free Software
Foundation (version 2 ou bien toute autre version ultérieure choisie par vous).

Ce programme est distribué car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but spécifique. Reportez-vous à la
Licence Publique Générale GNU pour plus de détails.

Vous devez avoir reçu une copie de la Licence Publique Générale GNU en même
temps que ce programme ; si ce n'est pas le cas, écrivez à la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
États-Unis.

Contact: Guillaume.Huard@imag.fr
	 Bâtiment IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'Hères
*/
#include "registers.h"
#include "arm_constants.h"
#include "util.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#define NB_MODES 32

//Un registre (r0,r1,r2,r3,r4,r5,r6,r7,r8,r8_fiq,r9,r9_fiq,r10,r10_fiq...) --> instancié 37 fois
struct reg {
   uint32_t reg_value;
};

typedef struct reg reg;

//Ensemble de pointeurs sur les registres d'un mode intitulé mode_name
struct mode_registers {
   int nb_regs;
   reg ** registers;
};

typedef struct mode_registers mode_registers;

struct registers_data {
   mode_registers * m_regs;
};

/*##### METHODES INTERNES #####*/
//Crée un registre avec la valeur 0.
static reg create_reg();

//Ajoute le registre reg au mode mode dans la structure r.
static void add_reg_to_mode(registers r, int mode, reg * reg);

registers registers_create() {
   registers r = malloc(sizeof(struct registers_data));
   assert(r != NULL);
   r -> m_regs = malloc(sizeof(mode_registers) * NB_MODES); //32 modes : 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, User, FIQ, IRQ, Supervisor,0,0,0, Abort,0,0,0, Undefined,0,0,0, System
   int i;
   //Initialisation des modes
   for (i = 0; i < NB_MODES; i++) {
      r -> m_regs[i].nb_regs = 0;
      r -> m_regs[i].registers = malloc(sizeof(reg * ) * 18); //18 = nb registres max par mode
   }
   /*##############################################################*/
   /*# Initialisation des registres et association aux bons modes #*/
   /*##############################################################*/
   //Parcours des registres (sauf SPSR)
   for (i = 0; i < 17; i++) {
      reg r1 = create_reg();

      //Mode USR et SYS
      add_reg_to_mode(r, USR, & r1);
      add_reg_to_mode(r, SYS, & r1);

      //Mode FIQ
      if (i < 8 || i == 15 || i == 16) {
         add_reg_to_mode(r, FIQ, & r1);
      } else {
         reg r_fiq = create_reg();
         add_reg_to_mode(r, FIQ, & r_fiq);
      }

      //Modes IRQ, SVC, ABT et UND
      if (i < 13 || i == 15 || i == 16) {
         add_reg_to_mode(r, IRQ, & r1);
         add_reg_to_mode(r, SVC, & r1);
         add_reg_to_mode(r, ABT, & r1);
         add_reg_to_mode(r, UND, & r1);
      } else if (i < 15) {
         reg r_svc = create_reg();
         reg r_abt = create_reg();
         reg r_und = create_reg();
         reg r_irq = create_reg();
         add_reg_to_mode(r, IRQ, & r_irq);
         add_reg_to_mode(r, SVC, & r_svc);
         add_reg_to_mode(r, ABT, & r_abt);
         add_reg_to_mode(r, UND, & r_und);
      }
   }
   //Registre SPSR
   reg r_svc = create_reg();
   reg r_abt = create_reg();
   reg r_und = create_reg();
   reg r_irq = create_reg();
   reg r_fiq = create_reg();
   add_reg_to_mode(r, FIQ, & r_fiq);
   add_reg_to_mode(r, IRQ, & r_irq);
   add_reg_to_mode(r, SVC, & r_svc);
   add_reg_to_mode(r, ABT, & r_abt);
   add_reg_to_mode(r, UND, & r_und);
   return r;
}

static reg create_reg() {
   reg registre;
   registre.reg_value = 0x0;
   return registre;
}

static void add_reg_to_mode(registers r, int mode, reg * reg) {
   r -> m_regs[mode].registers[r -> m_regs[mode].nb_regs] = malloc(sizeof(reg));
   *(r -> m_regs[mode].registers[r -> m_regs[mode].nb_regs]) = * reg;
   r -> m_regs[mode].nb_regs++;
}

void registers_destroy(registers r) {
   int i, j;
   for (i = 0; i < NB_MODES; i++) {
      for (j = 0; j < r -> m_regs[i].nb_regs; j++) {
         if (r -> m_regs[i].registers[j] != NULL) {
            free(r -> m_regs[i].registers[j]);
         }
      }
      free(r -> m_regs[i].registers);
   }
   free(r -> m_regs);
   free(r);
}

uint8_t get_mode(registers r) {
   return get_bits(r -> m_regs[USR].registers[CPSR] -> reg_value, 4, 0);
}

int current_mode_has_spsr(registers r) {
   uint8_t mode = get_mode(r);
   return mode != USR && mode != SYS;
}

int in_a_privileged_mode(registers r) {
   uint8_t mode = get_mode(r);
   return mode != USR;
}

uint32_t read_register(registers r, uint8_t reg) {
   uint32_t value = 0;
   uint8_t mode = get_mode(r);
   value = r -> m_regs[mode].registers[reg] -> reg_value;
   return value;
}

uint32_t read_usr_register(registers r, uint8_t reg) {
   uint32_t value = 0;
   //if(reg > 16 || reg < 0) {} --> UNPREDICTABLE
   value = r -> m_regs[USR].registers[reg] -> reg_value;
   return value;
}

uint32_t read_cpsr(registers r) {
   uint32_t value = 0;
   value = r -> m_regs[USR].registers[CPSR] -> reg_value;
   return value;
}

uint32_t read_spsr(registers r) {
   uint32_t value = 0;
   uint8_t mode = get_mode(r);
   value = r -> m_regs[mode].registers[SPSR] -> reg_value;
   return value;
}

void write_register(registers r, uint8_t reg, uint32_t value) {
   uint8_t mode = get_mode(r);
   r -> m_regs[mode].registers[reg] -> reg_value = value;
}

void write_usr_register(registers r, uint8_t reg, uint32_t value) {
   r -> m_regs[USR].registers[reg] -> reg_value = value;
}

void write_cpsr(registers r, uint32_t value) {
   //CPSR accessible depuis tout les modes, donc on passe par le mode USR
   r -> m_regs[USR].registers[CPSR] -> reg_value = value;
}

void write_spsr(registers r, uint32_t value) {
   uint8_t mode = get_mode(r);
   r -> m_regs[mode].registers[SPSR] -> reg_value = value;
}
