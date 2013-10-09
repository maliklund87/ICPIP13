
#include "math/f4x4.h"
#include <stdio.h> /* Include printf for output */
#include <stdlib.h>
#include <stdarg.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <cmath>

#include <GL/glew.h>
#include <GL/gl.h> /* Include gl commands */
#include <GL/glut.h> /* Include GLUT for context creation etc. */


/* sec:global */
/* Global storage*/
unsigned int object_count = 0;
int timestamp;

float angle1, angle2, angle3;

struct node;

node* origin;
node* camera;
node* square;
node* pyramid;
node* pyramid_p;

#define CENTER_LIST_SIZE 3
unsigned int centerListSize = CENTER_LIST_SIZE;
unsigned int selectedCenter = 0;
node* centerList[sizeof(node*) * CENTER_LIST_SIZE];

GLuint spongebob;
GLuint pineapple;

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


/*/////////////////////////////////////////////////////////////////////////////
// sec:scenegraph
// Scene graph
//
/////////////////////////////////////////////////////////////////////////////*/

#define GRAPH_NODE_MAX_CHILDREN 10

struct node
{
    f4x4 transform;
    void (*draw)();
    unsigned int object_no;
    int children_count;
    node* children[GRAPH_NODE_MAX_CHILDREN];
    node* parent;
};

node* make_node(void (*drawFunction)())
{
    node* n = (node*) malloc(sizeof(node));
    
    for (int i = 0; i < GRAPH_NODE_MAX_CHILDREN; i++)
    {
	n->children[i] = 0;
    }

    n->draw = drawFunction;
    
    n->object_no = object_count;
    object_count += 1;

    
    n->children_count = 0;
    n->parent = 0;
    
    return n;
}

void add_child(node* parent, node* child)
{
    if(parent->children_count < GRAPH_NODE_MAX_CHILDREN - 1)
    {
	parent->children[parent->children_count] = child;
	parent->children_count += 1;
	child->parent = parent;
    }
}

void remove_child(node* parent, node* child)
{
    int i = 0;
    int found = 0;
    while(i < GRAPH_NODE_MAX_CHILDREN && found == 0)
    {
	if (parent->children[i] == child)
	{
	    found = 1;
	    parent->children[i] = 0;
	    parent->children_count -= 1;
	    child->parent = 0;
	} else
	{
	    i++;
	}
    }

    /* Move any remaining children back one spot*/
    while(i < (GRAPH_NODE_MAX_CHILDREN - 1))
    {
	parent->children[i] = parent->children[i+1];
	i++;
    }
}

void destroy_node(node* n)
{
    free(n);
}

void destroy_node_rec(node* n)
{
    for (int i = 0; i < n->children_count; i++)
    {
	destroy_node_rec(n->children[i]);
    }
    destroy_node(n);
}

f4x4 frame_to_canonical(node* n){
	node* currentNode = n;
	node* currentParent = n->parent;

	f4x4 intermediateInverseMatrix;
	f4x4 inverseParentMatrix;
	f4x4_invert(currentNode->transform, &intermediateInverseMatrix);

	while (currentParent != 0){
	    /* Get the parents transformation matrix */
	    f4x4_invert(currentParent->transform, &inverseParentMatrix);

	    /* Multiply the parents inverse transformation matrix onto the partial reversal matrix */
	    intermediateInverseMatrix = f4x4_mul(intermediateInverseMatrix, inverseParentMatrix);

	    // Prepare for next loop;
	    currentNode = currentParent;
	    currentParent = currentNode->parent;
	}

	return intermediateInverseMatrix;
}
    
/*/////////////////////////////////////////////////////////////////////////////
// sec:shader
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
// sec:mygl
// My GL functions
//
/////////////////////////////////////////////////////////////////////////////*/

GLuint load_texture(const char *filename, int w, int h, int depth) {
    /* Load a texture*/
    const unsigned int fileSize = w*h*depth;
    char *tex = (char*) malloc(fileSize);
    FILE *texFile = fopen(filename,"rb");
    if(texFile == NULL){
        printf("Panic: Couldn't open texture file\n");
        exit(-1);
    }
    fread(tex, sizeof(char), fileSize, texFile);

    GLuint tex_id;
    glGenTextures(1, &tex_id);

    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, depth, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, tex);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);	/* Linear Filtering */
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);	/* Linear Filtering */
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);    /* Clamp to border  */
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);    /* Clamp to border  */

    free(tex);
    return tex_id;
}

