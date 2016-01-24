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
#include <string.h>
#include <stdint.h>

#include "opcodes.h"
#include "disass.h"

typedef struct _ins INS;


/**
 * Detect whether a given byte is an x86 instruction prefix.
 * @param byte Byte to analyze.
 * @return A non-zero value if the specified byte is an x86 instruction
 * prefix.
 */
static int
_isprefix(uint8_t byte)
{
	switch (byte) {
	case PREFIX1_LOCK:
	case PREFIX1_REPNZ:
	case PREFIX1_REPZ:
	case PREFIX2_CSSEG:
	case PREFIX2_SSSEG:
	case PREFIX2_DSSEG:
	case PREFIX2_ESSEG:
	case PREFIX2_FSSEG:
	case PREFIX2_GSSEG:
	case PREFIX3_OPSIZ:
	case PREFIX3_ADSIZ:
		return (1);
	default:;
	}
	return (0);
}


/**
 * Detect all prefixes of an x86 instruction.
 * @param ins Updated with the detected prefixes;
 * @param code Instruction address; at exit it is uptaded with
 * the address of the actual opcode.
 * @see _isprefix()
 */
static void
_detect_prefix(INS *ins, uint8_t **code)
{
	for (ins->prefixes = 0;
	    ins->prefixes < PREFIX_MAX && _isprefix(**code);
	    ins->prefixes++, (*code)++) {
		ins->prefix[ins->prefixes] = **code;
		if (**code == PREFIX3_OPSIZ) {
			ins->opsiz = 1;
		} else if (**code == PREFIX3_ADSIZ) {
			ins->adsiz = 1;
		}
	}
}


/**
 * Detect the opcode of an x86 instruction.
 * @param ins Updated with the detected opcode;
 * @param code Address with the opcode to detect; at exit it is updated
 * with the address that follows the detected opcode.
 * @todo Coprocessor opcodes (If a coporcessor opcode is detected a trap
 * interrupt is generated)
 */
static void
_detect_opcode(INS *ins, uint8_t **code)
{
	/*
	 * Get flags and store the first opcode byte.
	 */
	ins->flags = opcode1_map[**code];
	ins->opcode[0] = **code;
	ins->opcodes = 1;
	(*code)++;
	if (ins->flags & _E) {
		/*
		 * The opcode has 2 bytes.
		 */
		ins->flags = opcode2_map[**code];
		ins->opcode[ins->opcodes++] = **code;
		(*code)++;
	} else if (ins->flags & _C) {
		/*
		 * Coprocessor instruction.
		 * TODO
		 */
#ifdef WIN32
		_asm int 3
		_asm push 0xC0C0CACA
		_asm pop eax
#else
		asm("int $3\n"
		    "pushl $0xC0C0CACA\n"
		    "popl %eax\n");
#endif
	}
}


/**
 * Detect the MODRM of an x86 instruction.
 * @param ins Update with the MODRM;
 * @param code Address containing the instruction arguments; at exit
 * it is updated with the address that follows the detected MODRM.
 */
static void
_detect_modrm(INS *ins, uint8_t **code)
{
	if (ins->flags & _M) {
		ins->modrm = **code;
		(*code)++;
	}
}


/**
 * Detect the SIB of an x86 instruction.
 * @param ins Updated with the SIB;
 * @param code Address containing the instruction arguments; at exit
 * it is updated with the address that follows to the detected SIB.
 */
static void
_detect_sib(INS *ins, uint8_t **code)
{
	/*
	 * The SIB could be there only if MODRM is present.
	 */
	if ((ins->flags & _M) == 0) {
		return;
	}

	/*
	 * The SIB is only present on 32 bits operand only when the
	 * MODRM contains:
	 *	00xxx100
	 *	01xxx100
	 *	10xxx100
	 */
	if (ins->opsiz) {
		return;
	}
	switch (ins->modrm & 0xC7) {
	case 0x04:
	case 0x44:
	case 0x84:
		ins->has_sib = 1;
		ins->sib = **code;
		(*code)++;
	default:;
	}
}


/**
 * Detect the displacement value of an x86 instruction.
 * @param ins Updated with the displacement value;
 * @param code Address that potentially contains the displacement; at exit
 * it is updated with the address that follows the displacement.
 */
