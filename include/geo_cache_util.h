#include <vector>
#include "vector.h"


#ifndef GEO_CACHE_UTIL_H
#define GEO_CACHE_UTIL_H


//stores intersection in "triangle" space(barycentric coordinates)
typedef struct CachedIntersection_{
    uint32_t vIndex[3];
    Vec4 baryCoord;
}CachedIntersection;

typedef struct CachedAxis_{
    CachedIntersection minPoint;
    CachedIntersection maxPoint;
}CachedAxis;

namespace GEOCACHE {
    void calcWorldSpace(std::vector<CVec4> *vList,CachedIntersection *ci,Vec4 *worldCoord);
}

#endif // GEO_CACHE_UTIL_H