void setLightSource(){
    /* Setup the light source properties */
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

void drawSun()
{
    glColor3f(1.0f, 1.0f, 0.3f);
    glutSolidSphere(0.6f, 16, 16);
}

void drawCube(){
    float l = 0.5f;
    glBindTexture(GL_TEXTURE_2D, spongebob);

    glBegin(GL_QUADS);

    glColor3f(1.0f, 1.0f, 1.0f);
    /* Front */
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( l, l,  l);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-l, l,  l);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-l,-l,  l);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( l,-l,  l);

    /* Back */
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( l, l, -l);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-l, l, -l);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-l,-l, -l);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( l,-l, -l);


    /* Top */
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( l, l, -l);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-l, l, -l);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-l, l, l);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( l, l, l);

    /* Bottom */
    glNormal3f(0.0f,-1.0f,0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( l,-l, -l);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-l,-l, -l);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-l,-l,  l);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( l,-l,  l);


    /* Left */
    glNormal3f(-1.0f,0.0f,0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-l, l, -l);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-l, l,  l);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-l,-l,  l);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-l,-l, -l);

    /* Right */
    glNormal3f(1.0f,0.0f,0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( l, l, -l);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( l, l,  l);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( l,-l,  l);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( l,-l, -l);
    glEnd();
}

void drawPyramid(){
    float l = 1.0f;
    float invsqrt5 = 0.4472135955; /* 1.0f/sqrt(5.0f); */
    glBindTexture(GL_TEXTURE_2D, pineapple);
    glBegin(GL_TRIANGLES);

    /* Front */
    glColor3f(1.0f, 1.0f, 0.5f);
    glNormal3f(0.0f, invsqrt5, 2.0f*invsqrt5);
    glTexCoord2f(0.5f, 0.0f); glVertex3f( 0.0f, l,  0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( l,-l,  l);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-l,-l,  l);

    /* Back */
    glNormal3f(0.0f, invsqrt5, -2.0f*invsqrt5);
    glTexCoord2f(0.5f, 0.0f); glVertex3f( 0.0f, l,  0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-l,-l, -l);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( l,-l, -l);

    /* Left */
    glNormal3f(-2.0f*invsqrt5, invsqrt5, 0);
    glTexCoord2f(0.5f, 0.0f); glVertex3f( 0.0f, l,  0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-l,-l,  l); 
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-l,-l, -l);

    /* Right */
    glNormal3f(2.0f*invsqrt5, invsqrt5, 0);
    glTexCoord2f(0.5f, 0.0f); glVertex3f( 0.0f, l,  0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( l,-l, -l);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( l,-l,  l);

    /* Bottom */
    glColor3f(1.0f, 1.0f, 0.5f);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-l,-l,  l);
    glVertex3f( l,-l,  l);
    glVertex3f( l,-l, -l);
    glVertex3f(-l,-l,  l);
    glVertex3f( l,-l, -l);
    glVertex3f(-l,-l, -l);
    glEnd();
}

void drawSquare()
{
    glColor3f(0.2f, 0.6f, 1.0f);
    glutSolidSphere(0.2f, 16, 16);
}

void drawNothing()
{
}

void movePlanets()
{
    f4x4 l, l2;
    f3 xAxis = f3_make(1.0f, 0.0f, 0.0f);
    f3 yAxis = f3_make(0.0f, 1.0f, 0.0f);
    f3 zAxis = f3_make(0.0f, 0.0f, 1.0f);

    /* Square */
    l = f4x4_rotate(angle2, yAxis);
    l2 = f4x4_translate(-2.5f, 0.0f, 0.0f);
    square->transform = f4x4_mul(l, l2);

    /* Pyramid */
    f4x4 t1, t2, t3, t4;
    t1 = f4x4_rotate(angle2,xAxis);
    t2 = f4x4_rotate(angle3,yAxis);
    t3 = f4x4_mul(t1,t2);
    t2 = f4x4_rotate(angle1,zAxis);
    t4 = f4x4_mul(t3,t2);
    pyramid->transform = t4;

    /* Pyramid position */
    l = f4x4_rotate(angle2, yAxis);
    l2 = f4x4_translate(1.0f, 0.0f, 0.0f);
    pyramid_p->transform = f4x4_mul(l, l2);
}