static void
_detect_disp(INS *ins, uint8_t **code)
{
	int type;

	/*
	 * The displacement could be there only if there is a MODRM.
	 */
	if ((ins->flags & _M) == 0) {
		return;
	}
	if (ins->adsiz) {
		/*
		 * On 16 bits there is a displacement if the MODRM contains:
		 *	00xxx110 -> 16 bits displacement
		 *	01xxxxxx -> 8 bits displacement
		 *	10xxxxxx -> 16 bits displacement
		 */
		switch (ins->modrm & 0xC0) {
		case 0x00:
			if ((ins->modrm & 0x07) == 0x06) {
				type = INS_PARAM_TYPE_WORD;
			}
			break;
		case 0x40:
			type = INS_PARAM_TYPE_BYTE;
			break;
		case 0x80:
			type = INS_PARAM_TYPE_WORD;
			break;
		default:
			return;
		}
	} else {
		/*
		 * On 32 bits there is a displacement if the MODRM contains:
		 *	00xxx101 -> 32 bits displacement
		 *	01xxxxxx -> 8 bits displacement
		 *	10xxxxxx -> 32 bits displacement
		 */
		switch (ins->modrm & 0xC0) {
		case 0x00:
			if ((ins->modrm & 0x07) == 0x05) {
				type = INS_PARAM_TYPE_DWORD;
			}
			break;
		case 0x40:
			type = INS_PARAM_TYPE_BYTE;
			break;
		case 0x80:
			type = INS_PARAM_TYPE_DWORD;
			break;
		default:
			return;
		}
	}
	ins->has_disp = 1;
	(*code) += insparam_set(&ins->disp, type, *code);
}


/**
 * Detect the immediate data of an x86 instruction.
 * @param ins Updated with the immediate data;
 * @param code Address containing the immediate data; at exit
 * it is updated with the address that follows the immediate data.
 */
static void
_detect_immd(INS *ins, uint8_t **code)
{
	int type;

	switch (ins->flags & (_b | _w | _d | _q | _v | _p | _s)) {
	case _b: /* BYTE */
		type = INS_PARAM_TYPE_BYTE;
		break;
	case _w: /* WORD */
		type = INS_PARAM_TYPE_WORD;
		break;
	case _d: /* DWORD */
		type = INS_PARAM_TYPE_DWORD;
		break;
	case _q: /* QWORD */
		type = INS_PARAM_TYPE_QWORD;
		break;
	case _v: /* WORD o DWORD */
		type = ins->opsiz ?
		    INS_PARAM_TYPE_WORD : INS_PARAM_TYPE_DWORD;
		break;
	case _p: /* DWORD u OFFSET:SELECTOR */
		type = ins->adsiz ?
		    INS_PARAM_TYPE_DWORD : INS_PARAM_TYPE_OFFSEL;
		break;
	case _s: /* WORD:BYTE */
		type = INS_PARAM_TYPE_D24;
		break;
	default:
		return;
	}
	ins->has_immd = 1;
	(*code) += insparam_set(&ins->immd, type, *code);
}


/**
 * Read a value.
 * @param param Updated with the value;
 * @param type Value type (one of
 * <code>INS_PARAM_TYPE_BYTE</code>,
 * <code>INS_PARAM_TYPE_WORD</code>,
 * <code>INS_PARAM_TYPE_DWORD</code>,
 * <code>INS_PARAM_TYPE_OFFSEL</code>);
 * @param data Value to read.
 * @return Number of bytes read.
 * @see insparam_copy()
 */
int
insparam_set(INS_PARAM *param, int type, void *data)
{
	param->type = type;
	switch (type) {
	case INS_PARAM_TYPE_BYTE:
		param->data.byte = *(uint8_t *)data;
		return (1);
	case INS_PARAM_TYPE_WORD:
		param->data.word = *(uint16_t *)data;
		return (2);
	case INS_PARAM_TYPE_D24:
		param->data.d24.word = *(uint16_t *)data;
		param->data.d24.byte = *(uint8_t *)((uint16_t *)data + 1);
		return (3);
	case INS_PARAM_TYPE_DWORD:
		param->data.dword = *(uint32_t *)data;
		return (4);
	case INS_PARAM_TYPE_OFFSEL:
		param->data.offsel.offset = *(uint32_t *)data;
		param->data.offsel.selector =
		    *(uint16_t *)((uint32_t *)data + 1);
		return (6);
	case INS_PARAM_TYPE_QWORD:
		param->data.qword = *(uint64_t *)data;
		return (8);
	case INS_PARAM_TYPE_NONE:
	default:;
	}
	return (0);
}


