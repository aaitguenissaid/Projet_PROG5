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
#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_load_store.h"
#include "arm_branch_other.h"
#include "util.h"
#include "debug.h"

/*-------------------------- Fonctions auxiliaires --------------------------*/

uint8_t get_opcode(uint32_t ins) {
  uint8_t valeur = 0;
  valeur = get_bits(ins, 24, 21);
  return valeur;
}

uint8_t bit_S(uint32_t ins) {
  uint8_t valeur = 0;
  valeur = get_bit(ins, 20);
  return valeur;
}

uint32_t get_C_flag(arm_core p) {
  uint32_t valeur = 0;
  valeur = get_bit(arm_read_cpsr(p), C);
  return valeur;
}

uint8_t get_rn(uint32_t ins) {
  uint8_t valeur = 0;
  valeur = get_bits(ins, 19, 16);
  return valeur;
}

uint8_t get_rd(uint32_t ins) {
  uint8_t valeur = 0;
  valeur = get_bits(ins, 15, 12);
  return valeur;
}

uint32_t Rm_value(arm_core p, uint32_t ins) {
  return arm_read_register(p, (uint8_t) get_bits(ins, 3, 0));
}

uint32_t Rs_value(arm_core p, uint32_t ins) {
  return arm_read_register(p, (uint8_t) get_bits(ins, 11, 8));
}