void initSceneGraph()
{
    timestamp = glutGet(GLUT_ELAPSED_TIME);
    angle1 = 0.0f;
    angle2 = 0.0f;
    angle3 = 0.0f;
    
    /* Initialize camera */
    camera = make_node(0);
    f4x4 cam = f4x4_translate(0.0f, 0.0f, 10.0f);
    f3 xAxis = f3_make(1.0f, 0.0f, 0.0f);
    f4x4 rot = f4x4_rotate(-90, xAxis);
    cam = f4x4_mul(rot, cam);
    camera->transform = cam;

    /* Square */
    square = make_node(drawCube);
    
    /* Pyramid */
    pyramid = make_node(drawPyramid);

    /* Pyramid position */
    pyramid_p = make_node(0);
    add_child(pyramid_p, square);
    add_child(pyramid_p, camera);
    add_child(pyramid_p, pyramid);

    origin = make_node(0);
    origin->transform = f4x4_id();
    add_child(origin, pyramid_p);

    centerList[0] = origin;
    centerList[1] = pyramid_p;
    centerList[2] = square;

    movePlanets();
}

void glInit()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 0.1f);

    glEnable(GL_CULL_FACE);

    spongebob = load_texture("spongebob.raw", 512, 512, 3);
    pineapple = load_texture("pineapple.raw", 512, 512, 3);

    load_shaders();
}

void glCleanUp()
{
    /* destroy the allocated nodes */
    destroy_node_rec(origin);
    glDeleteObjectARB(pipelineProgram);
    glDeleteObjectARB(vertexShader);
    glDeleteObjectARB(fragmentShader);
}


/*/////////////////////////////////////////////////////////////////////////////
// sec:myglut
// My GLUT functions
//
/////////////////////////////////////////////////////////////////////////////*/


void traverseGraph(node* n)
{
    glPushMatrix();
    glMultMatrixf(n->transform.e);

    if (n->draw != 0)
    {
	(*n->draw)(); // call function pointed to by draw
    }

    /* Render children recursively */
    for (int i = 0; i < n->children_count; i++)
    {
	traverseGraph(n->children[i]);
    }
    
    glPopMatrix();
}

/* Changes on which object the camera is centered */
void changeCenter()
{
    remove_child(camera->parent, camera);
    selectedCenter = (selectedCenter + 1) % centerListSize;
    node* selectedNode = centerList[selectedCenter];
    add_child(selectedNode, camera);
}


void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 27:
	glCleanUp();
	exit(0);
	break;
    case 32:
	changeCenter();
	break;
    default:
	break;
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Comment out the glUseProgramObjectARB-lines to view simple texturing */
    glUseProgramObjectARB(pipelineProgram);

    glEnable(GL_TEXTURE_2D);

    /* Select the model view */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); /* Reset the model view matrix */

    /* Use canonical-to-camera for view transformations */
    f4x4 camTrans = frame_to_canonical(camera);
    glLoadMatrixf(camTrans.e);

    /* glLightfv(GL_LIGHT0, GL_POSITION, camTrans.e); */
    setLightSource();

    /* Use the same material properties for all objects */
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    traverseGraph(origin);

    glUseProgramObjectARB(0);

    glutSwapBuffers();
}

void idle()
{
    /* Attempt to smooth things out a bit */
    int time = glutGet(GLUT_ELAPSED_TIME);
    int dt = time - timestamp;
    if (dt > 10)
    {
	float dSec = dt/1000.0f;
	/* Move angles a bit */
	angle1 += 90*dSec;
	angle2 += 120*dSec;
	angle3 += 180*dSec;	
	
	timestamp = time;
    }

    
    /* Do stuff */
    movePlanets();
    
    glutPostRedisplay();
}

void resize(int w, int h) {

	if(h == 0)
		h = 1;

	float ratio = 1.0* w / h;

	/* Set the viewport to be the entire window */
	glViewport(0, 0, w, h);

	/* Reset the coordinate system before modifying */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	/*  Set the correct perspective using the perspective transform of glu*/
	/* This will create a perspective matrix and multiply it onto current matrix */
	gluPerspective(45,ratio,1,100);
}


/*/////////////////////////////////////////////////////////////////////////////
// sec:main
// Main
//
/////////////////////////////////////////////////////////////////////////////*/

int main(int argc, char**argv)
{
    printf("Initializing GLUT\n");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("IGPIP13 graphics assignment");

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

    printf("Initializing my OpenGL\n");
    glInit();

    /* INITIALIZE SCENE GRAPH */
    initSceneGraph();

    printf("Executing GLUT Main Loop\n");
    glutMainLoop();

    return 0;
}
