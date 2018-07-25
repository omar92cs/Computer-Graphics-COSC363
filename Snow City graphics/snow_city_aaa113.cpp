//  ========================================================================
//  COSC363: Computer Graphics (2018);  University of Canterbury.
//
//  FILE NAME: Yard.cpp
//  See Lab03.pdf for details.
//  ========================================================================

#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include "loadTGA.h"
#include <time.h>
#include <fstream>
#include <climits>
#include <math.h>
#include <GL/freeglut.h>
#define GL_CLAMP_TO_EDGE 0x812F //To get rid of seams between textures

//Used for the door and human movement
float door_z = -300.0f;
float door_x = 300.0f;
float theta = 7.5;
float move_human = -1250;
int direction = 1;

//Spinning the surface of revolution object
float spin = 5;

//To make our hero jump up and down
bool up;
float jump = 0;

//Used for car movement
float move_car = 0;

//Used for building and changing sizes
float x_left = -1200.0f;
float x_right = 1200.0f;
float y_height_building = 800.0f;
float z_front = -300.0f;
float z_back = -1500.0f;

//Used for drawing the cannon
float *x, *y, *z; //vertex coordinate arrays
int *t1, *t2, *t3; //triangles
int nvrt, ntri; //total number of vertices and triangles
using namespace std;

//Calculations for cannon ball trajectory
float velocity = -20;
float position = 63;
float elapsed;
float gravity = -9.8;
float fire = 38.88;
bool firing = false;
float time_when_f_pressed;

//Camera switch with F1
bool switch_camera = false;
float old_angle, old_look_x, old_look_z, old_eye_x, old_eye_z; //Camera parameters to switch back between camera modes
float angle = 0, look_x = 0.0, look_z = -1., eye_x = 0.0, eye_z = 1400.0; //Camera parameters
float lookAngle = 0.0; //Camera rotation

//Initialising textures
GLuint texId[9];

