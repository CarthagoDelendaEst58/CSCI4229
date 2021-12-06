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
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 900
#define LEN 8194
#define NUM_SCENES 3
#define ANGLE_RES 10
#define NUM_OBJECTS 5
#define NUM_MODES 4
#define NUM_CAMERAS 1

double th=25;  //  Rotation angle
double ph=25;
double fov=50;
int mode = 1;
int scene = 2;
const double dim=40;
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
double ambient   =  0.3;  // Ambient intensity (%)
double diffuse   =  0.5;  // Diffuse doubleensity (%)
double specular  =   0.05;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   20;  // Elevation of light
int t_since_spc;
double rep=1;
int auto_move = 0;
unsigned int textures[10];
int objects[NUM_OBJECTS];
typedef struct {double x,y,z;} vtx;
typedef struct {int A,B,C;} tri;
int car_th = 0;
double car_velocity = 0;
double deceleration = 0.8;
double acceleration = 5;
double rotation_rate = 90;
double windmill_rotation_rate = 30;
double windmill_angle = 0;
double t;
int num_tree_positions = 0;
#define n 500
vtx is[n];
vtx car_vector;
vtx car_pos;
vtx cams[NUM_CAMERAS] = {{0, 20, 0}};
float * positions;
int daytime = 1;

// This structure keeps track of which keys are being held down
typedef struct {
    char m;
    char f;
    char F;
    char g;
    char r;
    char R;
    char w;
    char a;
    char s;
    char d;
    char v;
    char V;
    char spc;
    char esc;
    char left;
    char down;
    char up;
    char right;
} KeyState;

KeyState state = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

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
   glNormal3f(x,y,z);
   glVertex3f(x,y,z);
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

