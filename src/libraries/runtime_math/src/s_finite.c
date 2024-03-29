/* @(#)s_finite.c 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

#if defined(LIBM_SCCS) && !defined(lint)
static char rcsid[] = "$NetBSD: s_finite.c,v 1.8 1995/05/10 20:47:17 jtc Exp $";
#endif

/*
 * isfinite(x) returns 1 is x is isfinite, else 0;
 * no branching!
 */

#include "runtime/rtapi_math.h"
#include "mathP.h"

#ifdef __STDC__
	int rtapi_isfinite(double x)
#else
	int rtapi_isfinite(x)
	double x;
#endif
{
	int32_t hx;
	GET_HIGH_WORD(hx,x);
	return (int)((u_int32_t)((hx&0x7fffffff)-0x7ff00000)>>31);
}
