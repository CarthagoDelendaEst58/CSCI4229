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
#define NUM_SCENES 1
#define ANGLE_RES 10
#define NUM_OBJECTS 1
#define NUM_MODES 4
#define NUM_CAMERAS 1

// These two convenience functions were borrowed from ex8
// #define Cos(x) (cos((x)*3.14159265/180))
// #define Sin(x) (sin((x)*3.14159265/180))

double th=30;  //  Rotation angle
double ph=15;
double fov=50;
int mode = 1;
int scene = 0;
const double dim=20;
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
double specular  =   0.05;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   2;  // Elevation of light
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
double t;
#define n 500
vtx is[n];
vtx car_vector;
vtx car_pos;
vtx cams[NUM_CAMERAS] = {{0, 20, 0}};

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

// I wrote this function with the guidance of the cube function from ex8 | texture stuff guided by ex15 and ex16
static void rectangular_prism(double x, double y, double z, double dx, double dy, double dz, double th, double ph, double rgb[6], int obj) {
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
    // if (!obj)
    //     glBindTexture(GL_TEXTURE_2D,textures[3]);
    // else if (obj < 0 || obj > 2)
    //     glDisable(GL_TEXTURE_2D);

    glColor3f(1,1,1);
    //right
    // if (obj == 1)
    //     glBindTexture(GL_TEXTURE_2D, textures[5]);
    // else if (obj == 2)
    //     glBindTexture(GL_TEXTURE_2D, textures[0]);
    glBegin(GL_QUADS);
    // glColor3f(rgb[0],rgb[1],rgb[2]);
    glNormal3f(+1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(1, -1, -1);
    glTexCoord2f(1, 0); glVertex3f(1, 1, -1);
    glTexCoord2f(1, 1); glVertex3f(1, 1, 1);
    glTexCoord2f(0, 1); glVertex3f(1, -1, 1);
    glEnd();
    //back
    // if (obj == 1)
    //     glBindTexture(GL_TEXTURE_2D, textures[4]);
    // else if (obj == 2)
    //     glBindTexture(GL_TEXTURE_2D, textures[0]);
    glBegin(GL_QUADS);
    // glColor3f(rgb[3],rgb[4],rgb[5]);
    glNormal3f( 0, 0,-1);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, -1);
    glTexCoord2f(1, 0); glVertex3f(1, -1, -1);
    glTexCoord2f(1, 1); glVertex3f(1, 1, -1);
    glTexCoord2f(0, 1); glVertex3f(-1, 1, -1);
    glEnd();
    //left
    // if (obj == 1)
    //     glBindTexture(GL_TEXTURE_2D, textures[9]);
    // else if (obj == 2)
    //     glBindTexture(GL_TEXTURE_2D, textures[0]);
    glBegin(GL_QUADS);
    // glColor3f(rgb[0],rgb[1],rgb[2]);
    glNormal3f(-1, 0, 0);
    glTexCoord2f(1, 0); glVertex3f(-1, -1, -1);
    glTexCoord2f(1, 1); glVertex3f(-1, 1, -1);
    glTexCoord2f(0, 1); glVertex3f(-1, 1, 1);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, 1);
    glEnd();
    //top
    // if (obj == 1)
    //     glBindTexture(GL_TEXTURE_2D, textures[0]);
    // else if (obj == 2)
    //     glBindTexture(GL_TEXTURE_2D, textures[8]);
    glBegin(GL_QUADS);
    // glColor3f(rgb[3],rgb[4],rgb[5]);
    glNormal3f( 0,+1, 0);
    glTexCoord2f(0, 0); glVertex3f(-1, 1, -1);
    glTexCoord2f(1, 0); glVertex3f(1, 1, -1);
    glTexCoord2f(1, 1); glVertex3f(1, 1, 1);
    glTexCoord2f(0, 1); glVertex3f(-1, 1, 1);
    glEnd();
    //bottom
    // if (obj == 1)
    //     glBindTexture(GL_TEXTURE_2D, textures[0]);
    // else if (obj == 2)
    //     glBindTexture(GL_TEXTURE_2D, textures[0]);
    glBegin(GL_QUADS);
    // glColor3f(rgb[3],rgb[4],rgb[5]);
    glNormal3f( 0,-one, 0);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, -1);
    glTexCoord2f(1, 0); glVertex3f(1, -1, -1);
    glTexCoord2f(1, 1); glVertex3f(1, -1, 1);
    glTexCoord2f(0, 1); glVertex3f(-1, -1, 1);
    glEnd();
    //front
    // if (obj == 1)
    //     glBindTexture(GL_TEXTURE_2D, textures[0]);
    // else if (obj == 2)
    //     glBindTexture(GL_TEXTURE_2D, textures[0]);
    glBegin(GL_QUADS);
    // glColor3f(rgb[3],rgb[4],rgb[5]);
    glColor3f(1,1,1);
    glNormal3f( 0, 0, 1);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, 1);
    glTexCoord2f(1, 0); glVertex3f(1, -1, 1);
    glTexCoord2f(1, 1); glVertex3f(1, 1, 1);
    glTexCoord2f(0, 1); glVertex3f(-1, 1, 1);
    glEnd();
    
    glPopMatrix();
}

