/*
 * mbtowc.c
 *
 * Copyright (C) STEricsson Le MANS SA 2010
 * Author: P.Chotard <patrice PT chotard AT stericsson PT com> for STEricsson Le MANS.
 * License terms:  GNU General Public License (GPL), version 2
 */

#include <stdlib.h>


int mbtowc(wchar_t *pwc, register const char *s, size_t n)
{
	if (s == (const char *)NULL)
		return 0;

	if (n <= 0)
		return 0;

	if (pwc)
		*pwc = *s;

	return (*s != 0);
 }
