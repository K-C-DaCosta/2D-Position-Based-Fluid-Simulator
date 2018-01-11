#include "../include/math_util.h"


float MTHUTL::line_v_line(Vec4 *p0, Vec4 *dp, Vec4 *q0, Vec4 *dq){
   Vec4 N, up = {dq->x+1,dq->y+1,dq->z+1,0};//choose up so that it is not colinear with dq
   v4Cross(&N,&up,dq);
   return (v4Dot(q0,&N)-v4Dot(p0,&N))/v4Dot(dp,&N);
}

float MTHUTL::line_v_plane(Vec4 *p0, Vec4 *dp, Vec4 t[]){
    Vec4 N,d0,d1;
    v4Su(&d0,t+1,t+0);
    v4Su(&d1,t+2,t+0);
    v4Cross(&N,&d0,&d1);
    return (v4Dot(t,&N)-v4Dot(p0,&N))/v4Dot(dp,&N);
}

Vec4 MTHUTL::point_v_triangle(Vec4 *p, Vec4 t[]){
    Vec4 d0,d1,d2;
    float a,b,c,d,e,det,lambda0,lambda1,lambda2;
    v4Su(&d0, p ,t+0);
    v4Su(&d1,t+1,t+0);
    v4Su(&d2,t+2,t+0);

    a = v4Dot(&d1,&d1);
    b = v4Dot(&d2,&d1);
    c = v4Dot(&d2,&d2);
    d = v4Dot(&d0,&d1);
    e = v4Dot(&d0,&d2);

    det = b*b - a*c;
    if( fabsf(det) < 0.00125f ){
        return {INFINITY,INFINITY,INFINITY,0.0f};
    }

    det = 1.0f/det;
    lambda2 = (b*d-a*e)*det;
    lambda1 = (b*e-c*d)*det;
    lambda0 = (1.0f-lambda1-lambda2);

    return {lambda0,lambda1,lambda2,0};
}

Vec4 MTHUTL::line_v_triangle(Vec4 *p, Vec4 *dp, Vec4 tri[]){
    Vec4 planePoint,baryCoord;
    float t;
    t = MTHUTL::line_v_plane(p,dp,tri);
    v4ASA(&planePoint,dp,t,p);
    baryCoord = MTHUTL::point_v_triangle(&planePoint,tri);
    baryCoord.w = t;
    return baryCoord;
}

float MTHUTL::point_p_plane(Vec4 *p, Vec4 t[]){
    return 0;//not yet implemented(possibly redundant)
}



bool MTHUTL::isValidBaryCoord(Vec4 *bary){
    return bary->x >= 0.0f &&
           bary->y >= 0.0f &&
           bary->z >= 0.0f ;
}

float MTHUTL::segment_v_segment(Vec4 *p0, Vec4 *p1, Vec4 *q0, Vec4 *q1){
    Vec4 dp,dq;
    v4Su(&dp,p1,p0);
    v4Su(&dq,q1,q0);
    return line_v_line(p0,&dp,q0,&dq);
}

//some implementation I lifted off the net (for testing)
float MTHUTL::map2(float s,float a1,float a2,float b1,float b2){
    return b1 + (s-a1)*(b2-b1)/(a2-a1);
}

//my own implementation
float MTHUTL::map(float x,float x0,float x1, float y0,float y1){
   float rise = (y1-y0);
   float run  = (x1-x0);
   float m,b;

   if( fabs(run) < 0.0001f){
       return x;
   }
   // y= mx + b     -> y1 - m*x1 = b

   m = rise/run;
   b = y1 - m*x1;
   return m*x+b;
}

bool MTHUTL::point_v_sphere(Vec4 *point, Vec4 *center, float r, Vec4 *contactNormal){
    Vec4 disp;
    float distance,overlap;

    /*
     * overlap = (r - |D|)/|D| = r/|D| -1
     */

    v4Su(&disp,point,center);
    distance = v4Dot(&disp,&disp);

    if( distance < r*r ){
        distance = sqrtf(distance);
        if(distance < 0.00125){
            return false;
        }
        distance = 1.0f/distance;


        overlap = r*distance-1.0f;
        v4S(contactNormal,&disp,overlap);
        return true;
    }
    return false;
}

