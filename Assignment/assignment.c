
#include "math/f4x4.h"
#include <stdio.h> /* Include printf for output */
#include <stdlib.h>
#include <GL/gl.h> /* Include gl commands */
#include <GL/glut.h> /* Include GLUT for context creation etc. */

/* Global storage*/
unsigned int object_count = 0;

struct node;

node* camera;
node* earth;
node* sun;

/*/////////////////////////////////////////////////////////////////////////////
//
// Scene graph
//
/////////////////////////////////////////////////////////////////////////////*/

#define GRAPH_NODE_MAX_CHILDREN

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
//
// My GL functions
//
/////////////////////////////////////////////////////////////////////////////*/

void drawSun()
{
    glColor3f(1.0f, 1.0f, 0.3f);
    glutSolidSphere(0.6f, 16, 16);
}

void drawEarth()
{
    glColor3f(0.2f, 0.6f, 1.0f);
    glutSolidSphere(0.2f, 16, 16);
}

void drawCamera()
{
}

void movePlanets()
{
    f4x4 l, l2;
    f3 yAxis = f3_make(0.0f, 1.0f, 0.0f);

    /* Earth */
    l = f4x4_rotate(0.33f./, yAxis);
    l2 = f4x4_translate(-2.5f, 0.0f, 0.0f);
    earth->transform = f4x4_mul(l, l2);
}

void initSceneGraph()
{
    /* Initialize camera */
    camera = make_node(0);
    f4x4 cam = f4x4_translate(0.0f, 0.0f, 5.0f);
    f3 yAxis = f3_make(1.0f, 0.0f, 0.0f);
    f4x4 rot = f4x4_rotate(-90, yAxis);
    cam = f4x4_mul(rot, cam);
    camera->transform = cam;

    /* Earth */
    earth = make_node(drawEarth);

    /* Sun */
    sun = make_node(drawSun);
    sun->transform = f4x4_id();
    add_child(sun, earth);
}

void glInit()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
}

void glCleanUp()
{
    /* destroy the allocated nodes */
}


/*/////////////////////////////////////////////////////////////////////////////
//
// My GLUT functions
//
/////////////////////////////////////////////////////////////////////////////*/


void traverseGraph(node* n)
{
    glPushMatrix();
    glMultMatrixf(n->transform.e);

    if (n->draw != 0)
    {
	(*n->draw)();
    }

    /* Render children recursively */
    for (int i = 0; i < n->children_count; i++)
    {
	traverseGraph(n->children[i]);
    }
    
    glPopMatrix();
}


void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 27:
	glCleanUp();
	exit(0);
	break;
    default:
	break;
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Select the model view */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); /* Reset the model view matrix */

    /* Use canonical-to-camera for view transformations */
    f4x4 camTrans = frame_to_canonical(camera);
    glLoadMatrixf(camTrans.e);

    traverseGraph(sun);

    glutSwapBuffers();
}

void idle()
{
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
//
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
