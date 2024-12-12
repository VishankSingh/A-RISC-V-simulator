/**
 * File Name: forwarding_unit.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef FORWARDING_UNIT_H
#define FORWARDING_UNIT_H

#include "multi_cycle_vm.h"

void forwarding_unit(mc_vm_s *vm);

void forward_EXMEM(mc_vm_s *vm);

void forward_MEMWB(mc_vm_s *vm);

#endif // FORWARDING_UNIT_H
