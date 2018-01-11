#include <math.h>

#ifndef _MACRO_UTIL_H_
#define _MACRO_UTIL_H_

/*
 * GI: acronym for 'get index'
 * Description: Maps a 2D index (i,j)-> to 1D array
 * This is the convention i've chosen for 2D access
 */
#define GI(cols,rows,width)(((rows)*(width)+(cols)))

/*
 * GI3D: Its GI(...), except we index  3D  coordinates to a 1D integer instead
 */
#define GI3D(x,y,z,w,h)( ((w*h)*(z)+(y)*(w)+(x)) )

/*
 * GIint: short for 'get index integer'
 * Desciption: Same as GI but includes integer casts to
 * ensure truncation and avoid type mismatches.
 */
#define GIint(I,J,w)((((int)(J))*w +((int)(I))))

/*
 * MIN: short form of 'minimum'
 * Description: Computes the minimum of two numerical values
 */
#define MIN(a,b)((((a)<(b))?a:b))

/*
 * MAX: short form of 'maximum'
 * Description: does the opposite of MIN
 */
#define MAX(a,b)((((a)>(b))?a:b))



#define CLAMP(a,min,max)(MAX(MIN(a,max),min))


/*
 *
 * A handy piece of code to get error reports from the opengl state machine
 */

//#define _OPENGL_DEBUG_

#ifdef _OPENGL_DEBUG_
#define OPENGL_ERROR(line) (printf("OGL Report: (%d)[%s]: %s\n", __LINE__, __FILE__ ,gluErrorString(glGetError())))
#else
#define OPENGL_ERROR()
#endif


#define TO_DEGREES(RAD)( ((RAD)*(180.0f/M_PI)) )
#define TO_RADIANS(DEG)( ((DEG)*(M_PI/180.0f)) )

//defines layout locations for the shader
#define LAYOUT_VERTEX 1
#define LAYOUT_NORMALS 2
#define LAYOUT_TEXTURE_UV 3


#endif