void get_operand_carryout(arm_core p, uint32_t ins, uint32_t * shifter_operand, uint8_t * shifter_carry_out) {
  uint8_t shift_imm = get_bits(ins, 11, 7);
  uint8_t C_flag = get_bit(arm_read_cpsr(p), C);
  if (get_bits(ins, 27, 25) == 1) {
    // 1.Data-processing operands - Immediate
    uint32_t immed_8 = get_bits(ins, 7, 0);
    uint8_t rotate_imm = get_bits(ins, 11, 8);
    * shifter_operand = ror(immed_8, rotate_imm * 2);
    if (rotate_imm == 0) {
      * shifter_carry_out = C_flag;
    } else {
      * shifter_carry_out = get_bit( * shifter_operand, 31);
    }
  } else if (get_bits(ins, 27, 25) == 0) {

    if (get_bits(ins, 11, 4) == 0) {
      // 2.Data-processing operands - Register
      * shifter_operand = Rm_value(p, ins);
      * shifter_carry_out = C_flag;
    } else if (get_bits(ins, 6, 4) == 0) {
      // 3.Data-processing operands - Logical shift left by immediate
      if (shift_imm == 0) {
        * shifter_operand = Rm_value(p, ins);
        * shifter_carry_out = C_flag;
      } else {
        * shifter_operand = Rm_value(p, ins) << shift_imm;
        * shifter_carry_out = get_bit(Rm_value(p, ins), 32 - shift_imm);
      }
    } else if (get_bits(ins, 7, 4) == 1) {
      // 4.Data-processing operands - Logical shift left by register
      if (get_bits(Rs_value(p, ins), 7, 0) == 0) {
        * shifter_operand = Rm_value(p, ins);
        * shifter_carry_out = C_flag;
      } else if (get_bits(Rs_value(p, ins), 7, 0) < 32) {
        * shifter_operand = Rm_value(p, ins) << get_bits(Rs_value(p, ins), 7, 0);
        * shifter_carry_out = get_bit(Rm_value(p, ins), 32 - get_bits(Rs_value(p, ins), 7, 0));
      } else if (get_bits(Rs_value(p, ins), 7, 0) == 32) {
        * shifter_operand = 0;
        * shifter_carry_out = get_bit(Rm_value(p, ins), 0);
      } else {
        * shifter_operand = 0;
        * shifter_carry_out = 0;
      }
    } else if (get_bits(ins, 6, 4) == 2) {
      // 5.Data-processing operands - Logical shift right by immediate
      if (shift_imm == 0) {
        * shifter_operand = 0;
        * shifter_carry_out = get_bit(Rm_value(p, ins), 31);
      } else {
        * shifter_operand = Rm_value(p, ins) >> shift_imm;
        * shifter_carry_out = get_bit(Rm_value(p, ins), shift_imm - 1);
      }
    } else if (get_bits(ins, 7, 4) == 3) {
      // 6.Data-processing operands - Logical shift right by register
      if (get_bits(Rs_value(p, ins), 7, 0) == 0) {
        * shifter_operand = Rm_value(p, ins);
        * shifter_carry_out = C_flag;
      } else if (get_bits(Rs_value(p, ins), 7, 0) < 32) {
        * shifter_operand = Rm_value(p, ins) >> get_bits(Rs_value(p, ins), 7, 0);
        * shifter_carry_out = get_bit(Rm_value(p, ins), get_bits(Rs_value(p, ins), 7, 0) - 1);
      } else if (get_bits(Rs_value(p, ins), 7, 0) == 32) {
        * shifter_operand = 0;
        * shifter_carry_out = get_bit(Rm_value(p, ins), 31);
      } else {
        * shifter_operand = 0;
        * shifter_carry_out = 0;
      }
    } else if (get_bits(ins, 6, 4) == 4) {
      // 7.Data-processing operands - Arithmetic shift right by immediate
      if (shift_imm == 0) {
        if (get_bit(Rm_value(p, ins), 31) == 0) {
          * shifter_operand = 0;
          * shifter_carry_out = get_bit(Rm_value(p, ins), 31);
        } else {
          * shifter_operand = 0xFFFFFFFF;
          * shifter_carry_out = get_bit(Rm_value(p, ins), 31);
        }
      } else {
        * shifter_operand = asr(Rm_value(p, ins), shift_imm);
        * shifter_carry_out = get_bit(Rm_value(p, ins), shift_imm - 1);
      }
    } else if (get_bits(ins, 7, 4) == 5) {
      // 8.Data-processing operands - Arithmetic shift right by register
      if (get_bits(Rs_value(p, ins), 7, 0) == 0) {
        * shifter_operand = Rm_value(p, ins);
        * shifter_carry_out = C_flag;
      } else if (get_bits(Rs_value(p, ins), 7, 0) < 32) {
        * shifter_operand = asr(Rm_value(p, ins), (uint8_t) get_bits(Rs_value(p, ins), 7, 0));
        * shifter_carry_out = (uint8_t) get_bit(Rm_value(p, ins), get_bits(Rs_value(p, ins), 7, 0) - 1);
      } else {
        if (get_bit(Rm_value(p, ins), 31)) {
          * shifter_operand = 0xFFFFFFFF;
          * shifter_carry_out = get_bit(Rm_value(p, ins), 31);
        } else {
          * shifter_operand = 0;
          * shifter_carry_out = get_bit(Rm_value(p, ins), 31);
        }
      }
    } else if (get_bits(ins, 6, 4) == 6) {
      // 9.Data-processing operands - Rotate right by immediate
      if (shift_imm == 0) {
        * shifter_operand = (C_flag << 31) | (Rm_value(p, ins) >> 1);
        * shifter_carry_out = get_bit(Rm_value(p, ins), 0);
      } else {
        * shifter_operand = ror(Rm_value(p, ins), shift_imm);
        * shifter_carry_out = get_bit(Rm_value(p, ins), shift_imm - 1);
      }
    } else if (get_bits(ins, 7, 4) == 7) {
      // 10.Data-processing operands - Rotate right by register
      if (get_bits(Rs_value(p, ins), 7, 0) == 0) {
        * shifter_operand = Rm_value(p, ins);
        * shifter_carry_out = C_flag;
      } else if (get_bits(Rs_value(p, ins), 4, 0) == 0) {
        * shifter_operand = Rm_value(p, ins);
        * shifter_carry_out = get_bit(Rm_value(p, ins), 31);;
      } else if (get_bits(Rs_value(p, ins), 4, 0) > 0) {
        * shifter_operand = ror(Rm_value(p, ins), (uint8_t) get_bits(Rs_value(p, ins), 4, 0));
        * shifter_carry_out = get_bit(Rm_value(p, ins), get_bits(Rs_value(p, ins), 4, 0) - 1);
      }
    } else if (get_bits(ins, 11, 4) == 6) {
      // 11.Data-processing operands - Rotate right with extend
      * shifter_operand = (C_flag << 31) | (Rm_value(p, ins) >> 1);
      * shifter_carry_out = get_bit(Rm_value(p, ins), 0);
    }
  }
}

