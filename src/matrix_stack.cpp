#include "../include/matrix_stack.h"
#include <GL/glew.h>

MatrixStack::MatrixStack(){
    stkPtr = 1;
    for(int I = 0; I < MATRIX_STACK_MAX_SIZE; I++){
        matStack[I].setIdentity();
    }
    accum.setIdentity();
    for(auto &M: keyMatrices){
        M.setIdentity();
    }
    cameraBasis.setIdentity();
}

MatrixStack::~MatrixStack(){

}

void MatrixStack::pushMatrix(){
    matStack[stkPtr] = matStack[stkPtr-1]*accum;
    stkPtr++;
}

void MatrixStack::accumulate(CMAT44 * T){

    accum = accum*(*T);
//    printf("\n");
//    mat44Print((MAT44*)T);
}

void MatrixStack::clearAccum(){
//    printf("\n");
//    mat44Print((MAT44*)&accum);
    accum.setIdentity();
}

CMAT44 &MatrixStack::peek(){
//    printf("\n");
//    mat44Print((MAT44*)matStack-1);
    return matStack[stkPtr-1];
}

void MatrixStack::popMatrix(){
    if(stkPtr > 0){
        stkPtr--;
    }
    clearAccum();
}

void MatrixStack::updateModelView(){
    keyMatrices[MS_MODELVIEWMAT]=keyMatrices[MS_VIEWMAT]*peek();//update modelview
    keyMatrices[MS_NORMALMAT]= !keyMatrices[MS_MODELVIEWMAT]; //calculate inverse transpose (since my matracies have to be transposed by default, I do not need to transpose this matrxi)
//    glUniformMatrix4fv(LAYOUT_MODELVIEW,1,GL_TRUE,MODELVIEWMAT.getMatPtr()->data);OPENGL_ERROR();
//    glUniformMatrix4fv(LAYOUT_NORMALMATRIX,1,GL_FALSE,NORMALMAT.getMatPtr()->data);OPENGL_ERROR();
//    glUniform4fv(LAYOUT_EYE,1,(GLfloat*)&cameraBasis.o);
}

CMAT44 &MatrixStack::getViewMat(){
    return keyMatrices[MS_VIEWMAT];
}

CMAT44 &MatrixStack::getPerspMat(){
    return keyMatrices[MS_PERSPMAT];
}

CMAT44 &MatrixStack::getModelViewMat(){
    return keyMatrices[MS_MODELVIEWMAT];
}

CMAT44 &MatrixStack::getNormalMat(){
    return keyMatrices[MS_NORMALMAT];
}

CMAT44 &MatrixStack::getCameraBasis(){
    return cameraBasis;
}

CMAT44 *MatrixStack::getKeyMatricies(){
    return keyMatrices;
}

void MatrixStack::uploadEverything(){
    //currently unimplemented
    printf("This function is unimplemented\n");
    //glUniformMatrix4fv(LAYOUT_MODELVIEW,1,GL_TRUE,VIEWMAT.getMatPtr()->data);OPENGL_ERROR();
    //glUniformMatrix4fv(LAYOUT_PERSPECTIVE,1,GL_TRUE,PERSPMAT.getMatPtr()->data);OPENGL_ERROR();
}
