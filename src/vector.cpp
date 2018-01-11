#include "../include/vector.h"
#include "../include/macro_util.h"

#include <GL/glew.h>
#include <GL/gl.h>

Vec2* v2NVP(float x,float y){
    Vec2* v = new Vec2();
	v->x = x;
	v->y = y;
	return v;
}

Vec2 v2NV(float x,float y){
	Vec2 v;
	v.x = x;
	v.y = y;
	return v;
}

void  v2IA(Vec2*a ,Vec2*b){
	a->x+=b->x;
	a->y+=b->y;
}

void  v2IAS(Vec2* a,Vec2* b, float t){
	a->x += b->x*t;
	a->y += b->y*t;
}

void  v2IASA(Vec2*a,Vec2* b, float t, Vec2* c){
	a->x+= b->x*t + c->x;
	a->y+= b->y*t + c->y;
}

void  v2IS(Vec2*a,float t){
	a->x*=t;
	a->y*=t;
}

void  v2A(Vec2* a, Vec2* b, Vec2* c){          /* a = b+c */
	a->x = b->x + c->x;
	a->y = b->y + c->y;
}

void v2S(Vec2* a,Vec2* b,float t){          /* a = b*t;*/
	a->x = b->x * t;
	a->y = b->y * t;
}

float v2Dot(Vec2* a, Vec2* b){                  /* a.b*/
	return a->x*b->x + a->y*b->y;
}

float v2Mag(Vec2* a){                           /* |a|*/
	return v2Dot(a,a);
}

void v2Norm(Vec2* a){                          /* a= 1/|a|*/
	v2IS(a,1.0f/v2Mag(a));
}

void  v2setVector(Vec2* a,float x, float y){
	a->x = x;
	a->y = y;
}

void  v2Su(Vec2*a, Vec2* b, Vec2* c){           /*a = b-c*/
	a->x = b->x - c->x;
	a->y = b->y - c->y;
}

float v2Cross(Vec2* a, Vec2* b){
	return ((a->x*b->y) - (a->y*b->x));
}

void v2RN(Vec2* a,Vec2 *b){ /* a = b.rightNormal*/
    a->x = -b->y;
    a->y =  b->x;
}

void v2LN(Vec2* a,Vec2 *b){ /* a = b.leftNormal*/
    a->x =  b->y;
    a->y = -b->x;
}

void vNPrint(float* vec,int size){
	int I;
	printf("<");
	for(I = 0; I < size-1;I++){
		printf("%f,",vec[I]);
	}
	printf("%f>\n",vec[size-1]);
}


Vec4  v4NV(float x, float y,float z,float w){
	Vec4 v ={x,y,z,w};
	return v;
}

void  v4IA(Vec4*a , Vec4*b){
	a->x+=b->x;
	a->y+=b->y;
	a->z+=b->z;
	a->w+=b->w;
}

void  v4IAS(Vec4* a, Vec4* b, float t){
	a->x+=b->x*t;
	a->y+=b->y*t;
	a->z+=b->z*t;
	a->w+=b->w*t;
}

void  v4IASA(Vec4*a, Vec4* b, float t, Vec4* c){
	a->x+=b->x*t + c->x;
	a->y+=b->y*t + c->y;
	a->z+=b->z*t + c->z;
	a->w+=b->w*t + c->w;
}

void v4ASA(Vec4 *a, Vec4*b,float t,Vec4 *c){
	a->x=b->x*t + c->x;
	a->y=b->y*t + c->y;
	a->z=b->z*t + c->z;
	a->w=b->w*t + c->w;
}

void  v4IS(Vec4*a, float t){
	a->x*=t;
	a->y*=t;
	a->z*=t;
	a->w*=t;
}

void  v4A(Vec4* a, Vec4* b, Vec4* c){
	a->x = b->x + c->x;
	a->y = b->y + c->y;
	a->z = b->z + c->z;
	a->w = b->w + c->w;
}

void  v4S(Vec4* a, Vec4* b, float t){
	a->x = b->x*t;
	a->y = b->y*t;
	a->z = b->z*t;
	a->w = b->w*t;
}

void  v4Su(Vec4*a, Vec4* b, Vec4* c){
	a->x = b->x - c->x;
	a->y = b->y - c->y;
	a->z = b->z - c->z;
	a->w = b->w - c->w;
}

float v4Dot(Vec4* a, Vec4* b){
	return a->x*b->x + a->y*b->y + a->z*b->z + a->w*b->w;
}

float v4DotIW(Vec4 *a,Vec4 *b){//'IW' == ignore W component
    return a->x*b->x + a->y*b->y + a->z*b->z;
}

