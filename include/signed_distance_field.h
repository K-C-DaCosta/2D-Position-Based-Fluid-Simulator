#include "vector.h"
#include <stdint.h>
#include "vertex_array_object.h"
#include <string>

#ifndef SIGNED_DISTANCE_FIELD_H
#define SIGNED_DISTANCE_FIELD_H
typedef struct SDF_{
    float *buffer;
    uint32_t w;
    uint32_t h;
    uint32_t d;
    Vec4 min;
    Vec4 max;
}SignedDistanceField;

namespace SDFUTL{
    void initSignedDistanceField(SignedDistanceField *sdf,int w,int h,int d);
    void computeDistanceField(SignedDistanceField* sdf, VAO *object);
    Vec4 computeNearest( SignedDistanceField *sdf, Vec4 *p0 );
    Vec4 computeNearestAvg(SignedDistanceField *sdf,Vec4 *p0);
    float computeNearestDistance(SignedDistanceField *sdf, Vec4 *p0);
    float getMinDistToSurface(Vec4 p,VAO *plane,Vec4 *minPointPtr);
    void  writeToDisk(SignedDistanceField *sdf,std::string fileName);
    bool readFromDisk(SignedDistanceField *sdf,std::string fileName);

}

#endif // SIGNED_DISTANCE_FIELD_H
