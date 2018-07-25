/*========================================================================
* COSC 363  Computer Graphics (2018)
* Ray Tracing
* Assignment 2
* Ahmad Alsaleh
* Student ID: 14749959
* USER ID: aaa113
*=========================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unistd.h>
#include "Sphere.h"
#include "Cylinder.h"
#include "SceneObject.h"
#include "Ray.h"
#include "Plane.h"
#include <GL/glut.h>
#include "TextureBMP.h"
#include <stdio.h>
#include <GLFW/glfw3.h>


using namespace std;

TextureBMP texture; //Ground texture
TextureBMP texture1; //Sphere texture
TextureBMP texture2; //Skybox texture

void processInput(GLFWwindow *window);

const float WIDTH = 20.0;
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 1250;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;


//--Camera-Eye-Position globals
float x_cam = 0.0;
float y_cam = 0.0;
float z_cam = 15.0;

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene

//---The most important function in a ray tracer! ----------------------------------
//   Computes the colour value obtained by tracing a ray and finding its
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{

    glm::vec3 backgroundCol(1,0.7,0);
    glm::vec3 light(10, 40, -3);
    glm::vec3 ambientCol(0.4);

    glm::vec3 colorSum; //Used to store the accumulated colour values

    ray.closestPt(sceneObjects); //Compute the closest point of intersetion of objects with the ray

    if(ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour

    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt); //Normal vector on the sphere at the point of intersection

    glm::vec3 lightVector = light - ray.xpt; //The light vector (the vector from the point of intersection towards the light source)

    float lightDist = glm::length(lightVector); //Distance between the light and the sphere

    glm::vec3 normalizedLight= glm::normalize(lightVector); //Normalize the light vector (Convert the light vector to a unit vector using the normalize())

    glm::vec3 reflVector = glm::reflect(-lightVector, normalVector); //For computing specular reflections, we require the reflection vector r

    glm::vec3 normalizedReflVector = glm::normalize(reflVector);

    glm::vec3 viewVector = ray.pt- ray.xpt;

    glm::vec3 normalizedViewVector = glm::normalize(viewVector);

    float lDotn2 = glm::dot(normalizedReflVector, normalizedViewVector);

    glm::vec3 specular_term = glm::vec3(1.f, 1.f, 1.f)*pow((max(lDotn2, 0.f)), 4); //Computing the specular term (r.v)^f, where f is the Phong's constant (shininess)

    glm::vec3 normalizeNormal = glm::normalize(normalVector);
    float lDotn1 = glm::dot(normalizedLight, normalizeNormal);

    //Creating a shadow ray from the point of intersection with the ray and object in the scene.
    Ray shadow(ray.xpt, normalizedLight);
    shadow.closestPt(sceneObjects);

    glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor();

    /*---------      TEXTURING       ---------*/

    //Sphere texture
    if (ray.xindex == 1)
    {
        float texcoords = asin(normalVector.x) / M_PI + 0.5;
        float texcoordt = asin(normalVector.y) / M_PI + 0.5;
        materialCol = texture1.getColorAt(texcoords, texcoordt);

    }

    //Sphere texture pattern
    if (ray.xindex == 2)
    {
        float texcoords = asin(normalVector.x) / M_PI + 0.5;
        //float texcoordt = asin(normalVector.y) / M_PI + 0.5;
        int odd_even = texcoords/0.1;
        if (odd_even % 2 == 0)
        {
            materialCol = glm::vec3(1,1,1);
        }
        else
        {
            materialCol = glm::vec3(0,0,0);
        }
    }

    //Texture floor
    if (ray.xindex == 3)
    {
         float texcoords = (ray.xpt.x - (-40.0))/(40 - (-40));
         float texcoordt = (ray.xpt.z - (-40.0))/((-200.0)-(-40));
         materialCol = texture.getColorAt(texcoords, texcoordt);
    }

    //Texture skybox
    if (ray.xindex == 10)
    {
         float texcoords = (ray.xpt.x - (-100.0))/(100 - (-100));
         float texcoordt = (ray.xpt.y - (-50.0))/((100)-(-50));
         materialCol = texture2.getColorAt(texcoords, texcoordt);

    }

    /* If the value of this variable is negative,
     * the function should return only the ambient component (=ambientCol*materialCol),
     * otherwise it should return the sum of ambient and
    diffuse colours (ambientCol*materialCol + lDotn*materialCol).*/
    if (lDotn1 <= 0 || (shadow.xindex > -1 && shadow.xdist < lightDist)){
        colorSum = ambientCol * materialCol;
    } else {
        colorSum = ambientCol * materialCol + (max(lDotn1, 0.f) * materialCol) + specular_term;
    }

    //Reflections
    if(ray.xindex == 0 && step < MAX_STEPS)
    {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(ray.xpt, reflectedDir);
        glm::vec3 reflectedCol = trace(reflectedRay, step+1);
        colorSum = colorSum + (0.8f*reflectedCol);
    }

    return colorSum;
}

/*
void processInput(GLFWwindow *window)
{

    float cameraSpeed = 50.0f; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        eye += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        eye -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        eye -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        eye += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}
*/


//----Used for special keyboard buttons for the scene ---------------------------
void special(int key, int x, int y)
{
    //Forward
    if ((key == GLUT_KEY_UP))
    {
        z_cam -= 20;
    }
    //Backwards
    if (key == GLUT_KEY_DOWN)
    {
        z_cam += 20;
    }
    //Right
    if (key == GLUT_KEY_RIGHT)
    {
        x_cam += 20;
    }
    //Left
    if (key == GLUT_KEY_LEFT)
    {
        x_cam -= 20;
    }
    //Up(higher camera)
    if (key == GLUT_KEY_F1)
    {
        y_cam += 10;
    }
    //Down(lower camera)
    if (key == GLUT_KEY_F2)
    {
        y_cam -= 10;
    }
    glutPostRedisplay();
}


