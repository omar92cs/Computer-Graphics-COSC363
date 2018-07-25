/*----------------------------------------------------------
* COSC363  Ray Tracer
* Ray Tracing
* Assignment 2
* Ahmad Alsaleh
* Student ID: 14749959
* USER ID: aaa113
*  The sphere class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>

/**
* Cylinder's intersection method.  The input is a ray (pos, dir).
*/
float Cylinder::intersect(glm::vec3 posn, glm::vec3 dir)
{
    float a = (pow(dir.x, 2)) + (pow(dir.z, 2));
    float b = 2*(dir.x*(posn.x-center.x) + dir.z*(posn.z-center.z));
    float c = (posn.x - center.x) * (posn.x - center.x) + (posn.z - center.z) * (posn.z - center.z) - (pow(radius, 2));

    float delta = b * b - 4 * (a * c);
    if(fabs(delta) < 0.001) return -1.0;
    if(delta < 0.0) return -1.0;

    float t1 = (-b - sqrt(delta))/(2 * a);
    float t2 = (-b + sqrt(delta))/(2 * a);
    float t;

    if (t1>t2) t = t2;
    else t = t1;

    float r = posn.y + t*dir.y;

    if ((r >= center.y) and (r <= center.y + height))
    {
        return t;
    }
    else
    {
        return -1;
    }
}

/**
* Returns the unit normal vector at a given point.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
    glm::vec3 n = glm::vec3(p.x-center.x,0,p.z-center.z);
    n = glm::normalize(n);
    return n;
}
