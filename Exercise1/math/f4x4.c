#include <math/f4x4.h>

#include <math/f3.h>
#include <math/f3x3.h>

#include <stdio.h>
#include <math.h>

#define entry(r,c) (4*c+r)

f4x4 f4x4_id() {
    f4x4 m;
    m.e[0] = 1;
    m.e[1] = 0;
    m.e[2] = 0;
    m.e[3] = 0;

    m.e[4] = 0;
    m.e[5] = 1;
    m.e[6] = 0;
    m.e[7] = 0;

    m.e[ 8] = 0;
    m.e[ 9] = 0;
    m.e[10] = 1;
    m.e[11] = 0;
    
    m.e[12] = 0;
    m.e[13] = 0;
    m.e[14] = 0;
    m.e[15] = 1;

    return m;
}

f4x4 f4x4_mul(f4x4 l, f4x4 r) {
    f4x4 m;
    unsigned int i, j, t;
    for (i=0; i<4; i++) 
        for (j=0; j<4; j++) {
            float s = 0;
            for (t=0; t<4; t++)
                s += l.e[i+t*4] * r.e[t+j*4];
            m.e[i+j*4] = s;
        }
    return m;
}

f4x4 f3x3_expand(f3x3 m) {
    f4x4 v;
    v.e[0] = m.e[0];
    v.e[1] = m.e[1];
    v.e[2] = m.e[2];
    v.e[3] = 0;

    v.e[4] = m.e[3];
    v.e[5] = m.e[4];
    v.e[6] = m.e[5];
    v.e[7] = 0;

    v.e[ 8] = m.e[6];
    v.e[ 9] = m.e[7];
    v.e[10] = m.e[8];
    v.e[11] = 0;
    
    v.e[12] = 0;
    v.e[13] = 0;
    v.e[14] = 0;
    v.e[15] = 1;

    return v;
}

f4x4 f4x4_translate(float x, float y, float z) {
    f4x4 m;
    m.e[0] = 1;
    m.e[1] = 0;
    m.e[2] = 0;
    m.e[3] = 0;

    m.e[4] = 0;
    m.e[5] = 1;
    m.e[6] = 0;
    m.e[7] = 0;

    m.e[ 8] = 0;
    m.e[ 9] = 0;
    m.e[10] = 1;
    m.e[11] = 0;
    
    m.e[12] = x;
    m.e[13] = y;
    m.e[14] = z;
    m.e[15] = 1;

    return m;
}

f4x4 f4x4_transpose(f4x4 m) {
    f4x4 t;
    t.e[entry(0,0)] = m.e[entry(0,0)];
    t.e[entry(1,0)] = m.e[entry(0,1)];
    t.e[entry(2,0)] = m.e[entry(0,2)];
    t.e[entry(3,0)] = m.e[entry(0,3)];

    t.e[entry(0,1)] = m.e[entry(1,0)];
    t.e[entry(1,1)] = m.e[entry(1,1)];
    t.e[entry(2,1)] = m.e[entry(1,2)];
    t.e[entry(3,1)] = m.e[entry(1,3)];

    t.e[entry(0,2)] = m.e[entry(2,0)];
    t.e[entry(1,2)] = m.e[entry(2,1)];
    t.e[entry(2,2)] = m.e[entry(2,2)];
    t.e[entry(3,2)] = m.e[entry(2,3)];

    t.e[entry(0,3)] = m.e[entry(3,0)];
    t.e[entry(1,3)] = m.e[entry(3,1)];
    t.e[entry(2,3)] = m.e[entry(3,2)];
    t.e[entry(3,3)] = m.e[entry(3,3)];


    return t;
}

f4x4 f4x4_scale(float sx, float sy, float sz) {
    f4x4 m;
    m.e[0] = sx;
    m.e[1] = 0;
    m.e[2] = 0;
    m.e[3] = 0;

    m.e[4] = 0;
    m.e[5] = sy;
    m.e[6] = 0;
    m.e[7] = 0;

    m.e[ 8] = 0;
    m.e[ 9] = 0;
    m.e[10] = sz;
    m.e[11] = 0;
    
    m.e[12] = 0;
    m.e[13] = 0;
    m.e[14] = 0;
    m.e[15] = 1;

    return m;
}

