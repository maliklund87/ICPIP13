#ifndef _f3x3_
#define _f3x3_

#include <math/f3.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float e[9]; /* elements */
} f3x3;

f3x3 f3x3_make(f3 x, f3 y, f3 z);
f3x3 f3x3_rotate(double angle, f3 axis);

#ifdef __cplusplus
}
#endif

#endif /* _f3x3_ */