void update_flags_cpsr(arm_core p, int n, int z, int c, int v) {
  uint32_t val_cpsr = arm_read_cpsr(p);
  val_cpsr = n == 1 ? set_bit(val_cpsr, N) : clr_bit(val_cpsr, N);
  val_cpsr = z == 1 ? set_bit(val_cpsr, Z) : clr_bit(val_cpsr, Z);
  val_cpsr = c == 1 ? set_bit(val_cpsr, C) : clr_bit(val_cpsr, C);
  val_cpsr = v == 1 ? set_bit(val_cpsr, V) : clr_bit(val_cpsr, V);
  arm_write_cpsr(p, val_cpsr);
}

uint32_t two_complement(uint32_t operand) {
  uint32_t valeur = operand;
  valeur = ~(valeur) + 1;
  return valeur;
}

uint64_t operation(uint64_t operand1, uint64_t operand2, int op) {
  uint64_t valeur = 0;
  if (op == 1) { // addition
    valeur = operand1 + operand2;
  } else if (op == 2) { // soustraction
    uint32_t ca2_operand2 = two_complement(operand2);
    valeur = operand1 + (uint64_t) ca2_operand2;
  }
  return valeur;
}

int borrow_from(uint32_t operand1, uint32_t operand2) {
  if (operand1 >= operand2) {
    return 0;
  } else {
    return 1;
  }
}

int overflow_from(uint32_t operand1, uint32_t operand2, int op) {
  uint64_t valeur = operation((uint64_t) operand1, (uint64_t) operand2, op);
  int signe_operand1 = get_bit(operand1, 31);
  int signe_operand2 = get_bit(operand2, 31);
  int signe_valeur = get_bit(valeur, 31);
  if (signe_operand1 == signe_operand2 && signe_operand1 != signe_valeur)
    return 1;
  else {
    return 0;
  }
}

int carry_from(uint32_t operand1, uint32_t operand2, int op) {
  uint64_t valeur = operation((uint64_t) operand1, (uint64_t) operand2, op);
  int signe_valeur = get_bit(valeur, 32);
  if (op == 1) { // addition
    if (signe_valeur == 1)
      return 1;
    else {
      return 0;
    }
  } else if (op == 2) { // soustraction
    if (signe_valeur == 0)
      return 1;
    else {
      return 0;
    }
  } else
    return 0;
}

/*-------------------------- Instructions  de traitement de données --------------------------*/

int ins_AND(arm_core p, uint32_t ins) {
  uint32_t shifter_operand = 0;
  uint8_t shifter_carry_out = 0;

  get_operand_carryout(p, ins, & shifter_operand, & shifter_carry_out);
  uint32_t valeur = shifter_operand & arm_read_register(p, get_rn(ins));
  arm_write_register(p, get_rd(ins), valeur);

  if (bit_S(ins) == 1 && get_rd(ins) == 15) {
    if (arm_current_mode_has_spsr(p))
      arm_write_cpsr(p, arm_read_spsr(p));
    else {
      return 0; // UNPREDICTABLE
    }
  } else if (bit_S(ins) == 1) {
    int N_Flag = get_bit(arm_read_register(p, get_rd(ins)), 31);
    int Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
    int C_Flag = shifter_carry_out;
    int V_Flag = (arm_read_cpsr(p) >> 28) & 1;
    update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
  }

  return 0;
}

int ins_EOR(arm_core p, uint32_t ins) {
  uint32_t shifter_operand = 0;
  uint8_t shifter_carry_out = 0;

  get_operand_carryout(p, ins, & shifter_operand, & shifter_carry_out);
  int32_t valeur = arm_read_register(p, get_rn(ins)) ^ shifter_operand;
  arm_write_register(p, get_rd(ins), valeur);

  if (bit_S(ins) == 1 && get_rd(ins) == 15) {
    if (arm_current_mode_has_spsr(p))
      arm_write_cpsr(p, arm_read_spsr(p));
    else {
      return 0; // UNPREDICTABLE
    }
  } else if (bit_S(ins) == 1) {
    int N_Flag = get_bit(arm_read_register(p, get_rd(ins)), 31);
    int Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
    int C_Flag = shifter_carry_out;
    int V_Flag = get_bit(arm_read_cpsr(p), V); //unaffected
    update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
  }

  return 0;
}

