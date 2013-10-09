
varying vec3 n;
varying vec4 v;

void main() {
    n = normalize(gl_NormalMatrix*gl_Normal);
    v = gl_ModelViewMatrix*gl_Vertex;
 
    /* Pass on projected coords */
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

