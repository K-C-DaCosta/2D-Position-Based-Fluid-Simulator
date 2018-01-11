
#include <GLFW/glfw3.h>
#include "../include/signed_distance_field.h"
#include "../include/math_util.h"

void SDFUTL::initSignedDistanceField(SignedDistanceField *sdf, int w, int h, int d){
    sdf->buffer = new float[w*h*d];
    sdf->w = w;
    sdf->h = h;
    sdf->d = d;
    sdf->min = sdf->max = {0,0,0,0};
}

void SDFUTL::computeDistanceField(SignedDistanceField *sdf, VAO *object){
    float *buffer = sdf->buffer;
    Vec4 *min = &sdf->min;
    Vec4 *max = &sdf->max;
    uint32_t &w = sdf->w;
    uint32_t &h = sdf->h;
    uint32_t &d = sdf->d;


    uint32_t I, x,y,z;
    std::vector<CVec4> & vList = *object->getVertexBuffer();

    Vec4 * vData = (Vec4*)vList.data();
    MTHUTL::getBoundingBox(vList,min,max);
    max->y = max->x;
    min->y = min->x;

//    v4IS(max,0.9);
//    v4IS(min,0.9);


    //translate to origin and map into a custom rectangle
    for(I = 0; I < vList.size(); I++){
        //squeeze points into a w*h*d rectangle
        vData[I].x = MTHUTL::map2(vData[I].x,min->x,max->x,0.0f,w);
        vData[I].y = MTHUTL::map2(vData[I].y,min->y,max->y,0.0f,h);
        vData[I].z = MTHUTL::map2(vData[I].z,min->z,max->z,0.0f,d);
    }
    double t0 = glfwGetTime(),
           progress;

    //compute SDF(hopefully)
    for(z = 0; z < d; z++ ){
        for(y = 0; y < h; y++){
            for(x = 0; x < w; x++){
                buffer[GI3D(x,y,z,w,h)] = getMinDistToSurface({x+0.5f,y+0.5f,z+0.5f,1.0f},object,NULL);
                if( (glfwGetTime() - t0 ) > 0.5 ){
                    progress = (double(GI3D(x,y,z,w,h))/double((w*h*d)-1))*100.0;
                    printf(" progress .... %.1f percent \n",progress);
                    t0 = glfwGetTime();
                }
            }
        }
    }

    //transform points back into its original positions
    for(I = 0; I < vList.size(); I++){
        //squeeze points into a w*h*d rectangle
        vData[I].x = MTHUTL::map2(vData[I].x,0.0f,w,min->x,max->x);
        vData[I].y = MTHUTL::map2(vData[I].y,0.0f,h,min->y,max->y);
        vData[I].z = MTHUTL::map2(vData[I].z,0.0f,d,min->z,max->z);
    }
}

float SDFUTL::computeNearestDistance(SignedDistanceField *sdf, Vec4 *p0){
    Vec4 p = *p0;
    Vec4 &min = sdf->min;
    Vec4 &max = sdf->max;

    uint32_t &w = sdf->w,
             &h = sdf->h,
             &d = sdf->d;

    float *buffer = sdf->buffer;

    p.x = MTHUTL::map(p.x,min.x,max.x,0,w);
    p.y = MTHUTL::map(p.y,min.y,max.y,0,h);
    p.z = MTHUTL::map(p.z,min.z,max.z,0,d);

    return MTHUTL::trilinear(buffer,p,w,h);
}


Vec4 SDFUTL::computeNearest(SignedDistanceField *sdf, Vec4 *p0){
    Vec4 p = *p0, grad;
    Vec4 &min = sdf->min;
    Vec4 &max = sdf->max;

    uint32_t &w = sdf->w,
             &h = sdf->h,
             &d = sdf->d;

    float *buffer = sdf->buffer;

    float DIST;

    p.x = MTHUTL::map(p.x,min.x,max.x,0,w);
    p.y = MTHUTL::map(p.y,min.y,max.y,0,h);
    p.z = MTHUTL::map(p.z,min.z,max.z,0,d);


//        Vec4 tlp = *lp;
//        for(int I = 0; I < 3; I++){
//            float d0  = MTHUTL::trilinear(buffer,{lp->x,lp->y,lp->z,1},w,h);
//            dd.x = MTHUTL::trilinear(buffer,{lp->x-0.01f,lp->y     ,lp->z     ,1},w,h) - d0;
//            dd.y = MTHUTL::trilinear(buffer,{lp->x     ,lp->y-0.01f,lp->z     ,1},w,h) - d0;
//            dd.z = MTHUTL::trilinear(buffer,{lp->x     ,lp->y     ,lp->z-0.01f,1},w,h) - d0;
//            dd.w = 0.0f;
//            v4Norm(&dd);
//            v4IS(&dd,d0*0.2f);
//            tlp.x+=dd.x;
//            tlp.y+=dd.y;
//            tlp.z+=dd.z;
//        }

    for(int I = 0; I < 2; I++){
        grad = MTHUTL::gradient(buffer,p,w,h);
        v4Norm(&grad);
        DIST = MTHUTL::trilinear(buffer,p,w,h);
        DIST*=0.5f;
        p.x-=grad.x*DIST;
        p.y-=grad.y*DIST;
        p.z-=grad.z*DIST;
    }

    p.x = MTHUTL::map(p.x,0,w,min.x,max.x);
    p.y = MTHUTL::map(p.y,0,h,min.y,max.y);
    p.z = MTHUTL::map(p.z,0,d,min.z,max.z);


    return p;
}

