#include "../include/matrix.h"
#include "../include/macro_util.h"
#include <stdio.h>
#include <string.h>


void mat44SetZero(MAT44 *a){
	memset(a->data,0,sizeof(float)*16);
}

void mat44SetIdentity(MAT44 *a){
    const float I[16] = {1,0,0,0,
                         0,1,0,0,
                         0,0,1,0,
                         0,0,0,1};
    memcpy(a->data,I,16*sizeof(float));
    /*int I = 0;  what was I thinking?
	for(I = 0; I < 16; I++){
		a->data[I]=(I%5==0)?1:0;
    }*/
}

void mat44Add(MAT44 *a,MAT44 *b,MAT44 *c){
	int I;
	for(I = 0; I < 16; I++){
		a->data[I] = b->data[I] + c->data[I];
	}
}

void mat44Scale(MAT44 *a,float s){
	int I;
	for(I = 0; I < 16; I++){
		a->data[I]*=s;
	}
}

void mat44Mul(MAT44 *a, MAT44 *b, MAT44 *c){
	int x,y,k;
	float *da = b->data, *db = c->data, *dc = a->data;
	for(y = 0; y < 4; y++){
		for(x = 0; x < 4; x++){
			for(k = 0; k < 4; k++){
				dc[GI(x,y,4)]+= da[GI(k,y,4)]*db[GI(x,k,4)];
			}
		}
	}
}

Vec4 mat44Transform(MAT44 *a, Vec4 *v){
	int x,y,k;
	float *da = a->data, *db = (float*)v;
	float dc[4] = {0,0,0,0};
	for(y = 0; y < 4; y++){
		for(x = 0; x < 1; x++){
			for(k = 0; k < 4; k++){
				dc[GI(x,y,1)]+= da[GI(k,y,4)]*db[GI(x,k,1)];
			}
		}
	}
	return *((Vec4*)dc);
}

void mat44SetRotationOnAxis(MAT44 *m,Vec4 *u,float angle){
	float cos0 = cosf(angle);
	float sin0 = sinf(angle);
	float _1_cos0 = 1.0f-cos0;
	//rotation matrix
	float *d = m->data;
 	d[0] = cos0+(u->x*u->x)*_1_cos0   ; d[1] = u->x*u->y*_1_cos0-u->z*sin0  ; d[2] = u->x*u->z*_1_cos0+u->y*sin0 ; d[3] = 0.0f;
	d[4] = u->y*u->x*_1_cos0+u->z*sin0; d[5] = cos0+(u->y*u->y)*_1_cos0     ; d[6] = u->y*u->z*_1_cos0-u->x*sin0 ; d[7] = 0.0f;
	d[8] = u->z*u->x*_1_cos0-u->y*sin0; d[9] = u->z*u->y*_1_cos0 + u->x*sin0; d[10]= cos0 + u->z*u->z*_1_cos0    ; d[11]= 0.0f;
	d[12]=0.0f                        ; d[13]=0.0f                          ; d[14]=0.0f                         ; d[15]= 1.0f;
}

void mat44SetXRotation(MAT44 *m,float angle){
	float cos0 = cosf(angle);
	float sin0 = sinf(angle);
	//rotation matrix
	float *d = m->data;
 	d[0] = 1.0f; d[1]  = 0.0f ; d[2] = 0.0f  ; d[3] = 0.0f;
	d[4] = 0.0f; d[5]  = cos0 ; d[6] = -sin0 ; d[7] = 0.0f;
	d[8] = 0.0f; d[9]  = sin0 ; d[10]= cos0  ; d[11]= 0.0f;
	d[12]= 0.0f; d[13] = 0.0f ; d[14]= 0.0f  ; d[15]= 1.0f;
}

void mat44SetZRotation(MAT44 *m, float angle){
    float cos0 = cosf(angle);
    float sin0 = sinf(angle);
    //rotation matrix
    float *d = m->data;
    d[0] = cos0; d[1]  = -sin0 ; d[2] = 0.0f  ; d[3] = 0.0f;
    d[4] = sin0; d[5]  = cos0  ; d[6] = 0.0f  ; d[7] = 0.0f;
    d[8] = 0.0f; d[9]  = 0.0f  ; d[10]= 1.0f  ; d[11]= 0.0f;
    d[12]= 0.0f; d[13] = 0.0f  ; d[14]= 0.0f  ; d[15]= 1.0f;

}

