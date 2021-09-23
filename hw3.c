/*
*  Hello World Triangle Version 3
*/

#define GL_GLEXT_PROTOTYPES

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <stdarg.h>
#include <math.h>

#ifdef USEGLEW
#include <GL/glew.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glut.h>
#endif

#ifndef RES
#define RES 1
#endif

#define NUM_COORDS 50000
#define BUFSIZE 128
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define LEN 8194

#define Cos(x) (cos((x)*3.14159265/180))
#define Sin(x) (sin((x)*3.14159265/180))

double th=0;  //  Rotation angle
double ph=0;


void Print(char s[LEN]) {
    int i;
    // int bitmap_len = glutBitmapLength(GLUT_BITMAP_8_BY_13, s);
    // glRasterPos2f(0, 0);
    // glColor3f(1, 1, 1);
    char * ch = s;
    for (i = 0; i < strlen(s); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *ch++);
    }
}

static void rectangular_prism(double x, double y, double z, double dx, double dy, double dz, double th, double ph, double rgb[6]) {
    glPushMatrix();
    //  Offset
    glTranslatef(x,y,z);
    glRotatef(th,0,1,0);
    glRotatef(ph,1,0,0);
    glScalef(dx,dy,dz);
    //  Cube
    glBegin(GL_QUADS);
    //right
    glColor3f(rgb[0],rgb[1],rgb[2]);
    glVertex3f(1,-1,-1);
    glVertex3f(1,1,-1);
    glVertex3f(1,1,1);
    glVertex3f(1,-1,1);
    //back
    glColor3f(rgb[3],rgb[4],rgb[5]);
    glVertex3f(-1, -1, -1);
    glVertex3f(1, -1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(-1, 1, -1);
    //left
    glColor3f(rgb[0],rgb[1],rgb[2]);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, 1, -1);
    glVertex3f(-1, 1, 1);
    glVertex3f(-1, -1, 1);
    //top
    glColor3f(rgb[3],rgb[4],rgb[5]);
    glVertex3f(-1, 1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(1, 1, 1);
    glVertex3f(-1, 1, 1);
    //bottom
    glColor3f(rgb[3],rgb[4],rgb[5]);
    glVertex3f(-1, -1, -1);
    glVertex3f(1, -1, -1);
    glVertex3f(1, -1, 1);
    glVertex3f(-1, -1, 1);
    //front
    glColor3f(rgb[3],rgb[4],rgb[5]);
    glVertex3f(-1, -1, 1);
    glVertex3f(1, -1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(-1, 1, 1);

    //  End
    glEnd();
    //  Undo transformations
    glPopMatrix();
}

static void cylinder(double x, double y, double z, double dx, double dy, double dz, double th, double ph) {
    glPushMatrix();
    
    glTranslatef(x,y,z);
    glRotatef(th,0,1,0);
    glRotatef(ph,1,0,0);
    glScalef(dx,dy,dz);

    glColor3f(0.314, 0.345, 0.361);
    glBegin(GL_QUAD_STRIP);
    for (int angle = 0; angle <= 360; angle += 10) {
        glVertex3f(Cos(angle), 1, Sin(angle));
        glVertex3f(Cos(angle), -1, Sin(angle));
    }
    glEnd();

    glColor3f(0.871, 0.871, 0.122);
    for (int i = 1; i >= -1; i-=2) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, i, 0);
        for (int angle = 0; angle <= 360; angle += 10) {
            glVertex3f(Cos(angle), i, Sin(angle));
        }
        glEnd();
    }

    glPopMatrix();
}

/*
* This function is called by GLUT to display the scene
*/
void display()
{
    char info[LEN];
    //  Clear screen and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    //  Reset transformations
    glLoadIdentity();

    glRotatef(ph,1,0,0);
    glRotatef(th,0,1,0);

    double rgb[] = {0.129, 0.529, 0.118, 0.071, 0.388, 0.059};

    rectangular_prism(-1, 0, 0, 0.5, 0.5, 1.2, 90, 0, rgb); // long body piece
    rectangular_prism(0.5, 0.2, 0, 0.5, 0.65, 1, 0, 0, rgb); // wide body piece

    cylinder(0.5, -0.15, 1.4, 1, 0.2, 1, 0, 90); // back left wheel
    cylinder(0.5, -0.15, -1.4, 1, 0.2, 1, 0, 90); // back right wheel

    cylinder(0.5, -0.15, 0, 0.1, 1.5, 0.1, 0, 90); // back axle

    cylinder(-2, -0.45, 0.8, 0.7, 0.2, 0.7, 0, 90); // front left wheel
    cylinder(-2, -0.45, -0.8, 0.7, 0.2, 0.7, 0, 90); // front left wheel

    cylinder(-2, -0.45, 0, 0.1, 0.8, 0.1, 0, 90); // back axle

    glEnd();
    //  Flush and swap buffer
    glFlush();
    glutSwapBuffers();
}

/*
* This function is called by GLUT when special keys are pressed
*/
// This function was borrowed from ex4.c
void special(int key,int x,int y)
{
    //  Right arrow - increase rotation by 5 degree
    if (key == GLUT_KEY_RIGHT)
        th += 5;
    //  Left arrow - decrease rotation by 5 degree
    else if (key == GLUT_KEY_LEFT)
        th -= 5;
    //  Up arrow key - increase elevation by 5 degrees
    else if (key == GLUT_KEY_UP)
        ph += 5;
    //  Down arrow key - decrease elevation by 5 degrees
    else if (key == GLUT_KEY_DOWN)
        ph -= 5;
    //  Request display update
    glutPostRedisplay();
}

// This function was borrowed from ex4.c
void reshape(int width,int height)
{
    //  Set the viewport to the entire window
    glViewport(0,0, RES*width,RES*height);
    //  Tell OpenGL we want to manipulate the projection matrix
    glMatrixMode(GL_PROJECTION);
    //  Undo previous transformations
    glLoadIdentity();
    //  Orthogonal projection
    const double dim=2.5;
    double asp = (height>0) ? (double)width/height : 1;
    glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
    //  Switch to manipulating the model matrix
    glMatrixMode(GL_MODELVIEW);
    //  Undo previous transformations
    glLoadIdentity();
}

/*
* GLUT based Hello World
*/
int main(int argc,char* argv[])
{
    //  Initialize GLUT
    glutInit(&argc,argv);
    //  Request double buffered true color window with Z-buffer
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //  Create window
    glutCreateWindow("HW3");
    //  Register display and key callbacks
    glutDisplayFunc(display);
    glutSpecialFunc(special);
    glutReshapeFunc(reshape);
    //  Enable Z-buffer depth test
    glEnable(GL_DEPTH_TEST);
    //  Pass control to GLUT for events
    glutMainLoop();
    //  Return to OS
    return 0;
}