Vec4 SDFUTL::computeNearestAvg(SignedDistanceField *sdf,Vec4 *p0){
    Vec4 avg = {0,0,0,0};
    Vec4 p;
    Vec4 tp;
    const float DEL = 0.25f;

    tp = {p0->x,p0->y,p0->z,p0->w};
    p = computeNearest(sdf,p0);
    v4IA(&avg,&p);

    tp = {p0->x+DEL,p0->y,p0->z,p0->w};
    p = computeNearest(sdf,p0);
    v4IA(&avg,&p);

    tp = {p0->x-DEL,p0->y,p0->z,p0->w};
    p = computeNearest(sdf,p0);
    v4IA(&avg,&p);

    tp = {p0->x,p0->y+DEL,p0->z,p0->w};
    p = computeNearest(sdf,p0);
    v4IA(&avg,&p);

    tp = {p0->x,p0->y-DEL,p0->z,p0->w};
    p = computeNearest(sdf,p0);
    v4IA(&avg,&p);

    tp = {p0->x,p0->y,p0->z+DEL,p0->w};
    p = computeNearest(sdf,p0);
    v4IA(&avg,&p);

    tp = {p0->x,p0->y,p0->z-DEL,p0->w};
    p = computeNearest(sdf,p0);
    v4IA(&avg,&p);

    v4IS(&avg,1.0f/7.0f);

    return avg;

}

float SDFUTL::getMinDistToSurface(Vec4 p,VAO *plane,Vec4 *minPointPtr){
    /*This is a brute force calculation of the minimum distance from a point 'P' to a 3D mesh.
     *This is easily the worst preforming function in the entire program and call this POS every frame.
     *
     */

    const float MAX_DIST = 9999999;
    uint32_t I,J;
    float minDist,curDist,sign,seg_t;
    Vec4 N,d0,d1,bc,minN,minP,minPoint;
    Vec4 tri[3];

    std::vector<int> & indexList = *plane->getIndexBuffer();
    Vec4 * vBuf = (Vec4*)plane->getVertexBuffer()->data();

    minDist = MAX_DIST;
    sign = 1.0f;
    minN = minP = {0,0,0,0};


    for(I = 0; I < indexList.size(); I+=3){

        tri[0] = vBuf[indexList[I+0]];
        tri[1] = vBuf[indexList[I+1]];
        tri[2] = vBuf[indexList[I+2]];

        v4Su(&d0,tri+1,tri);
        v4Su(&d1,tri+2,tri);
        v4Cross(&N,&d0,&d1);
        bc = MTHUTL::line_v_triangle(&p,&N,tri);

        if(MTHUTL::isValidBaryCoord(&bc)){
            //reuse d0,d1;
            //compute point on triangle
            v4ASA(&d0,&N,bc.w,&p);
            //compute displacement between projection and p
            v4Su(&d1,&d0,&p);
            //compute distance between projection and p
            curDist =  v4Mag(&d1);
            //keep the smaller distance
            if(curDist < minDist){
                minDist = curDist;
                minP = tri[0];
                minN = N;
                minPoint = d0;
            }
        }else{
            //since the projected point is not inside the triangle I will look at the edges instead
            for(J = 0; J < 3; J++){
                v4Su(&d0,&tri[(J+1)%3],&tri[J]);
                v4Su(&d1,&p,&tri[J]);
                seg_t = v4Dot(&d0,&d1)/v4Dot(&d0,&d0);
                //reuse d1 to store projected
                seg_t = CLAMP(seg_t,0.0f,1.0f);
                v4ASA(&d1,&d0,seg_t,&tri[J]);
                //reuse d0 to store displacement
                v4Su(&d0,&p,&d1);
                curDist = v4Mag(&d0);

                if(curDist < minDist){
                    minDist = curDist;
                    minP = tri[0];
                    minN = N;
                    minPoint = d1;
                }
            }
        }
    }

    if(fabsf(minDist-MAX_DIST) < 0.0001f){
        printf("THE FUNCTION FAILED ):<  \n");
    }

    if(minPointPtr){
        *minPointPtr = minPoint;
    }
    //if p is below the plane make the minDist negative
    v4Su(&d0,&p,&minP);
    if( v4Dot(&d0,&minN) < 0.0f ){
        sign = -1.0f;
    }


    return minDist*sign;
}