//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
    float xp, yp;  //grid point
    float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
    float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

    glm::vec3 eye(x_cam, y_cam, z_cam);  //The eye position (source of primary rays) is the origin CAMERA POS

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);  //Each cell is a quad.

    for(int i = 0; i < NUMDIV; i++)     //For each grid point xp, yp
    {
        xp = XMIN + i*cellX;
        for(int j = 0; j < NUMDIV; j++)
        {
            yp = YMIN + j*cellY;

            glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST);  //direction of the primary ray

            Ray ray = Ray(eye, dir);        //Create a ray originating from the camera in the direction 'dir'
            ray.normalize();                //Normalize the direction of the ray to a unit vector
            glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value

            glColor3f(col.r, col.g, col.b);
            glVertex2f(xp, yp);             //Draw each cell with its color value
            glVertex2f(xp+cellX, yp);
            glVertex2f(xp+cellX, yp+cellY);
            glVertex2f(xp, yp+cellY);
        }
    }
    glEnd();
    glFlush();
}



//---This function initializes the scene -------------------------------------------
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);

    /*-------------------- TEXTURE LOADING --------------------*/
    //Ground Plane texture
    texture = TextureBMP((char*)"LOL4.bmp");
    //Sphere texture
    texture1 = TextureBMP((char*)"verticallines.bmp");
    //Skybox texture
    texture2 = TextureBMP((char*)"000.bmp");

    /*-------------------- DRAWING PLANES --------------------*/
    //Ground plane
    Plane *plane = new Plane (glm::vec3(-30., -20, -40), //Point 1
                                glm::vec3(40., -20, -40), //Point 2
                                glm::vec3(40., -20, -200), //Point 3
                                glm::vec3(-30., -20, -200), //Point 4
                                glm::vec3(0, 0, 0)); //Colour

    /*
    //Skybox plane
    Plane *skybox = new Plane (glm::vec3(-100., -50, -600), //Point 1
                                glm::vec3(100., -50, -600), //Point 2
                                glm::vec3(100., 100, -600), //Point 3
                                glm::vec3(-100., 100, -600), //Point 4
                                glm::vec3(0, 0, 0)); //Colour
    */

    //Drawing a cube with planes (COUNTER-CLOCKWISE POINTS)
    //Front Face
    Plane *face1 = new Plane (glm::vec3(15., -20, -110), //Point 1
                                glm::vec3(30., -20, -110), //Point 2
                                glm::vec3(30., -5, -110), //Point 3
                                glm::vec3(15., -5, -110), //Point 4
                                glm::vec3(1, 0, 0)); //Colour

    //Right Face
    Plane *face2 = new Plane (glm::vec3(30., -20, -110), //Point 1
        glm::vec3(30., -20, -125), //Point 2
        glm::vec3(30., -5, -125), //Point 3
        glm::vec3(30., -5, -110), //Point 4
        glm::vec3(1, 0, 0)); //Colour

    //Back Face
    Plane *face3 = new Plane (glm::vec3(15., -20, -125), //Point 1
        glm::vec3(30., -20, -125), //Point 2
        glm::vec3(30., -5, -125), //Point 3
        glm::vec3(15., -5, -125), //Point 4
        glm::vec3(1, 0, 0)); //Colour

    //Left Face
    Plane *face4 = new Plane (glm::vec3(15., -20, -110), //Point 1
        glm::vec3(15., -20, -125), //Point 2
        glm::vec3(15., -5, -125), //Point 3
        glm::vec3(15., -5, -110), //Point 4
        glm::vec3(1, 0, 0)); //Colour

    //bottom Face
    Plane *face5 = new Plane (glm::vec3(15., -20, -110), //Point 1
        glm::vec3(30., -20, -110), //Point 2
        glm::vec3(30., -20, -125), //Point 3
        glm::vec3(15., -20, -125), //Point 4
        glm::vec3(1, 0, 0)); //Colour

    //Top Face
    Plane *face6 = new Plane (glm::vec3(15., -5, -110), //Point 1
        glm::vec3(30., -5, -110), //Point 2
        glm::vec3(30., -5, -125), //Point 3
        glm::vec3(15., -5, -125), //Point 4
        glm::vec3(1, 0, 0)); //Colour



    //-- Create a pointer to a sphere object
    Sphere *sphere1 = new Sphere(glm::vec3(-6.0, -5.0, -120.0), 15.0, glm::vec3(0, 0, 1));
    Sphere *sphere2 = new Sphere(glm::vec3(-14, 2.0, -90.0), 2.0, glm::vec3(1, 0, 0));
    Sphere *sphere3 = new Sphere(glm::vec3(0.5, -10.0, -95.0), 3.0, glm::vec3(0, 1, 0));

    //--Creating cylinder
    Cylinder *cylinder1 = new Cylinder(glm::vec3(22.5, -5, -117.5), 4.0, 15.0, glm::vec3(0, 0, 1));

    //--Add the above to the list of scene objects.
    sceneObjects.push_back(sphere1);
    sceneObjects.push_back(sphere2);
    sceneObjects.push_back(sphere3);
    sceneObjects.push_back(plane);
    sceneObjects.push_back(face1);
    sceneObjects.push_back(face2);
    sceneObjects.push_back(face3);
    sceneObjects.push_back(face4);
    sceneObjects.push_back(face5);
    sceneObjects.push_back(face6);
    //sceneObjects.push_back(skybox);
    sceneObjects.push_back(cylinder1);
}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");
    glutDisplayFunc(display);
    glutSpecialFunc(special);
    initialize();
    glutMainLoop();
    return 0;
}