f4x4 f4x4_lookAt(double eyeX, double eyeY, double eyeZ, 
                        double centerX, double centerY, double centerZ,
                        double upX, double upY, double upZ) {

    f3 F = f3_make(centerX-eyeX, centerY-eyeY, centerZ-eyeZ);
    f3 UP = f3_make(upX, upY, upZ);

    f3 f = f3_normalized(F);
    f3 up = f3_normalized(UP);

    f3 s = f3_cross(f, up);
    f3 u = f3_cross(s, f);

    /* add rotation */
    f3x3 rotation = f3x3_make(s,u, f3_minus(f));
    f4x4 v = f3x3_expand(rotation);

    /* add translation */
    v.e[12] = -eyeX;
    v.e[13] = -eyeY;
    v.e[14] = -eyeZ;
    return v;
}

/* from Real-time rendering 2nd edition, page 65 */
f4x4 f4x4_frustum(double l, double r, double b, double t, double n, double f) {
    f4x4 m;

    /* first row */
    m.e[ 0] = 2.0f*n/(r-l);
    m.e[ 4] = 0;
    m.e[ 8] = (r+l)/(r-l);
    m.e[12] = 0;

    /* second row */
    m.e[ 1] = 0;
    m.e[ 5] = 2.0f*n/(t-b);
    m.e[ 9] = (t+b)/(t-b);
    m.e[13] = 0;

    /* third row */
    m.e[ 2] = 0;
    m.e[ 6] = 0;
    m.e[10] = -(f+n)/(f-n);
    m.e[14] = -2.0f*(f*n)/(f-n);

    /* fourth row */
    m.e[ 3] = 0;
    m.e[ 7] = 0;
    m.e[11] = -1;
    m.e[15] = 0;

    return m;
}

