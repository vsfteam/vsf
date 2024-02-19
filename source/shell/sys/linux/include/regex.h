/*	$OpenBSD: regex.h,v 1.7 2012/12/05 23:19:57 deraadt Exp $	*/
/*	$NetBSD: regex.h,v 1.4.6.1 1996/06/10 18:57:07 explorer Exp $	*/

/*-
 * Copyright (c) 1992 Henry Spencer.
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Henry Spencer of the University of Toronto.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)regex.h	8.1 (Berkeley) 6/2/93
 */

#ifndef _REGEX_H_
#define	_REGEX_H_

#include <sys/types.h>

/* types */
typedef off_t regoff_t;

typedef struct {
	int re_magic;
	size_t re_nsub;		/* number of parenthesized subexpressions */
	const char *re_endp;	/* end pointer for REG_PEND */
	struct re_guts *re_g;	/* none of your business :-) */
} regex_t;

typedef struct {
	regoff_t rm_so;		/* start of match */
	regoff_t rm_eo;		/* end of match */
} regmatch_t;

/* regcomp() flags */
#define	REG_BASIC	0000
#define	REG_EXTENDED	0001
#define	REG_ICASE	0002
#define	REG_NOSUB	0004
#define	REG_NEWLINE	0010
#define	REG_NOSPEC	0020
#define	REG_PEND	0040
#define	REG_DUMP	0200

/* regerror() flags */
#define	REG_NOMATCH	 1
#define	REG_BADPAT	 2
#define	REG_ECOLLATE	 3
#define	REG_ECTYPE	 4
#define	REG_EESCAPE	 5
#define	REG_ESUBREG	 6
#define	REG_EBRACK	 7
#define	REG_EPAREN	 8
#define	REG_EBRACE	 9
#define	REG_BADBR	10
#define	REG_ERANGE	11
#define	REG_ESPACE	12
#define	REG_BADRPT	13
#define	REG_EMPTY	14
#define	REG_ASSERT	15
#define	REG_INVARG	16
#define	REG_ATOI	255	/* convert name to number (!) */
#define	REG_ITOA	0400	/* convert number to name (!) */

/* regexec() flags */
#define	REG_NOTBOL	00001
#define	REG_NOTEOL	00002
#define	REG_STARTEND	00004
#define	REG_TRACE	00400	/* tracing of execution */
#define	REG_LARGE	01000	/* force large representation */
#define	REG_BACKR	02000	/* force use of backref code */

#if VSF_LINUX_APPLET_USE_REGEX == ENABLED
typedef struct vsf_linux_regex_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(regcomp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(regerror);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(regexec);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(regfree);
} vsf_linux_regex_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_regex_vplt_t vsf_linux_regex_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_REGEX_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_REGEX == ENABLED

#ifndef VSF_LINUX_APPLET_REGEX_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_REGEX_VPLT                                      \
            ((vsf_linux_regex_vplt_t *)(VSF_LINUX_APPLET_VPLT->regex_vplt))
#   else
#       define VSF_LINUX_APPLET_REGEX_VPLT                                      \
            ((vsf_linux_regex_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_REGEX_ENTRY(__NAME)                                    \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_REGEX_VPLT, __NAME)
#define VSF_LINUX_APPLET_REGEX_IMP(...)                                         \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_REGEX_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_REGEX_IMP(regcomp, int, regex_t *preg, const char *regex, int cflags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_REGEX_ENTRY(regcomp)(preg, regex, cflags);
}
VSF_LINUX_APPLET_REGEX_IMP(regerror, size_t, int errcode, const regex_t *preg, char *errbuf, size_t errbuf_size) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_REGEX_ENTRY(regerror)(errcode, preg, errbuf, errbuf_size);
}
VSF_LINUX_APPLET_REGEX_IMP(regexec, int, const regex_t *preg, const char *string, size_t nmatch, regmatch_t pmatch[], int eflags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_REGEX_ENTRY(regexec)(preg, string, nmatch, pmatch, eflags);
}
VSF_LINUX_APPLET_REGEX_IMP(regfree, void, regex_t *preg) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_REGEX_ENTRY(regfree)(preg);
}

#else       // __VSF_APPLET__ && VSF_LINUX_APPLET_USE_GLOB

__BEGIN_DECLS
int	regcomp(regex_t *, const char *, int);
size_t	regerror(int, const regex_t *, char *, size_t);
int	regexec(const regex_t *, const char *, size_t, regmatch_t [], int);
void	regfree(regex_t *);
__END_DECLS

#endif

#endif /* !_REGEX_H_ */