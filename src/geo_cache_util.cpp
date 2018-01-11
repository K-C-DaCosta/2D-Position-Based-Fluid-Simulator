#include "../include/geo_cache_util.h"

void GEOCACHE::calcWorldSpace(std::vector<CVec4> *vList,
                              CachedIntersection *ci,
                              Vec4 *worldCoord){

    Vec4 * vBuffer = (Vec4*)vList->data(),
           result  = {0,0,0,0};

    v4IAS(&result,vBuffer + ci->vIndex[0], ci->baryCoord.x);
    v4IAS(&result,vBuffer + ci->vIndex[1], ci->baryCoord.y);
    v4IAS(&result,vBuffer + ci->vIndex[2], ci->baryCoord.z);
    result.w = 1.0f;

    *worldCoord = result;
}