//-- Load bitmaps And Convert To Textures   -----------------
void loadTexture()
{
    glGenTextures(9, texId); // Create texture ids
    // *** left ***
    glBindTexture(GL_TEXTURE_2D, texId[0]);
    loadTGA("left.tga");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // *** front ***
    glBindTexture(GL_TEXTURE_2D, texId[1]);
    loadTGA("front.tga");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // *** right ***
    glBindTexture(GL_TEXTURE_2D, texId[2]);
    loadTGA("right.tga");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // *** back***
    glBindTexture(GL_TEXTURE_2D, texId[3]);
    loadTGA("back.tga");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // *** top ***
    glBindTexture(GL_TEXTURE_2D, texId[4]);
    loadTGA("top.tga");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // *** down ***
    glBindTexture(GL_TEXTURE_2D, texId[5]);
    loadTGA("down.tga");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //bricks
    glBindTexture(GL_TEXTURE_2D, texId[6]);
    loadTGA("bricks_white.tga");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //blue door
    glBindTexture(GL_TEXTURE_2D, texId[7]);
    loadTGA("door2.tga");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //floor for building
    glBindTexture(GL_TEXTURE_2D, texId[8]);
    loadTGA("Floor.tga");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

//-- Loads mesh data in OFF format    -------------------------------------
void loadMeshFile(const char* fname)
{
    ifstream fp_in;
    int num, ne;

    fp_in.open(fname, ios::in);
    if (!fp_in.is_open()) {
        cout << "Error opening mesh file" << endl;
        exit(1);
    }

    fp_in.ignore(INT_MAX, '\n'); //ignore first line
    fp_in >> nvrt >> ntri >> ne; // read number of vertices, polygons, edges

    x = new float[nvrt]; //create arrays
    y = new float[nvrt];
    z = new float[nvrt];

    t1 = new int[ntri];
    t2 = new int[ntri];
    t3 = new int[ntri];

    for (int i = 0; i < nvrt; i++) //read vertex list
        fp_in >> x[i] >> y[i] >> z[i];

    for (int i = 0; i < ntri; i++) //read polygon list
    {
        fp_in >> num >> t1[i] >> t2[i] >> t3[i];
        if (num != 3) {
            cout << "ERROR: Polygon with index " << i << " is not a triangle." << endl; //not a triangle!!
            exit(1);
        }
    }

    fp_in.close();
    cout << " File successfully read." << endl;
}

//--Function to compute the normal vector of a triangle with index tindx ----------
void normal(int tindx)
{
    float x1 = x[t1[tindx]], x2 = x[t2[tindx]], x3 = x[t3[tindx]];
    float y1 = y[t1[tindx]], y2 = y[t2[tindx]], y3 = y[t3[tindx]];
    float z1 = z[t1[tindx]], z2 = z[t2[tindx]], z3 = z[t3[tindx]];
    float nx, ny, nz;
    nx = y1 * (z2 - z3) + y2 * (z3 - z1) + y3 * (z1 - z2);
    ny = z1 * (x2 - x3) + z2 * (x3 - x1) + z3 * (x1 - x2);
    nz = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
    glNormal3f(nx, ny, nz);
}

//--------Draws the mesh model of the cannon----------------------------
void drawCannon()
{
    glColor3f(0.66, 0.66, 0.66);

    //Construct the object model here using triangles read from OFF file
    glBegin(GL_TRIANGLES);
    for (int tindx = 0; tindx < ntri; tindx++) {
        normal(tindx);
        glVertex3d(x[t1[tindx]], y[t1[tindx]], z[t1[tindx]]);
        glVertex3d(x[t2[tindx]], y[t2[tindx]], z[t2[tindx]]);
        glVertex3d(x[t3[tindx]], y[t3[tindx]], z[t3[tindx]]);
    }
    glEnd();
}

//-- Drawing the skybox for the background    -------------------------------------
void skybox()
{
    glEnable(GL_TEXTURE_2D);
    int size = 2000;

    ////////////////////// LEFT WALL ///////////////////////
    glBindTexture(GL_TEXTURE_2D, texId[0]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(-size, 0, size);
    glTexCoord2f(1, 0);
    glVertex3f(-size, 0., -size);
    glTexCoord2f(1, 1);
    glVertex3f(-size, size, -size);
    glTexCoord2f(0, 1);
    glVertex3f(-size, size, size);
    glEnd();

    ////////////////////// FRONT WALL ///////////////////////
    glBindTexture(GL_TEXTURE_2D, texId[1]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(-size, 0, -size);
    glTexCoord2f(1, 0);
    glVertex3f(size, 0., -size);
    glTexCoord2f(1, 1);
    glVertex3f(size, size, -size);
    glTexCoord2f(0, 1);
    glVertex3f(-size, size, -size);
    glEnd();

    ////////////////////// RIGHT WALL ///////////////////////
    glBindTexture(GL_TEXTURE_2D, texId[2]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(size, 0, -size);
    glTexCoord2f(1, 0);
    glVertex3f(size, 0, size);
    glTexCoord2f(1, 1);
    glVertex3f(size, size, size);
    glTexCoord2f(0, 1);
    glVertex3f(size, size, -size);
    glEnd();

    ////////////////////// REAR WALL ////////////////////////
    glBindTexture(GL_TEXTURE_2D, texId[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(size, 0, size);
    glTexCoord2f(1, 0);
    glVertex3f(-size, 0, size);
    glTexCoord2f(1, 1);
    glVertex3f(-size, size, size);
    glTexCoord2f(0, 1);
    glVertex3f(size, size, size);
    glEnd();

    /////////////////////// TOP //////////////////////////
    glBindTexture(GL_TEXTURE_2D, texId[4]);
    glBegin(GL_QUADS);
    glTexCoord2f(1, 0);
    glVertex3f(-size, size, -size);
    glTexCoord2f(1, 1);
    glVertex3f(size, size, -size);
    glTexCoord2f(0, 1);
    glVertex3f(size, size, size);
    glTexCoord2f(0, 0);
    glVertex3f(-size, size, size);
    glEnd();

    /////////////////////// FLOOR //////////////////////////
    glBindTexture(GL_TEXTURE_2D, texId[5]);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1);
    glVertex3f(-size, 0., size);
    glTexCoord2f(0, 0);
    glVertex3f(size, 0., size);
    glTexCoord2f(1, 0);
    glVertex3f(size, 0., -size);
    glTexCoord2f(1, 1);
    glVertex3f(-size, 0., -size);
    glEnd();
}

//-- Used for keyboard buttons for the scene    -------------------------------------
void keyboard(unsigned char key, int x, int y)
{

    switch (key) {
    //Used for opening the door
    case 'o':
        if (door_x == 300.0) {
            door_x -= 600.0;
            door_z += 600.0;
            break;
        }
        else if (door_x == -300.0) {
            door_x += 600.0;
            door_z -= 600.0;
            break;
        }

    //Used to fire the cannon
    case 'f':

        firing = true;
        time_when_f_pressed = ((glutGet(GLUT_ELAPSED_TIME)) / 1000);
        break;
    }
    glutPostRedisplay(); //update display
}

//-- The timer (used for calculating movements and camera angles)   -------------------------------------
void myTimer(int value)
{
    spin += 10;
    theta = theta * -1;

    //Car movement
    move_car += 4;

    //Humanoid movement
    if (move_human < -1250) {
        direction *= -1;
    }
    else if (move_human == -400) {
        direction *= -1;
    }

    //Firing projectile
    if (firing == true) {
        if (position >= 10) {
            elapsed = ((glutGet(GLUT_ELAPSED_TIME)) / 1000) - time_when_f_pressed;
            velocity += gravity * (elapsed / 1000);
            position += velocity * (elapsed / 1000) * 210 + 8.5;
            fire -= (velocity);
        }
    }

    //Switching camera angle
    if (switch_camera == true) {
        angle = 0, look_x = 850, look_z = -1., eye_x = 850, eye_z = move_human; //Camera parameters
    }

    move_human += 25 * direction;

    //Hero up and down jump
    if (jump == 90) {
        up = false;
    }
    else if (jump == 0) {
        up = true;
    }

    if (up == true) {
        jump += 15;
    }
    else if (up == false) {
        jump -= 15;
    }

    glutPostRedisplay();
    glutTimerFunc(100, myTimer, 0);
}

//----Used for special keyboard buttons for the scene ---------------------------
void special(int key, int x, int y)
{
    //Rotating camera
    if (key == GLUT_KEY_LEFT)
        angle -= 0.1;
    else if (key == GLUT_KEY_RIGHT)
        angle += 0.1;
    else if (key == GLUT_KEY_DOWN) { //Move backward
        eye_x -= 15 * sin(angle);
        eye_z += 15 * cos(angle);
    }
    else if (key == GLUT_KEY_UP) { //Move forward
        eye_x += 15 * sin(angle);
        eye_z -= 15 * cos(angle);
    }

    //For second camera view when F1 is pressed
    if ((key == GLUT_KEY_F1) and (switch_camera == false)) {
        old_angle = angle, old_look_x = look_x, old_look_z = look_z, old_eye_x = eye_x, old_eye_z = eye_z; //Camera parameters
        switch_camera = true;
    }
    else if ((key == GLUT_KEY_F1) and (switch_camera == true)) {
        angle = old_angle, look_x = old_look_x, look_z = old_look_z, eye_x = old_eye_x, eye_z = old_eye_z;
        switch_camera = false;
    }

    look_x = eye_x + 100 * sin(angle);
    look_z = eye_z - 100 * cos(angle);
    glutPostRedisplay();
}

//--------------------------------------------------------------------------------

void initialise()
{

    float grey[4] = { 0.2, 0.2, 0.2, 1.0 };
    float white[4] = { 1.0, 1.0, 1.0, 1.0 };

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    //  Define light's ambient, diffuse, specular properties
    glLightfv(GL_LIGHT1, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_SPECULAR, white);

    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialf(GL_FRONT, GL_SHININESS, 40);

    glShadeModel(GL_SMOOTH);
    loadMeshFile("Cannon.off");
    loadTexture();
    glEnable(GL_TEXTURE_2D);
    glClearColor(0., 1., 1., 1.); //Background colour
    glEnable(GL_NORMALIZE);
}

//--------Drawing the desk------------------------------
void desk()
{
    glPushMatrix();
    glTranslatef(-899.95, 100, -500.05);
    glColor3f(0.3, 0.4, 0.4);
    glScalef(1, 1, 2);
    glutSolidCube(200);
    glPopMatrix();
}

//---------- Drawing all the walls for the building and adding textures to them ---------
void building()
{
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texId[6]);

    glBegin(GL_POLYGON); //left
    glTexCoord2f(1, 0);
    glVertex3f(x_left, 0.0f, z_front); //first vertex
    glTexCoord2f(1, 1);
    glVertex3f(x_left, y_height_building, z_front); //second vertex
    glTexCoord2f(0, 1);
    glVertex3f(x_left, y_height_building, z_back); //third vertex
    glTexCoord2f(0, 0);
    glVertex3f(x_left, 0.0f, z_back); //fourth vertex
    glEnd();

    glBegin(GL_POLYGON); //right
    glTexCoord2f(0, 0);
    glVertex3f(x_right, y_height_building, z_back); //first vertex
    glTexCoord2f(1, 0);
    glVertex3f(x_right, y_height_building, z_front); //second vertex
    glTexCoord2f(1, 1);
    glVertex3f(x_right, 0.0f, z_front); //third vertex
    glTexCoord2f(0, 1);
    glVertex3f(x_right, 0.0f, z_back); //fourth vertex
    glEnd();

    glBegin(GL_POLYGON); //back
    glTexCoord2f(0, 0);
    glVertex3f(x_left, 0.0f, z_back); //first vertex
    glTexCoord2f(0, 1);
    glVertex3f(x_left, y_height_building, z_back); //second vertex
    glTexCoord2f(1, 1);
    glVertex3f(x_right, y_height_building, z_back); //third vertex
    glTexCoord2f(1, 0);
    glVertex3f(x_right, 0.0f, z_back); //fourth vertex
    glEnd();

    glBegin(GL_POLYGON); //top
    glTexCoord2f(0, 0);
    glVertex3f(x_right, y_height_building, z_back); //first vertex
    glTexCoord2f(1, 0);
    glVertex3f(x_left, y_height_building, z_back); //second vertex
    glTexCoord2f(1, 1);
    glVertex3f(x_left, y_height_building, z_front); //third vertex
    glTexCoord2f(0, 1);
    glVertex3f(x_right, y_height_building, z_front); //fourth vertex
    glEnd();

    //LEFT SIDE FRONT
    glBegin(GL_POLYGON); //front
    glTexCoord2f(0, 0);
    glVertex3f(x_left, 0.0f, z_front); //first vertex
    glTexCoord2f(0.3125, 0);
    glVertex3f(-300, 0.0f, z_front); //second vertex
    glTexCoord2f(0.3125, 1);
    glVertex3f(-300.0f, y_height_building, z_front); //third vertex
    glTexCoord2f(0, 1);
    glVertex3f(x_left, y_height_building, z_front);
    glEnd();

    //RIGHT SIDE FRONT
    glBegin(GL_POLYGON); //front
    glTexCoord2f(0.6875, 0);
    glVertex3f(x_right, 0.0f, z_front); //first vertex
    glTexCoord2f(1, 0);
    glVertex3f(300, 0.0f, z_front); //second vertex
    glTexCoord2f(1, 1);
    glVertex3f(300.0f, y_height_building, z_front); //third vertex
    glTexCoord2f(0.6875, 1);
    glVertex3f(x_right, y_height_building, z_front);
    glEnd();

    //TOP SIDE FRONT
    glBegin(GL_POLYGON); //front
    glTexCoord2f(0.3125, 0.5);
    glVertex3f(-300.0f, 400.0f, z_front); //first vertex
    glTexCoord2f(0.6875, 0.5);
    glVertex3f(300.0f, 400.0f, z_front); //second vertex
    glTexCoord2f(0.6875, 1);
    glVertex3f(300.0f, y_height_building, z_front); //third vertex
    glTexCoord2f(0.3125, 1);
    glVertex3f(-300.0f, y_height_building, z_front);
    glEnd();
}

//----------------Drawing the door for the building with a texture -------------------
void door()
{

    glBindTexture(GL_TEXTURE_2D, texId[7]);

    glBegin(GL_POLYGON);
    glTexCoord2f(0, 0);
    glVertex3f(-300.0f, 0.0f, -300.0f); //first vertex
    glTexCoord2f(1, 0);
    glVertex3f(door_x, 0.0f, door_z); //second vertex
    glTexCoord2f(1, 1);
    glVertex3f(door_x, 400.0f, door_z); //third vertex
    glTexCoord2f(0, 1);
    glVertex3f(-300.0f, 400.0f, -300.0f); //fourth vertex
    glEnd();
}

//----------------Drawing the floor for the building with a texture -------------------
void building_floor()
{

    float white[4] = { 1., 1., 1., 1. };
    float black[4] = { 0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, black);

    float building_floor_height = 0.05;
    glBindTexture(GL_TEXTURE_2D, texId[8]);

    glBegin(GL_POLYGON);
    glTexCoord2f(0, 0);
    glVertex3f(x_left, building_floor_height, z_front); //first vertex
    glTexCoord2f(1, 0);
    glVertex3f(x_right, building_floor_height, z_front); //second vertex
    glTexCoord2f(1, 1);
    glVertex3f(x_right, building_floor_height, z_back); //third vertex
    glTexCoord2f(0, 1);
    glVertex3f(x_left, building_floor_height, z_back); //fourth vertex
    glEnd();

    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
}

//--Draws a character model constructed using GLUT objects ------------------
void human()
{
    //head
    glColor3f(0.4, 0.1, 0.8);
    glPushMatrix();
    glTranslatef(650, 448, move_human);
    glutSolidSphere(50, 20, 20);
    glPopMatrix();

    //Right leg
    glColor3f(0., 0., 1.);

    glPushMatrix();

    glTranslatef(700, 100 * 2, move_human);
    glRotatef(-theta, 1, 0, 0);
    glTranslatef(-700, -100 * 2, -move_human);

    glTranslatef(700, 100, move_human);
    glScalef(1, 4, 1);
    glutSolidCube(50);
    glPopMatrix();

    //Left leg
    glColor3f(0., 0., 1.);
    glPushMatrix();

    glTranslatef(600, 100 * 2, move_human);
    glRotatef(theta, 1, 0, 0);
    glTranslatef(-600, -100 * 2, -move_human);

    glTranslatef(600, 100, move_human);
    glScalef(1, 4, 1);
    glutSolidCube(50);
    glPopMatrix();

    //Right arm
    glColor3f(0., 0., 1.);
    glPushMatrix();

    glTranslatef(775, 275 * 1.5, move_human);
    glRotatef(theta, 1, 0, 0);
    glTranslatef(-775, -275 * 1.5, -move_human);

    glTranslatef(775, 275, move_human);
    glScalef(1, 5, 1);
    glutSolidCube(50);
    glPopMatrix();

    //left arm
    glColor3f(0., 0., 1.);
    glPushMatrix();

    glTranslatef(525, 275 * 1.5, move_human);
    glRotatef(-theta, 1, 0, 0);
    glTranslatef(-525, -275 * 1.5, -move_human);

    glTranslatef(525, 275, move_human);
    glScalef(1, 5, 1);
    glutSolidCube(50);
    glPopMatrix();

    //Body
    glColor3f(0., 0., 0.);
    glPushMatrix();
    glTranslatef(650, 300, move_human);
    glScalef(4, 4.05, 1);
    glutSolidCube(50);
    glPopMatrix();
}

//--Draws a cannon using a .off mesh file and using GLUT objects ------------------
void cannon()
{
    //First cube
    glPushMatrix();
    glTranslatef(-10, 5, 17);
    glScalef(80, 10, 6);
    glColor3f(1, 0.843, 0);
    glutSolidCube(1);
    glPopMatrix();

    //second cube
    glPushMatrix();
    glTranslatef(-20, 25, 17);
    glScalef(40, 30, 6);
    glColor3f(1, 0.843, 0);
    glutSolidCube(1);
    glPopMatrix();

    //third cube
    glPushMatrix();
    glTranslatef(-10, 5, -17);
    glScalef(80, 10, 6);
    glColor3f(1, 0.843, 0);
    glutSolidCube(1);
    glPopMatrix();

    //fourth cube
    glPushMatrix();
    glTranslatef(-20, 25, -17);
    glScalef(40, 30, 6);
    glColor3f(1, 0.843, 0);
    glutSolidCube(1);
    glPopMatrix();

    //Projectile
    glPushMatrix();
    glTranslatef(fire, position, 0);
    glColor3f(1, 0.843, 0);
    glutSolidSphere(5, 36, 18);
    glPopMatrix();

    //Extra projectiles
    glPushMatrix();
    glTranslatef(-25, 5, 50);
    glColor3f(1, 0.843, 0);
    glutSolidSphere(5, 36, 18);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-15, 5, 50);
    glColor3f(1, 0.843, 0);
    glutSolidSphere(5, 36, 18);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-25, 5, 60);
    glColor3f(1, 0.843, 0);
    glutSolidSphere(5, 36, 18);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-15, 5, 60);
    glColor3f(1, 0.843, 0);
    glutSolidSphere(5, 36, 18);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-20, 12.5, 55);
    glColor3f(1, 0.843, 0);
    glutSolidSphere(5, 36, 18);
    glPopMatrix();

    //Rotating the cannon
    glPushMatrix();
    glTranslatef(-20, 30, 0); //Pivot point coordinates
    glRotatef(30, 0, 0, 1); //Rotation
    glTranslatef(20, -30, 0);
    drawCannon();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //GL_LINE = Wireframe;   GL_FILL = Solid
    glLoadIdentity();
    glPopMatrix();
}

