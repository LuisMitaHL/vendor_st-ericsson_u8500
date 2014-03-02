/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/* $Id: vsprintf.c,v 1.6 2003/06/11 14:30:30 leducs Exp $ */
/*
 * vsprintf.c
 *
 * 920304 rpg - changed arg list of __prtd() to accept va_list instead of
 *              double, to delete sign_str and pos_sign_str,
 *              and to add long_double, space, and plus flags.
 *                 Also, moved some code from here to __prtd so that we
 *              would not get calls to the floating point emulator
 *              generated here.
 *
 * Copyright (c) 1988-1990 by Bit Slice Software, All Rights Reserved
 * Copyright (c) 1990 by Archelon Inc.  All Rights Reserved.
 * Copyright (c) 1992 by Archelon Inc.  All Rights Reserved.
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#define prtc(c)	(*p++ = (c))

static char *prts( char *p, char *sarg, size_t width,
	size_t precision, int right );
static char *prtu( char *p, register unsigned uarg, register unsigned base,
	char zero, const char *alt, int right, size_t width, size_t precision,
	int upper );
static char *prtlu( char *p, register unsigned long ularg, register unsigned base,
	char zero, const char *alt, int right, size_t width, size_t precision,
	int upper );
#ifdef WITH_FLOAT
extern char *__prtd( va_list *ap, char *p, register char format,
	int upper, char zero, int alt, 
	int right, int width, int precision, int long_double,
	int space, int plus );
#endif

#ifdef NODIV
static int div( int a, int b );
static int mod( int a, int b );
#else
#define div(a, b)	((a) / (b))
#define mod(a, b)	((a) % (b))
#endif


int
vsprintf( char *buf, const char *fmt, va_list ap )
{
	register char c, *p = buf;
	char zero;
	int alt, right, done, longs, long_double, gotdot;
	size_t width;
	int dwidth;
	char *sarg;
	int iarg, *iarg_ptr;
	long ilarg, *ilarg_ptr;
	unsigned int uarg;
	unsigned long ularg;
	int upper, plus, space;
	const char *alt_s;
	unsigned char ucstr[2] = { '\0', '\0' };
	char fmt_char;

	/* while( c = *fmt++ ){ => Warning: = in conditional context */
	c = *fmt++;
	while( c ){	
		if( c == '%' ){
			alt =  right = longs = long_double = gotdot = done = 0;
			width = 0;
			dwidth = 1;
			upper = plus = space = 0;
			zero = ' ';
			while( done == 0 ){
				switch( c = *fmt++ ){
				case '#':
					alt = !alt;
					break;
				case '.':
					dwidth = 0;
					gotdot = 1;
					zero = ' ';
					break;
				case '0':
					if( width == 0 && gotdot == 0 ){
						if( right )
							zero = ' ';
						else
							zero = '0';
					} else {
						if( gotdot )
							dwidth *= 10;
						else
							width *= 10;
					}
					break;
				case '1': case '2': case '3': case '4': case '5':
				case '6': case '7': case '8': case '9':
					if( gotdot )
						dwidth = dwidth * 10 + (c - '0');
					else
						width = width * 10 + (c - '0');
					break;
				case '*':
					iarg = va_arg( ap, int );
					if( gotdot ){
						dwidth = iarg;
					} else {
						if( iarg < 0 ){
							right = 1;
							zero = ' ';
							iarg = -iarg;
						}
						width = iarg;
					}
					break;
				case 'h':
					longs = 0;
					break;
				case 'l':
					longs = 1;
					break;
				case 'L':
					long_double = 1;
					break;
				case '-':
					right = 1;
					zero = ' ';
					break;
				case '+':
					plus = 1;
					space = 0;
					break;
				case ' ':
					if( !plus )
						space = 1;
					break;
				case 'c':
					ucstr[0] = (unsigned char) va_arg( ap, int );
					if( dwidth < 0 ) dwidth = 1;
					p = prts( p, (char *) ucstr, width, (size_t)dwidth, right );
					done = 1;
					break;
				case 's':
					/* print string */
					sarg = va_arg( ap, char * );
					if( sarg == 0 )
						sarg = "(null)";
					if( !gotdot || (dwidth < 0) ) dwidth = 0;
					p = prts( p, sarg, width, (size_t) dwidth, right );
					done = 1;
					break;
				case 'd':
				case 'i':
					if (longs)
					{
						ilarg = va_arg( ap, long );
						if( ilarg < 0 ){
							ularg = -ilarg;
							alt_s = "-";
						} else {
							ularg = ilarg;
							alt_s = plus ? "+" : (space ? " " : "");
						}
						if( dwidth < 0 ) dwidth = 1;
						p = prtlu( p, ularg, 10, zero, alt_s, right,
								width, (size_t) dwidth, upper );
					}
					else
					{
						iarg = va_arg( ap, int );
						if( iarg < 0 ){
							uarg = -iarg;
							alt_s = "-";
						} else {
							uarg = iarg;
							alt_s = plus ? "+" : (space ? " " : "");
						}
						if( dwidth < 0 ) dwidth = 1;
						p = prtu( p, uarg, 10, zero, alt_s, right,
								width, (size_t) dwidth, upper );
					}
					done = 1;
					break;
				case 'u':
					if (longs)
					{
						ularg = va_arg( ap, unsigned long );
						if( dwidth < 0 ) dwidth = 1;
						p = prtlu( p, ularg, 10, zero, "", right,
								width, (size_t) dwidth, upper );
					}
					else
					{
						uarg = va_arg( ap, unsigned int );
						if( dwidth < 0 ) dwidth = 1;
						p = prtu( p, uarg, 10, zero, "", right,
								width, (size_t) dwidth, upper );
					}
					done = 1;
					break;
				case 'o':
					if (longs)
					{
						ularg = va_arg( ap, unsigned long );
						alt_s = alt ? (upper ? "0" : "") : "";
						if( dwidth < 0 ) dwidth = 1;
						p = prtlu( p, ularg, 8, zero, alt_s, right,
								width, (size_t) dwidth, upper );
					}
					else
					{
						uarg = va_arg( ap, unsigned int );
						alt_s = alt ? "0" : "";
						if( dwidth < 0 ) dwidth = 1;
						p = prtu( p, uarg, 8, zero, alt_s, right,
								width, (size_t) dwidth, upper );
					}
					done = 1;
					break;
				case 'p':
#if defined(__acu24__)
					ularg = (unsigned long) va_arg( ap, void * );
					alt_s = alt ? "0x" : "";
					if( dwidth < 0 ) dwidth = 1;
					p = prtlu( p, ularg, 16, zero, alt_s, right,
							width, (size_t) dwidth, upper );
					done = 1;
					break;
#else
					uarg = (unsigned int) va_arg( ap, void * );
					alt_s = alt ? "0x" : "";
					if( dwidth < 0 ) dwidth = 1;
					p = prtu( p, uarg, 16, zero, alt_s, right,
						width, (size_t) dwidth, upper );
					done = 1;
					break;
#endif	// __acu24__
				case 'X':
					upper = 1;
					/* no "break" here */
				case 'x':
					if (longs)
					{
						ularg = va_arg( ap, unsigned long );
						alt_s = alt ? (upper ? "0X" : "0x") : "";
						if( dwidth < 0 ) dwidth = 1;
						p = prtlu( p, ularg, 16, zero, alt_s, right,
								width, (size_t) dwidth, upper );
					}
					else
					{
						uarg = va_arg( ap, unsigned int );
						alt_s = alt ? (upper ? "0X" : "0x") : "";
						if( dwidth < 0 ) dwidth = 1;
						p = prtu( p, uarg, 16, zero, alt_s, right,
								width, (size_t) dwidth, upper );
					}
					done = 1;
					break;
				case 'E':
				case 'G':
					upper = 1;
					/* no "break" here */
				case 'f':
				case 'e':
				case 'g':
					fmt_char = *(fmt-1);
					if( upper )
						fmt_char = tolower( fmt_char );
					if( !gotdot || (dwidth < 0) ) dwidth = 6;
#ifdef WITH_FLOAT
					p = __prtd( &ap, p, fmt_char, upper, zero, alt, 
						right, width, dwidth, long_double, space, plus );
#else
					p = prts( p, "ERROR float not supported , please use stdlib.lib\n", 0,0,0);
#endif
					done = 1;
					break;
				case 'n':
					if (longs)
					{
						ilarg_ptr = va_arg( ap, long * );
						*ilarg_ptr = (long) (p - buf);
					}
					else
					{
						iarg_ptr = va_arg( ap, int * );
						*iarg_ptr = (int) (p - buf);
					}
					done = 1;
					break;
				default:
					prtc( c );
					done = 1;
					break;
				}
			}
		} else
			prtc( c );
		c = *fmt++;
	}
	*p = 0;
	return p - buf;
}


