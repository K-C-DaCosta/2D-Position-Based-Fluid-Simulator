#include "matrix.h"

#ifndef MATRIX_STACK_H
#define MATRIX_STACK_H

#define MATRIX_STACK_MAX_SIZE 32
#define MS_VIEWMAT 0
#define MS_PERSPMAT 1
#define MS_MODELVIEWMAT 2
#define MS_NORMALMAT 3
/*
 * An array based matrix stack
 */

class MatrixStack{
public:
    MatrixStack();
    ~MatrixStack();
    void pushMatrix();//push matrix onto stack
    void accumulate(CMAT44 *T);
    void clearAccum();
    CMAT44& peek();
    void popMatrix(); //pop matrix off the stack;
    void updateModelView();
    CMAT44 &getViewMat();
    CMAT44 &getPerspMat();
    CMAT44 &getModelViewMat();
    CMAT44 &getNormalMat();
    CMAT44 &getCameraBasis();
    CMAT44* getKeyMatricies();
    void uploadEverything();
private:
    CMAT44 keyMatrices[4];
    CMAT44 accum;
    int stkPtr;
    CMAT44 matStack[MATRIX_STACK_MAX_SIZE];
    CMAT44 cameraBasis;

};

#endif // MATRIX_STACK_H