int ins_SUB(arm_core p, uint32_t ins) {
  uint32_t shifter_operand = 0;
  uint8_t shifter_carry_out = 0;
  get_operand_carryout(p, ins, & shifter_operand, & shifter_carry_out);
  uint32_t Rn_value = arm_read_register(p, (uint8_t) get_bits(ins, 19, 16));
  uint32_t Rd_value = Rn_value - shifter_operand;
  arm_write_register(p, get_rd(ins), Rd_value);

  if (bit_S(ins) == 1 && get_rd(ins) == 15) {
    if (arm_current_mode_has_spsr(p))
      arm_write_cpsr(p, arm_read_spsr(p));
    else {
      return 0; // UNPREDICTABLE
    }
  } else if (bit_S(ins) == 1) {
    int N_Flag = get_bit(Rd_value, 31);
    int Z_Flag = (Rd_value == 0 ? 1 : 0);
    int C_Flag = (Rn_value >= shifter_operand ? 1 : 0);
    int V_Flag = overflow_from(Rn_value, shifter_operand, 2); //unaffected
    update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
  }
  return 0;
}

int ins_RSB(arm_core p, uint32_t ins) {
  uint32_t shifter_operand = 0;
  uint8_t shifter_carry_out = 0;

  get_operand_carryout(p, ins, & shifter_operand, & shifter_carry_out);
  uint32_t valeur = shifter_operand - arm_read_register(p, get_rn(ins));
  arm_write_register(p, get_rd(ins), valeur);

  if (bit_S(ins) == 1 && get_rd(ins) == 15) {
    if (arm_current_mode_has_spsr(p))
      arm_write_cpsr(p, arm_read_spsr(p));
    else {
      return 0; // UNPREDICTABLE
    }
  } else if (bit_S(ins) == 1) {
    int N_Flag = get_bit(arm_read_register(p, get_rd(ins)), 31);
    int Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
    int C_Flag = ~borrow_from(shifter_operand, arm_read_register(p, get_rn(ins))) & 1;
    int V_Flag = overflow_from(shifter_operand, arm_read_register(p, get_rn(ins)), 2);
    update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
  }

  return 0;
}

int ins_ADD(arm_core p, uint32_t ins) {
  uint32_t shifter_operand = 0;
  uint8_t shifter_carry_out = 0;

  get_operand_carryout(p, ins, & shifter_operand, & shifter_carry_out);
  uint32_t valeur = shifter_operand + arm_read_register(p, get_rn(ins));
  arm_write_register(p, get_rd(ins), valeur);

  if (bit_S(ins) == 1 && get_rd(ins) == 15) {
    if (arm_current_mode_has_spsr(p))
      arm_write_cpsr(p, arm_read_spsr(p));
    else {
      return 0; // UNPREDICTABLE
    }
  } else if (bit_S(ins) == 1) {
    int N_Flag = get_bit(arm_read_register(p, get_rd(ins)), 31);
    int Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
    int C_Flag = carry_from(shifter_operand, arm_read_register(p, get_rn(ins)), 1);
    int V_Flag = overflow_from(arm_read_register(p, get_rn(ins)), shifter_operand, 1);
    update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
  }

  return 0;
}

int ins_ADC(arm_core p, uint32_t ins) {
  uint32_t shifter_operand = 0;
  uint8_t shifter_carry_out = 0;

  get_operand_carryout(p, ins, & shifter_operand, & shifter_carry_out);
  uint32_t valeur = arm_read_register(p, get_rn(ins)) + shifter_operand + get_C_flag(p);
  arm_write_register(p, get_rd(ins), valeur);
  if (bit_S(ins) == 1 && get_rd(ins) == 15) {
    if (arm_current_mode_has_spsr(p))
      arm_write_cpsr(p, arm_read_spsr(p));
    else {
      return 0; // UNPREDICTABLE
    }
  } else if (bit_S(ins) == 1) {
    int N_Flag = get_bit(arm_read_register(p, get_rd(ins)), 31);
    int Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
    int C_Flag = carry_from(arm_read_register(p, get_rn(ins)), (shifter_operand + get_C_flag(p)), 1);
    int V_Flag = overflow_from(arm_read_register(p, get_rn(ins)), (shifter_operand + get_C_flag(p)), 1);
    update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
  }

  return 0;
}