Vec4  MTHUTL::gradient(float *buffer, Vec4 p, uint32_t w, uint32_t h){
    int x  = p.x,
        y  = p.y,
        z  = p.z;

    float DX = p.x - floorf(p.x),
          DY = p.y - floorf(p.y),
          DZ = p.z - floorf(p.z);
    //I compute the gradient of the function defined by the  trilinear interpolation function

    float A = buffer[GI3D(x+0,y+1,z+0,w,h)],
          B = buffer[GI3D(x+1,y+1,z+0,w,h)],
          C = buffer[GI3D(x+1,y+1,z+1,w,h)],
          D = buffer[GI3D(x+0,y+1,z+1,w,h)];

    float E = buffer[GI3D(x+0,y+0,z+0,w,h)],
          F = buffer[GI3D(x+1,y+0,z+0,w,h)],
          G = buffer[GI3D(x+1,y+0,z+1,w,h)],
          H = buffer[GI3D(x+0,y+0,z+1,w,h)];

    //The function that I take the gradient of is of the form:  f(x,y,z) = V0*xyz + .... + V7

    float V0 = A-B+C-D+F-G+H-E ,
          V1 = -A+D-H+E,
          V2 = -F+G-H+E,
          V3 =  H-E,
          V4 = -A+B-F+E,
          V5 = A-E,
          V6 = F-E;
//          V7 = H;

    return {  DY*DZ*V0 + DZ*V2 + DY*V4 + V6,
              DX*DZ*V0 + DZ*V1 + DX*V4 + V5,
              DX*DY*V0 + DY*V1 + DX*V2 + V3,
             0};

}

float MTHUTL::trilinear(float *buffer, Vec4 p, uint32_t w, uint32_t h){
        int x  = p.x,
            y  = p.y,
            z  = p.z;

        float xfrac = p.x - floorf(p.x),
              yfrac = p.y - floorf(p.y),
              zfrac = p.z - floorf(p.z);

        float bf = linear(buffer[GI3D(x+0,y+0,z+0,w,h)],buffer[GI3D(x+1,y+0,z+0,w,h)],xfrac);
        float bn = linear(buffer[GI3D(x+0,y+0,z+1,w,h)],buffer[GI3D(x+1,y+0,z+1,w,h)],xfrac);
        float tf = linear(buffer[GI3D(x+0,y+1,z+0,w,h)],buffer[GI3D(x+1,y+1,z+0,w,h)],xfrac);
        float tn = linear(buffer[GI3D(x+0,y+1,z+1,w,h)],buffer[GI3D(x+1,y+1,z+1,w,h)],xfrac);
        float bm = linear(bn,bf,zfrac);
        float tm = linear(tn,tf,zfrac);
        return linear(bm,tm,yfrac);



}


float MTHUTL::linear(float a, float b, float t){

    return (b-a)*t + a;//assuming 0 <= t <= 1
}


void MTHUTL::getBoundingBox(std::vector<CVec4> &vList, Vec4 *pmin, Vec4 *pmax){
    uint32_t I;
    Vec4 * list = (Vec4*)vList.data();
    Vec4 min,max;

    min = max = list[0];
    for(I = 1; I < vList.size(); I++){

        min.x = MIN(list[I].x,min.x);
        min.y = MIN(list[I].y,min.y);
        min.z = MIN(list[I].z,min.z);

        max.x = MAX(list[I].x,max.x);
        max.y = MAX(list[I].y,max.y);
        max.z = MAX(list[I].z,max.z);
    }

    *pmin = min;
    *pmax = max;
}


Vec4 MTHUTL::reflect(Vec4 V,Vec4 N){
    // This code simply reflects a vector about some normal using the equation...
    // V' = V + N -2*((V.N))
    Vec4 Vprime,Vpara;
    float t = v4Dot(&V,&N);
    v4S(&Vpara,&N,-2.0f*t);
    v4A(&Vprime,&Vpara,&V);
    return Vprime;
}

bool MTHUTL::sphere_v_sphere(Vec4 *a, Vec4 *b, float ra, float rb, Vec4 *normal){
    float dist,minDist,overlap;
    Vec4 disp;

    v4Su(&disp,b,a);
    dist = v4Dot(&disp,&disp);
    minDist = ra+rb;

    if(dist < 0.0125f){
        return false;
    }

    if( dist < minDist*minDist){
        dist = sqrtf(dist);
        overlap = (minDist-dist)/dist;
        v4S(normal,&disp,overlap*0.5f);
        return true;
    }

    return false;
}
