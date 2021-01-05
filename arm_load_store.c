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
#include "arm_load_store.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "util.h"
#include "debug.h"

int arm_load_store(arm_core p, uint32_t ins) {
	if(((ins>>27)&0x01==0X00)&&((ins>>26)&0x01==0x01)){
		if ((ins>>22)&0x01==0x00){
			if((ins>>20)&0x01==0x01){
				//LDR
			}else{ // donc ((ins>>20)&0x01==0x00) 
				//STR
			}
		}else { //donc ((ins>>20)&0x01==0x01)
			if((ins>>20)&0x01==0x01){
				//LDRB
			}else { // donc ((ins>>20)&0x01==0x00) 
				//STRB
			}
		}
	}else if(((ins>>27)&0x01==0X00)&&((ins>>26)&0x01==0x00)&&((ins>>25)&0x01==0x00)&&((ins>>4)&0x0F==0x0B)){
		if((ins>>20)&0x01==0x01){
				//LDRH
			}else { // donc ((ins>>20)&0x01==0x00) 
				//STRH
			}
	}
    return UNDEFINED_INSTRUCTION;
}

int arm_load_store_multiple(arm_core p, uint32_t ins) {
	if(((ins>>27)&0x01==0X01)&&((ins>>26)&0x01==0x00)&&((ins>>25)&0x01==0x00)){
		if((ins>>20)&0x01==0x01){
				//LDM
			}else { // donc ((ins>>20)&0x01==0x00) 
				//STM
			}	
	}
    return UNDEFINED_INSTRUCTION;
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    if((ins>>27)&0x01)
    	return arm_load_store_multiple(p,ins);
    else
		return arm_load_store(p,ins);
 
}