//--Draws a surface of revolution object (paraboloid) using equations ----------
void paraboloid_robot()
{
    float vertices[30][50][3];
    float LAT = 30;
    float LONG = 50;

    for (int N_LAT = 0; N_LAT < LAT; N_LAT++) { // To calculate along the Z-AXIS
        for (int N_LONG = 0; N_LONG < LONG; N_LONG++) { // Along the X and Y AXIS
            float e = sin(M_PI * N_LAT / ((LAT - 1)) - M_PI / 2.0f) / 5;
            float r = cos(M_PI * N_LAT / ((LAT - 1)) - M_PI / 2.0f);
            float q = (cos(M_PI * N_LONG / (LONG - 1)) * r) / 5;
            float w = (sin(M_PI * N_LONG / (LONG - 1)) * r * 2) / 5;

            //Different variables used for x,y,z due to them already being used
            vertices[N_LAT][N_LONG][0] = q; //X
            vertices[N_LAT][N_LONG][1] = w; //Y
            vertices[N_LAT][N_LONG][2] = e; //Z
        }
    }

    for (int N_LAT = 0; N_LAT < LAT - 1; N_LAT++) { //To calculate along the Z-AXIS
        glBegin(GL_QUAD_STRIP);
        for (int N_LONG = 0; N_LONG < LONG; N_LONG++) {
            glVertex3f(vertices[N_LAT][N_LONG][0], vertices[N_LAT][N_LONG][1], vertices[N_LAT][N_LONG][2]);
            glVertex3f(vertices[N_LAT + 1][N_LONG][0], vertices[N_LAT + 1][N_LONG][1], vertices[N_LAT + 1][N_LONG][2]);
        }
        glEnd();
    }

    //Eyes for surface of revolution object
    glPushMatrix();
    glTranslatef(0.13, 0.28, +0.05);
    glColor3f(10, 0.843, 0);
    glutSolidCube(0.05);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.13, 0.28, -0.05);
    glColor3f(10, 0.843, 0);
    glutSolidCube(0.05);
    glPopMatrix();
}

