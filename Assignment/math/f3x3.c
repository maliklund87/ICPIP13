#include <math/f3x3.h>

#include <math.h>

f3x3 f3x3_make(f3 x, f3 y, f3 z) {
    f3x3 m;
    m.e[0] = x.e[0];
    m.e[1] = x.e[1];
    m.e[2] = x.e[2];

    m.e[3] = y.e[0];
    m.e[4] = y.e[1];
    m.e[5] = y.e[2];

    m.e[6] = z.e[0];
    m.e[7] = z.e[1];
    m.e[8] = z.e[2];
    return m;
}

/* from: http://www.gamedev.net/topic/600537-instead-of-glrotatef-build-a-matrix/ */
f3x3 f3x3_rotate(double angle, f3 axis) {
    axis = f3_normalized(axis);
    double x = axis.e[0];
    double y = axis.e[1];
    double z = axis.e[2];

    double pi = 4.0 * atan( 1.0 );
    double alpha = -angle * pi / 180.0;
    double c = cos(alpha);
    double s = sin(alpha);

    f3x3 m;
    m.e[0] = x*x*(1-c)+c;
    m.e[1] = x*y*(1-c)-z*s; 
    m.e[2] = x*z*(1-c)+y*s;

    m.e[3] = y*x*(1-c)+z*s;
    m.e[4] = y*y*(1-c)+c;
    m.e[5] = y*z*(1-c)-x*s;

    m.e[6] = x*z*(1-c)-y*s;
    m.e[7] = y*z*(1-c)+x*s;
    m.e[8] = z*z*(1-c)+c;

    return m;
}
