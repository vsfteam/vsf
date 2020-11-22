// This is is modified from evm/include/evm_main.h
//  Chinese comments are removed or transalate to english because it destroys the font in IAR

/****************************************************************************
**
** Copyright (C) 2020 @scriptiot
**
**  EVM is a gerneric virtual machine engine with lexer front-end, virtual machine and virtual machine extension framework.
**  Supporting js, python, qml, lua, etc; pure C language; zero dependency; REPL embedded; supporting MCU with ROM > 40KB, RAM > 2KB
**  embedded memory management with garbage collecting, complex comression algo, no memory fragmentation
**  Version	: 1.0
**  Email	: scriptiot@aliyun.com
**  Website	: https://github.com/scriptiot/evm
**            https://gitee.com/scriptiot/evm
**  Licence: Apache-2.0
****************************************************************************/

#ifndef EVM_MAIN_H
#define EVM_MAIN_H

#include "evm_module.h"

#define NEVM_HEAP_SIZE         (3 * 1024)
#define NEVM_STACK_SIZE        (1 * 1024)
#define NEVM_MODULE_SIZE       3

#define EVM_HEAP_SIZE          (5 * 1024)
#define EVM_STACK_SIZE         (2 * 1024)
#define EVM_MODULE_SIZE        5

int evm_main(void);

#endif