static void cube(double x, double y, double z, double dx, double dy, double dz, double th, double ph) {
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
    glVertex3f(-1, -1, -1);
    glVertex3f(1, -1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(-1, 1, -1);
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
    glVertex3f(-1, 1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(1, 1, 1);
    glVertex3f(-1, 1, 1);
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
            rectangular_prism(i, y, j, (dim-3)/res, 1, (dim-3)/res, 0, 0, rgb2, 0);
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


    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D,textures[2]);
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

static void road_90(int x, int y, int z, double dx, double dy, double dz, double th, double ph) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(th,0,1,0);
    glRotatef(ph,1,0,0);
    glScalef(dx,dy,dz);
    for (int angle = 0; angle < 90; angle += ANGLE_RES) {
        glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glVertex3f(2*Cos(angle)/3, 0, 2*Sin(angle)/3);
        glVertex3f(2*Cos(angle+ANGLE_RES)/3, 0, 2*Sin(angle+ANGLE_RES)/3);
        glVertex3f(Cos(angle+ANGLE_RES)/3, 0, Sin(angle+ANGLE_RES)/3);
        glVertex3f(Cos(angle)/3, 0, Sin(angle)/3);
        glEnd();
    }
    glPopMatrix();
}

static void road_Straight(int x, int y, int z, double dx, double dy, double dz, double th, double ph) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(th,0,1,0);
    glRotatef(ph,1,0,0);
    glScalef(dx,dy,dz);
    for (double i = 0; i < 1-1.0/ANGLE_RES; i += 1.0/ANGLE_RES) {
        glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glVertex3f(i, 0, -1.0/3);
        glVertex3f(i, 0, -2.0/3);
        glVertex3f(i+1.0/ANGLE_RES, 0, -2.0/3);
        glVertex3f(i+1.0/ANGLE_RES, 0, -1.0/3);
        glEnd();
    }
    glPopMatrix();
}

static void tractorScene() {
    double rgb1[] = {0.129, 0.529, 0.118, 0.071, 0.388, 0.059}; // array of 2 different colors to be used when making the vehicle body

    rectangular_prism(-1, 0, 0, 0.5, 0.5, 1.2, 90, 0, rgb1, 1); // long body piece
    rectangular_prism(0.5, 0.2, 0, 0.5, 0.65, 1, 0, 0, rgb1, 2); // wide body piece

    cylinder(0.5, -0.15, 1.4, 1, 0.2, 1, 0, 90); // back left wheel
    cylinder(0.5, -0.15, -1.4, 1, 0.2, 1, 0, 90); // back right wheel

    cylinder(0.5, -0.15, 0, 0.1, 1.5, 0.1, 0, 90); // back axle

    cylinder(-2, -0.45, 0.8, 0.7, 0.2, 0.7, 0, 90); // front left wheel
    cylinder(-2, -0.45, -0.8, 0.7, 0.2, 0.7, 0, 90); // front left wheel

    cylinder(-2, -0.45, 0, 0.1, 0.8, 0.1, 0, 90); // front axle

    ground(0, -1.13, 0, 1, 0.2, 1, 0, 0, 10);
}

