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
#ifndef OPCODES_H
#define OPCODES_H


#define _M		0x0001	/**< Instruction with MODRM		*/
#define _R		0x0002	/**< Instruction with a relative value	*/
#define _b		0x0004	/**< Immediate data is byte		*/
#define _w		0x0008	/**< Immediate data is word		*/
#define _d		0x0010	/**< Immediate data is dword		*/
#define _q		0x0020	/**< Immediate data is qword		*/
#define _v		0x0040	/**< Immediate data is word o dword	*/
#define _p		0x0080	/**< Immediate data is dword o offsel	*/
#define _s		0x0100	/**< Immediate data is 24 bits		*/
#define _E		0x4000	/**< 2 bytes opcode escape		*/
#define _C		0x8000	/**< Coprocessor escape			*/


/**
 * @var opcode1_map[256]
 * 1 byte length opcodes map; each element contains flags specifying
 * type and arguments for the instruction.
 * @see Intel Architecture Software Developer's Manual Volume 2:
 * Instruction Set Reference
 * @see opcode2_map
 */
static uint32_t opcode1_map[256] = {
/* 00 */
	_M,		/* ADD  Eb,  Gb			*/
	_M,		/* ADD  Ev,  Gv			*/
	_M,		/* ADD  Gb,  Eb			*/
	_M,		/* ADD  Gv,  Ev			*/
	_b,		/* ADD  AL,  Ib			*/
	_v,		/* ADD  eAX, Iv			*/
	0,		/* PUSH ES			*/
	0,		/* POP  ES			*/

/* 08 */
	_M,		/* OR   Eb,  Gb			*/
	_M,		/* OR   Ev,  Gv			*/
	_M,		/* OR   Gb,  Eb			*/
	_M,		/* OR   Gv,  Ev			*/
	_b,		/* OR   AL,  Ib			*/
	_v,		/* OR   eAX, Iv			*/
	0,		/* PUSH CS			*/
	_E,		/* Escape. 2 bytes opcode	*/

/* 10 */
	_M,		/* ADC  Eb,  Gb			*/
	_M,		/* ADC  Ev,  Gv			*/
	_M,		/* ADC  Gb,  Eb			*/
	_M,		/* ADC  Gv,  Ev			*/
	_b,		/* ADC  AL,  Ib			*/
	_v,		/* ADC  eAX, Iv			*/
	0,		/* PUSH SS			*/
	0,		/* POP SS			*/

/* 18 */
	_M,		/* SBB  Eb,  Gb			*/
	_M,		/* SBB  Ev,  Gv			*/
	_M,		/* SBB  Gb,  Eb			*/
	_M,		/* SBB  Gv,  Ev			*/
	_b,		/* SBB  AL,  Ib			*/
	_v,		/* SBB  eAX, Iv			*/
	0,		/* PUSH DS			*/
	0,		/* POP  DS			*/

/* 20 */
	_M,		/* AND  Eb,  Gb			*/
	_M,		/* AND  Ev,  Gv			*/
	_M,		/* AND  Gb,  Eb			*/
	_M,		/* AND  Gv,  Ev			*/
	_b,		/* AND  AL,  Ib			*/
	_v,		/* AND  eAX, Iv			*/
	0,		/* PREFIX ES			*/
	0,		/* DAA				*/

/* 28 */
	_M,		/* SUB  Eb,  Gb			*/
	_M,		/* SUB  Ev,  Gv			*/
	_M,		/* SUB  Gb,  Eb			*/
	_M,		/* SUB  Gv,  Ev			*/
	_b,		/* SUB  AL,  Ib			*/
	_v,		/* SUB  eAX, Iv			*/
	0,		/* PREFIX CS			*/
	0,		/* DAS				*/

/* 30 */
	_M,		/* XOR  Eb,  Gb			*/
	_M,		/* XOR  Ev,  Gv			*/
	_M,		/* XOR  Gb,  Eb			*/
	_M,		/* XOR  Gv,  Ev			*/
	_b,		/* XOR  AL,  Ib			*/
	_v,		/* XOR  eAX, Iv			*/
	0,		/* PREFIX SS			*/
	0,		/* AAA				*/

/* 38 */
	_M,		/* CMP  Eb,  Gb			*/
	_M,		/* SEG  Ev,  Gv			*/
	_M,		/* AAS  Eb,  Gb			*/
	_M,		/*      Ev,  Gv			*/
	_b,		/*      AL,  Ib			*/
	_v,		/*      eAX, Iv			*/
	0,		/* PREFIX DS			*/
	0,		/*				*/

/* 40 */
	0,		/* INC  eAX			*/
	0,		/* INC  eCX			*/
	0,		/* INC  eDX			*/
	0,		/* INC  eBX			*/
	0,		/* INC  eSP			*/
	0,		/* INC  eBP			*/
	0,		/* INC  eSI			*/
	0,		/* INC  eDI			*/

/* 48 */
	0,		/* DEC  eAX			*/
	0,		/* DEC  eCX			*/
	0,		/* DEC  eDX			*/
	0,		/* DEC  eBX			*/
	0,		/* DEC  eSP			*/
	0,		/* DEC  eBP			*/
	0,		/* DEC  eSI			*/
	0,		/* DEC  eDI			*/

/* 50 */
	0,		/* PUSH  eAX			*/
	0,		/* PUSH  eCX			*/
	0,		/* PUSH  eDX			*/
	0,		/* PUSH  eBX			*/
	0,		/* PUSH  eSP			*/
	0,		/* PUSH  eBP			*/
	0,		/* PUSH  eSI			*/
	0,		/* PUSH  eDI			*/

/* 58 */
	0,		/* POP  eAX			*/
	0,		/* POP  eCX			*/
	0,		/* POP  eDX			*/
	0,		/* POP  eBX			*/
	0,		/* POP  eSP			*/
	0,		/* POP  eBP			*/
	0,		/* POP  eSI			*/
	0,		/* POP  eDI			*/

/* 60 */
	0,		/* PUSHA			*/
	0,		/* POPA				*/
	_M,		/* BOUND Gv, Ma			*/
	_M,		/* ARPL  Ew, Gw			*/
	0,		/* PREFIX FS			*/
	0,		/* PREFIX GS			*/
	0,		/* PREFIX OPSIZ			*/
	0,		/* PREFIX ADSIZ			*/

/* 68 */
	_v,		/* PUSH  Iv			*/
	_M | _v,	/* IMUL Gv, Ev, Iv		*/
	_b,		/* PUSH  Ib			*/
	_M | _b,	/* IMUL Gv, Ev, Ib		*/
	0,		/* INSB  Yb,  DX	XXX	*/
	0,		/* INSW/D Yv, DX	XXX	*/
	0,		/* OUTSB  DX, Xb	XXX	*/
	0,		/* OUTSW/D Dx, Xv	XXX	*/

/* 70 */
	_R | _b,	/* JO   rel8			*/
	_R | _b,	/* JNO  rel8			*/
	_R | _b,	/* JB/JNAE/JC  rel8		*/
	_R | _b,	/* JNB/JAE/JNC rel8		*/
	_R | _b,	/* JZ   rel8			*/
	_R | _b,	/* JNZ  rel8			*/
	_R | _b,	/* JBE  rel8			*/
	_R | _b,	/* JNBE rel8			*/

/* 78 */
	_R | _b,	/* JS   rel8			*/
	_R | _b,	/* JNS  rel8			*/
	_R | _b,	/* JP   rel8			*/
	_R | _b,	/* JNP  rel8			*/
	_R | _b,	/* JL   rel8			*/
	_R | _b,	/* JNL  rel8			*/
	_R | _b,	/* JLE  rel8			*/
	_R | _b,	/* JNLE rel8			*/

/* 80 */
	_M | _b,	/* Immediate Group 1		*/
	_M | _v,	/* Immediate Group 1		*/
	_M | _b,	/* Immediate Group 1		*/
	_M | _b,	/* Immediate Group 1		*/
	_M,		/* TEST Eb, Gb			*/
	_M,		/* TEST Ev, Gv			*/
	_M,		/* XCHG Eb, Gb			*/
	_M,		/* XCHG Ev, Gv			*/

/* 88 */
	_M,		/* MOV  Eb, Gb			*/
	_M,		/* MOV  Ev, Gv			*/
	_M,		/* MOV  Gb, Eb			*/
	_M,		/* MOV  Gv, Ev			*/
	_M,		/* MOV  Ew, Sw			*/
	_M,		/* LEA  Gv, M			*/
	_M,		/* MOV  Sw, Ew			*/
	_M,		/* POP  Ev			*/

/* 90 */
	0,		/* NOP				*/
	0,		/* XGHG eAX, eCX		*/
	0,		/* XGHG eAX, eDX		*/
	0,		/* XGHG eAX, eBX		*/
	0,		/* XGHG eAX, eSP		*/
	0,		/* XGHG eAX, eBP		*/
	0,		/* XGHG eAX, eSI		*/
	0,		/* XGHG eAX, eDI		*/

/* 98 */
	0,		/* CBW				*/
	0,		/* CWD/CDQ			*/
	0,		/* CALL Ap			*/
	0,		/* WAIT				*/
	0,		/* PUSHF			*/
	0,		/* POPF				*/
	0,		/* SAHF				*/
	0,		/* LAHF				*/

/* A0 */
	_v,		/* MOV  AL,  Ob		XXX	*/
	_d,		/* MOV  eAX, Ov			*/
	_v,		/* MOV  Ob,  AL		XXX	*/
	_v,		/* MOV  Ov,  eAX	XXX	*/
	_M,		/* MOVSB Xb, Yb		XXX	*/
	_M,		/* MOVSW Xv, Yv		XXX	*/
	_M,		/* CMPSB Xb, Yb		XXX	*/
	_M,		/* CMPSW Xv, Yv		XXX	*/

/* A8 */
	_b,		/* TEST AL, Ib			*/
	_v,		/* TEST eAX, Iv			*/
	_M,		/* STOSB Yb, AL		XXX	*/
	_M,		/* STOSW/D Yv, eAX	XXX	*/
	_M,		/* LODSB AL, Xb		XXX	*/
	_M,		/* LODSW/D eAX, Xv	XXX	*/
	_M,		/* SCASB AL, Yb		XXX	*/
	_M,		/* SCASW/D AL, Yb	XXX	*/

/* B0 */
	_b,		/* MOV  AL,  b			*/
	_b,		/* MOV  CL,  b			*/
	_b,		/* MOV  DL,  b			*/
	_b,		/* MOV  BL,  b			*/
	_b,		/* MOV  AH,  b			*/
	_b,		/* MOV  CH,  b			*/
	_b,		/* MOV  DH,  b			*/
	_b,		/* MOV  BH,  b			*/

/* B8 */
	_v,		/* MOV eAX, v			*/
	_v,		/* MOV eCX, v			*/
	_v,		/* MOV eDX, v			*/
	_v,		/* MOV eBX, v			*/
	_v,		/* MOV eSP, v			*/
	_v,		/* MOV eBP, v			*/
	_v,		/* MOV eSI, v			*/
	_v,		/* MOV eDI, v			*/

/* C0 */
	_M | _b,	/* Shift Group 2a		*/
	_M | _b,	/* Shift Group 2a		*/
	_w,		/* RET  near			*/
	0,		/* RET  near			*/
	_M,		/* LES  Gv, Mp			*/
	_M,		/* LDS  Gv, Mp			*/
	_M | _b,	/* MOV  Eb, Ib			*/
	_M | _v,	/* MOV  Ev, Iv			*/

/* C8 */
	_s,		/* ENTER Iw, Ib			*/
	0,		/* LEAVE			*/
	_w,		/* RET far			*/
	0,		/* RET far			*/
	0,		/* INT 3			*/
	_b,		/* INT Ib			*/
	0,		/* INTO				*/
	0,		/* IRET				*/

/* D0 */
	_M,		/* Shift Group 2		*/
	_M,		/* Shift Group 2		*/
	_M,		/* Shift Group 2		*/
	_M,		/* Shift Group 2		*/
	0,		/* AAM				*/
	0,		/* AAM				*/
	0,		/*				*/
	0,		/* XLAT				*/

/* D8 */
	_C,		/* Escape Copro			*/
	_C,		/* Escape Copro			*/
	_C,		/* Escape Copro			*/
	_C,		/* Escape Copro			*/
	_C,		/* Escape Copro			*/
	_C,		/* Escape Copro			*/
	_C,		/* Escape Copro			*/
	_C,		/* Escape Copro			*/

/* E0 */
	_R | _b,	/* LOOPN Jb			*/
	_R | _b,	/* LOOPE Jb			*/
	_R | _b,	/* LOOP  Jb			*/
	_R | _b,	/* JCXZ/JECXZ Jb		*/
	_b,		/* IN AL,  Ib			*/
	_b,		/* IN eAX, Ib			*/
	_b,		/* OUT Ib, AL			*/
	_b,		/* OUT Ib, eAX			*/

/* E8 */
	_R | _v,	/* CALL Jv			*/
	_R | _v,	/* JMP  Jv			*/
	_p,		/* JMP  Ap			*/
	_R | _b,	/* JMP  Jb			*/
	0,		/* IN   AL, DX			*/
	0,		/* IN  eAX, DX			*/
	0,		/* OUT DX,  AL			*/
	0,		/* OUT DX,  eAX			*/

/* F0 */
	0,		/* PREFIX LOCK			*/
	0,		/*				*/
	0,		/* PREFIX REPNE			*/
	0,		/* PREFIX REP/REPE		*/
	0,		/* HLT				*/
	0,		/* CMC				*/
	_M,		/* Group 3			*/
	_M,		/* Group 3			*/

/* F8 */
	0,		/* CLC				*/
	0,		/* STC				*/
	0,		/* CLI				*/
	0,		/* STI				*/
	0,		/* CLD				*/
	0,		/* STD				*/
	_M,		/* INC/DEC Group 4 XXX		*/
	_M		/* INC/DEC Group 5 XXX		*/
};


