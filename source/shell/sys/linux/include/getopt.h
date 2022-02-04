/*	$OpenBSD: getopt.h,v 1.3 2013/11/22 21:32:49 millert Exp $	*/
/*	$NetBSD: getopt.h,v 1.4 2000/07/07 10:43:54 ad Exp $	*/

/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Dieter Baron and Thomas Klausner.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _GETOPT_H_
#define _GETOPT_H_

#include "shell/sys/linux/vsf_linux_cfg.h"
//#include <sys/cdefs.h>
#ifdef __cplusplus
#   define __BEGIN_DECLS    extern "C" {
#   define __END_DECLS      }
#else
#   define __BEGIN_DECLS
#   define __END_DECLS
#endif

/*
 * GNU-like getopt_long()
 */
#define no_argument        0
#define required_argument  1
#define optional_argument  2

struct option {
	/* name of long option */
	const char *name;
	/*
	 * one of no_argument, required_argument, and optional_argument:
	 * whether option takes an argument
	 */
	int has_arg;
	/* if not NULL, set *flag to val when option found */
	int *flag;
	/* if flag not NULL, value to set *flag to; else return value */
	int val;
};

__BEGIN_DECLS
int	 getopt_long(int, char * const *, const char *,
	    const struct option *, int *);
int	 getopt_long_only(int, char * const *, const char *,
	    const struct option *, int *);
#ifndef _GETOPT_DEFINED_
#define _GETOPT_DEFINED_
int	 getopt(int, char * const *, const char *);

#	if VSF_LINUX_USE_GETOPT == ENABLED
// for vsf linux, put these variable in process context and implement them as macro
#		include "shell/sys/linux/vsf_linux.h"
extern int __getopt_lib_idx;
extern const vsf_linux_dynlib_mod_t __getopt_long_mod;
struct __getopt_lib_ctx_t {
	char *__optarg;
	int __opterr;
	int __optind;
	int __optopt;
	int __optreset;
};
#		define getopt_ctx		((struct __getopt_lib_ctx_t *)vsf_linux_dynlib_ctx(&__getopt_long_mod))
#		define opterr			(getopt_ctx->__opterr)
#		define optind			(getopt_ctx->__optind)
#		define optopt			(getopt_ctx->__optopt)
#		define optarg			(getopt_ctx->__optarg)
#		define optreset			(getopt_ctx->__optreset)
#	else
extern   char *optarg;                  /* getopt(3) external variables */
extern   int opterr;
extern   int optind;
extern   int optopt;
extern   int optreset;
#	endif
#endif
__END_DECLS
 
#endif /* !_GETOPT_H_ */