static void tile_90Right(int x, int y, int z, double dx, double dy, double dz, double th, double ph) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(th,0,1,0);
    glRotatef(ph,1,0,0);
    glScalef(dx,dy,dz);

    glColor3f(0.008, 0.459, 0.086);
    cube(0, -0.2, 0, 1, 0.2, 1, 0, 0);
    glColor3f(0.1, 0.1, 0.1);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1,-1);
    road_90(-1, 0, -1, 2, 2, 2, 0, 0);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopMatrix();
}

static void tile_Straight(int x, int y, int z, double dx, double dy, double dz, double th, double ph) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(th,0,1,0);
    glRotatef(ph,1,0,0);
    glScalef(dx,dy,dz);

    glColor3f(0.008, 0.459, 0.086);
    cube(0, -0.2, 0, 1, 0.2, 1, 0, 0);
    glColor3f(0.1, 0.1, 0.1);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1,-1);
    road_Straight(-1, 0, 1, 2, 2, 2, 0, 0);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopMatrix();
}

static void f1(double x, double y, double z, double dx, double dy, double dz, double th, double ph) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(th,0,1,0);
    glRotatef(ph,1,0,0);
    glScalef(dx,dy,dz);

    glColor3f(1,1,1);
    glCallList(objects[0]);

    glPopMatrix();
    return;
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

