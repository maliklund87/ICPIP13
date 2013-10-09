#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <cmath>

#include <GL/glew.h> /* Must be before gl.h */

#include <GL/gl.h>
#include <GL/glut.h>


/*/////// Global storage ///////*/
float xAngle, yAngle, zAngle;

/* storage for phong program */
GLenum pipelineProgram;

GLenum vertexShader;
GLenum fragmentShader;
GLint uniformTime;
GLint uniformTexture;

/* Storage for Lights and Materials */
unsigned int moveLight = 0;

GLfloat lightAmbient[] =  { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse[] =  { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPos[] = { 0.0, 0.0, 10.0, 1.0 };

GLfloat matAmbient[] =    { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat matDiffuse[] =    { 0.0f, 1.0f, 1.0f, 1.0f };
GLfloat matSpecular[] =   { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat matEmission[] =   { 0.0f, 0.0f, 0.0f, 1.0f }; 
GLfloat matShininess[] =  { 100.0 };

GLfloat matDiffuseBlue[]  =  { 0.8f, 0.6f, 1.0f, 1.0f };
GLfloat matDiffuseGreen[] =  { 0.6f, 1.0f, 0.8f, 1.0f };
GLfloat matDiffuseRed[]   =  { 1.0f, 0.8f, 0.6f, 1.0f };

#include <string>
#include <fstream>
#include <iostream>

/*/////////////////////////////////////////////////////////////////////////////
//
// GLSL helper functions
//
/////////////////////////////////////////////////////////////////////////////*/

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define CHECK_FOR_GL_ERROR() CHECK_FOR_GL_ERROR_FUNCTION(__FILE__,TOSTRING(__LINE__))

void exit_with_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

void CHECK_FOR_GL_ERROR_FUNCTION(const char* file, const char* line) {
    GLenum errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) {
        const GLubyte* errorString = gluErrorString(errorCode);
        exit_with_error("[file:%s line:%s] OpenGL Error: %s\n", 
                        file, line, (char*)errorString);
    }
}

char* file_to_string(const char* path) {
    FILE* fd;
    long len, r;
    char* str;
 
    if (!(fd = fopen(path, "r"))) {
        exit_with_error("Can't open file '%s' for reading\n", path);
    }
 
    fseek(fd, 0, SEEK_END);
    len = ftell(fd);
 
    fseek(fd, 0, SEEK_SET);
    if (!(str = (char*) malloc(len * sizeof(char)))) {
        exit_with_error("Can't malloc space for '%s'\n", path);
    }
 
    r = fread(str, sizeof(char), len, fd);
    str[r - 1] = '\0'; /* Shader sources have to term with null */
    fclose(fd);
    return str;
}

void print_shader_info_log(GLuint shader) {
    GLint infologLength = 0, charsWritten = 0;
    GLchar *infoLog;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);
    if (infologLength > 0) {
        infoLog = (GLchar *)malloc(infologLength);
        if (infoLog==NULL) {
            exit_with_error("Could not allocate InfoLog buffer for shader: %u", shader);
        }
        glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
        if (charsWritten > 0)
            fprintf(stderr, "Shader InfoLog: %u\n%s", shader, infoLog);
        free(infoLog);
    }
}

void print_program_info_log(GLuint program) {
    GLint infologLength = 0, charsWritten = 0;
    GLchar* infoLog;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);
    if (infologLength > 0) {
        infoLog = (GLchar *)malloc(infologLength);
        if (infoLog==NULL) {
            exit_with_error("Could not allocate InfoLog buffer for program: %u", program);
        }
        glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
        if (charsWritten > 0)
            fprintf(stderr, "Program InfoLog: %u\n%s", program, infoLog);
        free(infoLog);
    }
}


void load_shaders() {
    GLint compiled;

    /* Create vertex shader */
    const char *vertexFile = "vPhongVert.glsl";
    const char* sourceChar = file_to_string(vertexFile);
    vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER);
    glShaderSourceARB(vertexShader, 1, &sourceChar,NULL);
    glCompileShaderARB(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
    print_shader_info_log(vertexShader);
    if (compiled==0) {
        exit_with_error("Failed compiling shader program: %s\n", vertexFile);
    }

    /* Create fragment shader */
    const char *fragmentFile = "vPhongFrag.glsl";
    const char *sourceFragChar = file_to_string(fragmentFile);
    fragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER);
    glShaderSourceARB(fragmentShader, 1, &sourceFragChar,NULL);
    glCompileShaderARB(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
    print_shader_info_log(fragmentShader);
    if (compiled==0) {
        exit_with_error("Failed compiling shader program: %s\n", fragmentFile);
    }

    /* Create program and link shaders */
    pipelineProgram = glCreateProgramObjectARB();
    glAttachObjectARB(pipelineProgram, vertexShader);
    glAttachObjectARB(pipelineProgram, fragmentShader);

    glLinkProgramARB(pipelineProgram);
    GLint linked;
    glGetProgramiv(pipelineProgram, GL_LINK_STATUS, &linked);
    print_program_info_log(pipelineProgram);
    if (linked == 0) {
        exit_with_error("Could not link shader program\n");
    }

    CHECK_FOR_GL_ERROR();

    printf("Loaded shaders and linked shader program\n");
}

/*/////////////////////////////////////////////////////////////////////////////
//
// GL functions
//
/////////////////////////////////////////////////////////////////////////////*/
void glInit(){

    glEnable(GL_DEPTH_TEST);
    //glEnable( GL_LIGHTING );
    //glEnable( GL_LIGHT0 );
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glEnable(GL_CULL_FACE);

    /* Initialize angles */
    xAngle = 0.0f;	  
    yAngle = 0.0f;	  
    zAngle = 0.0f;	  

    load_shaders();
}

void glCleanUp(){
    glDeleteObjectARB(pipelineProgram);
    glDeleteObjectARB(vertexShader);
    glDeleteObjectARB(fragmentShader);

}

void setLightSource(){
    /* Setup the light source properties */
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

void drawCube(){

    glBegin(GL_QUADS);
    /* Front */
    glNormal3f(0.0f, 0.0f,  1.0f);
    glVertex3f( 1.0f, 1.0f,  1.0f);
    glVertex3f(-1.0f, 1.0f,  1.0f);
    glVertex3f(-1.0f,-1.0f,  1.0f);
    glVertex3f( 1.0f,-1.0f,  1.0f);

    /* Back */
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f( 1.0f, 1.0f, -1.0f);
    glVertex3f( 1.0f,-1.0f, -1.0f);
    glVertex3f(-1.0f,-1.0f, -1.0f);

    /* Top */
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f( 1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f( 1.0f, 1.0f, 1.0f);

    /* Bottom */
    glNormal3f(0.0f,-1.0f,0.0f);
    glVertex3f(-1.0f,-1.0f, -1.0f);
    glVertex3f( 1.0f,-1.0f, -1.0f);
    glVertex3f( 1.0f,-1.0f,  1.0f);
    glVertex3f(-1.0f,-1.0f,  1.0f);

    /* Left */
    glNormal3f(-1.0f,0.0f,0.0f);
    glVertex3f(-1.0f, 1.0f,  1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f,-1.0f, -1.0f);
    glVertex3f(-1.0f,-1.0f,  1.0f);

    /* Right */
    glNormal3f(1.0f,0.0f,0.0f);
    glVertex3f( 1.0f, 1.0f, -1.0f);
    glVertex3f( 1.0f, 1.0f,  1.0f);
    glVertex3f( 1.0f,-1.0f,  1.0f);
    glVertex3f( 1.0f,-1.0f, -1.0f);
    glEnd();

}

void drawPyramid(){

    float invsqrt5 = 0.4472135955; /* 1.0f/sqrt(5.0f); */
    glBegin(GL_TRIANGLES);
    /* Front */
    glNormal3f(0.0f, invsqrt5, 2.0f*invsqrt5);
    glVertex3f( 0.0f, 1.0f,  0.0f);
    glVertex3f(-1.0f,-1.0f,  1.0f);
    glVertex3f( 1.0f,-1.0f,  1.0f);

    /* Back */
    glNormal3f(0.0f, invsqrt5, -2.0f*invsqrt5);
    glVertex3f( 0.0f, 1.0f,  0.0f);
    glVertex3f( 1.0f,-1.0f, -1.0f);
    glVertex3f(-1.0f,-1.0f, -1.0f);

    /* Left */
    glNormal3f(-2.0f*invsqrt5, invsqrt5, 0);
    glVertex3f( 0.0f, 1.0f,  0.0f);
    glVertex3f(-1.0f,-1.0f, -1.0f);
    glVertex3f(-1.0f,-1.0f,  1.0f);

    /* Right */
    glNormal3f(2.0f*invsqrt5, invsqrt5, 0);
    glVertex3f( 0.0f, 1.0f,  0.0f);
    glVertex3f( 1.0f,-1.0f,  1.0f);
    glVertex3f( 1.0f,-1.0f, -1.0f);

    /* Bottom */
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f( 1.0f,-1.0f,  1.0f);
    glVertex3f(-1.0f,-1.0f,  1.0f);
    glVertex3f( 1.0f,-1.0f, -1.0f);
    glVertex3f(-1.0f,-1.0f,  1.0f);
    glVertex3f(-1.0f,-1.0f, -1.0f);
    glVertex3f( 1.0f,-1.0f, -1.0f);
    glEnd();
}


///////////////////////////////////////////////////////////////////////////////
//
// GLUT functions
//
///////////////////////////////////////////////////////////////////////////////

void keyboard(unsigned char key, int x, int y){

    switch(key) {
    case 27:
        glCleanUp();
        exit(0);
        break;
    case 'r':
        load_shaders();
        break;
    default:
        break;
    }
}

void display(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Enable pipeline program */
    glUseProgramObjectARB(pipelineProgram);

    /* Reset Modelview */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /* Inverse Camera Transform */
    glTranslatef(0.0f, 0.0f, -10.0f);

    setLightSource();

    glRotatef(2.0*xAngle, 1.0f, 0.0f, 0.0f);
    glRotatef(1.2*yAngle, 0.0f, 1.0f, 0.0f);
    glRotatef(zAngle, 0.0f, 0.0f, 1.0f);

    /* Setup the material properties for all objects*/
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    glColor3f(1.0f, 1.0f, 1.0f);

    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuseBlue);
    glutSolidSphere(1.0f, 8,8);

    glPushMatrix();
    glTranslatef(3.0f,0.0f,0.0f);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuseRed);
    drawCube();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-3.0f,0.0f,0.0f);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuseGreen);
    drawPyramid();
    glPopMatrix();

    glUseProgramObjectARB(0);

    glutSwapBuffers();
}

void idle(){
    xAngle+=0.01;
    yAngle+=0.02;
    zAngle+=0.03;
    glutPostRedisplay();
}

void resize(int w, int h) {

    if(h == 0)
        h = 1;

    float ratio = 1.0* w / h;

    /* Reset the coordinate system before modifying */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
    /* Set the viewport to be the entire window */
    glViewport(0, 0, w, h);

    /* Set the correct perspective. */
    gluPerspective(45,ratio,1,100);
}

/*/////////////////////////////////////////////////////////////////////////////
//
// Main
//
/////////////////////////////////////////////////////////////////////////////*/

int main(int argc, char**argv){

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow(argv[0]);

    /* Initialize extensions AFTER having an OpenGL context */
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutIdleFunc(idle);

    glInit();
    glutMainLoop();
    return 0;
}
