#include <math.h>
#include "vector.h"
#include <vector>

#ifndef MATH_UTIL_H
#define MATH_UTIL_H

/*NOTE TO SELF:
 *
 * Some Math Utility functions have the form:
 *  [obj1]_v_[object2](arg0,arg1,...,argn)
 *          -or-
 * [obj1]_p_[object2](arg0,arg1,...,argn)
 *
 * 'v' usually means obj1 and obj2 are intersecting
 * 'p' usually means obj1 is being projected down onto obj2
 */

namespace  MTHUTL {


    float line_v_line(Vec4 * p0, Vec4 *dp, Vec4 *q0,Vec4 *dq);
    float line_v_plane(Vec4 *p0, Vec4 *dp, Vec4 t[3]);
    float segment_v_segment(Vec4 *p0, Vec4 *p1, Vec4 *q0, Vec4 *q1);
    Vec4  point_v_triangle(Vec4 * p, Vec4 t[3]);
    Vec4  line_v_triangle(Vec4 * p, Vec4 *dp, Vec4 tri[3]);
    float point_p_plane(Vec4 * p,Vec4 t[3]);

    /*Tests whether a point is in the sphere or not.
    If true, then the contact normal is returned as well*/
    bool  point_v_sphere(Vec4 *point, Vec4 *center, float r, Vec4 *contactNormal);


    bool sphere_v_sphere(Vec4 * a, Vec4 *b, float ra,float rb,Vec4 *normal);

    //if this is true then the point is in the triangle
    bool  isValidBaryCoord(Vec4 *bary);

    /*Maps a number x from a range [x0,x1] to [y0,y1] */
    float map(float x,float x0,float x1, float y0,float y1);

    float map2(float s,float a1,float a2,float b1,float b2);

    /*computes bounding box defined by two points (min and max) */
    void  getBoundingBox(std::vector<CVec4> & vList,Vec4 *pmin,Vec4 *pmax);

    /*computes the gradient of a 4D function f(x,y,z)  */
    Vec4  gradient(float *buffer, Vec4 p, uint32_t w, uint32_t h);

    /* computes f(x,y,z) */
    float trilinear(float *buffer, Vec4 p, uint32_t w, uint32_t h);

    /* linear interpolation function. Assumes 0 <= t <= 1 . */
    float linear(float a,float b,float t);


    Vec4 reflect(Vec4 V,Vec4 N);
}

#endif // MATH_UTIL_H
