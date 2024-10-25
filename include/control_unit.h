/**
 * File Name: control_unit.h
 * Author: Vishank Singh
 * Github: https://github.com/VishankSingh
 */
#ifndef CONTROL_UNIT_H
#define CONTROL_UNIT_H

#include <stdint.h>

#include "multi_cycle_vm.h"

void update_control_signals(mc_vm_s *vm, uint32_t instruction);

#endif // CONTROL_UNIT_H