void mat44SetPerspective(MAT44 *m,float l,float r,float n,float f,float t,float b){
    float *d = m->data;
    d[0] = (2.0f*n)/(r-l); d[1]  = 0.0f           ; d[2] = (r+l)/(r-l) ; d[3] = 0.0f              ;
    d[4] = 0.0f          ; d[5]  = (2.0f*n)/(t-b) ; d[6] = (t+b)/(t-b) ; d[7] = 0.0f              ;
    d[8] = 0.0f          ; d[9]  = 0.0f           ; d[10]= (f+n)/(n-f) ; d[11]= (-2.0f*f*n)/(f-n) ;
    d[12]= 0.0f          ; d[13] = 0.0f           ; d[14]= -1.0f       ; d[15]= 0.0f              ;
}

void mat44SetWindow(MAT44 *m,float w,float h){ //viewport transform
    float *d = m->data;
    const float _w = w*0.5f;
    const float _h = h*0.5f;
    d[0] = _w  ; d[1]  = 0.0f; d[2] = _w   ; d[3] = 0.0f;
    d[4] = 0.0f; d[5]  = _h  ; d[6] = _h   ; d[7] = 0.0f;
    d[8] = 0.0f; d[9]  = 0.0f; d[10]= 1.0f ; d[11]= 0.0f;
    d[12]= 0.0f; d[13] = 0.0f; d[14]= 0.0f ; d[15]= 1.0f;
}

void mat44SetYRotation(MAT44 *m,float angle){
	float cos0 = cosf(angle);
	float sin0 = sinf(angle);
	//rotation matrix
	float *d = m->data;
 	d[0] = cos0  ; d[1]  = 0.0f ; d[2] = sin0  ; d[3] = 0.0f;
	d[4] = 0.0f  ; d[5]  = 1.0f ; d[6] = 0.0f  ; d[7] = 0.0f;
	d[8] = -sin0 ; d[9]  = 0.0f ; d[10]= cos0  ; d[11]= 0.0f;
	d[12]= 0.0f  ; d[13] = 0.0f ; d[14]= 0.0f  ; d[15]= 1.0f;
}

void mat44Print(MAT44 *a){
	int x,y;
	for(y = 0; y < 4; y++){
		printf("[");
		for(x = 0; x < 4; x++){
			printf((x==0)?"%7.2f":",%7.2f",a->data[GI(x,y,4)]);
		}
		printf("]\n");
	}
}

void mat44SetTranslation(MAT44 *m, float tx, float ty,float tz){
    float *d = m->data;
    d[0] = 1.0f  ; d[1]  = 0.0f ; d[2] = 0.0f  ; d[3] = tx;
    d[4] = 0.0f  ; d[5]  = 1.0f ; d[6] = 0.0f  ; d[7] = ty;
    d[8] = 0.0f  ; d[9]  = 0.0f ; d[10]= 1.0f  ; d[11]= tz;
    d[12]= 0.0f  ; d[13] = 0.0f ; d[14]= 0.0f  ; d[15]= 1.0f;
}

void mat44SetPlaneTransform(MAT44 *m, Vec4 *p, Vec4 *q, Vec4 *o){
    float *d = m->data;
    d[0] = p->x  ; d[1]  = q->x ; d[2] = 0.0f  ; d[3] = o->x;
    d[4] = p->y  ; d[5]  = q->y ; d[6] = 0.0f  ; d[7] = o->y;
    d[8] = p->z  ; d[9]  = q->z ; d[10]= 1.0f  ; d[11]= o->z;
    d[12]= 0.0f  ; d[13] = 0.0f ; d[14]= 0.0f  ; d[15]= 1.0f;
}

void mat44SetViewTransform(MAT44 *m, Vec4 *p, Vec4 *q, Vec4 *r, Vec4 *o){
    float *d = m->data;
    d[0] = p->x ; d[1]  = p->y ; d[2] = p->z  ; d[3] = -v4DotIW(o,p);
    d[4] = q->x ; d[5]  = q->y ; d[6] = q->z  ; d[7] = -v4DotIW(o,q);
    d[8] = r->x ; d[9]  = r->y ; d[10]= r->z  ; d[11]= -v4DotIW(o,r);
    d[12]= 0.0f ; d[13] = 0.0f ; d[14]= 0.0f  ; d[15]=    1.0f    ;
}

