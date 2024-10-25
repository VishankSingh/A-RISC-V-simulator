/**
 * File Name: hazard_unit.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef HAZARD_UNIT_H
#define HAZARD_UNIT_H

#include "multi_cycle_vm.h"

void hazard_unit(mc_vm_s *vm);

void find_hazards(mc_vm_s *vm);

void stall_IF(mc_vm_s *vm);

void stall_ID(mc_vm_s *vm);

void stall_EX(mc_vm_s *vm);

void stall_MEM(mc_vm_s *vm);

void stall_WB(mc_vm_s *vm);

#endif // HAZARD_UNIT_H