/* from: http://steinsoft.net/index.php?site=Programming/Code%20Snippets/OpenGL/gluperspective */
f4x4 f4x4_perspective(double fovy, double aspect, double zNear, double zFar) {
    double xmin, xmax, ymin, ymax;
    double pi = 4.0 * atan( 1.0 );
    ymax = zNear * tan(fovy * pi / 360.0);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;
    return f4x4_frustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

f4x4 f4x4_rotate(double angle, f3 axis) {
    f3x3 rot = f3x3_rotate(angle, axis);
    return f3x3_expand(rot);
}

void f4x4_print(f4x4 m) {
    printf("----------------------\n");
    printf("% .2f % .2f % .2f % .2f\n", m.e[0], m.e[4], m.e[ 8], m.e[12]);
    printf("% .2f % .2f % .2f % .2f\n", m.e[1], m.e[5], m.e[ 9], m.e[13]);
    printf("% .2f % .2f % .2f % .2f\n", m.e[2], m.e[6], m.e[10], m.e[14]);
    printf("% .2f % .2f % .2f % .2f\n", m.e[3], m.e[7], m.e[11], m.e[15]);
    printf("----------------------\n");
}

/* Calculate inverse using ... */

float det2(float a0, float a1, float b0, float b1){
	return a0*b1-a1*b0;
}

float det3(float a0, float a1, float a2, 
		   float b0, float b1, float b2, 
		   float c0, float c1, float c2){
	return a0*det2(b1,b2,c1,c2)-b0*det2(a1,a2,c1,c2)+c0*det2(a1,a2,b1,b2);
}

float det4(float a0, float a1, float a2, float a3,
		   float b0, float b1, float b2, float b3,
		   float c0, float c1, float c2, float c3,
		   float d0, float d1, float d2, float d3){
	return
		 a0*det3(b1,b2,b3,c1,c2,c3,d1,d2,d3)
		-b0*det3(a1,a2,a3,c1,c2,c3,d1,d2,d3) 
		+c0*det3(a1,a2,a3,b1,b2,b3,d1,d2,d3)
		-d0*det3(a1,a2,a3,b1,b2,b3,c1,c2,c3);
}

float det(f4x4 a){
	return det4(a.e[entry(0,0)],a.e[entry(1,0)],a.e[entry(2,0)],a.e[entry(3,0)],
				a.e[entry(0,1)],a.e[entry(1,1)],a.e[entry(2,1)],a.e[entry(3,1)],
				a.e[entry(0,2)],a.e[entry(1,2)],a.e[entry(2,2)],a.e[entry(3,2)],
				a.e[entry(0,3)],a.e[entry(1,3)],a.e[entry(2,3)],a.e[entry(3,3)]);
}

int f4x4_invert(f4x4 A, f4x4 *inv){
	/* Using Laplace:
	   A^{-1} = 1/det(A) * adj(A); */

	float d = det(A);
	if(fabs(d) < 1E-30){
		/* Determinant is zero! */
		printf("Determinant 0...!\n");
		return 0;
	}

	/* build matrix of cofactors; */
	f4x4 G;

	/*
	     | a0 b0 c0 d0 |
	     | a1 b1 c1 d1 |
	 A = | a2 b2 c2 d2 |
	     | a3 b3 c3 d3 |
	*/
	float a0,b0,c0,d0;
	float a1,b1,c1,d1;
	float a2,b2,c2,d2;
	float a3,b3,c3,d3;

	a0 = A.e[entry(0,0)]; b0 = A.e[entry(0,1)]; c0 = A.e[entry(0,2)]; d0 = A.e[entry(0,3)];
	a1 = A.e[entry(1,0)]; b1 = A.e[entry(1,1)]; c1 = A.e[entry(1,2)]; d1 = A.e[entry(1,3)];
	a2 = A.e[entry(2,0)]; b2 = A.e[entry(2,1)]; c2 = A.e[entry(2,2)]; d2 = A.e[entry(2,3)];
	a3 = A.e[entry(3,0)]; b3 = A.e[entry(3,1)]; c3 = A.e[entry(3,2)]; d3 = A.e[entry(3,3)];

	G.e[entry(0,0)] = +det3(b1,b2,b3,c1,c2,c3,d1,d2,d3);
	G.e[entry(0,1)] = -det3(a1,a2,a3,c1,c2,c3,d1,d2,d3);
	G.e[entry(0,2)] = +det3(a1,a2,a3,b1,b2,b3,d1,d2,d3);
	G.e[entry(0,3)] = -det3(a1,a2,a3,b1,b2,b3,c1,c2,c3);

	G.e[entry(1,0)] = -det3(b0,b2,b3,c0,c2,c3,d0,d2,d3);
	G.e[entry(1,1)] = +det3(a0,a2,a3,c0,c2,c3,d0,d2,d3);
	G.e[entry(1,2)] = -det3(a0,a2,a3,b0,b2,b3,d0,d2,d3);
	G.e[entry(1,3)] = +det3(a0,a2,a3,b0,b2,b3,c0,c2,c3);

	G.e[entry(2,0)] = +det3(b0,b1,b3,c0,c1,c3,d0,d1,d3);
	G.e[entry(2,1)] = -det3(a0,a1,a3,c0,c1,c3,d0,d1,d3);
	G.e[entry(2,2)] = +det3(a0,a1,a3,b0,b1,b3,d0,d1,d3);
	G.e[entry(2,3)] = -det3(a0,a1,a3,b0,b1,b3,c0,c1,c3);

	G.e[entry(3,0)] = -det3(b0,b1,b2,c0,c1,c2,d0,d1,d2);
	G.e[entry(3,1)] = +det3(a0,a1,a2,c0,c1,c2,d0,d1,d2);
	G.e[entry(3,2)] = -det3(a0,a1,a2,b0,b1,b2,d0,d1,d2);
	G.e[entry(3,3)] = +det3(a0,a1,a2,b0,b1,b2,c0,c1,c2);

	/* std::cout<<"Adjoint: "<<G<<std::endl; */
	/* printf("G"); */
	/* f4x4_print(G); */
	G = f4x4_transpose(G);
	/* printf("G^T"); */
	/* f4x4_print(G); */
	/* std::cout<<"AdjTran: "<<G<<std::endl; */
	int i;
	for(i = 0; i < 16; i++)
	inv->e[i] = G.e[i]/d;
	/* std::cout<<"Inverse: "<<inv<<std::endl; */
	return 1;
}