static char *
prts( char *p, char *sarg, size_t width, size_t precision, int right )
{
	size_t len = strlen( sarg );
	register char *b;
	char *d;

	if( precision > 0 && precision < len ) len = precision;
	if( len > width ){
		(void) strncpy( p, sarg, len );
		return p + len;
	} else {
		register size_t w = width - len;
		if( right ){
			d = p;
			b = p + len;
		} else {
			d = p + width - len;
			b = p;
		}
		(void) strncpy( d, sarg, len );
		while( w-- > 0 )
			*b++ = ' ';
		p[width] = 0;
		return p + width;
	}
}
static char *
prtu( char *p, register unsigned uarg, register unsigned base, char zero,
	 const char *alt, int right, size_t width, size_t precision, int upper )
{
	char *mapc;
	register int digits = 1, pad, al, leading_zeroes;
	int pw;
	register unsigned int cn = uarg;
	register char d, *as, *bs, *ds, *zs;
	char *r;
	
	if( upper )
		mapc = "0123456789ABCDEF";
	else
		mapc = "0123456789abcdef";
	if( precision == 0 && uarg == 0 ){
		digits = 0;
	} else {
		while( cn >= base ){
			cn = div( cn, base );
			++digits;
		}
	}
	al = strlen( alt );
	if( (al != 0) && (zero == '0') )
		precision = width - al;
	if( (leading_zeroes = (int) precision - digits) < 0 )
		leading_zeroes = 0;
	pw = digits + al + leading_zeroes;
	pad = width - pw;
	if( pad < 0 ){
		as = bs = p;
		ds = r = p + pw;
	} else {
		if( right ){
			as = p;
			ds = bs = p + pw;
			bs = p + pw;
		} else {
			as = p + pad;
			ds = p + pad + pw;
			bs = p;
		}
		r = p + width;
	}
	while( digits-- > 0 ){
		d = mapc[ mod( uarg, base ) ];
		*--ds = d;
		uarg = div( uarg, base );
	}
	zs = as + al;
	while( leading_zeroes-- > 0 )
		*zs++ = '0';
	while( al-- > 0 )
		*as++ = *alt++;
	while( pad-- > 0 )
		*bs++ = zero;
	return r;
}

