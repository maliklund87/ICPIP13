varying vec3 n;
varying vec4 v;

void main() {
     vec3 l = normalize(gl_LightSource[0].position.xyz - v.xyz);
     vec3 e = -normalize(v.xyz);
     vec3 r = normalize(-l + 2.0 * dot(l, n) * n);

     vec4 La = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
     vec4 Ld = gl_FrontMaterial.diffuse * max(0.0, dot(l,n));
     vec4 Ls = gl_FrontMaterial.specular * pow(max(0.0, dot(e, r)), gl_FrontMaterial.shininess);
     
     gl_FragColor = La+Ld+Ls;
}