// This function was taken from an example
static void cube(double x, double y, double z, double dx, double dy, double dz, double th, double ph, double delta, int object) {
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
    glRotatef(delta, 0, 0, 1);
    glScalef(dx,dy,dz);

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor3f(1, 1, 1);
    glBindTexture(GL_TEXTURE_2D,textures[object]);
    // glBindTexture(GL_TEXTURE_2D,textures[1]);

    glBegin(GL_QUADS);
    glNormal3f(+1, 0, 0);
    glVertex3f(1, -1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(1, 1, 1);
    glVertex3f(1, -1, 1);
    glEnd();
    //back
    glBegin(GL_QUADS);
    glNormal3f( 0, 0,-1);
    if (object == 2 || object == 7) { glTexCoord2f(0, 0); }  glVertex3f(-1, -1, -1);
    if (object == 2 || object == 7) { glTexCoord2f(1, 0); } glVertex3f(1, -1, -1);
    if (object == 2 || object == 7) { glTexCoord2f(1, 1); } glVertex3f(1, 1, -1);
    if (object == 2 || object == 7) { glTexCoord2f(0, 1); } glVertex3f(-1, 1, -1);
    glEnd();
    //left
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, 1, -1);
    glVertex3f(-1, 1, 1);
    glVertex3f(-1, -1, 1);
    glEnd();
    //top
    glBegin(GL_QUADS);
    glNormal3f( 0,+1, 0);
    if (object == 0) { glTexCoord2f(0, 0); } glVertex3f(-1, 1, -1);
    if (object == 0) { glTexCoord2f(1, 0); } glVertex3f(1, 1, -1);
    if (object == 0) { glTexCoord2f(1, 1); } glVertex3f(1, 1, 1);
    if (object == 0) { glTexCoord2f(0, 1); } glVertex3f(-1, 1, 1);
    glEnd();
    //bottom
    glBegin(GL_QUADS);
    glNormal3f( 0,-one, 0);
    glVertex3f(-1, -1, -1);
    glVertex3f(1, -1, -1);
    glVertex3f(1, -1, 1);
    glVertex3f(-1, -1, 1);
    glEnd();
    //front
    glBegin(GL_QUADS);
    glNormal3f( 0, 0, 1);
    if (object == 2 || object == 7) { glTexCoord2f(0, 0); }  glVertex3f(-1, -1, 1);
    if (object == 2 || object == 7) { glTexCoord2f(1, 0); }  glVertex3f(1, -1, 1);
    if (object == 2 || object == 7) { glTexCoord2f(1, 1); }  glVertex3f(1, 1, 1);
    if (object == 2 || object == 7) { glTexCoord2f(0, 1); }  glVertex3f(-1, 1, 1);
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

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor3f(1,1,1);
    // glBindTexture(GL_TEXTURE_2D,textures[2]);
    glBegin(GL_QUADS);
    for (int angle = 0; angle < 360; angle += 10) {
        glNormal3f(Cos(angle), 0, Sin(angle));
        glTexCoord2f(0, 0); glVertex3f(Cos(angle), 1, Sin(angle));
        glTexCoord2f(0, 1); glVertex3f(Cos(angle), -1, Sin(angle));
        glTexCoord2f(1, 1); glVertex3f(Cos(angle+10), -1, Sin(angle+10));
        glTexCoord2f(1, 0); glVertex3f(Cos(angle+10), 1, Sin(angle+10));
    }
    glEnd();

    // drawing the two lids
    glColor3f(1,1,1);
    for (int i = 1; i >= -1; i-=2) {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glColor3f(1,1,1);
        // glBindTexture(GL_TEXTURE_2D,textures[1]);

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
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

// Draws a road turning 90 degrees
static void road_90(int x, int y, int z, double dx, double dy, double dz, double th, double ph) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(th,0,1,0);
    glRotatef(ph,1,0,0);
    glScalef(dx,dy,dz);

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor3f(1, 1, 1);
    glBindTexture(GL_TEXTURE_2D,textures[1]);

    for (int angle = 0; angle < 90; angle += ANGLE_RES) {
        glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glTexCoord2f(angle/90, 1); glVertex3f(2*Cos(angle)/3, 0, 2*Sin(angle)/3);
        glTexCoord2f(angle+ANGLE_RES/90, 1); glVertex3f(2*Cos(angle+ANGLE_RES)/3, 0, 2*Sin(angle+ANGLE_RES)/3);
        glTexCoord2f(angle+ANGLE_RES/90,0); glVertex3f(Cos(angle+ANGLE_RES)/3, 0, Sin(angle+ANGLE_RES)/3);
        glTexCoord2f(angle/90, 0); glVertex3f(Cos(angle)/3, 0, Sin(angle)/3);
        glEnd();
    }
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

// Draws a straight road
static void road_Straight(int x, int y, int z, double dx, double dy, double dz, double th, double ph) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(th,0,1,0);
    glRotatef(ph,1,0,0);
    glScalef(dx,dy,dz);

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor3f(1, 1, 1);
    glBindTexture(GL_TEXTURE_2D,textures[1]);

    for (double i = 0; i < 1-1.0/ANGLE_RES; i += 1.0/ANGLE_RES) {
        glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glTexCoord2f(i, 1); glVertex3f(i, 0, -1.0/3);
        glTexCoord2f(i, 0); glVertex3f(i, 0, -2.0/3);
        glTexCoord2f(i + 1.0/ANGLE_RES, 0); glVertex3f(i+1.0/ANGLE_RES, 0, -2.0/3);
        glTexCoord2f(i + 1.0/ANGLE_RES, 1); glVertex3f(i+1.0/ANGLE_RES, 0, -1.0/3);
        glEnd();
    }
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

// Draws a banner denoting the finish line
static void finishLine(double x, double y, double z, double dx, double dy, double dz, double th, double ph) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(th,0,1,0);
    glRotatef(ph,1,0,0);
    glScalef(dx,dy,dz);

    glColor3f(1, 1, 1);
    cylinder(-0.5, 0.25, 0, 0.01, 0.25, 0.01, 0, 0);
    cylinder(0.5, 0.25, 0, 0.01, 0.25, 0.01, 0, 0);
    cube(0, 0.45, 0, 0.5, 0.08, 0.01, 0, 0, 0, 2);

    glPopMatrix();
}

// Draws a tile using the 90 degree road
static void tile_90Right(int x, int y, int z, double dx, double dy, double dz, double th, double ph, char isFinish) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(th,0,1,0);
    glRotatef(ph,1,0,0);
    glScalef(dx,dy,dz);

    glColor3f(0.008, 0.459, 0.086);
    cube(0, -0.05, 0, 1, 0.05, 1, 0, 0, 0, 0);
    glColor3f(0.1, 0.1, 0.1);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1,-1);
    road_90(-1, 0, -1, 2, 2, 2, 0, 0);
    glDisable(GL_POLYGON_OFFSET_FILL);

    if (isFinish) {
        finishLine(-0.3, 0, -0.3, 1, 1, 1, 135, 0);
    }

    glPopMatrix();
}

