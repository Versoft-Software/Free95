/*
 * Free 95
 *
 * NAME:
 *		stdbool.h
 *
 * DESCRIPTION:
 *		Bool support.
 *
 * Author: Rainy101112
 *
 */

#ifndef _STDBOOL_H
#define _STDBOOL_H

#ifndef __cplusplus

#define bool _Bool
#if defined __STDC_VERSION__ && __STDC_VERSION__ > 201710L
#define true ((_Bool) + 1u)
#define false ((_Bool) + 0u)
#else
#define true 1
#define false 0
#endif

#else /* __cplusplus */

/* Supporting _Bool in C++ is a GCC extension.  */
#define _Bool bool

#endif /* __cplusplus */

/* Signal that all the definitions are present.  */
#define __bool_true_false_are_defined 1

#endif /* stdbool.h */