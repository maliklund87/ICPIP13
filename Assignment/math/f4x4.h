#ifndef _f4x4_
#define _f4x4_

#include <math/f3.h>
#include <math/f3x3.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float e[16]; /* elements */
} f4x4;

f4x4 f4x4_id();
f4x4 f4x4_mul(f4x4 l, f4x4 r);
f4x4 f3x3_expand(f3x3 m);
f4x4 f4x4_transpose(f4x4 m);
f4x4 f4x4_translate(float x, float y, float z);
f4x4 f4x4_scale(float sx, float sy, float sz);
f4x4 f4x4_lookAt(double eyeX, double eyeY, double eyeZ, 
                 double centerX, double centerY, double centerZ,
                 double upX, double upY, double upZ);
f4x4 f4x4_frustum(double l, double r, double b, double t, double n, double f);
f4x4 f4x4_perspective(double fovy, double aspect, double zNear, double zFar);
f4x4 f4x4_rotate(double angle, f3 axis);
int f4x4_invert(f4x4 m,f4x4 *inv);
void f4x4_print(f4x4 m);

#ifdef __cplusplus
}
#endif

#endif /* _f4x4_ */