//void computeSDF(VAO *plane,float *buffer,uint32_t w,uint32_t h,uint32_t d,Vec4 *min,Vec4 *max){
//    uint32_t I, x,y,z;

//    std::vector<CVec4> & vList = *plane->getVertexBuffer();
//    Vec4 * vData = (Vec4*)vList.data();

//    MTHUTL::getBoundingBox(vList,min,max);
//    max->y = max->x;
//    min->y = min->x;


//    //translate to origin and map into a custom rectangle
//    for(I = 0; I < vList.size(); I++){
//        //squeeze points into a w*h*d rectangle
//        vData[I].x = MTHUTL::map2(vData[I].x,min->x,max->x,0.0f,w);
//        vData[I].y = MTHUTL::map2(vData[I].y,min->y,max->y,0.0f,h);
//        vData[I].z = MTHUTL::map2(vData[I].z,min->z,max->z,0.0f,d);
//    }
//    double t0 = glfwGetTime(),
//           progress;

//    //compute SDF(hopefully)
//    for(z = 0; z < d; z++ ){
//        for(y = 0; y < h; y++){
//            for(x = 0; x < w; x++){
//                buffer[GI3D(x,y,z,w,h)] = getMinDistToSurface({x+0.5f,y+0.5f,z+0.5f,1.0f},plane,NULL);
//                if( (glfwGetTime() - t0 ) > 0.5 ){
//                    progress = (double(GI3D(x,y,z,w,h))/double((w*h*d)-1))*100.0;
//                    printf(" progress .... %.1f percent \n",progress);
//                    t0 = glfwGetTime();
//                }
//            }
//        }
//    }

//    //transform points back into its original positions
//    for(I = 0; I < vList.size(); I++){
//        //squeeze points into a w*h*d rectangle
//        vData[I].x = MTHUTL::map2(vData[I].x,0.0f,w,min->x,max->x);
//        vData[I].y = MTHUTL::map2(vData[I].y,0.0f,h,min->y,max->y);
//        vData[I].z = MTHUTL::map2(vData[I].z,0.0f,d,min->z,max->z);
//    }
//}

void SDFUTL::writeToDisk(SignedDistanceField *sdf, std::string fileName){
    FILE *fp;
    uint32_t sdfSize = sdf->w*sdf->h*sdf->d;
    if( (fp  = fopen(fileName.c_str(),"wb")) != NULL  ){
        fwrite(&sdf->w,4,1,fp);
        fwrite(&sdf->h,4,1,fp);
        fwrite(&sdf->d,4,1,fp);
        fwrite(&sdf->max,sizeof(Vec4),1,fp);
        fwrite(&sdf->min,sizeof(Vec4),1,fp);
        fwrite(sdf->buffer,sizeof(float),sdfSize,fp);//write sdf data
    }
    fclose(fp);
}

bool SDFUTL::readFromDisk(SignedDistanceField *sdf, std::__cxx11::string fileName){
    FILE *fp;
    uint32_t sdfSize;
    float *buffer;


    fp  = fopen(fileName.c_str(),"rb");

    if( fp == NULL   )
        return false;

    fread(&sdf->w,4,1,fp);
    fread(&sdf->h,4,1,fp);
    fread(&sdf->d,4,1,fp);
    fread(&sdf->max,sizeof(Vec4),1,fp);
    fread(&sdf->min,sizeof(Vec4),1,fp);

    sdfSize = sdf->w*sdf->h*sdf->d;
    buffer = new float[sdfSize];
    fread(buffer,sizeof(float),sdfSize,fp);//read sdf data

    sdf->buffer = buffer;
    fclose(fp);

    return true;
}
