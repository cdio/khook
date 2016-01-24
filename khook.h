/*
 * Copyright (c) 2013 Claudio Castiglia <ccastiglia@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the author nor the names of its contributors
 *       may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __KHOOK_H__
#define __KHOOK_H__

#include <stdint.h>


#define KHOOK_VERSION	1.0.0


struct _ins;
typedef struct _ins INS;


/**
 * Max. number of bytes to hold recoded instructions.
 * An i386 instruction is coded as follows:
 * <pre>
 *  +--------+--------+-------+-----+-------------+-----------------+
 *  | prefix | opcode | modrm | sib | addr. displ | immediate const |
 *  +--------+--------+-------+-----+-------------+-----------------+
 * </pre>
 * There can be not more than 4 bytes for the prefix, 2 for the opcode,
 * 1 for the MODRM, 1 for the SIB, 4 for the displacement and 4 for the
 * immediate value. That is a maximum of 16 bytes for each instruction.
 * A <code>jmp rel32</ocde> uses 4 bytes and the smallest instruction
 * uses 1 byte, so, in the worst case this <code>jmp</code> needs to replace
 * not more than 5 instructions: 3 of 1-byte, and 2 of 16-bytes.
 * Then, to hold the recoded instructuions it is needed at least
 * 3 + 2 * 16 = 35 bytes long.
 */
#define KHOOK_SIZEOF_MAXRECODED		0x80 /* XXX */


/**
 * Maximum size of the hooking code.
 */
#define KHOOK_SIZEOF_MAXCODE		(KHOOK_OFFSET_RECODED + \
					KHOOK_SIZEOF_MAXRECODED + \
					SIZEOF_JMP32)

/*
 * Prototypes.
 */
int	disass_fetch(INS *, uint8_t **);
int	disass_put(uint8_t *, const INS *);
int	disass_recode(uint8_t *, const INS *, const uint8_t *);
size_t	khook(void *, void *, size_t *, long, void (*)(long, long, ...));


#endif	/* __KHOOK_H__ */