static char *
prtlu( char *p, register unsigned long ularg, register unsigned base, char zero,
	 const char *alt, int right, size_t width, size_t precision, int upper )
{
	char *mapc;
	register int digits = 1, pad, al, leading_zeroes;
	int pw;
	register unsigned long cn = ularg;
	register char d, *as, *bs, *ds, *zs;
	char *r;
	
	if( upper )
		mapc = "0123456789ABCDEF";
	else
		mapc = "0123456789abcdef";
	if( precision == 0 && ularg == 0 ){
		digits = 0;
	} else {
		while( cn >= base ){
			cn = div( cn, base );
			++digits;
		}
	}
	al = strlen( alt );
	if( (al != 0) && (zero == '0') )
		precision = width - al;
	if( (leading_zeroes = (int) precision - digits) < 0 )
		leading_zeroes = 0;
	pw = digits + al + leading_zeroes;
	pad = width - pw;
	if( pad < 0 ){
		as = bs = p;
		ds = r = p + pw;
	} else {
		if( right ){
			as = p;
			ds = bs = p + pw;
			bs = p + pw;
		} else {
			as = p + pad;
			ds = p + pad + pw;
			bs = p;
		}
		r = p + width;
	}
	while( digits-- > 0 ){
		d = mapc[ mod( ularg, base ) ];
		*--ds = d;
		ularg = div( ularg, base );
	}
	zs = as + al;
	while( leading_zeroes-- > 0 )
		*zs++ = '0';
	while( al-- > 0 )
		*as++ = *alt++;
	while( pad-- > 0 )
		*bs++ = zero;
	return r;
}


#ifdef NODIV

static int
div( int a, int b )
{
	register int result = 0xDEAD;
	if( b == 8 )
		result = a >> 3;
	else if( b == 16 )
		result = a >> 4;
	else if( b == 10 ){
		/* Compute (a / 5) using
		 *   1   1    1    1    1      1
		 *	 - = - - -- + -- - --- + ---- - + ...
		 *   5   4   16   64   256   1024
		 */
		register int s = 4, r = a >> 2, q, m = -1;
		while( q = (a >> s) ){
			r += m * q;
			m = -m;
			s += 2;
		}
		result = r >> 1;
	}
	return result;
}

static int
mod( int a, int b )
{
	register int result = 0xDEAD;

	/* a is positive */
	if( b == 8 )
		result = a & 0x7;
	else if( b == 16 )
		result = a & 0xF;
	else if( b == 10 ){
		/* divide by 10 -- see above */
		register int s = 4, r = a >> 2, q, m = -1;
		while( q = (a >> s) ){
			r += m * q;
			m = -m;
			s += 2;
		}
		r >>= 1;
		/* r == a / 10 */
		result = a - (10 * r);
	}
	return result;
}

#endif