void  v4Cross(Vec4* a, Vec4* b,Vec4* c){
	a->x = b->y*c->z-b->z*c->y;
	a->y = b->z*c->x-b->x*c->z;
	a->z = b->x*c->y-b->y*c->x;
    a->w = 0.0f;
}

float v4Mag(Vec4* a){
	return sqrtf(v4Dot(a,a));
}

void  v4Norm(Vec4* a){
	v4IS(a,1.0f/v4Mag(a));
}

void v4Clamp(Vec4 *a,float lbound,float ubound){
    a->x = CLAMP(a->x,lbound,ubound);
    a->y = CLAMP(a->y,lbound,ubound);
    a->z = CLAMP(a->z,lbound,ubound);
    a->w = CLAMP(a->w,lbound,ubound);
}

void  v4Print(Vec4 *a){
	printf("[%.2f,%.2f,%.2f,%.2f]",a->x,a->y,a->z,a->w);
}

void v4Seg(Vec4 *a, Vec4 *b, Vec4 *c, float t, Vec4 *d){
    a->x = (b->x - c->x)*t + d->x;
    a->y = (b->y - c->y)*t + d->y;
    a->z = (b->z - c->z)*t + d->z;
    a->w = (b->w - c->w)*t + d->w;
}



/****************************************************************
 *******************WRAPPER CODE IS BELOW************************
 ****************************************************************/


CVec4::CVec4(){
    a.x = 0;
    a.y = 0;
    a.z = 0;
    a.w = 0;
}

CVec4::CVec4(float x,float y){
    a.x = x;
    a.y = y;
    a.z = 0;
    a.w = 0;
}

CVec4::CVec4(float x,float y,float z){
    a.x = x;
    a.y = y;
    a.z = z;
    a.w = 0;
}

CVec4::CVec4(float x,float y,float z,float w){
   a.x = x;
   a.y = y;
   a.z = z;
   a.w = w;
}

CVec4::CVec4(Vec4 v){
    a = v;
}

CVec4 CVec4::operator +(CVec4 b){
    Vec4 c;
    v4A(&c,&b.a,&a);
    return CVec4(c);
}

void CVec4::operator +=(CVec4 b){
    v4IA(&a,&b.a);
}

CVec4 CVec4::operator - (CVec4 b){
    Vec4 c;
    v4Su(&c,&this->a,&b.a);
    return CVec4(c);
}

void CVec4::operator -= (CVec4 b){
    Vec4 &vb = b.a;
    a.x-=vb.x;
    a.y-=vb.y;
    a.z-=vb.z;
    a.w-=vb.w;
}

float CVec4::operator * (CVec4 b){
    return v4Dot(&a,&b.a);
}

CVec4 CVec4::operator * (float t){
    Vec4 c;
    v4S(&c,&a,t);
    return Vec4(c);
}

void CVec4::operator *= (float t){
    v4IS(&a,t);
}

CVec4 CVec4::operator ^ (CVec4 b){
    Vec4 c;
    v4Cross(&c,&a,&b.a);
    return CVec4(c);
}

void CVec4::sx(float x){
    a.x = x;
}

void CVec4::sy(float y){
    a.y = y;
}

void CVec4::sz(float z)
{
    a.z = z;
}

void CVec4::sw(float w)
{
    a.w = w;
}

float CVec4::x(){
    return a.x;
}

float CVec4::y(){
    return a.y;
}

float CVec4::z(){
    return a.z;
}

float CVec4::w(){
    return a.w;
}

void CVec4::print(){
    v4Print(&a);
}

float CVec4::mag(){
    Vec4 * v = &a;
    return sqrtf(v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w);
}

float CVec4::magSquared(){
    Vec4 * v = &a;
    return v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w;
}

float CVec4::normalize(){
    Vec4 * v = &a;
    float mag =  sqrtf(v->x*v->x + v->y*v->y + v->z*v->z + v->w*v->w);
    v4IS(v,1.0f/mag);
    return mag;
}

CVec4 CVec4::getNormal(){
    float mag = sqrtf(v4DotIW(&this->a,&this->a));
    mag = 1.0f/mag;
    CVec4 a = *this;
    return a*mag;
}

void CVec4::uploadVector(int layout, CVec4 v){
    glUniform4fv(layout,1,(GLfloat*)&v);OPENGL_ERROR();
}

void CVec4::halfHomogTransform(){
    a.x/=a.w;
    a.y/=a.w;
    a.z/=a.w;
    a.w=-1.0f/a.w; // a.w = 1/a.z
}

