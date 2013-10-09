#include <stdio.h>

#include <GL/gl.h>
#include <GL/glut.h>

/*/// Global storage ///*/
float xAngle, yAngle, zAngle;

GLuint texture;

/*/////////////////////////////////////////////////////////////////////////////
//
// GL functions
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
    glTexParameterf(GL_TEXTURE_2D, GLT_EXTURE_WRAP_T, GL_CLAMP);    /* Clamp to border  */

    free(tex);
    return tex_id;
}


void glInit(){
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.2f, 0.0f);

    /* Initialize angles */
    xAngle = 0.0f;	  
    yAngle = 0.0f;	  
    zAngle = 0.0f;	  

    texture = load_texture("corn_shade.raw", 256, 256, 3);

}

void glCleanUp(){

}

void drawCube(){
    glBegin(GL_QUADS);
    /* Front */
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f,  1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,-1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,-1.0f,  1.0f);

    /* Back */
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,-1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,-1.0f, -1.0f);


    /* Top */
    glColor3f(0.0f, 1.0f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, 1.0f, 1.0f);

    /* Bottom */
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,-1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,-1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,-1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,-1.0f,  1.0f);


    /* Left */
    glColor3f(1.0f, 0.0f, 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f,  1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,-1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f,-1.0f, -1.0f);

    /* Right */
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, 1.0f,  1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f,-1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,-1.0f, -1.0f);
    glEnd();
}

void drawPyramid(){
    glBegin(GL_TRIANGLES);
    /* Front */
    glColor3f(1.0f, 1.0f, 0.5f);
    glVertex3f( 0.0f, 1.0f,  0.0f);
    glVertex3f( 1.0f,-1.0f,  1.0f);
    glVertex3f(-1.0f,-1.0f,  1.0f);

    /* Back */
    glVertex3f( 0.0f, 1.0f,  0.0f);
    glVertex3f(-1.0f,-1.0f, -1.0f);
    glVertex3f( 1.0f,-1.0f, -1.0f);

    /* Left */
    glColor3f(0.5f, 0.0f, 1.0f);
    glVertex3f( 0.0f, 1.0f,  0.0f);
    glVertex3f(-1.0f,-1.0f,  1.0f);
    glVertex3f(-1.0f,-1.0f, -1.0f);

    /* Right */
    glVertex3f( 0.0f, 1.0f,  0.0f);
    glVertex3f( 1.0f,-1.0f, -1.0f);
    glVertex3f( 1.0f,-1.0f,  1.0f);

    /* Bottom */
    glColor3f(1.0f, 1.0f, 0.5f);
    glVertex3f(-1.0f,-1.0f,  1.0f);
    glVertex3f( 1.0f,-1.0f,  1.0f);
    glVertex3f( 1.0f,-1.0f, -1.0f);
    glVertex3f(-1.0f,-1.0f,  1.0f);
    glVertex3f( 1.0f,-1.0f, -1.0f);
    glVertex3f(-1.0f,-1.0f, -1.0f);
    glEnd();
}

/*/////////////////////////////////////////////////////////////////////////////
//
// GLUT functions
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

void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, -10.0f);

    glRotatef(xAngle, 1.0f, 0.0f, 0.0f);
    glRotatef(yAngle, 0.0f, 1.0f, 0.0f);
    glRotatef(zAngle, 0.0f, 0.0f, 1.0f);

    glPushMatrix();
    glTranslatef(2.0f,0.0f,0.0f);

    glDisable(GL_TEXTURE_2D);
    drawPyramid();

    glPopMatrix();
    glTranslatef(-2.0f,0.0f,0.0f);
    glEnable(GL_TEXTURE_2D);
    drawCube();
    glPopMatrix();

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
    glutInitWindowSize(1024, 768);
    glutCreateWindow(argv[0]);

    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutIdleFunc(idle);

    glInit();
    glutMainLoop();

    return 0;
}