void mat44SetScaleTransform(MAT44 *m, float x, float y, float z){
    float *d = m->data;
    d[0] = x    ; d[1]  = 0.0f ; d[2] = 0.0f  ; d[3] =  0.0f;
    d[4] = 0.0f ; d[5]  = y    ; d[6] = 0.0f  ; d[7] =  0.0f;
    d[8] = 0.0f ; d[9]  = 0.0f ; d[10]= z     ; d[11]=  0.0f;
    d[12]= 0.0f ; d[13] = 0.0f ; d[14]= 0.0f  ; d[15]=  1.0f;
}



bool mat44InvertMatrix(MAT44 *mat, MAT44 *mOut){

    float inv[16], det;
    int i;

    float* m = mat->data;

    inv[0] = m[5]  * m[10] * m[15] -
             m[5]  * m[11] * m[14] -
             m[9]  * m[6]  * m[15] +
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] -
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] +
              m[4]  * m[11] * m[14] +
              m[8]  * m[6]  * m[15] -
              m[8]  * m[7]  * m[14] -
              m[12] * m[6]  * m[11] +
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] -
             m[4]  * m[11] * m[13] -
             m[8]  * m[5] * m[15] +
             m[8]  * m[7] * m[13] +
             m[12] * m[5] * m[11] -
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] +
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] -
               m[8]  * m[6] * m[13] -
               m[12] * m[5] * m[10] +
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] +
              m[1]  * m[11] * m[14] +
              m[9]  * m[2] * m[15] -
              m[9]  * m[3] * m[14] -
              m[13] * m[2] * m[11] +
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] -
             m[0]  * m[11] * m[14] -
             m[8]  * m[2] * m[15] +
             m[8]  * m[3] * m[14] +
             m[12] * m[2] * m[11] -
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] +
              m[0]  * m[11] * m[13] +
              m[8]  * m[1] * m[15] -
              m[8]  * m[3] * m[13] -
              m[12] * m[1] * m[11] +
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] -
              m[0]  * m[10] * m[13] -
              m[8]  * m[1] * m[14] +
              m[8]  * m[2] * m[13] +
              m[12] * m[1] * m[10] -
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] -
             m[1]  * m[7] * m[14] -
             m[5]  * m[2] * m[15] +
             m[5]  * m[3] * m[14] +
             m[13] * m[2] * m[7] -
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] +
              m[0]  * m[7] * m[14] +
              m[4]  * m[2] * m[15] -
              m[4]  * m[3] * m[14] -
              m[12] * m[2] * m[7] +
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] -
              m[0]  * m[7] * m[13] -
              m[4]  * m[1] * m[15] +
              m[4]  * m[3] * m[13] +
              m[12] * m[1] * m[7] -
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] +
               m[0]  * m[6] * m[13] +
               m[4]  * m[1] * m[14] -
               m[4]  * m[2] * m[13] -
               m[12] * m[1] * m[6] +
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
              m[1] * m[7] * m[10] +
              m[5] * m[2] * m[11] -
              m[5] * m[3] * m[10] -
              m[9] * m[2] * m[7] +
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
             m[0] * m[7] * m[10] -
             m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] +
             m[8] * m[2] * m[7] -
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
               m[0] * m[7] * m[9] +
               m[4] * m[1] * m[11] -
               m[4] * m[3] * m[9] -
               m[8] * m[1] * m[7] +
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
              m[0] * m[6] * m[9] -
              m[4] * m[1] * m[10] +
              m[4] * m[2] * m[9] +
              m[8] * m[1] * m[6] -
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        mOut->data[i] = inv[i] * det;

    return true;
}



void mat44SetOrthographic(MAT44 *m, float l, float r, float n, float f, float t, float b){
    float *d = m->data;
    d[0] = 2.0f/(r-l) ; d[1]  = 0.0f       ; d[2] = 0.0f       ; d[3] =  -(r+l)/(r-l);
    d[4] = 0.0f       ; d[5]  = 2.0f/(t-b) ; d[6] = 0.0f       ; d[7] =  -(t+b)/(t-b);
    d[8] = 0.0f       ; d[9]  = 0.0f       ; d[10]= -2.0f/(f-n); d[11]=  -(f+n)/(f-n);
    d[12]= 0.0f       ; d[13] = 0.0f       ; d[14]= 0.0f       ; d[15]=  1.0f;
}


