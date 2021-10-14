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
#include "CSCIx229.h"

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
// #define Cos(x) (cos((x)*3.14159265/180))
// #define Sin(x) (sin((x)*3.14159265/180))

double th=30;  //  Rotation angle
double ph=15;
double fov=55;
int mode = 0;
const double dim=8;
double r;
// double asp = (height>0) ? (double)width/height : 1;
double asp = 1;
double Lx = 0;
double Ly = 1;
double Lz = 0;
double Fx = 0;
double Fy = 1;
double Fz;
double xdiff = 0;
double ydiff = 0;
double zdiff;
int light     =   1;  // Lighting
int one       =   1;  // Unit value
double distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
double ambient   =  0.05;  // Ambient intensity (%)
double diffuse   =  0.7;  // Diffuse doubleensity (%)
double specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   2;  // Elevation of light
int t_since_spc;
double rep=1;
int auto_move = 1;
unsigned int textures[10];
typedef struct {float x,y,z;} vtx;
typedef struct {int A,B,C;} tri;
#define n 500
vtx is[n];


/*
 *  Draw vertex in polar coordinates with normal
 *  This function was borrowed from ex13
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 *  This function was borrowed from ex13
 */
static void ball(double x,double y,double z,double r)
{
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball with yellow specular
   float yellow[]   = {1.0,1.0,0.0,1.0};
   float Emission[] = {0.0,0.0,0.01*emission,1.0};
   glColor3f(1,1,1);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (int ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}

// I wrote this function with the guidance of the cube function from ex8
static void rectangular_prism(double x, double y, double z, double dx, double dy, double dz, double th, double ph, double rgb[6]) {
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
    glPushMatrix();
    //  Offset
    glTranslatef(x,y,z);
    glRotatef(th,0,1,0);
    glRotatef(ph,1,0,0);
    glScalef(dx,dy,dz);

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D,textures[1]);

    //  Cube
    glBegin(GL_QUADS);
    //right
    glColor3f(rgb[0],rgb[1],rgb[2]);
    glNormal3f(+1, 0, 0);
    /*glTexCoord2f(0, 0);*/ glVertex3f(1, -1, -1);
    /*glTexCoord2f(1, 0);*/ glVertex3f(1, 1, -1);
    /*glTexCoord2f(1, 1);*/ glVertex3f(1, 1, 1);
    /*glTexCoord2f(0, 1);*/ glVertex3f(1, -1, 1);
    //back
    glColor3f(rgb[3],rgb[4],rgb[5]);
    glNormal3f( 0, 0,-1);
    glVertex3f(-1, -1, -1);
    glVertex3f(1, -1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(-1, 1, -1);
    //left
    glColor3f(rgb[0],rgb[1],rgb[2]);
    glNormal3f(-1, 0, 0);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, 1, -1);
    glVertex3f(-1, 1, 1);
    glVertex3f(-1, -1, 1);
    //top
    glColor3f(rgb[3],rgb[4],rgb[5]);
    glNormal3f( 0,+1, 0);
    glVertex3f(-1, 1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(1, 1, 1);
    glVertex3f(-1, 1, 1);
    //bottom
    glColor3f(rgb[3],rgb[4],rgb[5]);
    glNormal3f( 0,-one, 0);
    glVertex3f(-1, -1, -1);
    glVertex3f(1, -1, -1);
    glVertex3f(1, -1, 1);
    glVertex3f(-1, -1, 1);
    //front
    glColor3f(rgb[3],rgb[4],rgb[5]);
    glNormal3f( 0, 0, 1);
    glVertex3f(-1, -1, 1);
    glVertex3f(1, -1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(-1, 1, 1);

    glEnd();
    
    glPopMatrix();
}

// draws the dirt ground as an array of res x res rectangular prisms
static void ground(double x, double y, double z, double dx, double dy, double dz, double th, double ph, int res) {
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
    glPushMatrix();
    //  Offset
    glTranslatef(x,y,z);
    glRotatef(th,0,1,0);
    glRotatef(ph,1,0,0);
    glScalef(dx,dy,dz);

    double rgb2[] = {0.459, 0.239, 0, 0.459, 0.239, 0}; // brown
    for (float i = -(dim-3); i <= (dim-3); i += (dim-3)/res) {
        for (float j = -(dim-3); j <= (dim-3); j += (dim-3)/res) {
            rectangular_prism(i, y, j, (dim-3)/res, 1, (dim-3)/res, 0, 0, rgb2);
        }
    }
    
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
    // glColor3f(0.314, 0.345, 0.361);
    // glBegin(GL_QUAD_STRIP);
    // for (int angle = 0; angle <= 360; angle += 10) {
    //     glNormal3f(Cos(angle), 0, Sin(angle));
    //     glVertex3f(Cos(angle), 1, Sin(angle));
    //     glVertex3f(Cos(angle), -1, Sin(angle));
    // }
    // glEnd();

    glBegin(GL_QUADS);
    for (int angle = 0; angle < 360; angle += 10) {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glColor3f(1,1,1);
        glBindTexture(GL_TEXTURE_2D,textures[2]);
        glNormal3f(Cos(angle), 0, Sin(angle));
        glTexCoord2f(0, 0); glVertex3f(Cos(angle), 1, Sin(angle));
        glTexCoord2f(0, 1); glVertex3f(Cos(angle), -1, Sin(angle));
        glTexCoord2f(1, 1); glVertex3f(Cos(angle+10), -1, Sin(angle+10));
        glTexCoord2f(1, 0); glVertex3f(Cos(angle+10), 1, Sin(angle+10));
    }
    glEnd();

    // drawing the two lids
    glColor3f(0.871, 0.871, 0.122);
    for (int i = 1; i >= -1; i-=2) {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glColor3f(1,1,1);
        glBindTexture(GL_TEXTURE_2D,textures[1]);

        glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0, i, 0);
        glTexCoord2f(0.5,0.5);
        glVertex3f(0, i, 0);
        for (int angle = 0; angle <= 360; angle += 10) {
            glNormal3f(Cos(angle), i, Sin(angle));
            glTexCoord2f(rep/2*Cos(angle)+0.5,rep/2*Sin(angle)+0.5);
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
      glOrtho(-asp*dim, +asp*dim, -dim,+dim, -dim,+dim);
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

    glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

    //  Light switch
    if (light) // this section was borrowed from ex13
    {
        //  Translate intensity to color vectors
        float Ambient[] = {ambient, ambient, ambient, 1.0};
        float Diffuse[] = {diffuse, diffuse, diffuse, 1.0};
        float Specular[] = {specular, specular, specular, 1.0};
        //  Light position
        float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
        //  Draw light position as ball (still no lighting here)
        glColor3f(1,1,1);
        ball(Position[0],Position[1],Position[2] , 0.1);
        //  OpenGL should normalize normal vectors
        glEnable(GL_NORMALIZE);
        //  Enable lighting
        glEnable(GL_LIGHTING);
        //  Location of viewer for specular calculations
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
        //  glColor sets ambient and diffuse color materials
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        //  Enable light 0
        glEnable(GL_LIGHT0);
        //  Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT0,GL_POSITION,Position);
    }
    else
        glDisable(GL_LIGHTING);

    double rgb1[] = {0.129, 0.529, 0.118, 0.071, 0.388, 0.059}; // array of 2 different colors to be used when making the vehicle body

    rectangular_prism(-1, 0, 0, 0.5, 0.5, 1.2, 90, 0, rgb1); // long body piece
    rectangular_prism(0.5, 0.2, 0, 0.5, 0.65, 1, 0, 0, rgb1); // wide body piece

    cylinder(0.5, -0.15, 1.4, 1, 0.2, 1, 0, 90); // back left wheel
    cylinder(0.5, -0.15, -1.4, 1, 0.2, 1, 0, 90); // back right wheel

    cylinder(0.5, -0.15, 0, 0.1, 1.5, 0.1, 0, 90); // back axle

    cylinder(-2, -0.45, 0.8, 0.7, 0.2, 0.7, 0, 90); // front left wheel
    cylinder(-2, -0.45, -0.8, 0.7, 0.2, 0.7, 0, 90); // front left wheel

    cylinder(-2, -0.45, 0, 0.1, 0.8, 0.1, 0, 90); // front axle

    ground(0, -1.13, 0, 1, 0.2, 1, 0, 0, 15);
    // double rgb2[] = {0.459, 0.239, 0, 0.459, 0.239, 0};
    // rectangular_prism(0, -1.4, 0, dim, 0.2, dim, 0, 0, rgb2);

    // glWindowPos2i(5,5);
    // Print("Mode: Orthogonal");

    // glutGet(GLUT_ELAPSED_TIME);
    
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

// this function was borrowed from ex13
void idle()
{
    //  Elapsed time in seconds
    double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
    zh = fmod(90*t,360.0);
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
    else if (ch == 'F') { // pushes ball along its orbit
        zh += 5;
    }
    else if (ch == 'f') { // pushes ball backwards along its orbit
        zh -= 5;
    }
    else if (ch == 'r') { // lowers ball
        ylight -= 0.1;
        if (ylight < 0) {
            ylight = 0;
        }   
    }
    else if (ch == 'R') { // raises ball
        ylight += 0.1;  
        if (ylight > 10) {
            ylight = 10;
        }   
    }
    else if (ch == 32) { // spacebar | toggles ball orbiting automatically
        auto_move = !auto_move;
        t_since_spc = glutGet(GLUT_ELAPSED_TIME);
    }
    else if (ch == 'V') { // increases radius of orbit
        distance += 0.1;
        if (distance > dim-0.5) {
            distance = dim-0.5;
        }
    }
    else if (ch == 'v') { // decreases radius of orbit
        distance -= 0.1;
        if (distance < 1) {
            distance = 1;
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
    glutIdleFunc(auto_move?idle:NULL);
    //  Reproject
    Project();
    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

// This function was borrowed from ex4.c
void reshape(int width,int height)
{
    asp = (height>0) ? (double)width/height : 1;
    //  Set the viewport to the entire window
    glViewport(0,0, RES*width,RES*height);

    Project();
}

int main(int argc,char* argv[])
{
    r = dim*2;
    Fz = r;
    zdiff = r;
    //  Initialize GLUT
    glutInit(&argc,argv);
    //  Request double buffered true color window with Z-buffer
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //  Create window
    glutCreateWindow("HW6 - Robert Dumitrescu");
    glClearColor((double)12/255,(double)28/255,(double)65/255, 1); // background color
    //  Register display and key callbacks
    glutDisplayFunc(display);
    glutSpecialFunc(special);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);
    //  Enable Z-buffer depth test
    glEnable(GL_DEPTH_TEST);
    //  Pass control to GLUT for events
    textures[0] = LoadTexBMP("textures/img1.bmp");
    textures[1] = LoadTexBMP("textures/wheel.bmp");
    textures[2] = LoadTexBMP("textures/tire.bmp");
    glutMainLoop();
    //  Return to OS
    return 0;
}
