/*
 * Header file for compatibility functions.
 *
 * Copyright 2000 by Gray Watson
 *
 * This file is part of the dmalloc package.
 *
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies, and that the name of Gray Watson not be used in advertising
 * or publicity pertaining to distribution of the document or software
 * without specific, written prior permission.
 *
 * Gray Watson makes no representations about the suitability of the
 * software described herein for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * The author may be contacted via http://dmalloc.com/
 *
 * $Id: compat.h 3078 2006-07-18 01:16:24Z vapier $
 */

#ifndef __COMPAT_H__
#define __COMPAT_H__

#if HAVE_STDARG_H
# include <stdarg.h>				/* for ... */
#endif

#include "conf.h"				/* for HAVE... */

/*<<<<<<<<<<  The below prototypes are auto-generated by fillproto */

#if HAVE_ATOI == 0
/*
 * Turn a ascii-string into an integer which is returned
 */
extern
int	atoi(const char *str);
#endif /* if HAVE_ATOI == 0 */

#if HAVE_ATOL == 0
/*
 * Turn a ascii-string into an integer which is returned
 */
extern
long	atol(const char *str);
#endif /* if HAVE_ATOL == 0 */

/*
 * Local vsnprintf which handles the buffer-size or not.  Returns the
 * number of characters copied into BUF.
 */
extern
int	loc_vsnprintf(char *buf, const int buf_size, const char *format,
		      va_list args);

/*
 * Local snprintf which handles the buf-size not.  Returns the number
 * of characters copied into BUF.
 */
extern
int	loc_snprintf(char *buf, const int buf_size, const char *format, ...);

#if HAVE_MEMCMP == 0
/*
 * Compare LEN characters, return -1,0,1 if STR1 is <,==,> STR2
 */
extern
int	memcmp(const void *str1, const void *str2, DMALLOC_SIZE len);
#endif /* if HAVE_MEMCMP == 0 */

#if HAVE_MEMCPY == 0
/*
 * Copy LEN characters from SRC to DEST
 */
extern
void	*memcpy(void *dest, const void *src, DMALLOC_SIZE len);
#endif /* if HAVE_MEMCPY == 0 */

#if HAVE_MEMMOVE == 0
/*
 * Copy LEN characters from SRC to DEST
 */
extern
void	*memmove(void *dest, const void *src, DMALLOC_SIZE len);
#endif /* if HAVE_MEMMOVE == 0 */

#if HAVE_MEMSET == 0
/*
 * Set LEN characters in STR to character CH
 */
extern
void	*memset(void *str, const int ch, DMALLOC_SIZE len);
#endif /* if HAVE_MEMSET == 0 */

#if HAVE_STRCHR == 0
/*
 * Find CH in STR by searching backwards through the string
 */
extern
char	*strchr(const char *str, const int ch);
#endif /* if HAVE_STRCHR == 0 */

#if HAVE_STRCMP == 0
/*
 * Returns -1,0,1 on whether STR1 is <,==,> STR2
 */
extern
int	strcmp(const char *str1, const char *str2);
#endif /* if HAVE_STRCMP == 0 */

#if HAVE_STRCPY == 0
/*
 * Copies STR2 to STR1.  Returns STR1.
 */
extern
char	*strcpy(char *str1, const char *str2);
#endif /* if HAVE_STRCPY == 0 */

#if HAVE_STRLEN == 0
/*
 * Return the length in characters of STR
 */
extern
int	strlen(const char *str);
#endif /* if HAVE_STRLEN == 0 */

#if HAVE_STRNCMP == 0
/*
 * Compare at most LEN chars in STR1 and STR2 and return -1,0,1 or
 * STR1 - STR2
 */
extern
int	strncmp(const char *str1, const char *str2, const int len);
#endif /* if HAVE_STRNCMP == 0 */

#if HAVE_STRNCPY == 0
/*
 * Copy STR2 to STR1 until LEN or null
 */
extern
char	*strncpy(char *str1, const char *str2, const int len);
#endif /* if HAVE_STRNCPY == 0 */

#if HAVE_STRRCHR == 0
/*
 * Find CH in STR by searching backwards through the string
 */
extern
char	*strrchr(const char *str, const int ch);
#endif /* if HAVE_STRRCHR == 0 */

#if HAVE_STRSEP == 0
/*
 * char *strsep
 *
 * DESCRIPTION:
 *
 * This is a function which should be in libc in every Unix.  Grumble.
 * It basically replaces the strtok function because it is reentrant.
 * This tokenizes a string by returning the next token in a string and
 * punching a \0 on the first delimiter character past the token.  The
 * difference from strtok is that you pass in the address of a string
 * pointer which will be shifted allong the buffer being processed.
 * With strtok you passed in a 0L for subsequant calls.  Yeach.
 *
 * This will count the true number of delimiter characters in the string
 * and will return an empty token (one with \0 in the zeroth position)
 * if there are two delimiter characters in a row.
 *
 * Consider the following example:
 *
 * char *tok, *str_p = "1,2,3, hello there ";
 *
 * while (1) { tok = strsep(&str_p, " ,"); if (tok == 0L) { break; } }
 *
 * strsep will return as tokens: "1", "2", "3", "", "hello", "there", "".
 * Notice the two empty "" tokens where there were two delimiter
 * characters in a row ", " and at the end of the string where there
 * was an extra delimiter character.  If you want to ignore these
 * tokens then add a test to see if the first character of the token
 * is \0.
 *
 * RETURNS:
 *
 * Success - Pointer to the next delimited token in the string.
 *
 * Failure - 0L if there are no more tokens.
 *
 * ARGUMENTS:
 *
 * string_p - Pointer to a string pointer which will be searched for
 * delimiters.  \0's will be added to this buffer.
 *
 * delim - List of delimiter characters which separate our tokens.  It
 * does not have to remain constant through all calls across the same
 * string.
 */
extern
char	*strsep(char **string_p, const char *delim);
#endif /* if HAVE_STRSEP == 0 */

/*<<<<<<<<<<   This is end of the auto-generated output from fillproto. */

#endif /* ! __COMPAT_H__ */