//--Draws a Snowman called big hero using GLUT objects----------
void big_hero()
{
    //Bottom part
    glPushMatrix();
    glTranslatef(-900, 60, -1000);
    glColor3f(0.941, 0.60, 0.60);
    glScalef(1, 1, 1.5);
    glutSolidSphere(60, 36, 18);
    glPopMatrix();

    //Middle Part
    glPushMatrix();
    glTranslatef(-900, 150, -1000);
    glColor3f(0.941, 0.50, 0.50);
    glScalef(1, 1, 1.5);
    glutSolidSphere(45, 36, 18);
    glPopMatrix();

    //Top part

    glPushMatrix();
    glTranslatef(-900, 205, -1000);
    glColor3f(0.8, 0.36, 0.36);
    glScalef(1, 1, 1.5);
    glutSolidSphere(30, 36, 18);
    glPopMatrix();

    //Eyes
    glPushMatrix();
    glTranslatef(-870, 210, -1008);
    glColor3f(0, 0, 0);
    glScalef(1, 1, 1.5);
    glutSolidSphere(2, 36, 18);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-870, 210, -992);
    glColor3f(0, 0, 0);
    glScalef(1, 1, 1.5);
    glutSolidSphere(2, 36, 18);
    glPopMatrix();

    //Nose
    glPushMatrix();
    glTranslatef(-870, 200, -1000);
    glRotatef(90, 0, 1, 0);
    glColor3f(0, 0, 0);
    glScalef(1, 1, 1.5);
    glutSolidCone(4, 20, 20, 10);
    glPopMatrix();
}