/**
 * Copy a value to a memory address.
 * @param dst Destination address;
 * @param param Value;
 * @return Number of bytes copied.
 * @see insparam_set()
 */
int
insparam_copy(uint8_t *dst, const INS_PARAM *param)
{
	switch (param->type) {
	case INS_PARAM_TYPE_BYTE:
		*dst = param->data.byte;
		return (1);
	case INS_PARAM_TYPE_WORD:
		*(uint16_t *)dst = param->data.word;
		return (2);
	case INS_PARAM_TYPE_DWORD:
		*(uint32_t *)dst = param->data.dword;
		return (4);
	case INS_PARAM_TYPE_OFFSEL:
		*(uint32_t *)dst = param->data.offsel.offset;
		*(uint16_t *)(dst + sizeof(uint32_t)) =
		    param->data.offsel.selector;
		return (6);
	case INS_PARAM_TYPE_QWORD:
		*(uint64_t *)dst = param->data.qword;
		return (8);
	default:;
	}
	return (0);
}


/**
 * Fetch an x86 instruction.
 * An x86 instruction is coded as follows:
 * <pre>
 *  +--------+--------+-------+-----+--------------------+-----------------+
 *  | prefix | opcode | modrm | sib | addr. displacement | immediate const |
 *  +--------+--------+-------+-----+--------------------+-----------------+
 * </pre>
 * @param ins Updated with the detected instruction;
 * @param code Address with the instruction; at exit it is updated with the
 * address of the next instruction.
 * @return The length of the detected instruction.
 * @see _detect_disp()
 * @see _detect_immd()
 * @see _detect_modrm()
 * @see _detect_opcode()
 * @see _detect_prefix()
 * @see _detect_sib()
 * @see disass_put()
 * @see disass_recode()
 */
int
disass_fetch(INS *ins, uint8_t **code)
{
	uint8_t *start;

	start = *code;
	bzero(ins, sizeof(INS));
	_detect_prefix(ins, code);
	_detect_opcode(ins, code);
	_detect_modrm(ins, code);
	_detect_sib(ins, code);
	_detect_disp(ins, code);
	_detect_immd(ins, code);
	ins->size = *code - start;
	return (ins->size);
}


/**
 * Write an x86 instruction as is in the holding structure.
 * @param dst Destination address;
 * @param src Instruction.
 * @return The instruction's length.
 * @see disass_fetch()
 * @see disass_recode()
 */
int
disass_put(uint8_t *dst, const INS *src)
{
	int i;

	for (i = 0; i < src->prefixes; i++, dst++) {
		*dst = src->prefix[i];
	}
	for (i = 0; i < src->opcodes; i++, dst++) {
		*dst = src->opcode[i];
	}
	if (src->flags & _M) {
		*dst = src->modrm;
		dst++;
	}
	if (src->has_sib) {
		*dst = src->sib;
		dst++;
	}
	if (src->has_disp) {
		dst += insparam_copy(dst, &src->disp);
	}
	if (src->has_immd) {
		(void)insparam_copy(dst, &src->immd);
	}
	return (src->size);
}


/**
 * Recode and write an x86 instruction.
 * Recode an instruction in order to make it valid in the new destination
 * address.
 * @param dstaddr Destination address where the instruction should be written;
 * @param src Instruction to recode and write;
 * @param srcaddr Address from where the instruction was fetched.
 * @return On success the length of the new written instruction;
 * <code>0</code> on error.
 * @see disass_fetch()
 * @see disass_put()
 */