int ins_SBC(arm_core p, uint32_t ins) {
  uint32_t shifter_operand = 0;
  uint8_t shifter_carry_out = 0;

  get_operand_carryout(p, ins, & shifter_operand, & shifter_carry_out);
  uint32_t valeur = operation(arm_read_register(p, get_rn(ins)), shifter_operand, 2);
  valeur = operation(valeur, ~(get_C_flag(p)) & 1, 2);
  arm_write_register(p, get_rd(ins), valeur);
  if (bit_S(ins) == 1 && get_rd(ins) == 15) {
    if (arm_current_mode_has_spsr(p))
      arm_write_cpsr(p, arm_read_spsr(p));
    else {
      return 0; // UNPREDICTABLE
    }
  } else if (bit_S(ins) == 1) {
    int N_Flag = get_bit(arm_read_register(p, get_rd(ins)), 31);
    int Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
    int C_Flag = ~borrow_from(arm_read_register(p, get_rn(ins)), shifter_operand + get_C_flag(p)) & 1;
    int V_Flag = overflow_from(arm_read_register(p, get_rn(ins)), shifter_operand + get_C_flag(p), 2);
    update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
  }

  return 0;
}

int ins_RSC(arm_core p, uint32_t ins) {
  uint32_t shifter_operand = 0;
  uint8_t shifter_carry_out = 0;

  get_operand_carryout(p, ins, & shifter_operand, & shifter_carry_out);
  uint32_t valeur = operation(shifter_operand, arm_read_register(p, get_rn(ins)), 2);
  valeur = operation(valeur, ~(get_C_flag(p)) & 1, 2);
  arm_write_register(p, get_rd(ins), valeur);
  if (bit_S(ins) == 1 && get_rd(ins) == 15) {
    if (arm_current_mode_has_spsr(p))
      arm_write_cpsr(p, arm_read_spsr(p));
    else {
      return 0; // UNPREDICTABLE
    }
  } else if (bit_S(ins) == 1) {
    int N_Flag = get_bit(arm_read_register(p, get_rd(ins)), 31);
    int Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
    int C_Flag = ~borrow_from(shifter_operand, (arm_read_register(p, get_rn(ins)) + get_C_flag(p))) & 1;
    int V_Flag = overflow_from(shifter_operand, (arm_read_register(p, get_rn(ins)) + get_C_flag(p)), 2);
    update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
  }

  return 0;
}

int ins_TST(arm_core p, uint32_t ins) {
  uint32_t shifter_operand = 0;
  uint8_t shifter_carry_out = 0;

  get_operand_carryout(p, ins, & shifter_operand, & shifter_carry_out);
  int alu_out = arm_read_register(p, get_rn(ins)) & shifter_operand;
  int N_Flag = get_bit(alu_out, 31);
  int Z_Flag = alu_out == 0 ? 1 : 0;
  int C_Flag = shifter_carry_out;
  int V_Flag = get_bit(arm_read_cpsr(p), V); //unaffected
  update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);

  return 0;
}

int ins_TEQ(arm_core p, uint32_t ins) {
  uint32_t shifter_operand = 0;
  uint8_t shifter_carry_out = 0;

  get_operand_carryout(p, ins, & shifter_operand, & shifter_carry_out);
  int alu_out = arm_read_register(p, get_rn(ins)) ^ shifter_operand;
  int N_Flag = get_bit(alu_out, 31);
  int Z_Flag = alu_out == 0 ? 1 : 0;
  int C_Flag = shifter_carry_out;
  int V_Flag = get_bit(arm_read_cpsr(p), V); //unaffected
  update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);

  return 0;
}

