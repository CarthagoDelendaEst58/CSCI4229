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

// These two convenience functions were borrowed from ex8
#define Cos(x) (cos((x)*3.14159265/180))
#define Sin(x) (sin((x)*3.14159265/180))

double th=30;  //  Rotation angle
double ph=15;
double fov=55;
int mode = 0;
const double dim=10;
const double r = dim*2;
// double asp = (height>0) ? (double)width/height : 1;
double asp = 1;
double Lx = 0;
double Ly = 1;
double Lz = 0;
double Fx = 0;
double Fy = 1;
double Fz = r;
double xdiff = 0;
double ydiff = 0;
double zdiff = r;

// void Print(const char* format , ...)
// {
//    char    buf[LEN];
//    char*   ch=buf;
//    va_list args;
//    //  Turn the parameters into a character string
//    va_start(args,format);
//    vsnprintf(buf,LEN,format,args);
//    va_end(args);
//    //  Display the characters one at a time at the current raster position
//    while (*ch)
//       glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
// }

// I wrote this function with the guidance of the cube function from ex8
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
    glVertex3f(1, -1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(1, 1, 1);
    glVertex3f(1, -1, 1);
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

    glEnd();
    
    glPopMatrix();
}

// This function takes inspiration from the sphere1 function from ex8, essentially drawing one latitude band with no angle and using triangle fans for the lid
static void cylinder(double x, double y, double z, double dx, double dy, double dz, double th, double ph) {
    glPushMatrix();
    
    glTranslatef(x,y,z);
    glRotatef(th,0,1,0);
    glRotatef(ph,1,0,0);
    glScalef(dx,dy,dz);

    // drawing the shell
    glColor3f(0.314, 0.345, 0.361);
    glBegin(GL_QUAD_STRIP);
    for (int angle = 0; angle <= 360; angle += 10) {
        glVertex3f(Cos(angle), 1, Sin(angle));
        glVertex3f(Cos(angle), -1, Sin(angle));
    }
    glEnd();

    // drawing the two lids
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

int getSign(double x) {
    if (x < 0)
        return -1;
    return 1;
}

double absolute(double x) {
    if (x < 0) {
        return x*-1;
    }
    return x;
}

// This function was borrowed from ex9.c
static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode)
      gluPerspective(fov,asp,dim/16,16*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

void display()
{
    //  Clear screen and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    //  Reset transformations
    glLoadIdentity();

    if (mode == 1) // perspective
    {
        double Ex = -2*dim*Sin(th)*Cos(ph);
        double Ey = +2*dim        *Sin(ph);
        double Ez = +2*dim*Cos(th)*Cos(ph);
        gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
    }
    else if (mode == 2) { // first person
        Lx = r*Sin(th) + xdiff; // changing coordinates to look at based on angle and position changes
        Lz = -1*r*Cos(th)*Cos(ph) + zdiff;
        Ly = r*Sin(ph);

        gluLookAt(Fx,Fy,Fz , Lx,Ly,Lz , 0,Cos(ph),0);
    }
   //  Orthogonal - set world orientation
    else
    {
        glRotatef(ph,1,0,0);
        glRotatef(th,0,1,0);
    }

    double rgb[] = {0.129, 0.529, 0.118, 0.071, 0.388, 0.059}; // array of 2 different colors to be used when making the vehicle body

    rectangular_prism(-1, 0, 0, 0.5, 0.5, 1.2, 90, 0, rgb); // long body piece
    rectangular_prism(0.5, 0.2, 0, 0.5, 0.65, 1, 0, 0, rgb); // wide body piece

    cylinder(0.5, -0.15, 1.4, 1, 0.2, 1, 0, 90); // back left wheel
    cylinder(0.5, -0.15, -1.4, 1, 0.2, 1, 0, 90); // back right wheel

    cylinder(0.5, -0.15, 0, 0.1, 1.5, 0.1, 0, 90); // back axle

    cylinder(-2, -0.45, 0.8, 0.7, 0.2, 0.7, 0, 90); // front left wheel
    cylinder(-2, -0.45, -0.8, 0.7, 0.2, 0.7, 0, 90); // front left wheel

    cylinder(-2, -0.45, 0, 0.1, 0.8, 0.1, 0, 90); // front axle

    glPushMatrix();
    glBegin(GL_QUADS); // Ground
    glColor3f(0.459, 0.239, 0);
    glVertex3f(-dim, -1.2, -dim);
    glVertex3f(-dim, -1.2, dim);
    glVertex3f(dim, -1.2, dim);
    glVertex3f(dim, -1.2, -dim);
    glEnd();
    glPopMatrix();

    // glWindowPos2i(5,5);
    // Print("Mode: Orthogonal");
    
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
    if (key == GLUT_KEY_RIGHT) {
        th += 5;     
    }
    //  Left arrow - decrease rotation by 5 degree
    else if (key == GLUT_KEY_LEFT) {
        th -= 5;
    }
    //  Up arrow key - increase elevation by 5 degrees
    else if (key == GLUT_KEY_UP) {
        ph += 5;
        if (ph > 75) 
            ph = 75;
    }
    //  Down arrow key - decrease elevation by 5 degrees
    else if (key == GLUT_KEY_DOWN) {
        ph -= 5;
        if (ph < -75)
            ph = -75;
    }
    //  Request display update
    glutPostRedisplay();
}

// this function was borrowed from ex9
void key(unsigned char ch,int x,int y)
{
    //  Exit on ESC
    if (ch == 27)
        exit(0);
    else if (ch == 'm' || ch == 'M') {
        mode = (mode+1)%3;
        if (mode == 2) {
            th = 0;
            ph = 0;
        }
    }

    if (mode == 2) { // first person movement
        double R = 2*r;
        if (ch == 'w' || ch == 'W') {
            xdiff += -1*(Fx-Lx)/R; // determining direction of view
            zdiff += -1*(Fz-Lz)/R;
            Fx = xdiff;
            Fz = zdiff;
        }

        else if (ch == 'a' || ch == 'A') {
            xdiff += -1*(Fz-Lz)/R; 
            zdiff += (Fx-Lx)/R;
            Fx = xdiff;
            Fz = zdiff;
        }

        else if (ch == 's' || ch == 'S') {
            xdiff += (Fx-Lx)/R;
            zdiff += (Fz-Lz)/R;
            Fx = xdiff;
            Fz = zdiff;
        }

        else if (ch == 'd' || ch == 'D') {
            xdiff += (Fz-Lz)/R;
            zdiff += -1*(Fx-Lx)/R;
            Fx = xdiff;
            Fz = zdiff;
        }
    }
    //  Reproject
    Project();
    //  Tell GLUT it is necessary to redisplay the scene
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
    // glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
    Project();
    //  Switch to manipulating the model matrix
    glMatrixMode(GL_MODELVIEW);
    //  Undo previous transformations
    glLoadIdentity();
}

int main(int argc,char* argv[])
{
    //  Initialize GLUT
    glutInit(&argc,argv);
    //  Request double buffered true color window with Z-buffer
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //  Create window
    glutCreateWindow("HW4 - Robert Dumitrescu");
    glClearColor(0.063, 0.608, 0.902, 1); // background color
    //  Register display and key callbacks
    glutDisplayFunc(display);
    glutSpecialFunc(special);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key);
    //  Enable Z-buffer depth test
    glEnable(GL_DEPTH_TEST);
    //  Pass control to GLUT for events
    glutMainLoop();
    //  Return to OS
    return 0;
}