/**
 * @var opcode2_map[256]
 * 2 bytes length opcodes map; each element contains flags specifying
 * type and arguments for the instruction.
 * @see Intel Architecture Software Developer's Manual Volume 2:
 * Instruction Set Reference
 * @see opcode1_map
 */
static uint32_t opcode2_map[256] = {
/* 00 */
	_M,		/* Group 6			*/
	0,
	_M,		/* LAR  Gv, Ew			*/
	_M,		/* LSL  Gv, Ew			*/
	0,
	0,
	0,		/* CLTS				*/
	0,

/* 08 */
	0,		/* INVD				*/
	0,		/* WBINVD			*/
	0,
	0,		/* UD2			XXX	*/
	0,
	0,
	0,
	0,

/* 10 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

/* 18 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

/* 20 */
	_M,		/* MOV  Rd, Cd			*/
	_M,		/* MOV  Rd, Dd			*/
	_M,		/* MOV  Cd, Rd			*/
	_M,		/* MOV  Dd, Rd			*/
	0,
	0,
	0,
	0,

/* 28 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

/* 30 */
	0,		/* WRMSR			*/
	0,		/* RDTSC			*/
	0,		/* RDMSR			*/
	0,		/* RDPMC			*/
	0,
	0,
	0,
	0,

/* 38 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

/* 40 */
	_M,		/* CMOVO Gv, Ev			*/
	_M,		/* CMOVO Ev, Gv			*/
	_M,		/* CMOVB/CMOVC/CMOVNAE Gv, Ev	*/
	_M,		/* CMOVB/CMOVC/CMOVNAE Gv, Ev	*/
	_M,		/* CMOVE/CMOVZ Gv, Ev		*/
	_M,		/* CMOVNE/CMOVNZ Gv, Ev		*/
	_M,		/* CMOVBE/CMOVNA Gv, Ev		*/
	_M,		/* CMOVCA/CMOVNBE Gv, Ev	*/

/* 48 */
	_M,		/* CMOVS Gv, Ev			*/
	_M,		/* CMOVNS Gv, Ev		*/
	_M,		/* CMOVP/CMOVPE Gv, Ev		*/
	_M,		/* CMPVNP/CMOVPO Gv, Ev		*/
	_M,		/* CMOVGE/CMOVNL Gv, Ev		*/
	_M,		/* CMOVGE/CMOVNL Gv, Ev		*/
	_M,		/* CMOVLE/CMOVNG Gv, Ev		*/
	_M,		/* CMOVG/CMOVNLE Gv, Ev		*/

/* 50 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

/* 58 */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

/* 60 */
	_M,		/* PUNPCKLBW Pq, Qd		*/
	_M,		/* PUNPCKLWD Pq, Qd		*/
	_M,		/* PUNOCKLDQ Pq, Qd		*/
	_M,		/* PACKUSDW Pq, Qd		*/
	_M,		/* PCMPGTB Pq, Qd		*/
	_M,		/* PCMPGTW Pq, Qd		*/
	_M,		/* PCMPGTD Pq, Qd		*/
	_M,		/* PACKSSWB Pq, Qd		*/

/* 68 */
	_M,		/* PUNPCKHBW Pq, Qd		*/
	_M,		/* PUNPCKHWD Pq, Qd		*/
	_M,		/* PUNPCKHDQ Pq, Qd		*/
	_M,		/* PACKSSDW Pq, Qd		*/
	0,
	0,
	_M,		/* MOVD Pd, Ed			*/
	_M,		/* MOVQ Pq, Qq			*/

/* 70 */
	0,
	0,		/* PSHIMW			*/
	0,		/* PSHIMD			*/
	0,		/* PSHIMQ			*/
	_M,		/* PCMPEQB Pq, Qd		*/
	_M,		/* PCMPEQW Pq, Qd		*/
	_M,		/* PCMPEQD Pq, Qd		*/
	0,		/* EMMS				*/

/* 78 */
	0,
	0,
	0,
	0,
	0,
	0,
	_M,		/* MOVD Ed, Pd			*/
	_M,		/* MOVQ Qq, Pq			*/

/* 80 */
	_R | _v,	/* JO   rel32			*/
	_R | _v,	/* JNO  rel32			*/
	_R | _v,	/* JB   rel32			*/
	_R | _v,	/* JNB  rel32			*/
	_R | _v,	/* JZ   rel32			*/
	_R | _v,	/* JNZ  rel32			*/
	_R | _v,	/* JBE  rel32			*/
	_R | _v,	/* JNBE rel32			*/

/* 88 */
	_R | _v,	/* JS   rel32			*/
	_R | _v,	/* JNS  rel32			*/
	_R | _v,	/* JP   rel32			*/
	_R | _v,	/* JNP  rel32			*/
	_R | _v,	/* JL   rel32			*/
	_R | _v,	/* JNL  rel32			*/
	_R | _v,	/* JLE  rel32			*/
	_R | _v,	/* JNLE rel32			*/

/* 90 */
	0,		/* SETO				*/
	0,		/* SETNO			*/
	0,		/* SETB				*/
	0,		/* SETNB			*/
	0,		/* SETZ				*/
	0,		/* SETNZ			*/
	0,		/* SETBE			*/
	0,		/* SETNBE			*/

/* 98 */
	0,		/* SETS				*/
	0,		/* SETNS			*/
	0,		/* SETP				*/
	0,		/* SETNP			*/
	0,		/* SETL				*/
	0,		/* SETNL			*/
	0,		/* SETLE			*/
	0,		/* SETNLE			*/

/* A0 */
	0,		/* PUSH FS			*/
	0,		/* POP  FS			*/
	0,		/* CPUID			*/
	_M,		/* BT Ev, Gv			*/
	_M | _b,	/* SHLD Ev, Gv, Ib		*/
	_M,		/* SHLD Ev, Gv, CL		*/
	0,
	0,

/* A8 */
	0,		/* PUSH GS			*/
	0,		/* POP  GS			*/
	0,		/* RSM				*/
	_M,		/* BTS Ev, Gv			*/
	_M | _b,	/* SHRD Ev, Gv, Ib		*/
	_M,		/* SHRD Ev, Gv, CL		*/
	0,
	_M,		/* IMUL Gv, Ev			*/

/* B0 */
	_M,		/* CMPXCHG Eb, Gb		*/
	_M,		/* CMPXCHG Ev, Gv		*/
	_M,		/* LSS Mp			*/
	_M,		/* BTR Ev, Gv			*/
	_M,		/* LFS Mp			*/
	_M,		/* LGS Mp			*/
	_M,		/* MOVZX Gv, Eb			*/
	_M,		/* MOVZX Gv, Ew			*/

/* B8 */
	0,
	0,		/* Invalid code			*/
	_M | _b,	/* Group 8(Ev, Ib)		*/
	_M,		/* BTC Ev, Gv			*/
	_M,		/* BSF Gv, Ev			*/
	_M,		/* BSR Gv, Ev			*/
	_M,		/* MOVSX Gv, Eb			*/
	_M,		/* MOVSX Gv, Ew			*/

/* C0 */
	_M,		/* XADD Eb, Gb			*/
	_M,		/* XADD Ev, Gv			*/
	0,
	0,
	0,
	0,
	0,
	0,		/* Group 9			*/

/* C8 */
	0,		/* BSWAP EAX			*/
	0,		/* BSWAP ECX			*/
	0,		/* BSWAP EDX			*/
	0,		/* BSWAP EBX			*/
	0,		/* BSWAP ESP			*/
	0,		/* BSWAP EBP			*/
	0,		/* BSWAP ESI			*/
	0,		/* BSWAP EDI			*/

/* D0 */
	0,
	_M,		/* PSRLW Pq, Qd			*/
	_M,		/* PSRLD Pq, Qd			*/
	_M,		/* PSRLQ Pq, Qd			*/
	0,
	_M,		/* PMULLW Pq, Qd		*/
	0,
	0,

/* D8 */
	_M,		/* PSUBUSB Pq, Qq		*/
	_M,		/* PSUBUSW Pq, Qq		*/
	0,
	_M,		/* PAND Pq, Qq			*/
	_M,		/* PADDUSD Pq, Qq		*/
	_M,		/* PADDUSW Pq, Qq		*/
	0,
	_M,		/* PANDN Pq, Qq			*/

/* E0 */
	0,
	_M,		/* PSRAW Pq, Qd			*/
	_M,		/* PSRAD Pq, Qd			*/
	0,
	0,
	_M,		/* PMULHW Pq, Qd		*/
	0,
	0,

/* E8 */
	_M,		/* PSUBSB Pq, Qd		*/
	_M,		/* PSUBSW Pq, Qd		*/
	0,
	_M,		/* POR Pq, Qd			*/
	_M,		/* PADDSB Pq, Qd		*/
	_M,		/* PADDSW Pq, Qd		*/
	0,
	_M,		/* PXOR Pq, Qd			*/

/* F0 */
	0,
	_M,		/* PSLLW Pq, Qd			*/
	_M,		/* PSLLD Pq, Qd			*/
	_M,		/* PSLLQ Pq, Qd			*/
	0,
	_M,		/* PMADDWD Pq, Qd		*/
	0,
	0,

/* F8 */
	_M,		/* PSUBB Pq, Qd			*/
	_M,		/* PSUBW Pq, Qd			*/
	_M,		/* PSUBD Pq, Qd			*/
	0,
	_M,		/* PADDB Pq, Qd			*/
	_M,		/* PADDW Pq, Qd			*/
	_M,		/* PADDD Pq, Qd			*/
	0
};


#endif	/* OPCODES_H */