// Draws a tile using the straight road
static void tile_Straight(int x, int y, int z, double dx, double dy, double dz, double th, double ph, char isFinish) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(th,0,1,0);
    glRotatef(ph,1,0,0);
    glScalef(dx,dy,dz);

    // glColor3f(0.008, 0.459, 0.086);
    cube(0, -0.05, 0, 1, 0.05, 1, 0, 0, 0, 0);
    glColor3f(0.1, 0.1, 0.1);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1,-1);
    road_Straight(-1, 0, 1, 2, 2, 2, 0, 0);
    glDisable(GL_POLYGON_OFFSET_FILL);

    if (isFinish) {
        finishLine(0, 0, 0, 1, 1, 1, 90, 0);
    }

    glPopMatrix();
}

// Draws the f1 car model loaded from objects/low-poly-f1-car.obj using a draw list
static void f1(double x, double y, double z, double dx, double dy, double dz, double th, double ph) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(th,0,1,0);
    glRotatef(ph,1,0,0);
    glScalef(dx,dy,dz);

    glColor3f(1,1,1);
    glCallList(objects[0]);

    if (state.s) {
        float Ambient[] = {0, 0, 0, 1.0};
        float Diffuse[] = {0.2, 0, 0, 1.0};
        float Specular[] = {0, 0, 0, 1.0};
        float Position[]  = {8, 2, 0 ,1.0};

        // glColor3f(1,1,1);
        // ball(Position[0],Position[1],Position[2] , 0.1)

        glEnable(GL_NORMALIZE);
        glEnable(GL_LIGHTING);
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHT1);
        glLightfv(GL_LIGHT1,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT1,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT1,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT1,GL_POSITION,Position);
    }
    else {
        glDisable(GL_LIGHT1);
    }
    // glCallList(objects[2]);
    // glCallList(objects[1]);

    glPopMatrix();
}

// Draws a windmill using a cone and a conical cylinder shap
// The windmill blades rotate at a rate determined by the global variable: windmill_rotation_rate
static void windmill(double x, double y, double z, double dx, double dy, double dz, double th, double ph) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(th,0,1,0);
    glRotatef(ph,1,0,0);
    glScalef(dx,dy,dz);

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor3f(1, 1, 1);
    glBindTexture(GL_TEXTURE_2D,textures[5]);

    glBegin(GL_QUADS);
    for (int angle = 0; angle < 360; angle += 5) {
        glNormal3f(0.8*Cos(angle), 0, 0.8*Sin(angle));
        glTexCoord2f(angle/360.0, 0); glVertex3f(0.5*Cos(angle), 1, 0.5*Sin(angle));
        glTexCoord2f(angle/360.0, 1); glVertex3f(0.8*Cos(angle), -1, 0.8*Sin(angle));
        glTexCoord2f((angle+5)/360.0, 1); glVertex3f(0.8*Cos(angle+5), -1, 0.8*Sin(angle+5));
        glTexCoord2f((angle+5)/360.0, 0); glVertex3f(0.5*Cos(angle+5), 1, 0.5*Sin(angle+5));
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D,textures[6]);
    glBegin(GL_TRIANGLES);
    for (int angle = 0; angle < 360; angle += 5) {
        glNormal3f(0.8*Cos(angle), 0, 0.8*Sin(angle));
        glTexCoord2f((angle+2.5)/360.0, 1); glVertex3f(0, 1.5, 0);
        glTexCoord2f(angle/360.0, 0); glVertex3f(0.5*Cos(angle), 1, 0.5*Sin(angle));
        glTexCoord2f(angle+5/360, 0); glVertex3f(0.5*Cos(angle+5), 1, 0.5*Sin(angle+5));
    }
    glEnd();

    cylinder(0, 1.2, 0.4, 0.05, 0.3, 0.05, 0, 90);

    cube(0, 1.2, 0.7, 0.05, 1, 0.02, 0, 0, windmill_angle, 7);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1,-1);
    cube(0, 1.2, 0.7, 0.05, 1, 0.02, 0, 0, 90+windmill_angle, 7);
    glDisable(GL_POLYGON_OFFSET_FILL);

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