/*C++ WRAPPER BELOW*/



CMAT44::CMAT44(){
    mat44SetZero(&m);
}

CMAT44::CMAT44(MAT44 mp){
    memcpy(this->m.data,mp.data,sizeof(float)*16);
}

void CMAT44::setIdentity(){
   mat44SetIdentity(&m);
}

void CMAT44::setZero(){
    mat44SetZero(&m);
}

CMAT44* CMAT44::setXRotation(float angle){
    mat44SetXRotation(&m,angle);
    return this;
}

CMAT44* CMAT44::setYRotation(float angle){
    mat44SetYRotation(&m,angle);
    return this;
}

CMAT44* CMAT44::setZRotation(float angle){
    mat44SetZRotation(&m,angle);
    return this;
}

CMAT44* CMAT44::setNRotation(CVec4 n,float angle){
    mat44SetRotationOnAxis(&m,&n.a,angle);
    return this;
}

CMAT44* CMAT44::setScale(float x, float y, float z){
    mat44SetScaleTransform(&m,x,y,z);
    return this;
}

CMAT44 CMAT44::operator * (CMAT44 b){
    CMAT44 temp;
    mat44Mul(&temp.m,&m,&b.m);
    return temp;
}

CVec4  CMAT44::operator * (CVec4  b){
    return CVec4(mat44Transform(&m,&b.a));
}

CMAT44 CMAT44::operator ! (){
    CMAT44 inv;
    if( mat44InvertMatrix(&m,&inv.m) == false ){
        inv.setIdentity();
    }
    return inv;
}

void  CMAT44::setWindow(float w,float h){
    mat44SetWindow(&m,w,h);
}

void  CMAT44::setPerspective(float l, float r, float n, float f, float t, float b){
    mat44SetPerspective(&m,l,r,n,f,t,b);
}

void CMAT44::setPerspective(float fovy,float aspect,float n,float f){
    float t,b,l,r;
    t = n*tanf(TO_RADIANS(fovy));
    b = -t;
    l = b*aspect;
    r = t*aspect;
    mat44SetPerspective(&m,l,r,n,f,t,b);
}

void CMAT44::setOrthographic(float l, float r, float n, float f, float t, float b){
     mat44SetOrthographic(&m,l,r,n,f,t,b);
}

CMAT44* CMAT44::setTranslation(float x,float y ,float z){
    mat44SetTranslation(&m,x,y,z);
    return this;
}

void CMAT44::setPlaneTransform(CVec4 *p, CVec4 *q, CVec4 *o){
    mat44SetPlaneTransform(&m,(Vec4*)p,(Vec4*)q,(Vec4*)o);
}

void CMAT44::setViewTransform(CVec4 p, CVec4 q, CVec4 r, CVec4 o){
    mat44SetViewTransform(&m,(Vec4*)&p,(Vec4*)&q,(Vec4*)&r,(Vec4*)&o);
}

void CMAT44::setViewTransform(CMAT44 *cmat){
    Vec4 *rowVecs = (Vec4*)cmat;
    mat44SetViewTransform(&m,rowVecs+0,rowVecs+1,rowVecs+2,rowVecs+3);
}

void CMAT44::setViewTransform(CMAT44 * basis,CVec4 up){
    basis->orthonormalize(up);
    setViewTransform(basis);
}

void CMAT44::setColumnVectors(CVec4 cv[4]){
    float *d = this->m.data;
    Vec4* v = (Vec4*)cv;
    d[0] = v[0].x ; d[1]  = v[1].x  ; d[2] = v[2].x ; d[3] = v[3].x ;
    d[4] = v[0].y ; d[5]  = v[1].y  ; d[6] = v[2].y ; d[7] = v[3].y ;
    d[8] = v[0].z ; d[9]  = v[1].z  ; d[10]= v[2].z ; d[11]= v[3].z ;
    d[12]= 0.0f   ; d[13] = 0.0f    ; d[14]= 0.0f   ; d[15]=   1.0f ;
}