int ins_CMP(arm_core p, uint32_t ins) {
  uint32_t shifter_operand = 0;
  uint8_t shifter_carry_out = 0;

  get_operand_carryout(p, ins, & shifter_operand, & shifter_carry_out);
  int alu_out = arm_read_register(p, get_rn(ins)) - shifter_operand;
  int N_Flag = get_bit(alu_out, 31);
  int Z_Flag = alu_out == 0 ? 1 : 0;
  int C_Flag = carry_from(arm_read_register(p, get_rn(ins)), shifter_operand, 1);
  int V_Flag = overflow_from(arm_read_register(p, get_rn(ins)), shifter_operand, 0);
  update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);

  return 0;
}

int ins_CMN(arm_core p, uint32_t ins) {
  uint32_t shifter_operand = 0;
  uint8_t shifter_carry_out = 0;

  get_operand_carryout(p, ins, & shifter_operand, & shifter_carry_out);
  int alu_out = arm_read_register(p, get_rn(ins)) + shifter_operand;
  int N_Flag = get_bit(alu_out, 31);
  int Z_Flag = alu_out == 0 ? 1 : 0;
  int C_Flag = carry_from(arm_read_register(p, get_rn(ins)), shifter_operand, 1);
  int V_Flag = overflow_from(arm_read_register(p, get_rn(ins)), shifter_operand, 1);
  update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);

  return 0;
}

int ins_ORR(arm_core p, uint32_t ins) {
  uint32_t shifter_operand = 0;
  uint8_t shifter_carry_out = 0;

  get_operand_carryout(p, ins, & shifter_operand, & shifter_carry_out);
  int32_t valeur = arm_read_register(p, get_rn(ins)) | shifter_operand;
  arm_write_register(p, get_rd(ins), valeur);
  if (bit_S(ins) == 1 && get_rd(ins) == 15) {
    if (arm_current_mode_has_spsr(p))
      arm_write_cpsr(p, arm_read_spsr(p));
    else {
      return 0; // UNPREDICTABLE
    }
  } else if (bit_S(ins) == 1) {
    int N_Flag = get_bit(arm_read_register(p, get_rd(ins)), 31);
    int Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
    int C_Flag = shifter_carry_out;
    int V_Flag = get_bit(arm_read_cpsr(p), V); //unaffected
    update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
  }

  return 0;
}

int ins_MOV(arm_core p, uint32_t ins) {
  uint32_t Rd_value = arm_read_register(p, (uint8_t) get_bits(ins, 15, 12));
  uint32_t shifter_operand = 0;
  uint8_t shifter_carry_out = 0;
  get_operand_carryout(p, ins, & shifter_operand, & shifter_carry_out);

  arm_write_register(p, get_rd(ins), shifter_operand);
  if (bit_S(ins) == 1 && get_rd(ins) == 15) {
    if (arm_current_mode_has_spsr(p))
      arm_write_cpsr(p, arm_read_spsr(p));
    else {
      return 0; // UNPREDICTABLE
    }
  } else if (bit_S(ins) == 1) {
    int N_Flag = get_bit(Rd_value, 31);
    int Z_Flag = (Rd_value == 0 ? 1 : 0);
    int C_Flag = shifter_carry_out;
    int V_Flag = get_bit(arm_read_cpsr(p), V); //unaffected
    update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
  }

  return 0;
}

int ins_BIC(arm_core p, uint32_t ins) {
  uint32_t shifter_operand = 0;
  uint8_t shifter_carry_out = 0;

  get_operand_carryout(p, ins, & shifter_operand, & shifter_carry_out);
  int32_t valeur = arm_read_register(p, get_rn(ins)) & ~shifter_operand;
  arm_write_register(p, get_rd(ins), valeur);
  if (bit_S(ins) == 1 && get_rd(ins) == 15) {
    if (arm_current_mode_has_spsr(p))
      arm_write_cpsr(p, arm_read_spsr(p));
    else {
      return 0; // UNPREDICTABLE
    }
  } else if (bit_S(ins) == 1) {
    int N_Flag = get_bit(arm_read_register(p, get_rd(ins)), 31);
    int Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
    int C_Flag = shifter_carry_out;
    int V_Flag = get_bit(arm_read_cpsr(p), V); //unaffected
    update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
  }

  return 0;
}