static void skybox(double scale) {
    glPushMatrix();
    glScalef(scale, scale, scale);

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor3f(1, 1, 1);
    if (daytime) {
        glBindTexture(GL_TEXTURE_2D,textures[3]);
    }
    else {
        glBindTexture(GL_TEXTURE_2D,textures[4]);
    }

    glDisable(GL_LIGHTING);

    glBegin(GL_QUAD_STRIP);
    glTexCoord2f(0, 1.0/3+0.002); glVertex3f(-1, -1, -1);
    glTexCoord2f(0, 2.0/3-0.002); glVertex3f(-1, 1, -1);
    glTexCoord2f(0.25, 1.0/3+0.002); glVertex3f(1, -1, -1);
    glTexCoord2f(0.25, 2.0/3-0.002); glVertex3f(1, 1, -1);

    glTexCoord2f(0.5, 1.0/3+0.002); glVertex3f(1, -1, 1);
    glTexCoord2f(0.5, 2.0/3-0.002); glVertex3f(1, 1, 1);

    glTexCoord2f(0.75, 1.0/3+0.002); glVertex3f(-1, -1, 1);
    glTexCoord2f(0.75, 2.0/3-0.002); glVertex3f(-1, 1, 1);

    glTexCoord2f(1, 1.0/3+0.002); glVertex3f(-1, -1, -1);
    glTexCoord2f(1, 2.0/3-0.002); glVertex3f(-1, 1, -1);
    glEnd();

    glBegin(GL_QUADS);
    glTexCoord2f(0.25, 1); glVertex3f(-1, 1, -1);
    glTexCoord2f(0.25, 2.0/3); glVertex3f(1, 1, -1);
    glTexCoord2f(0.5, 2.0/3); glVertex3f(1, 1, 1);
    glTexCoord2f(0.5, 1); glVertex3f(-1, 1, 1);

    glTexCoord2f(1.0/3, 0); glVertex3f(-1, -1, -1);
    glTexCoord2f(0.25, 1.0/3); glVertex3f(1, -1, -1);
    glTexCoord2f(0.5, 1.0/3); glVertex3f(1, -1, 1);
    glTexCoord2f(0.5, -0); glVertex3f(-1, -1, 1);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

static void tree(double x, double y, double z, double dx, double dy, double dz, double th, double ph) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(th,0,1,0);
    glRotatef(ph,1,0,0);
    glScalef(dx,dy,dz);

    glColor3f(1,1,1);
    glCallList(objects[1]);

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

// Normalizes the x and z dimensions of the given vector
void normalize(vtx * vector) {
    double magnitude = sqrt(fabs(vector->x)*fabs(vector->x) + fabs(vector->z)*fabs(vector->z));
    vector->x = vector->x / magnitude;
    vector->z = vector->z / magnitude;
}

// Draws the tree defined by vertices in TreeGenerator/tree.txt
// This function is a modified version of the function of the same name stored in this repository: https://github.com/teaprog/RandomGeneratedFractalTrees/blob/master/main.cpp
void drawTree(int lines) {
    if (lines > num_tree_positions)
        return;

    for (size_t i = 0; i < lines*4; i+=4) {
        // glColor3f(1.0f / (sizes.at(i) * treeRed),  1.0f / (sizes.at(i) * treeGreen), 0.0f); 
        glColor3f(1,1,1);
        glLineWidth(positions[i+2]/40.0f + 1.0f);

        glPushMatrix();
        glTranslatef(positions[i], positions[i+1]+5, 0.0f);
        glRotatef(positions[i+3], 1.0f, 0.0f, 1.0f);

        glBegin(GL_LINES);

        glVertex2f(0.0f, 0.0f);
        glVertex2f(0.0f, -positions[i+2]);

        glEnd();
        glPopMatrix();
    }
}

// Wraps the drawTree function
void treeWrapper(double x, double y, double z, double dx, double dy, double dz, double th, double ph) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(th,0,1,0);
    glRotatef(ph,1,0,0);
    glScalef(dx,dy,dz);

    drawTree(num_tree_positions);
}

