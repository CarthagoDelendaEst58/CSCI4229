/*
*  Hello World Triangle Version 3
*/
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define GL_GLEXT_PROTOTYPES

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <stdarg.h>

#ifndef RES
#define RES 1
#endif

#define NUM_COORDS 50000
#define BUFSIZE 128
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define LEN 8194

double th=0;  //  Rotation angle
double ph=0;
float coords[NUM_COORDS*3];
float scale = 20;

double s = 10.0;
double b = 2.6666;
double r = 28.0;

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

// homebrew option parsing function (I'm sorry that it is so ugly)
void parseArgs(int argc, char * argv[]) {
    int i = 0;
    while(i < argc) {
        if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "-b") || !strcmp(argv[i], "-r")) { // option match
            if (i+1 < argc){ // value following option
                if (strlen(argv[i+1]) > 0) { // value exists
                    if (isdigit(argv[i+1][0])) { // value is numeric
                        switch (argv[i][1]) {
                            case 's':
                                s = atof(argv[i+1]);
                                break;
                            case 'b':
                                b = atof(argv[i+1]);
                                break;
                            case 'r':
                                r = atof(argv[i+1]);
                                break;
                        }
                    }
                }
            }
            i += 2;
        }
        else {
            i++;
        }
    }
}

// This function was borrowed from lorenz.c
void generateLorenzAttractor(double s, double b, double r) {
    int i;
    /*  Coordinates  */
    double x = 1;
    double y = 1;
    double z = 1;
    /*  Time step  */
    double dt = 0.001;

    /*
        *  Integrate 50,000 steps (50 time units with dt = 0.001)
        *  Explicit Euler integration
        */

    FILE * fp = fopen("lorenz.txt", "w+");

    if (fp != NULL) {
        for (i=0;i<NUM_COORDS;i++)
        {
            double dx = s*(y-x);
            double dy = x*(r-z)-y;
            double dz = x*y - b*z;
            x += dt*dx;
            y += dt*dy;
            z += dt*dz;

            fprintf(fp, "%d,%f,%f,%f\n",i+1,x,y,z); // printing cooridnates to lorenz.txt
        }
    }

    fclose(fp);
}

// This function reads coordinates from lorenz.txt and formats them into an array to be read in the display function
float readCooridinates(float coords[NUM_COORDS*3]) {
    FILE * fp = fopen("lorenz.txt", "r");
    if (fp != NULL) {
        int line = 0;
        char buf[BUFSIZE];

        float max = FLT_MIN;
        while (fgets(buf, BUFSIZE, fp) != NULL && line < NUM_COORDS) {
            char * x;
            char * y;
            char * z;

            strtok(buf, ","); // parse line
            x = strtok(NULL, ",");
            y = strtok(NULL, ",");
            z = strtok(NULL, "");

            if (atof(x) > max) {max = atof(x);} // finding maximum coordinate value to determine scaling factor
            if (atof(y) > max) {max = atof(y);}
            if (atof(z) > max) {max = atof(z);}

            coords[line*3] = atof(x);
            coords[line*3+1] = atof(y);
            coords[line*3+2] = atof(z);

            line++;
        }
        fclose(fp);
        return max/2;
    }
    else {
        return -1;
    }
}

/*
* This function is called by GLUT to display the scene
*/

void display()
{
    char info[LEN];
    //  Clear screen and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    //  Reset transformations
    glLoadIdentity();
    //  Rotate around Y axis
    glRotatef(ph,1,0,0);
    glRotatef(th,0.0,1.0,0.0);
    //  RGB triangle


    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < NUM_COORDS*3; i+=3){
        // glColor3f(r, g, b);
        glVertex3f(coords[i]/scale, coords[i+1]/scale, coords[i+2]/scale);
    }
    // glVertex3f(0, 0, 0);
    // glVertex3f(1, 1, 1);
    // glVertex3f(0.5, -0.5, -0.5);

    sprintf(info, "s: %f, b: %f, r: %f", s, b, r);
    // glWindowPos2f(5,5);
    Print(info);

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

    parseArgs(argc, argv);

    generateLorenzAttractor(s, b, r);

    scale = readCooridinates(coords);
    //  Initialize GLUT
    glutInit(&argc,argv);
    //  Request double buffered true color window with Z-buffer
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //  Create window
    glutCreateWindow("HW2");
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