void tree()
{
    glPushMatrix();
        glColor3f(0.8,0.52,0.24);
        GLUquadric *q;
        q = gluNewQuadric();
        gluCylinder(q, 40, 35, 150, 30,30);
        gluQuadricDrawStyle(q, GLU_FILL);
    glPopMatrix();

    glPushMatrix();
        glColor3f(0.6,0.8,0.19);
        glTranslatef(0,0,150);
        glutSolidCone(150, 200, 40, 40);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0,0,250);
        glutSolidCone(100, 150, 40, 40);
    glPopMatrix();
}

//------- Car drawing using quads and polygons, and Torus objects for the wheels-----------
void toy_car()
{
    float red = 0.1;
    float green = 1;
    float blue = 1;

    float strip_0_2 = 0.201f;
    float strip_0_6 = 0.601f;
    float strip_0_5 = 0.501f;
    float strip_0_4 = 0.401f;
    float strip_0_3 = 0.301f;

    glBegin(GL_QUADS);

    //TOP SIDE

    /**FRONT OF CAR BODY**/
    glColor3f(red, green, blue);
    glVertex3f(strip_0_2, strip_0_4, strip_0_6);
    glVertex3f(strip_0_6, strip_0_5, strip_0_6);
    glVertex3f(strip_0_6, strip_0_5, strip_0_2);
    glVertex3f(strip_0_2, strip_0_4, strip_0_2);

    //-------------------Bottom ---------------
    glVertex3f(strip_0_2, strip_0_2, strip_0_6);
    glVertex3f(strip_0_6, strip_0_2, strip_0_6);
    glVertex3f(strip_0_6, strip_0_2, strip_0_2);
    glVertex3f(strip_0_2, strip_0_2, strip_0_2);

    //-------------------Front ---------------
    glVertex3f(strip_0_2, strip_0_2, strip_0_6);
    glVertex3f(strip_0_2, strip_0_4, strip_0_6);
    glVertex3f(strip_0_2, strip_0_4, strip_0_2);
    glVertex3f(strip_0_2, strip_0_2, strip_0_2);

    //-------------------rear ---------------
    glVertex3f(strip_0_6, strip_0_2, strip_0_6);
    glVertex3f(strip_0_6, strip_0_5, strip_0_6);
    glVertex3f(strip_0_6, strip_0_5, strip_0_2);
    glVertex3f(strip_0_6, strip_0_2, strip_0_2);

    //-------------------LEFT ---------------
    glVertex3f(strip_0_2, strip_0_2, strip_0_6);
    glVertex3f(strip_0_6, strip_0_2, strip_0_6);
    glVertex3f(strip_0_6, strip_0_5, strip_0_6);
    glVertex3f(strip_0_2, strip_0_4, strip_0_6);

    //-------------------RIGHT ---------------
    glVertex3f(strip_0_2, strip_0_2, strip_0_2);
    glVertex3f(strip_0_6, strip_0_2, strip_0_2);
    glVertex3f(strip_0_6, strip_0_5, strip_0_2);
    glVertex3f(strip_0_2, strip_0_4, strip_0_2);

    //-------------------TOPSIDE COVER ---------------
    glVertex3f(0.7, 0.65, strip_0_6);
    glVertex3f(0.7, 0.65, strip_0_2);
    glVertex3f(1.7, 0.65, strip_0_2);
    glVertex3f(1.7, 0.65, strip_0_6);

    /**REAR GUARDS**/
    glColor3f(red, green, blue);
    glVertex3f(1.8, strip_0_5, strip_0_6);
    glVertex3f(1.8, strip_0_5, strip_0_2);
    glVertex3f(2.1, strip_0_4, strip_0_2);
    glVertex3f(2.1, strip_0_4, strip_0_6);

    //-------------------Bottom ---------------
    glVertex3f(2.1, strip_0_2, strip_0_6);
    glVertex3f(2.1, strip_0_2, strip_0_2);
    glVertex3f(1.8, strip_0_2, strip_0_6);
    glVertex3f(1.8, strip_0_2, strip_0_6);

    //-------------------REAR ---------------
    glVertex3f(2.1, strip_0_4, strip_0_6);
    glVertex3f(2.1, strip_0_4, strip_0_2);
    glVertex3f(2.1, strip_0_2, strip_0_2);
    glVertex3f(2.1, strip_0_2, strip_0_6);

    //-------------------left ---------------
    glVertex3f(1.8, strip_0_2, strip_0_2);
    glVertex3f(1.8, strip_0_5, strip_0_2);
    glVertex3f(2.1, strip_0_4, strip_0_2);
    glVertex3f(2.1, strip_0_2, strip_0_2);

    //-------------------Right ---------------
    glVertex3f(1.8, strip_0_2, strip_0_6);
    glVertex3f(1.8, strip_0_5, strip_0_6);
    glVertex3f(2.1, strip_0_4, strip_0_6);
    glVertex3f(2.1, strip_0_2, strip_0_6);

    //******************MIDDLE BODY************************************
    glVertex3f(strip_0_6, strip_0_5, strip_0_6);
    glVertex3f(strip_0_6, strip_0_2, strip_0_6);
    glVertex3f(1.8, strip_0_2, strip_0_6);
    glVertex3f(1.8, strip_0_5, strip_0_6);

    /* bottom of cube*/
    glVertex3f(strip_0_6, strip_0_2, strip_0_6);
    glVertex3f(strip_0_6, strip_0_2, strip_0_2);
    glVertex3f(1.8, strip_0_2, strip_0_2);
    glVertex3f(1.8, strip_0_2, strip_0_6);

    /* back of cube.*/
    glVertex3f(strip_0_6, strip_0_5, strip_0_2);
    glVertex3f(strip_0_6, strip_0_2, strip_0_2);
    glVertex3f(1.8, strip_0_2, strip_0_2);
    glVertex3f(1.8, strip_0_5, strip_0_2);

    /**WINDOWS**/
    //-------------------Front window ---------------
    glColor3f(strip_0_3, strip_0_3, strip_0_3);
    glVertex3f(0.77, 0.63, strip_0_2);
    glVertex3f(0.75, strip_0_5, strip_0_2);
    glVertex3f(1.2, strip_0_5, strip_0_2);
    glVertex3f(1.22, 0.63, strip_0_2);

    //-------------------back window ---------------
    glVertex3f(1.27, 0.63, .2);
    glVertex3f(1.25, strip_0_5, strip_0_2);
    glVertex3f(1.65, strip_0_5, strip_0_2);
    glVertex3f(1.67, 0.63, strip_0_2);

    //STRIPS AND MODIFICATIONS
    glColor3f(red, green, blue);
    glVertex3f(0.7, 0.65, strip_0_2);
    glVertex3f(0.7, strip_0_5, .2);
    glVertex3f(0.75, strip_0_5, strip_0_2);
    glVertex3f(0.77, 0.65, strip_0_2);

    glVertex3f(1.2, 0.65, strip_0_2);
    glVertex3f(1.2, strip_0_5, .2);
    glVertex3f(1.25, strip_0_5, strip_0_2);
    glVertex3f(1.27, 0.65, strip_0_2);

    glVertex3f(1.65, 0.65, strip_0_2);
    glVertex3f(1.65, strip_0_5, .2);
    glVertex3f(1.7, strip_0_5, strip_0_2);
    glVertex3f(1.7, 0.65, strip_0_2);

    glVertex3f(0.75, 0.65, strip_0_2);
    glVertex3f(0.75, 0.63, strip_0_2);
    glVertex3f(1.7, 0.63, strip_0_2);
    glVertex3f(1.7, 0.65, strip_0_2);

    glVertex3f(0.75, 0.65, strip_0_6);
    glVertex3f(0.75, 0.63, strip_0_6);
    glVertex3f(1.7, 0.63, strip_0_6);
    glVertex3f(1.7, 0.65, strip_0_6);

    //-------------------Front window 2---------------
    glColor3f(strip_0_3, strip_0_3, strip_0_3);
    glVertex3f(0.77, 0.63, strip_0_6);
    glVertex3f(0.75, strip_0_5, strip_0_6);
    glVertex3f(1.2, strip_0_5, strip_0_6);
    glVertex3f(1.22, 0.63, strip_0_6);

    //-------------------rear window 2 ---------------
    glVertex3f(1.27, 0.63, .6);
    glVertex3f(1.25, strip_0_5, strip_0_6);
    glVertex3f(1.65, strip_0_5, strip_0_6);
    glVertex3f(1.67, 0.63, strip_0_6);

    //SS
    glColor3f(red, green, blue);
    glVertex3f(0.7, 0.65, strip_0_6);
    glVertex3f(0.7, strip_0_5, .6);
    glVertex3f(0.75, strip_0_5, strip_0_6);
    glVertex3f(0.77, 0.65, strip_0_6);

    glVertex3f(1.2, 0.65, strip_0_6);
    glVertex3f(1.2, strip_0_5, .6);
    glVertex3f(1.25, strip_0_5, strip_0_6);
    glVertex3f(1.27, 0.65, strip_0_6);

    glVertex3f(1.65, 0.65, strip_0_6);
    glVertex3f(1.65, strip_0_5, .6);
    glVertex3f(1.7, strip_0_5, strip_0_6);
    glVertex3f(1.7, 0.65, strip_0_6);
    glEnd();

    glBegin(GL_QUADS);

    //TOP PART FRONT WINDOW
    glColor3f(strip_0_3, strip_0_3, strip_0_3);
    glVertex3f(strip_0_6, strip_0_5, strip_0_6);
    glVertex3f(strip_0_6, strip_0_5, strip_0_2);
    glVertex3f(0.7, 0.65, strip_0_2);
    glVertex3f(0.7, 0.65, strip_0_6);

    glVertex3f(1.7, 0.65, .6);
    glVertex3f(1.7, 0.65, strip_0_2);
    glVertex3f(1.8, strip_0_5, strip_0_2);
    glVertex3f(1.8, strip_0_5, strip_0_6);
    glEnd();

    //HOT WHEELS
    glPushMatrix();
        glTranslatef(strip_0_6, strip_0_2, strip_0_6);
        glColor3f(0, 0, 0);
        glutSolidTorus(0.025, 0.07, 10, 25);

        glTranslatef(0, 0, -strip_0_4);
        glutSolidTorus(0.025, 0.07, 10, 25);

        glTranslatef(1.1, 0, 0);
        glutSolidTorus(0.025, 0.07, 10, 25);

        glTranslatef(0, 0, strip_0_4);
        glutSolidTorus(0.025, 0.07, 10, 25);
    glPopMatrix();
}