void display()
{
    //  Clear screen and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    //  Reset transformations
    glLoadIdentity();

    double now = glutGet(GLUT_ELAPSED_TIME)/1000.0;
    double t_since_last_call =  now - t;
    t = now;
    // if (auto_move) {
    //     // double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
    //     zh = fmod(90*t,360.0);
    // }
    car_pos.x += car_velocity*car_vector.x*t_since_last_call;
    car_pos.z += car_velocity*car_vector.z*t_since_last_call;

    if (car_velocity > 0)
        car_velocity -= deceleration*t_since_last_call; // engine braking
    else if (car_velocity < 0)
        car_velocity += deceleration*t_since_last_call;

    if (state.w) { // the w key is being held
        car_velocity += acceleration*t_since_last_call;
    }
    else if (state.s) { // the s key is being held
        car_velocity -= acceleration*t_since_last_call;
    }
    
    if (state.d) { // the d key is being held
        double angle = rotation_rate*t_since_last_call;
        car_th -= angle;
        car_th = car_th%360;
        car_vector.x = -1*Cos(car_th); // rotate car
        car_vector.z = Sin(car_th);
        normalize(&car_vector); // ensure the vector does not change in magnitude
    }
    if (state.a) { // the a key is being held
        double angle = rotation_rate*t_since_last_call;
        car_th += angle;
        car_th = car_th%360;
        car_vector.x = -1*Cos(car_th); // rotate car
        car_vector.z = Sin(car_th);
        normalize(&car_vector); // ensure the vector does not change in magnitude
    }

    if (state.right) {
        th += rotation_rate*t_since_last_call;
        th = fmod(th, 360);
    }
    else if (state.left) {
        th -= rotation_rate*t_since_last_call;
        th = fmod(th, 360);
    }

    if (state.up) {
        ph += rotation_rate*t_since_last_call;
        if (ph > 75) {
            ph = 75;
        }
    }
    else if (state.down) {
        ph -= rotation_rate*t_since_last_call;
        if (ph < -75) {
            ph = -75;
        }
    }

    windmill_angle += windmill_rotation_rate*t_since_last_call;
    windmill_angle = fmod(windmill_angle, 360); // rotate windmill blades

    if (mode == 1) // perspective
    {
        double Ex = -dim*Sin(th)*Cos(ph);
        double Ey = dim        *Sin(ph);
        double Ez = dim*Cos(th)*Cos(ph);
        gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
    }
//    //  Orthogonal - set world orientation
    else if (mode == 0)
    {
        glRotatef(ph,1,0,0);
        glRotatef(th,0,1,0);
    }

    else if (mode == 2) {
        double Ex = car_pos.x - car_vector.x*5;
        double Ez = car_pos.z - car_vector.z*5;
        double Ey = car_pos.y + 2;
        gluLookAt(Ex,Ey,Ez , car_pos.x,car_pos.y+1,car_pos.z , 0,1,0);
    }

    else if (mode == 3) {
        double Ex = cams[0].x;
        double Ez = cams[0].z;
        double Ey = cams[0].y;
        gluLookAt(Ex,Ey,Ez , car_pos.x,car_pos.y,car_pos.z , 0,cams[0].y+1,0);
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
        // glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);

    if (!scene) {
        skybox(dim);
        // draw the track
        tile_Straight(0, 0, -dim/4, dim/4, dim/4, dim/4, 0, 0, 1);
        tile_Straight(0, 0, dim/4, dim/4, dim/4, dim/4, 0, 0, 0);
        tile_90Right(-dim/2, 0, -dim/4, dim/4, dim/4, dim/4, 180, 0, 0);
        tile_90Right(dim/2, 0, dim/4, dim/4, dim/4, dim/4, 0, 0, 0);
        tile_90Right(-dim/2, 0, dim/4, dim/4, dim/4, dim/4, 270, 0, 0);
        tile_90Right(dim/2, 0, -dim/4, dim/4, dim/4, dim/4, 90, 0, 0);

        // draw the car
        f1(car_pos.x, car_pos.y, car_pos.z, 0.1, 0.1, 0.1, car_th, 0);
    }
    else if (scene == 1) {
        tree(0, 0, 0, 1, 1, 1, 0, 0);
    }
    else if (scene == 2) {
        // treeWrapper(0, -5, -dim, 1, 1, 1, 0, 0);
        // tile_Straight(0, 0, 0, dim/4, dim/4, dim/4, 0, 0, 0);
        skybox(dim);
        tile_90Right(0, 0, 0, dim/4, dim/4, dim/4, 0, 0, 0);
    }
    
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
        // th += 10;     
        state.right = 1;
    }
    //  Left arrow - decrease rotation by 5 degree
    else if (key == GLUT_KEY_LEFT) {
        // th -= 10;
        state.left = 1;
    }
    //  Up arrow key - increase elevation by 5 degrees
    else if (key == GLUT_KEY_UP) {
        // ph += 10;
        state.up = 1;
        // if (ph > 75) 
        //     ph = 75;
    }
    //  Down arrow key - decrease elevation by 5 degrees
    else if (key == GLUT_KEY_DOWN) {
        // ph -= 10;
        state.down = 1;
        // if (ph < -75)
        //     ph = -75;
    }
    //  Request display update
    glutPostRedisplay();
}

