#include  <math/f3.h>

#include  <math.h>

f3 f3_make(float e0, float e1, float e2) {
    f3 f;
    f.e[0] = e0;
    f.e[1] = e1;
    f.e[2] = e2;
    return f;
}

float f3_length_squared(f3 v) {
    return v.e[0]*v.e[0] + v.e[1]*v.e[1] + v.e[2]*v.e[2];
}

float f3_length(f3 v) {
    return sqrt( f3_length_squared(v) );
}

f3 f3_normalized(f3 v) {
    float l = f3_length(v);
    v.e[0] = v.e[0] / l;
    v.e[1] = v.e[1] / l;
    v.e[2] = v.e[2] / l;
    return v;
}

f3 f3_cross(f3 l, f3 r) {
    f3 v;
    v.e[0] = l.e[1]*r.e[2] - l.e[2]*r.e[1];
    v.e[1] = l.e[2]*r.e[0] - l.e[0]*r.e[2];
    v.e[2] = l.e[0]*r.e[1] - l.e[1]*r.e[0];
    return v;
}

f3 f3_sub(f3 l, f3 r) {
    f3 v;
    v.e[0] = l.e[0] - r.e[0];
    v.e[1] = l.e[1] - r.e[1];
    v.e[2] = l.e[2] - r.e[2];
    return v;
}

f3 f3_minus(f3 v) {
    f3 r;
    r.e[0] = -v.e[0];
    r.e[1] = -v.e[1];
    r.e[2] = -v.e[2];
    return r;
}
