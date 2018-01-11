#include <vector>
#include "vector.h"

#ifndef SHAPE_UTIL_H
#define SHAPE_UTIL_H

namespace ShapeUtil {
    void createSphere(float radius,int zStep,int yStep,std::vector<int> *iB,std::vector<CVec4> *vB,std::vector<CVec4> *nB);
    void createCube(float l, float r, float t, float b, float n, float f, std::vector<int>*iB, std::vector<CVec4>*vB, std::vector<CVec4>*nB);
    void createCone(std::vector<int>*iB,std::vector<CVec4>*vB,std::vector<CVec4>*nB);
    void createPlane(uint32_t n, CVec4 p, CVec4 q, CVec4 o, std::vector<int> *iB, std::vector<CVec4> *vB, std::vector<CVec4> *nB);
    void createCylinder(uint32_t n, float radius, float height, CVec4 centerDisp, std::vector<int> *iB, std::vector<CVec4> *vB, std::vector<CVec4> *nB);
    void createCircle(uint32_t n, float radius, float normFactor, CVec4 disp, std::vector<int> *iB, std::vector<CVec4> *vB, std::vector<CVec4> *nB, std::vector<CVec4> *uvB);
    void computeNormals(std::vector<int> & iBr,std::vector<CVec4> &vBr,std::vector<CVec4> &nBr);
}

#endif // SHAPE_UTIL_H
