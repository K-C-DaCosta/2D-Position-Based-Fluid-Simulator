#include "vector.h"
#include "matrix.h"
#include <vector>
#include "vertex_array_object.h"
#include "matrix_stack.h"
#include "geo_cache_util.h"
#include "signed_distance_field.h"

#ifndef SOFTBODY_H
#define SOFTBODY_H

class SoftBody{
public:
    SoftBody(CVec4 sp);
    ~SoftBody();
    void rotateRobot(uint32_t axisIndex, float degrees);
    void translate(uint32_t axisIndex,float speed);
    void translate(Vec4 * disp);
    void pureTranslate(Vec4 direction);
    void pureTransform(MAT44 * tmat);
    void renderRobot(MatrixStack & matStack);
    void enforceContraints();
    void updatePosition(Vec4 *p);
    void updateVAOs();
    void updateBasis();
    void doTerrainCollision(std::vector<CVec4> *mesh);
    void doTerrainCollision(VAO *plane);
    void doSDFCollision(SignedDistanceField *sdf);
    CMAT44* getBasisMatrix(CMAT44 *mat);
    void enforceWallConstraints(float Xmin,float Xmax,float Ymin,float Ymax,float Zmin,float Zmax );

    Vec4 basis[4];
    float axesInvMags[3];
    VAO *vaoBoxPtr;
    VAO *vaoDummyBoxPtr;
private:
    /* Because im using position based dynamics I have to somehow track orientation.
    I do this by storing intersections along the standard axis.
    The three functions below help me do this.*/
    void initBasis();
    void cacheOrientation();
    void calculateAxis(Vec4 axis, CachedAxis *ca);
    /* These are just distance constrains I put on the robot*/
    void initConstraints();
    void resetOrientation();
private:
    std::vector<CVec4> oldBox;
    std::vector<int> constraints;
    std::vector<float> distances;
    CachedAxis axes[3];//I cache 3 local basis vectors. where x = 0 ,y =1  and z =2
};

#endif // SOFTBODY_H