int ins_MVN(arm_core p, uint32_t ins) {
  uint32_t shifter_operand = 0;
  uint8_t shifter_carry_out = 0;

  get_operand_carryout(p, ins, & shifter_operand, & shifter_carry_out);
  arm_write_register(p, get_rd(ins), ~shifter_operand);
  if (bit_S(ins) == 1 && get_rd(ins) == 15) {
    if (arm_current_mode_has_spsr(p))
      arm_write_cpsr(p, arm_read_spsr(p));
    else {
      return 0; // UNPREDICTABLE
    }
  } else if (bit_S(ins) == 1) {
    int N_Flag = get_bit(arm_read_register(p, get_rd(ins)), 31);
    int Z_Flag = arm_read_register(p, get_rd(ins)) == 0 ? 1 : 0;
    int C_Flag = shifter_carry_out;
    int V_Flag = get_bit(arm_read_cpsr(p), V); //unaffected
    update_flags_cpsr(p, N_Flag, Z_Flag, C_Flag, V_Flag);
  }

  return 0;
}

/* Decoding functions for different classes of instructions */
int arm_data_processing(arm_core p, uint32_t ins) {
  uint8_t opcode = get_opcode(ins);
  switch (opcode) {
  case 0:
    return ins_AND(p, ins);
    break;
  case 1:
    return ins_EOR(p, ins);
    break;
  case 2:
    return ins_SUB(p, ins);
    break;
  case 3:
    return ins_RSB(p, ins);
    break;
  case 4:
    return ins_ADD(p, ins);
    break;
  case 5:
    return ins_ADC(p, ins);
    break;
  case 6:
    return ins_SBC(p, ins);
    break;
  case 7:
    return ins_RSC(p, ins);
    break;
  case 8:
    return ins_TST(p, ins);
    break;
  case 9:
    return ins_TEQ(p, ins);
    break;
  case 10:
    return ins_CMP(p, ins);
    break;
  case 11:
    return ins_CMN(p, ins);
    break;
  case 12:
    return ins_ORR(p, ins);
    break;
  case 13:
    return ins_MOV(p, ins);
    break;
  case 14:
    return ins_BIC(p, ins);
    break;
  case 15:
    return ins_MVN(p, ins);
    break;
  default:
    return UNDEFINED_INSTRUCTION;
    break;
  }
}

int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {
  if ((get_bits(ins, 27, 26) == 0) && (get_bits(ins, 24, 23) == 2) && (get_bits(ins, 21, 20) == 2)) {
    uint32_t operand;
    uint32_t mask;
    if (get_bit(ins, 25) == 1) {
      uint32_t huit_bit_immediate = get_bits(ins, 7, 0);
      uint8_t rotate_imm = get_bits(ins, 11, 8);
      operand = ror(huit_bit_immediate, rotate_imm * 2);
    } else {
      uint8_t bit_3_0 = get_bits(ins, 3, 0);
      operand = arm_read_register(p, bit_3_0);
    }
    if ((operand & UnallocMask) != 0)
      return 0; // UNPREDICTABLE
    uint32_t byte_mask = (get_bit(ins, 16) ? 0x000000FF : 0x00000000) | \
      (get_bit(ins, 17) ? 0x0000FF00 : 0x00000000) | \
      (get_bit(ins, 18) ? 0x00FF0000 : 0x00000000) | \
      (get_bit(ins, 19) ? 0xFF000000 : 0x00000000);
    if (get_bit(ins, 22) == 0) { // R == 0
      if (arm_in_a_privileged_mode(p)) { // InAPrivilegedMode()
        if ((operand & StateMask) != 0) {
          return 0; // UNPREDICTABLE
        } else {
          mask = byte_mask & (UserMask | PrivMask);
        }
      } else {
        mask = byte_mask & UserMask;
      }
      uint32_t cpsr_value = (arm_read_cpsr(p) & ~mask) | (operand & mask);
      arm_write_cpsr(p, cpsr_value);

    } else { // R == 1
      if (arm_current_mode_has_spsr(p)) {
        mask = byte_mask & (UserMask | PrivMask | StateMask);
        uint32_t spsr_value = (arm_read_spsr(p) & ~mask) | (operand & mask);
        arm_write_spsr(p, spsr_value);
      } else {
        return 0;
      }
    }
  }
  return UNDEFINED_INSTRUCTION;
}