int
disass_recode(uint8_t *dstaddr, const INS *src, const uint8_t *srcaddr)
{
	static uint8_t ncode[] = {
		0x00,		0x02,
		OPCODE_JMP8,	0x05,
		OPCODE_JMP32,	0x00, 0x00, 0x00, 0x00
	};
	INS ni;
	uint32_t reladdr, i;

	/*
	 * An instruction with a relative value from srcaddr
	 * must be recoded to make it relative from dstaddr.
	 */
	if (src->flags & _R) {
		bzero(&ni, sizeof(ni));

		/*
		 * Recode CALL32, CALL16, JMP32 and JMP16:
		 * If it is a 16 bits instruction change it with its 32 bits
		 * counterpart.
		 */
		if (src->opcodes == 1 &&
		    (src->opcode[0] == OPCODE_CALL32 ||
		    src->opcode[0] == OPCODE_JMP32)) {
			ni.opcodes = 1;
			ni.flags = _R | _v;
			ni.opcode[0] = src->opcode[0];
			i = (uint32_t)((src->opsiz) ?
			    src->immd.data.word : src->immd.data.dword);
			reladdr = (uint32_t)
			    INS_ABS2REL(dstaddr + SIZEOF_CALL32, srcaddr + i);
			insparam_set(&ni.immd, INS_PARAM_TYPE_DWORD, &reladdr);
			ni.has_immd = 1;
			ni.size = SIZEOF_CALL32;
			return (disass_put(dstaddr, &ni));
		}

		/*
		 * Recode JMP8: Change it with a JMP32.
		 */
		if (src->opcodes == 1 && src->opcode[0] == OPCODE_JMP8) {
			ni.opcodes = 1;
			ni.flags = _R | _v;
			ni.opcode[0] = OPCODE_JMP32;
			reladdr = (uint32_t)
			    INS_ABS2REL(dstaddr + SIZEOF_JMP32,
			    srcaddr + src->immd.data.byte);
			insparam_set(&ni.immd, INS_PARAM_TYPE_DWORD, &reladdr);
			ni.has_immd = 1;
			ni.size = SIZEOF_JMP32;
			return (disass_put(dstaddr, &ni));
		}

		/*
		 * Recode 8 bits conditions:
		 * Change the condition with its 32 bits counterpart.
		 */
		if (src->opcodes == 1 &&
		    (src->opcode[0] >= 0x70 && src->opcode[0] < 0x7F)) {
			ni.opcodes = 2;
			ni.opcode[0] = OPCODE_ESCAPE;
			ni.opcode[1] = src->opcode[0] + 0x10;
			ni.flags = _R | _v;
			reladdr = (uint32_t)
			    INS_ABS2REL(dstaddr + SIZEOF_J32,
			    srcaddr + src->immd.data.byte);
			insparam_set(&ni.immd, INS_PARAM_TYPE_DWORD, &reladdr);
			ni.has_immd = 1;
			ni.size = SIZEOF_J32;
			return (disass_put(dstaddr, &ni));
		}

		/*
		 * Recode 16/32 bits conditions:
		 * Change the condition with its 32 bits counterpart.
		 */
		if (src->opcodes == 2 &&
		    (src->opcode[0] == OPCODE_ESCAPE &&
		    (src->opcode[1] >= 0x80 && src->opcode[1] <= 0x8F))) {
			ni.opcodes = 2;
			ni.opcode[0] = src->opcode[0];
			ni.opcode[1] = src->opcode[1];
			ni.flags = _R | _v;
			i = (uint32_t)((src->opsiz) ?
			    src->immd.data.word : src->immd.data.dword);
			reladdr = (uint32_t)
			    INS_ABS2REL(dstaddr + SIZEOF_J32, srcaddr + i);
			insparam_set(&ni.immd, INS_PARAM_TYPE_DWORD, &reladdr);
			ni.has_immd = 1;
			ni.size = SIZEOF_J32;
			return (disass_put(dstaddr, &ni));
		}

		/*
		 * Recode LOOPN, LOOPE, LOOP, and JCXZ:
		 * Change the instruction:
		 *	srcaddr:	loop/loopn/loope/jcxz caddr
		 *
		 * using the following sequence:
		 *	dstaddr:	loop/loopn/loope/jcxz dstaddr+4
		 *			jmp8  dstaddr+7
		 *	dstaddr+4:	jmp32 caddr
		 *	dstaddr+7:	...
		 */
		if (src->opcodes == 1 &&
		    (src->opcode[0] >= 0xE0 && src->opcode[0] <= 0xE3)) {
			memcpy(dstaddr, ncode, sizeof(ncode));
			*dstaddr = src->opcode[0];
			reladdr = (uint32_t)
			    INS_ABS2REL(dstaddr + sizeof(ncode),
			    srcaddr + src->immd.data.byte);
			*(uint32_t *)(dstaddr + 5) = reladdr;
			return (sizeof(ncode));
		}

		/*
		 * Relative instruction not recognized. Error.
		 */
		return (0);
	}

	/*
	 * The instruction has no relative values. Store it normally.
	 */
	return (disass_put(dstaddr, src));
}