void idle()
{
    glutPostRedisplay();
}

// this function was borrowed from ex9
void key(unsigned char ch,int x,int y)
{
    //  Exit on ESC
    if (ch == 27)
        exit(0);
    else if (ch == 'm' || ch == 'M') {
        mode = (mode+1)%NUM_MODES;
    }
    else if (ch == 'g') {
        scene = (scene+1)%NUM_SCENES;
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
    else if (ch == 'a') {
        state.a = 1;
    }
    else if (ch == 'd') {
        state.d = 1;
    }
    else if (ch == 'w') {
        state.w = 1;
    }
    else if (ch == 's') {
        state.s = 1;
    }
    else if (ch == 't') {
        daytime = !daytime;
        if (daytime) {
            ambient = 0.3;
            diffuse = 0.7;
            specular = 0.05;
        }
        else {
            ambient = 0.05;
            diffuse = 0.1;
            specular = 0.00;
        }
    }
    //  Reproject
    Project();
    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

void keyReleased(unsigned char ch,int x,int y) {
    if (ch == 'w') {
        state.w = 0;
    }
    else if (ch == 's') {
        state.s = 0;
    }
    else if (ch == 'a') {
        state.a = 0;
    }
    else if (ch == 'd') {
        state.d = 0;
    }
}

void keySpecialReleased(int key,int x,int y) {
    if (key == GLUT_KEY_RIGHT) {
        state.right = 0;
    }
    else if (key == GLUT_KEY_LEFT) {
        state.left = 0;
    }
    else if (key == GLUT_KEY_UP) {
        state.up = 0;
    }
    else if (key == GLUT_KEY_DOWN) {
        state.down = 0;
    }
}

void mouse(int button, int state, int x, int y)
{
    if (button == 3 && fov > 20) { // Scroll up
        fov -= 2;
    }
    else if (button == 4 && fov < 100) { // Scroll down
        fov += 2;
    }
    Project();
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

// Loads the tree denoted by values stored in TreeGenerator/tree.txt
void loadTree() {
    FILE * fp = fopen("TreeGenerator/tree.txt", "r");
    if (!fp) {
        printf("File could not be opened");
        return;
    }
    char len_str[6];
    char * goawaywarning = fgets(len_str, 6, fp);
    if (goawaywarning) {}
    int len = atoi(len_str);
    if (len > 0) {
        num_tree_positions = len;
        positions = malloc(sizeof(float)*num_tree_positions*4);
        char buf[BUFSIZE];
        int i = 0;
        char * x;
        char * y;
        char * size;
        char * angle;
        while (fgets(buf, BUFSIZE, fp) && i < num_tree_positions*4) {
            if (strlen(buf) > 6) {
                x = strtok(buf, ",");
                y = strtok(NULL, ",");
                size = strtok(NULL, ",");
                angle = strtok(NULL, "\n");

                float x_val = atof(x)/100;
                float y_val = atof(y)/100;
                float size_val = atof(size)/100;
                float angle_val = atof(angle);

                positions[i] = x_val;
                positions[i+1] = y_val;
                positions[i+2] = size_val;
                positions[i+3] = angle_val;

                i += 4;
            }
        }
    }
}

int main(int argc,char* argv[])
{
    r = dim*2;
    Fz = r;
    zdiff = r;

    car_vector.x=-1; car_vector.y=0; car_vector.z=0;
    car_pos.x=0; car_pos.y=-0; car_pos.z=0;
    loadTree();
    if (!positions) {
        printf("Tree could not load\n");
    }

    //  Initialize GLUT
    glutInit(&argc,argv);
    //  Request double buffered true color window with Z-buffer
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //  Create window
    glutCreateWindow("Project - Robert Dumitrescu");
    glClearColor((double)12/255,(double)28/255,(double)65/255, 1); // background color
    //  Register display and key callbacks
    glutDisplayFunc(display);
    glutSpecialFunc(special);
    glutSpecialUpFunc(keySpecialReleased);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key);
    glutKeyboardUpFunc(keyReleased);
    glutIdleFunc(idle);
    glutMouseFunc(mouse);

    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

    //  Enable Z-buffer depth test
    glEnable(GL_DEPTH_TEST);

    // using LoadTexBMP from the CSCIx229 library to load the textures
    textures[0] = LoadTexBMP("textures/grass (2).bmp");
    textures[1] = LoadTexBMP("textures/road.bmp");
    textures[2] = LoadTexBMP("textures/checker.bmp");
    textures[3] = LoadTexBMP("textures/skybox.bmp");
    textures[4] = LoadTexBMP("textures/night2.bmp");
    textures[5] = LoadTexBMP("textures/cobble.bmp");
    textures[6] = LoadTexBMP("textures/roof.bmp");
    textures[7] = LoadTexBMP("textures/wood.bmp");
    // textures[8] = LoadTexBMP("textures/steeringwheel.bmp");
    // textures[9] = LoadTexBMP("textures/side (2).bmp");    

    objects[0] = LoadOBJ("objects/low-poly-f1-car.obj");
    objects[1] = LoadOBJ("tree.obj");
    // objects[1] = LoadOBJ("mclaren_prost.obj");
    // objects[2] = LoadOBJ("Formula_1_mesh.obj");

    glutMainLoop();

    free(positions);
    //  Return to OS
    return 0;
}
