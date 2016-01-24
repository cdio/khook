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
 *     * Neither the name of the author nor the names of his contributors
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
#ifndef DISASS_H
#define DISASS_H


#define PREFIX1_LOCK	0xF0	/**< Prefix LOCK			*/
#define PREFIX1_REPNZ	0xF1	/**< Prefix REPNE/REPNZ			*/
#define PREFIX1_REPZ	0xF2	/**< Prefix REP/REPE/REPZ		*/
#define PREFIX2_CSSEG	0x2E	/**< Prefix CS Override			*/
#define PREFIX2_SSSEG	0x36	/**< Prefix SS Override			*/
#define PREFIX2_DSSEG	0x3E	/**< Prefix DS Override			*/
#define PREFIX2_ESSEG	0x26	/**< Prefix ES Override			*/
#define PREFIX2_FSSEG	0x64	/**< Prefix FS Override			*/
#define PREFIX2_GSSEG	0x65	/**< Prefix GS Override			*/
#define PREFIX3_OPSIZ	0x66	/**< Prefix Operand Size Override	*/
#define PREFIX3_ADSIZ	0x67	/**< Prefix Address Size Override	*/

#define PREFIX_MAX	4	/**< Max number of prefixes		*/
#define OPCODE_MAX	2	/**< Max number of opcodes		*/

#define OPCODE_CALL32	0xE8	/**< CALL rel32 opcode			*/
#define OPCODE_JMP32	0xE9	/**< JMP rel32 opcode			*/
#define OPCODE_JMP8	0xEB	/**< JMP rel8 opcode			*/
#define OPCODE_NOP	0x90	/**< NOP opcode				*/
#define OPCODE_PUSH32	0x68	/**< PUSH data32 opcode			*/
#define OPCODE_ESCAPE	0x0F	/**< Escape to 2 bytes opcode		*/
#define OPCODE_PUSHEAX	0x50	/**< PUSH EAX opcode			*/
#define OPCODE_POPEAX	0x58	/**< POP EAX opcode			*/
#define OPCODE_ADDEAX32	0x05	/**< ADD EAX, data32 opcode		*/

#define SIZEOF_CALL32	5	/**< Length of CALL rel32		*/
#define SIZEOF_JMP32	5	/**< Length of JMP rel32		*/

#define SIZEOF_J32	5	/**< Length of Jxx rel32		*/

#define SIZEOF_PUSH32	5	/**< Length of PUSH data32		*/
#define SIZEOF_PUSHEAX	1	/**< Length of PUSH EAX			*/
#define SIZEOF_POPEAX	1	/**< Length of POP EAX			*/
#define SIZEOF_ADDEAX32	5	/**< Length of ADD EAX, data32		*/


/**
 * Data container.
 * Holds a byte (8 bits), word (16 bits), dword (32 bits),
 * qword (64 bits) or a memory address specifying selector
 * and offset (46 bits).
 */
typedef struct _ins_param {
	/**
	 * Type of data.
	 */
	int	type;
#define INS_PARAM_TYPE_BYTE	 0	/**< type is Byte		*/
#define INS_PARAM_TYPE_WORD	 1	/**< type is Word		*/
#define INS_PARAM_TYPE_DWORD	 2	/**< type is DWord		*/
#define INS_PARAM_TYPE_OFFSEL	 3	/**< type is Offset+Selector	*/
#define INS_PARAM_TYPE_QWORD	 4	/**< type is QWORD		*/
#define INS_PARAM_TYPE_D24	 5	/**< type is word:byte		*/
#define INS_PARAM_TYPE_NONE	-1

	/**
	 * Data.
	 */
	union {
		uint8_t	 byte;		/**< Byte (8 bits)		*/
		uint16_t word;		/**< Word (16 bits)		*/
		uint32_t dword;		/**< DWord (32 bits)		*/
		uint64_t qword;		/**< QWord (64 bits)		*/

		/**
		 * Offset+Selector (48 bits)
		 */
		struct {
			uint32_t offset;
			uint16_t selector;
		} offsel;

		/**
		 * Word+Byte (24 bits).
		 */
		struct {
			uint16_t word;
			uint8_t byte;
		} d24;
	} data;
} INS_PARAM;


/**
 * Instruction container.
 * Contains detailed information about an x86 instruction.
 * An instruction is coded as follows:
 * <pre>
 *  +--------+--------+-------+-----+--------------------+-----------------+
 *  | prefix | opcode | modrm | sib | addr. displacement | immediate const |
 *  +--------+--------+-------+-----+--------------------+-----------------+
 * </pre>
 * The length of an isntruction is variable from 1 byte to 16 bytes.
 * @see INS_PARAM
 */
struct _ins {
	int	 size;			/**< Instruction length		*/
	uint32_t flags;			/**< Flags			*/
	int	 prefixes;		/**< No. of prefixes		*/
	int	 opcodes;		/**< No. of opcodes		*/
	int	 has_sib;		/**< Have SIB			*/
	int	 has_disp;		/**< Have displacement		*/
	int	 has_immd;		/**< Have immediate data	*/
	int	 opsiz;			/**< Prefix operand size	*/
	int	 adsiz;			/**< Prefix address size	*/
	int	 rel;			/**< Have relative value	*/
	uint8_t	 prefix[PREFIX_MAX];	/**< Prefixes			*/
	uint8_t	 opcode[OPCODE_MAX];	/**< Opcodes			*/
	uint8_t	 modrm;			/**< MODRM			*/
	uint8_t	 sib;			/**< SIB			*/
	INS_PARAM disp;			/**< Displacement		*/
	INS_PARAM immd;			/**< Immediate data		*/
};


/**
 * @def INS_ABS2REL(base, abs)
 * Generate a relative address.
 * @param base Base address;
 * @param abs Absolute address.
 */
#define INS_ABS2REL(base, abs)	((uint8_t *)(abs) - (uint8_t *)(base))

int	insparam_set(INS_PARAM *, int, void *);
int	insparam_copy(uint8_t *, const INS_PARAM *);


#endif	/* DISASS_H */
