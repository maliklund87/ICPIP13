#ifndef _f_3_
#define _f_3_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float e[3]; /* elements */
} f3;

f3 f3_make(float e0, float e1, float e2);
float f3_length_squared(f3 v);
float f3_length(f3 v);
f3 f3_normalized(f3 v);
f3 f3_cross(f3 l, f3 r);
f3 f3_sub(f3 l, f3 r);
f3 f3_minus(f3 v);

#ifdef __cplusplus
}
#endif

#endif /* _f_3_ */
