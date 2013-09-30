
#include "math/f4x4.h"
#include "simplegraph/simplegraph.h"
#include <stdio.h>   /* Include printf for output */
#include <GL/gl.h>   /* Include gl commands */
#include <GL/glut.h> /* Include GLUT for context creation etc. */


/* Global storage*/
float angle1, angle2, angle3;
int rotateCube=0;

node* sun;
node* mars;
node* earth;
node* moon;

/*///////////////////////////////////////////////////////////////////////////////
//
// My GL functions
//
///////////////////////////////////////////////////////////////////////////////*/

void movePlanets(){
	f4x4 l, l2, l3;
	f3 xAxis = f3_make(1.0f, 0.0f, 0.0f);
	f3 yAxis = f3_make(0.0f, 1.0f, 0.0f);
	f3 zAxis = f3_make(0.0f, 0.0f, 1.0f);

	/* Moon */
	l = f4x4_rotate(angle2,yAxis);
	l2 = f4x4_translate(0.7f,0.0f,0.0f); 
	moon->transform = f4x4_mul(l,l2);

	/* Earth */
	l = f4x4_rotate(angle2,yAxis);
	l2 = f4x4_translate(-2.5f,0.0f,0.0f);
	earth->transform = f4x4_mul(l,l2);

	/* Mars */
	l = f4x4_rotate(angle1,yAxis);
	l2 = f4x4_translate(3.7f,0.0f, 0.0f);

	if(rotateCube){
		f4x4 t1,t2,t3,t4;
		t1 = f4x4_rotate(angle2,xAxis);
		t2 = f4x4_rotate(angle3,yAxis);
		t3 = f4x4_mul(t1,t2);
		t2 = f4x4_rotate(angle1,zAxis);
		t4 = f4x4_mul(t3,t2);
		l2 = f4x4_mul(l2,t4);
		}
	l3 = f4x4_mul(l,l2);
	l = f4x4_scale(0.25f,0.25f,0.25f);
	mars->transform = f4x4_mul(l3, l);
}

void initSceneGraph(){

	/* Initialize angles */
	angle1 = 0.0f;	  
	angle2 = 0.0f;	  
	angle3 = 0.0f;	  

	/* Moon */
	moon = make_node();
	moon->object_no = 4;

	/* Earth */
	earth = make_node();
	earth->object_no = 3;
	add_child(earth,moon);

	/* Mars */
	mars = make_node();
	mars->object_no = 2;

	/* Sun */
	sun = make_node();
	sun->object_no = 1;
	sun->transform = f4x4_id();
	add_child(sun,mars);
	add_child(sun,earth);

	movePlanets();
}

void glInit(){
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void glCleanUp(){
	/* Destroy the allocated nodes */
	destroy_node_rec(sun);
}

void drawCube(){

	/* Select primitive type */
	glBegin(GL_QUADS);
	/* Feed vertices of primitives */

	/* Front */
	glColor3f(1.0f, 0.8f, 0.6f);
	glVertex3f( 1.0f, 1.0f,  1.0f);
	glVertex3f(-1.0f, 1.0f,  1.0f);
	glVertex3f(-1.0f,-1.0f,  1.0f);
	glVertex3f( 1.0f,-1.0f,  1.0f);

	/* Back */
	glVertex3f( 1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f,-1.0f, -1.0f);
	glVertex3f( 1.0f,-1.0f, -1.0f);

	/* Top */
	glColor3f(0.8f, 0.6f, 0.4f);
	glVertex3f( 1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f( 1.0f, 1.0f, 1.0f);

	/* Bottom */
	glVertex3f( 1.0f,-1.0f, -1.0f);
	glVertex3f(-1.0f,-1.0f, -1.0f);
	glVertex3f(-1.0f,-1.0f,  1.0f);
	glVertex3f( 1.0f,-1.0f,  1.0f);

	/* Left */
	glColor3f(0.6f, 0.4f, 0.2f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f,  1.0f);
	glVertex3f(-1.0f,-1.0f,  1.0f);
	glVertex3f(-1.0f,-1.0f, -1.0f);

	/* Right */
	glVertex3f( 1.0f, 1.0f, -1.0f);
	glVertex3f( 1.0f, 1.0f,  1.0f);
	glVertex3f( 1.0f,-1.0f,  1.0f);
	glVertex3f( 1.0f,-1.0f, -1.0f);

	glEnd(); 
	/* End command stream */
}

/*/////////////////////////////////////////////////////////////////////////////
//
// My GLUT functions
//
/////////////////////////////////////////////////////////////////////////////*/

void keyboard(unsigned char key, int x, int y){

	switch(key) {
	case 27:
		glCleanUp();
		exit(0);
		break;
	default:
		break;
	}
}

void traverseGraph(node* n){

	/* apply transformation */
	glPushMatrix();
	glMultMatrixf(n->transform.e);

	/* Select object to render */
	switch(n->object_no){
	case 4: /* Moon */
		glColor3f(0.7f, 0.7f, 0.7f);
		glutSolidSphere(0.1f, 16,16);
		break;
	case 3: /* Earth */
		glColor3f(0.2f, 0.6, 1.0f);
		glutSolidSphere(0.2f, 16,16);
		break;

	case 2: /* Mars */
		drawCube();
		break;

	case 1: /* Sun */
		glColor3f(1.0f, 1.0f, 0.3f);
		glutSolidSphere(0.6f, 16,16);
		break;
	default:
		break;
	}

	/* Render children recursively */
	unsigned int i = 0;
	for(i=0; i < n->children_count; i++){
		traverseGraph(n->children[i]);
	}
	glPopMatrix();
}

void display(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/* Setup camera */
	f4x4 cam = f4x4_translate(0.0f, 0.0f, 10.0f);
	f3 xAxis = f3_make(1.0f, 0.0f, 0.0f);
	f4x4 rot = f4x4_rotate(-90, xAxis);
	cam = f4x4_mul(rot,cam);

	/*  Select the matrix for transformations and reset it */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* Use canonical-to-camera for view transformation */
	f4x4 camTrans;
	f4x4_invert(cam,&camTrans);
	glLoadMatrixf(camTrans.e);

	traverseGraph(sun);
	glutSwapBuffers();

}

void idle(){

	/* Move angles a bit */
	angle1+=0.6;
	angle2+=0.8;
	angle3+=0.3;

	/* Create new transformations */
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
//
// Main
//
/////////////////////////////////////////////////////////////////////////////*/

int main(int argc, char**argv){

	/* On any extra commandline arguments, rotate the cube locally */
	if(argc>=2) rotateCube = 1;

	printf("Initializing GLUT\n");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Fixed Function Pipeline Setting Scenegraph Scene");
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);

	printf("Initializing My OpenGL\n");
	glInit();

	initSceneGraph();

	printf("Executing GLUT Main Loop\n");
	glutMainLoop();

	return 0;
}
