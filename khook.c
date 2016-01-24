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
#include <sys/types.h>
#include <stdint.h>

#include "disass.h"
#include "khook.h"


/**
 * Offset within the hooking code containing
 * the original (re-encoded) instructions.
 */
#define KHOOK_OFFSET_RECODED	(SIZEOF_PUSH32 + SIZEOF_CALL32 + SIZEOF_POPEAX)


/**
 * Install a hook.
 * A hook is a replacement of one or more x86 instructions at a given
 * memory address with a jump to an automatically generated portion of
 * executable code (<i>hooking code</i>).<br>
 * When a call is delivered to a hooked memory address, the normal
 * execution is deviated and a user defined callback is called first.
 * If the user callback returns, the original code is executed.
 * </p>
 * @param fn Address to hook (the memory pages containing this and the
 * following addresses should have read/write permissions otherwise a
 * page fault will be generated);
 * @param hcode Destination address for the automatically generated
 * <i>hooking code</i>;
 * @param hsize Available size in the <code>hcode</code> buffer (not
 * smaller than <code>KHOOK_SIZEOF_MAXCODE</code>); on success it is updated
 * with the size of the generated code;
 * @param arg Value passed to the callback;
 * param callback User defined callback.
 * @return On success the hook's size (number of bytes replaced at the
 * begining of the hooked function); <code>0</code> on error.
 * This function fails if:
 * - <code>hsize</code> is less than <code>KHOOK_SIZEOF_MAXCODE</code>;
 * - <code>disass_fetch()<code> does not recognize at least one of the
 * instructions to replace;
 * - <code>disass_recode()<code> does not recognize at least one of the
 * instructions to recode.
 *
 * @see KHOOK_SIZEOF_MAXCODE
 * @see disass_fetch()
 * @see disass_recode()
 */
size_t
khook(void *fn, void *hcode, size_t *hsize, long arg,
    void (*callback)(long, long, ...))
{
	INS ins;
	uint8_t *dst, *src;
	size_t pissiz, copied, totsiz, gensiz;

	/*
	 * Generated code:
	 * hcode:
	 *    push arg
	 *    call callback
	 *    pop  arg
	 *
	 * recoded:
	 *    ...         ; Re-encoded instructions
	 *    ...
	 *
	 * jmporig:
	 *    jmp fn + d  ; Jump to the original code
	 */

	/*
	 * Check available space for the hooking code.
	 */
	if (*hsize < KHOOK_SIZEOF_MAXCODE) {
		return (0);
	}
	totsiz = *hsize;

	/*
	 * hcode:
	 *    push arg
	 *    call callback
	 *    pop  arg
	 */
	dst = (uint8_t *)hcode;
	*dst = OPCODE_PUSH32;
	*(uint32_t *)(dst + 1) = (uint32_t)arg;
	dst += SIZEOF_PUSH32;

	*dst = OPCODE_CALL32;
	*(uint32_t *)(dst + 1) =
	    (uint32_t)INS_ABS2REL(dst + SIZEOF_CALL32, callback);
	dst += SIZEOF_CALL32;

	*dst = OPCODE_POPEAX;
	dst += SIZEOF_POPEAX;

	/*
	 * Re-encode the original replaced instructions.
	 */
	pissiz = 0;
	dst = hcode + KHOOK_OFFSET_RECODED;
	src = (uint8_t *)fn;
	while (pissiz < SIZEOF_JMP32) {
		if (disass_fetch(&ins, &src) == 0) {
			/*
			 * Istruction not recognized.
			 */
			return (0);
		}
		pissiz += ins.size;
		copied = disass_recode(dst, &ins, src);
		if (copied == 0) {
			/*
			 * Unable to recode the fetched instruction.
			 */
			return (0);
		}
		totsiz -= copied;
		dst += copied;
	}

	/*
	 * jmporig:
	 *    jmp orig+d        ;Jump to the original code.
	 */
	*dst = OPCODE_JMP32;

	*(uint32_t *)(dst + 1) =
	    (uint32_t)INS_ABS2REL(dst + SIZEOF_JMP32, ((uint8_t *)fn) + pissiz);

	dst += SIZEOF_JMP32;

	/*
	 * Size of the generated code.
	 */
	gensiz = (dst - (uint8_t *)hcode);
	*hsize -= gensiz;

	/*
	 * Replace the original instructions with a jump to hcode.
	 */
	dst = (uint8_t *)fn;
	*dst = OPCODE_JMP32;
	*(uint32_t *)(dst + 1) =
	    (uint32_t)INS_ABS2REL(dst + SIZEOF_JMP32, hcode);

	return (gensiz);

}

