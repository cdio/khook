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
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#ifdef USE_SYSCALL
#include <sys/syscall.h>
#endif
#include <dlfcn.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <khook.h>


#define HCODE_MAXSIZ	0x1000
static void *hcode;


static void
my_printf(long id, long ra, const char *fmt, ...)
{
	static int line = 1;

	fprintf(stdout, "%d> ", line++);

	switch ((long)fmt) {
	case 0:
		puts("I will not let you call printf with null fmt");

		/* Do not call printf */
		*(&id - 1) = ra;
		break;
	case 1:
		fmt = ">>>>>%s<<<<<\n";
		break;
	case 2:
		fmt = "\x1b[5;31;44m %s \x1b[0m\n";
		break;
	default:;
	}
}


static void
hook_printf()
{
	size_t avail = HCODE_MAXSIZ;
	void *printf_page;

#ifndef USE_SYSCALL
	hcode = mmap(NULL, HCODE_MAXSIZ, PROT_READ | PROT_WRITE | PROT_EXEC,
	    MAP_ANON | MAP_PRIVATE, -1, 0);
#else
	hcode = (void *)syscall(SYS_mmap, NULL, HCODE_MAXSIZ,
	    PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_PRIVATE,
	    -1, 0);
#endif

	if (hcode == MAP_FAILED) {
		err(-1, "Can't mmap");
	}

	printf_page = (void *)(((unsigned long)printf) & 0xfffff000);

	if (mprotect(printf_page, 0x1000,
	    PROT_READ | PROT_WRITE | PROT_EXEC) < 0) {
		err(-1, "Can't make pages writable");
	}

	if (khook(printf, hcode, &avail, 0, (void *)my_printf) == 0) {
		errx(-1, "Can't hook printf");
	}

	if (mprotect(printf_page, 0x1000, PROT_READ | PROT_EXEC) < 0) {
		err(-1, "Can't make pages unwritable");
	}
}


static void
run()
{
	printf(NULL);
	printf("Hello Ground!\n", "");
	printf((const char *)1, "print this");
	printf((const char *)2, "blinking fantasy");
}


int
main(int argc, char **argv)
{
	hook_printf();
	run();
	return 0;
}