void CMAT44::orthonormalize(CVec4 up){
    CVec4 &p = *(CVec4*)(m.data+0);//interpet first row as CVec4
    CVec4 &q = *(CVec4*)(m.data+4);//second row
    CVec4 &r = *(CVec4*)(m.data+8);//third row
    p = up^r;
    q = r^p;
    v4Norm((Vec4*)&p);
    v4Norm((Vec4*)&q);
    v4Norm((Vec4*)&r);
}

CVec4 &CMAT44::getRowVector(int I){
    CVec4* rowVectors  = (CVec4*)&m;//reinterpret matrix as a list of CVec4
    return rowVectors[I];
}

MAT44 *CMAT44::getMatPtr(){
    return &m;
}


//GENERAL C++ MATRIX CLASS (for m by n sized matrices )
CMAT::CMAT(int row, int col,float *data){
    this->row = row;
    this->col = col;
    this->data = new float[row*col];
    memcpy(this->data,data,sizeof(float)*row*col);
}

CMAT::CMAT(int row,int col){
    this->data = new float [row*col];
    this->col = col;
    this->row = row;
}

CMAT::~CMAT(){
    delete [] this->data;
}

CMAT* CMAT::operator * (CMAT *b){
    CMAT & A = *this;
    CMAT & B = *b;
    CMAT *C  = NULL;

    int x,y,k;
    float cij;

    if(A.col != B.row){
        return C;
    }

    C = new CMAT(A.row,B.col);

    for(y = 0; y < C->row ; y++){
        for(x = 0; x < C->col; x++){
            for(k = 0,cij = 0; k < B.row; k++ ){
                cij+= A.data[GI(k,y,A.col)]*B.data[GI(x,k,B.col)];
            }
            C->data[GI(x,y,C->col)] = cij;
        }
    }

    return C;
}

CMAT* CMAT::solve(){
    int i,j,k,m;
    float temp;
    const float TOLERANCE = 0.0125f;
    /*
     * NOTE TO SELF:It is assumed that system has a solution
     *
     */
    //put matrix in row echelon form
    i = 0;
    j = 0;
    while(j< row-1){
        if( fabsf(data[GI(i,j,col)]) < TOLERANCE){
            /*
             * Swap with the first non-zero row
             * in order to avoid dividing by zero
             */
            for(k = j+1; k < row; k++) {
                if( fabsf(data[GI(i,k,col)]) > TOLERANCE ){
                    for(i = 0; i < col; i++){
                        temp = data[GI(i,k,col)];
                        data[GI(i,k,col)] = data[GI(i,j,col)];
                        data[GI(i,j,col)] = temp;
                    }
                    break;
                }
            }
        }
        eliminate(i++,j++);
        //print();
    }

    //do back subsitution to find solutions
    float X[row];
    j = row-1;
    i = 0;
    k = row-1;
    for(j = row-1; j >= 0; j-- ){
        X[k] = data[GI(col-1,j,col)];
        for(i = col-2, m = 0; i >= 0 && data[GI(i-1,j,col)] != 0.0f ; i--, m++ ){
            X[k]-= data[GI(i,j,col)]*X[row-1-m];
        }
        X[k]/= data[GI(i,j,col)];
        k--;
    }

    return new CMAT(1,row,X);
}

void CMAT::eliminate(int i0, int j0){
    int i = i0 ,j = j0;
    float X;
    /*
     * Preforms row operations using i0,j0 as the pivot
     */
    for(j = j0+1; j < row; j++){
        X =  -data[GI(i,j,col)]/data[GI(i0,j0,col)];
        for(i = i0; i < col; i++){
           data[GI(i,j,col)]+= data[GI(i,j0,col)]*X;
        }
        i = i0;
    }
}

CMAT* CMAT::transpose(){
    return NULL; // not yet implemented
}

float CMAT::operator [] (int i){
     return this->data[i];
}

void CMAT::print(){
    int i,j;
    CMAT &M = *this;
    for(j = 0; j < M.row; j++){
        printf("[");
        for(i = 0; i < M.col; i++ ){
            printf( (i==0) ? "%7.2f":",%7.2f",M.data[GI(i,j,M.col)]);
        }
        printf("]\n");
    }
    printf("\n");
}
