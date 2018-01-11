#ifndef _MATRIX_H_
#define _MATRIX_H_

/*
 * MAT44 and its operations is my simple C matrix utility
 * that has been modified to work in C++ (CMAT44)
 */

#include "../include/vector.h"

typedef struct MAT4_4_{
	float data[16];
}MAT44;

//c++ matrix wrapper for matrix operations
class CMAT{
public:
    CMAT(int row, int col, float *data);
    CMAT(int row, int col);
    ~CMAT();
    CMAT *solve();
    CMAT* transpose();
    CMAT* operator * (CMAT *b);
    void print();
    void  eliminate(int i0,int j0);
private:
    float operator [](int i);
    float *data;
    int row;
    int col;
};

class CMAT44{
private:
     MAT44 m;
public:
    CMAT44();//initalizes to identity matrix by default;
    CMAT44(MAT44 m);
    void setIdentity();
    void setZero();
    CMAT44 *setXRotation(float angle);
    CMAT44 *setYRotation(float angle);
    CMAT44 *setZRotation(float angle);
    CMAT44 *setNRotation(CVec4 n,float angle);
    CMAT44 *setScale(float x,float y,float z);
    void setWindow(float w,float h);
    void setPerspective(float l, float r, float n, float f, float t, float b);
    void setPerspective(float fovy,float aspect,float n,float f);
    void setOrthographic(float l, float r, float n, float f, float t, float b);
    CMAT44 *setTranslation(float x,float y,float z);
    void setPlaneTransform(CVec4 *p, CVec4 *q, CVec4 *o);
    void setViewTransform(CVec4 p, CVec4 q, CVec4 r, CVec4 o);
    void setViewTransform(CMAT44 * cmat);
    void setViewTransform(CMAT44 *basis, CVec4 up);
    void setColumnVectors(CVec4 v[]);
    void orthonormalize(CVec4 up);
    CVec4 & getRowVector(int I);
    MAT44 *getMatPtr();
    CMAT44 operator * (CMAT44 b);//multiply
    CVec4  operator * (CVec4  b);//multiply
    CMAT44 operator ! ();//invert matrix
};

void mat44Add(MAT44 *a,MAT44 *b,MAT44 *c);
void mat44Scale(MAT44 *a,float s);
void mat44Mul(MAT44 *a, MAT44 *b, MAT44 *c);
void mat44SetIdentity(MAT44 *a);
void mat44SetZero(MAT44 *a);
void mat44Print(MAT44 *a);
Vec4 mat44Transform(MAT44 *a, Vec4 *v); //basically just multiplication but matrix vs column matrix
void mat44SetRotationOnAxis(MAT44 *m,Vec4 *u,float angle);
void mat44SetXRotation(MAT44 *m,float angle);
void mat44SetYRotation(MAT44 *m,float angle);
void mat44SetZRotation(MAT44 *m,float angle);
void mat44SetPerspective(MAT44 *m, float l, float r, float n, float f, float t, float b);
void mat44SetOrthographic(MAT44 *m, float l, float r, float n, float f, float t, float b);
void mat44SetWindow(MAT44 *m, float w, float h);
void mat44SetTranslation(MAT44 *m, float tx, float ty,float tz);
void mat44SetPlaneTransform(MAT44 *m,Vec4 *p, Vec4 *q, Vec4 *o);
void mat44SetViewTransform(MAT44 *m,Vec4 *p,Vec4 *q,Vec4 *r,Vec4 *o);
void mat44SetScaleTransform(MAT44 *m,float x,float y ,float z);
bool mat44InvertMatrix(MAT44 *mat, MAT44 *mOut);
#endif