void display()
{
    //  Clear screen and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    //  Reset transformations
    glLoadIdentity();

    double t_since_last_call = glutGet(GLUT_ELAPSED_TIME)/1000.0 - t;
    t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
    // if (auto_move) {
    //     // double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
    //     zh = fmod(90*t,360.0);
    // }
    car_pos.x += car_velocity*car_vector.x*t_since_last_call;
    car_pos.z += car_velocity*car_vector.z*t_since_last_call;

    if (car_velocity > 0)
        car_velocity -= deceleration*t_since_last_call;
    else if (car_velocity < 0)
        car_velocity += deceleration*t_since_last_call;

    if (state.w) {
        car_velocity += acceleration*t_since_last_call;
    }
    else if (state.s) {
        car_velocity -= acceleration*t_since_last_call;
    }
    
    if (state.d) {
        double angle = rotation_rate*t_since_last_call;
        car_th -= angle;
        car_th = car_th%360;
        car_vector.x = -1*Cos(car_th);
        car_vector.z = Sin(car_th);
        normalize(&car_vector);
    }
    if (state.a) {
        double angle = rotation_rate*t_since_last_call;
        car_th += angle;
        car_th = car_th%360;
        car_vector.x = -1*Cos(car_th);
        car_vector.z = Sin(car_th);
        normalize(&car_vector);
    }

    if (mode == 1) // perspective
    {
        double Ex = -2*dim*Sin(th)*Cos(ph);
        double Ey = +2*dim        *Sin(ph);
        double Ez = +2*dim*Cos(th)*Cos(ph);
        gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
    }
//     else if (mode == 2) { // first person
//         Lx = r*Sin(th) + xdiff; // changing coordinates to look at based on angle and position changes
//         Lz = -1*r*Cos(th)*Cos(ph) + zdiff;
//         Ly = r*Sin(ph);

//         gluLookAt(Fx,Fy,Fz , Lx,Ly,Lz , 0,Cos(ph),0);
//     }
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
        glDisable(GL_LIGHTING);

    if (!scene) {
        tile_Straight(0, 0, -dim/4, dim/4, 1, dim/4, 0, 0);
        tile_Straight(0, 0, dim/4, dim/4, 1, dim/4, 0, 0);
        tile_90Right(-dim/2, 0, -dim/4, dim/4, 1, dim/4, 180, 0);
        tile_90Right(dim/2, 0, dim/4, dim/4, 1, dim/4, 0, 0);
        tile_90Right(-dim/2, 0, dim/4, dim/4, 1, dim/4, 270, 0);
        tile_90Right(dim/2, 0, -dim/4, dim/4, 1, dim/4, 90, 0);

        f1(car_pos.x, car_pos.y, car_pos.z, 3/dim, 3/dim, 3/dim, car_th, 0);
    }
    else if (scene == 1)
        tractorScene();
    else if (scene == 2)
        f1(0, 0, 0, 1, 1, 1, 0, 0);
    
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
    
    // double t_since_last_call = glutGet(GLUT_ELAPSED_TIME)/1000.0 - t;
    // t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
    // // if (auto_move) {
    // //     // double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
    // //     zh = fmod(90*t,360.0);
    // // }
    // car_pos.x += car_velocity*car_vector.x*t_since_last_call;
    // car_pos.z += car_velocity*car_vector.z*t_since_last_call;

    // if (car_velocity > 0)
    //     car_velocity -= deceleration*t_since_last_call;
    // else if (car_velocity < 0)
    //     car_velocity += deceleration*t_since_last_call;

    // if (state.w) {
    //     car_velocity += acceleration*t_since_last_call;
    // }
    // else if (state.s) {
    //     car_velocity -= acceleration*t_since_last_call;
    // }
    
    // if (state.d) {
    //     double angle = rotation_rate*t_since_last_call;
    //     car_th -= angle;
    //     car_th = car_th%360;
    //     car_vector.x = -1*Cos(car_th);
    //     car_vector.z = Sin(car_th);
    //     normalize(&car_vector);
    // }
    // if (state.a) {
    //     double angle = rotation_rate*t_since_last_call;
    //     car_th += angle;
    //     car_th = car_th%360;
    //     car_vector.x = -1*Cos(car_th);
    //     car_vector.z = Sin(car_th);
    //     normalize(&car_vector);
    // }
    
    
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
        // if (mode == 2) {
        //     th = 0;
        //     ph = 0;
        // }
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

    // if (mode == 2) { // first person movement
    //     double R = 2*r;
    //     if (ch == 'w' || ch == 'W') {
    //         xdiff += -1*(Fx-Lx)/R; // determining direction of view
    //         zdiff += -1*(Fz-Lz)/R;
    //         Fx = xdiff;
    //         Fz = zdiff;
    //     }

    //     else if (ch == 'a' || ch == 'A') {
    //         xdiff += -1*(Fz-Lz)/R; 
    //         zdiff += (Fx-Lx)/R;
    //         Fx = xdiff;
    //         Fz = zdiff;
    //     }

    //     else if (ch == 's' || ch == 'S') {
    //         xdiff += (Fx-Lx)/R;
    //         zdiff += (Fz-Lz)/R;
    //         Fx = xdiff;
    //         Fz = zdiff;
    //     }

    //     else if (ch == 'd' || ch == 'D') {
    //         xdiff += (Fz-Lz)/R;
    //         zdiff += -1*(Fx-Lx)/R;
    //         Fx = xdiff;
    //         Fz = zdiff;
    //     }
    // }
    // glutIdleFunc(auto_move?idle:NULL);
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

int main(int argc,char* argv[])
{
    r = dim*2;
    Fz = r;
    zdiff = r;

    car_vector.x=-1; car_vector.y=0; car_vector.z=0;
    car_pos.x=0; car_pos.y=0; car_pos.z=0;
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
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key);
    glutKeyboardUpFunc(keyReleased);
    glutIdleFunc(idle);
    glutMouseFunc(mouse);

    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

    //  Enable Z-buffer depth test
    glEnable(GL_DEPTH_TEST);

    // using LoadTexBMP from the CSCIx229 library to load the textures
    textures[0] = LoadTexBMP("textures/metal.bmp");
    textures[1] = LoadTexBMP("textures/wheel.bmp");
    textures[2] = LoadTexBMP("textures/tire.bmp");
    textures[3] = LoadTexBMP("textures/dirt3.bmp");
    textures[4] = LoadTexBMP("textures/grill.bmp");
    textures[5] = LoadTexBMP("textures/side (1).bmp");
    textures[6] = LoadTexBMP("textures/back.bmp");
    textures[7] = LoadTexBMP("textures/bottom.bmp");
    textures[8] = LoadTexBMP("textures/steeringwheel.bmp");
    textures[9] = LoadTexBMP("textures/side (2).bmp");    

    objects[0] = LoadOBJ("objects/low-poly-f1-car.obj");

    glutMainLoop();
    //  Return to OS
    return 0;
}
