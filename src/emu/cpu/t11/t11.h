/*** T-11: Portable DEC T-11 emulator ******************************************/

#pragma once

#ifndef __T11_H__
#define __T11_H__

#include "cpuintrf.h"

enum
{
	T11_R0=1, T11_R1, T11_R2, T11_R3, T11_R4, T11_R5, T11_SP, T11_PC, T11_PSW
};

#define T11_IRQ0        0      /* IRQ0 */
#define T11_IRQ1		1	   /* IRQ1 */
#define T11_IRQ2		2	   /* IRQ2 */
#define T11_IRQ3		3	   /* IRQ3 */

#define T11_RESERVED    0x000   /* Reserved vector */
#define T11_TIMEOUT     0x004   /* Time-out/system error vector */
#define T11_ILLINST     0x008   /* Illegal and reserved instruction vector */
#define T11_BPT         0x00C   /* BPT instruction vector */
#define T11_IOT         0x010   /* IOT instruction vector */
#define T11_PWRFAIL     0x014   /* Power fail vector */
#define T11_EMT         0x018   /* EMT instruction vector */
#define T11_TRAP        0x01C   /* TRAP instruction vector */


struct t11_setup
{
	UINT16	mode;			/* initial processor mode */
};


extern CPU_GET_INFO( t11 );

/****************************************************************************/
/* Read a byte from given memory location                                   */
/****************************************************************************/
#define T11_RDMEM(A) ((unsigned)memory_read_byte_16le(t11.program, A))
#define T11_RDMEM_WORD(A) ((unsigned)memory_read_word_16le(t11.program, A))

/****************************************************************************/
/* Write a byte to given memory location                                    */
/****************************************************************************/
#define T11_WRMEM(A,V) (memory_write_byte_16le(t11.program, A,V))
#define T11_WRMEM_WORD(A,V) (memory_write_word_16le(t11.program, A,V))

CPU_DISASSEMBLE( t11 );

#endif /* __T11_H__ */
