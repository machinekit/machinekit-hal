/* @(#)w_log.c 5.1 93/09/24 */
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
static char rcsid[] = "$NetBSD: w_log.c,v 1.6 1995/05/10 20:49:33 jtc Exp $";
#endif

/*
 * wrapper log(x)
 */

#include "runtime/rtapi_math.h"
#include "mathP.h"


#ifdef __STDC__
	double rtapi_log(double x)		/* wrapper log */
#else
	double rtapi_log(x)			/* wrapper log */
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return __ieee754_log(x);
#else
	double z;
	z = __ieee754_log(x);
	if(_LIB_VERSION == _IEEE_ || isnan(x) || x > 0.0) return z;
	if(x==0.0)
	    return __kernel_standard(x,x,16); /* log(0) */
	else
	    return __kernel_standard(x,x,17); /* log(x<0) */
#endif
}