//--------Function used to display all the models and objects above-----------
void display()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, 1.0, 100.0, 5000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye_x, 200, eye_z, look_x, 200, look_z, 0, 1, 0);

    //Skybox and Building
    skybox();
    building();
    building_floor();
    door();

    //Disable use of textures
    glDisable(GL_TEXTURE_2D);

    //Cannon drawing
    glPushMatrix();
    glTranslatef(-950, 0, -1400);
    glScalef(2, 2, 2);
    cannon();
    glPopMatrix();

    //Surface of revolution robot
    glPushMatrix();
    glTranslatef(-900, 200, -500);
    glRotatef(spin, 0, 1, 0);
    glScalef(200, 200, 200);
    glColor3f(0.86, 0.07, 0.235);
    paraboloid_robot();
    glPopMatrix();

    //Draw trees
    glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        glTranslatef(-500,0,0);
        tree();
    glPopMatrix();

        glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        glTranslatef(500,0,0);
        tree();
    glPopMatrix();

    glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        glTranslatef(-500,-650,0);
        tree();
    glPopMatrix();

    glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        glTranslatef(500,-650,0);
        tree();
    glPopMatrix();

    //Draw humanoid
    glPushMatrix();
    glTranslatef(200, 0, 0);
    human();
    glPopMatrix();

    //Draw desk
    desk();

    //Drawing the snowman aka Big hero
    glPushMatrix();
    glTranslatef(0, jump, 0);
    big_hero();
    glPopMatrix();

    //Car drawing
    glPushMatrix();
    glTranslatef(0, 0, -900);
    glRotatef(move_car, 0, 1, 0);
    glTranslatef(0, 0, -400);
    glScalef(40, 100, 100);
    toy_car();
    glPopMatrix();

    //Lighting
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 50.0);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 0.01);
    float lgt_pos1[] = { -750, 300, -1400, 1 }; //spot position for light1
    float spotdir1[] = { 0, -1, 0 };
    glLightfv(GL_LIGHT1, GL_POSITION, lgt_pos1); //light
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotdir1); //light

    float lpos[4] = { 0., 2000., -600., 1.0 }; //light's position
    glLightfv(GL_LIGHT0, GL_POSITION, lpos); //set light position
    float spotdir[] = { 0, -1, -600 };
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotdir); //light

    glutSwapBuffers();
    glFlush();
}

//--------Main function to run all the code above------------------------
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 900);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("Snow City");
    initialise();
    glutDisplayFunc(display);
    glutSpecialFunc(special);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(50, myTimer, 0);
    glutMainLoop();
    return 0;
}
