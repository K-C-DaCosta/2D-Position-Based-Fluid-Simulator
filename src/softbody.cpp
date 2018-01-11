#include "../include/softbody.h"
#include "../include/math_util.h"

/*
 * Robot simulated with hacky verlet physics
 */

const int BASIS_P = 0;
const int BASIS_Q = 1;
const int BASIS_R = 2;
const int BASIS_O = 3;
const float GRAV_ACCEL = 0.04f;
const float TIRE_RADIUS = 3.0f;

//bool hasReset = false;
float temp = 0.0f;

//CMAT44 tempMat = (MAT44){1,0,0,0,
//                  0,1,0,0,
//                  0,0,1,0,
//                  0,0,0,1};

void applyFriction(float coef,Vec4 * curArr,Vec4 *preArr,uint32_t I){
    Vec4 disp;
    v4Su(&disp,curArr+I,preArr+I);
    v4IS(&disp,coef);
    v4IA(preArr+I,&disp);
}


SoftBody::SoftBody(CVec4 sp){
    uint32_t I;
    CVec4 center = {0,0,0,0};

    //create a 2x2x2 box which will serve as the 'frame' of the robot
    vaoBoxPtr      = new VAO(VAO_CUBE | VAO_DELETE_DATA ,GL_DYNAMIC_DRAW ,-2,2,5,1,2,-6);
    vaoDummyBoxPtr = new VAO(VAO_CUBE | VAO_DELETE_DATA ,GL_STATIC_DRAW  ,-2,2,2,-2,4,-4); //box of same dimenension centered at origin

    std::vector<CVec4>& vertexList = *vaoBoxPtr->getVertexBuffer();
    for(I = 0; I < vertexList.size(); I++ ){
        vertexList[I]+=CVec4(20,0.0,-20,0);
    }

    for(I = 0; I < vertexList.size(); I++ ){
        center+=vertexList[I];
    }
    center*= 1.0f/float(vertexList.size());
    basis[BASIS_O] = *(Vec4*)&center;

    initConstraints();
    cacheOrientation();
    initBasis();
}

SoftBody::~SoftBody(){
    delete vaoBoxPtr;
}

void SoftBody::rotateRobot(uint32_t axisIndex, float degrees){
    std::vector<CVec4> * vertexBufferVec = vaoBoxPtr->getVertexBuffer();
    Vec4 * curVertexList = (Vec4*)vertexBufferVec->data();
//    Vec4 * preVertexList = (Vec4*)oldBox.data();
    MAT44 rotMat;
    //compute rotation matrix
    mat44SetRotationOnAxis(&rotMat,&basis[axisIndex],TO_RADIANS(degrees));
    //apply rotation
    for(uint32_t I = 0; I < vertexBufferVec->size();I++ ){
        v4IAS(curVertexList+I,basis+BASIS_O,-1.0f);
        curVertexList[I] = mat44Transform(&rotMat,curVertexList+I);
        v4IAS(curVertexList+I,basis+BASIS_O,1.0f);

//        v4IAS(preVertexList+I,basis+BASIS_O,-1.0f);
//        preVertexList[I] = mat44Transform(&rotMat,preVertexList+I);
//        v4IAS(preVertexList+I,basis+BASIS_O,1.0f);
    }
}

void SoftBody::translate(uint32_t axisIndex, float speed){
    std::vector<CVec4> * vertexBufferVec = vaoBoxPtr->getVertexBuffer();
    Vec4 * curVertexList = (Vec4*)vertexBufferVec->data();
    Vec4 disp;

    //calculate displacement vector
    v4S(&disp,&basis[axisIndex],speed);
    for(uint32_t I = 0; I < vertexBufferVec->size();I++ ){
        v4IA(curVertexList+I,&disp);
    }
}

void SoftBody::translate(Vec4 *disp){
    std::vector<CVec4> * vertexBufferVec = vaoBoxPtr->getVertexBuffer();
    Vec4 * curVertexList = (Vec4*)vertexBufferVec->data();
    for(uint32_t I = 0; I < vertexBufferVec->size();I++ ){
        v4IA(curVertexList+I,disp);
    }
}

void SoftBody::pureTranslate(Vec4 direction){
    std::vector<CVec4> * vertexBufferVec = vaoBoxPtr->getVertexBuffer();
    Vec4 * curVertexList = (Vec4*)vertexBufferVec->data();
    Vec4 * preVertexList = (Vec4*)oldBox.data();
    Vec4 disp = direction;
    disp.w = 0.0f;
    for(uint32_t I = 0; I < vertexBufferVec->size();I++ ){
        v4IA(curVertexList+I,&disp);
        v4IA(preVertexList+I,&disp);
    }
}

void SoftBody::pureTransform(MAT44 *tmat){
    std::vector<CVec4> * vertexBufferVec = vaoBoxPtr->getVertexBuffer();
    Vec4 * curVertexList = (Vec4*)vertexBufferVec->data();
    Vec4 * preVertexList = (Vec4*)oldBox.data();
    for(uint32_t I = 0; I < vertexBufferVec->size();I++ ){
        curVertexList[I] = mat44Transform(tmat,curVertexList+I);
        preVertexList[I] = mat44Transform(tmat,preVertexList+I);
    }
}

void SoftBody::renderRobot(MatrixStack &matStack){
    CMAT44 ACCUM;
//    //really dumb way of drawing the body of the robot
//    vaoBoxPtr->update();
//    vaoBoxPtr->renderObject(GL_TRIANGLES);

    matStack.clearAccum();
    matStack.accumulate(getBasisMatrix(&ACCUM));
    matStack.pushMatrix();
        matStack.updateModelView();
        vaoDummyBoxPtr->renderObject(GL_TRIANGLES);
    matStack.popMatrix();
}

void SoftBody::enforceContraints(){
    uint32_t I,J;
    float curLen,overlap,targetLen;
    Vec4 disp,*c0,*c1;

    /*
     * Since this is a preformance critical routine,
     * I've decided to cast CVec4 to my simple C struct, Vec4,  in an attempt to avoid copy
     * construction.
     */
    Vec4* vertexList = (Vec4*)vaoBoxPtr->getVertexBuffer()->data();

    for(I=0, J=0; I < constraints.size(); I+=2,J++){
        c0 = vertexList+(constraints[I+0]);
        c1 = vertexList+(constraints[I+1]);
        targetLen = distances[J];
        v4Su(&disp,c1,c0);//disp = c1 - c0
        curLen = sqrtf(v4Dot(&disp,&disp));
        overlap = (targetLen-curLen)/curLen*0.5f;
        v4IS(&disp,overlap); //disp *= overlap
        v4IA(c1,&disp); //c1+=disp
        v4IS(&disp,-1.0f);
        v4IA(c0,&disp);//c+=(-disp) <-> c-=disp
    }
}

void SoftBody::updatePosition(Vec4 *p){
//    if(hasReset) return;

    uint32_t  I   = 0,
             size = oldBox.size();
    Vec4 *curPos = (Vec4*)vaoBoxPtr->getVertexBuffer()->data(),
         *prePos = (Vec4*)oldBox.data();
    Vec4 disp,tempCur;


//    curPos[0] = *p;

    for(I = 0; I < size; I++){
        tempCur = curPos[I];
        v4Su(&disp,curPos+I,prePos+I);
        v4IA(curPos+I,&disp);// curPos += disp
        curPos[I].y-= GRAV_ACCEL;//constant acceleration
        prePos[I] = tempCur;

//        //check if a vertex penetrates the xz-plane(temporary).
//        if(curPos[I].y < -3){
//            //Here I resolve penetration
//            curPos[I].y = -3;
//       }
    }
}

void SoftBody::updateVAOs(){
    vaoBoxPtr->update();
}

void SoftBody::updateBasis(){
    //update center
    Vec4 pmax,pmin,
         qmax,qmin,
         rmax,rmin;
    float t;

    //compute intersection positions and compute/build two line segments
    GEOCACHE::calcWorldSpace(vaoBoxPtr->getVertexBuffer(),&axes[0].minPoint,&pmin);
    GEOCACHE::calcWorldSpace(vaoBoxPtr->getVertexBuffer(),&axes[0].maxPoint,&pmax);
    GEOCACHE::calcWorldSpace(vaoBoxPtr->getVertexBuffer(),&axes[1].minPoint,&qmin);
    GEOCACHE::calcWorldSpace(vaoBoxPtr->getVertexBuffer(),&axes[1].maxPoint,&qmax);

    //calculate intersection of the two line segments to compute the new 'center of mass'
    t = MTHUTL::segment_v_segment(&pmin,&pmax,&qmin,&qmax);
    v4Seg(basis+BASIS_O,&pmax,&pmin,t,&pmin);

    //compute compute the final line segment and rebuild coordinate system
    GEOCACHE::calcWorldSpace(vaoBoxPtr->getVertexBuffer(),&axes[2].minPoint,&rmin);
    GEOCACHE::calcWorldSpace(vaoBoxPtr->getVertexBuffer(),&axes[2].maxPoint,&rmax);

    /*
     * Compute and store distances from intersections to center
     * to estimate basis vectors
     */
    //compute distance from center to P intersection
    v4Su(&basis[BASIS_P],&pmax,&basis[BASIS_O]);
    v4IS(&basis[BASIS_P],axesInvMags[BASIS_P]);

    //compute distance from center to Q intersection
    v4Su(&basis[BASIS_Q],&qmax,&basis[BASIS_O]);
    v4IS(&basis[BASIS_Q],axesInvMags[BASIS_Q]);

    //computer distance from center to R intersection
    v4Su(&basis[BASIS_R],&rmax,&basis[BASIS_O]);
    v4IS(&basis[BASIS_R],axesInvMags[BASIS_R]);
}

void SoftBody::doTerrainCollision(VAO *plane){
    uint32_t J;
    Vec4 *chassisPoints = (Vec4*)vaoBoxPtr->getVertexBuffer()->data();
    Vec4 *preChassisPoints = (Vec4*)oldBox.data();
    Vec4 normal,point;

    bool reset = false;
    for(J = 0; J < 4; J++){
        SDFUTL::getMinDistToSurface(chassisPoints[J],plane,&point);
        if( MTHUTL::point_v_sphere(&point,chassisPoints+J,TIRE_RADIUS,&normal) ){
            reset = true;
            break;
        }
    }

    if(reset){
       resetOrientation();
    }

    for(J = 4; J < 8; J++){
        SDFUTL::getMinDistToSurface(chassisPoints[J],plane,&point);
        if( MTHUTL::point_v_sphere(&point,chassisPoints+J,TIRE_RADIUS,&normal) ){
            v4IAS(chassisPoints+J,&normal,-0.5f);
            applyFriction(0.1,chassisPoints,preChassisPoints,J);
        }
    }
}

void SoftBody::doSDFCollision(SignedDistanceField *sdf){
    uint32_t J;
    Vec4 *chassisPoints = (Vec4*)vaoBoxPtr->getVertexBuffer()->data();
    Vec4 *preChassisPoints = (Vec4*)oldBox.data();
    Vec4 normal,point;

    bool reset = false;
    for(J = 0; J < 4; J++){
        point = SDFUTL::computeNearest(sdf,chassisPoints+J);
        if( MTHUTL::point_v_sphere(&point,chassisPoints+J,TIRE_RADIUS*0.25f,&normal) ){
            reset = true;
            break;
        }
    }

    if(reset){
       resetOrientation();
    }

    for(J = 4; J < 8; J++){
        point = SDFUTL::computeNearestAvg(sdf,chassisPoints+J);
        if( MTHUTL::point_v_sphere(&point,chassisPoints+J,TIRE_RADIUS,&normal) ){
            v4IAS(chassisPoints+J,&normal,-1.0f);
            applyFriction(0.1,chassisPoints,preChassisPoints,J);
        }
    }
}

CMAT44 *SoftBody::getBasisMatrix(CMAT44 *mat){
    Vec4 *b = basis;
    *mat = (MAT44){b[0].x,b[1].x,b[2].x,b[3].x,
                   b[0].y,b[1].y,b[2].y,b[3].y,
                   b[0].z,b[1].z,b[2].z,b[3].z,
                    0.0f , 0.0f , 0.0f , 1.0f };
return mat;
}

void SoftBody::enforceWallConstraints(float Xmin, float Xmax, float Ymin, float Ymax, float Zmin, float Zmax){
    std::vector<CVec4> & vaoBoxVec = *vaoBoxPtr->getVertexBuffer();
    std::vector<CVec4> & vaoDummyBoxVec = *vaoDummyBoxPtr->getVertexBuffer();
    Vec4 * curPosList = (Vec4 *)vaoBoxPtr->getVertexBuffer()->data();
    Vec4 Normal;
    Vec4 *cp;
    CMAT44 rotMat;
    Vec4 colVecs[4],R;

    uint32_t K,size;
    size = oldBox.size();
    bool hasCollided = false;
//    if(hasReset)return;

    for(K = 0; K < size; K++ ){
        cp = curPosList+K;
        if( cp->x < Xmin){
            Normal = {1,0,0,0};
            hasCollided = true;
            break;
        }
        if(cp->x > Xmax){
            Normal = {-1,0,0,0};
            hasCollided = true;
            break;
        }
        if(cp->z < Zmin){
            Normal = {0,0,1,0};
            hasCollided = true;
            break;
        }
        if(cp->z > Zmax){
            Normal = {0,0,-1,0};
            hasCollided = true;
            break;
        }
    }
    if(hasCollided){

        //reflect R about the normal of the surface barrier
        v4IS(&basis[BASIS_R],-1.0f);
        R = MTHUTL::reflect(basis[BASIS_R],Normal);

        v4IS(&R,-1.0f);
        basis[BASIS_Q] = {0,1,0,0};

        //reflected rotation matrix
        v4Cross( &colVecs[0] ,&basis[BASIS_Q],&R);
        colVecs[1] = basis[BASIS_Q] ;
        colVecs[2] = R;
        colVecs[3] = basis[BASIS_O];

        //pack it into rotation matrix
        rotMat.setColumnVectors((CVec4*)colVecs);

        for(K = 0; K < size; K++){
            vaoBoxVec[K] = rotMat*vaoDummyBoxVec[K];
            oldBox[K] = vaoBoxVec[K];
        }

        updateBasis();

        v4IS(&Normal,2.0f);
        pureTranslate(Normal);
    }
}

void SoftBody::initBasis(){
    //update center
    Vec4 pmax,pmin,
         qmax,qmin,
         rmax,rmin;
    float t;

    //compute intersection positions and compute build two line segments
    GEOCACHE::calcWorldSpace(vaoBoxPtr->getVertexBuffer(),&axes[0].minPoint,&pmin);
    GEOCACHE::calcWorldSpace(vaoBoxPtr->getVertexBuffer(),&axes[0].maxPoint,&pmax);
    GEOCACHE::calcWorldSpace(vaoBoxPtr->getVertexBuffer(),&axes[1].minPoint,&qmin);
    GEOCACHE::calcWorldSpace(vaoBoxPtr->getVertexBuffer(),&axes[1].maxPoint,&qmax);

    //calculate intersection of the two line segments to compute the new 'center of mass'
    t = MTHUTL::segment_v_segment(&pmin,&pmax,&qmin,&qmax);
    v4Seg(&basis[BASIS_O],&pmax,&pmin,t,&pmin);

    //compute compute the final line segment and rebuild coordinate system
    GEOCACHE::calcWorldSpace(vaoBoxPtr->getVertexBuffer(),&axes[2].minPoint,&rmin);
    GEOCACHE::calcWorldSpace(vaoBoxPtr->getVertexBuffer(),&axes[2].maxPoint,&rmax);

    /*
     * Compute and store distances from intersections to center
     * to estimate basis vectors
     */

    //compute distance from center to P intersection
    v4Su(&basis[BASIS_P],&pmax,&basis[BASIS_O]);
    axesInvMags[BASIS_P] = 1.0f/v4Mag(&basis[BASIS_P]);

    //compute distance from center to Q intersection
    v4Su(&basis[BASIS_Q],&qmax,&basis[BASIS_O]);
    axesInvMags[BASIS_Q] = 1.0f/v4Mag(&basis[BASIS_Q]);

    //computer distance from center to R intersection
    v4Su(&basis[BASIS_R],&rmax,&basis[BASIS_O]);
    axesInvMags[BASIS_R] = 1.0f/v4Mag(&basis[BASIS_R]);
}

void SoftBody::initConstraints(){
    uint32_t I;
    CVec4 disp,c0,c1;
    std::vector<CVec4> &vertexList = *vaoBoxPtr->getVertexBuffer();

    for(I = 0; I < vertexList.size(); I++){
        oldBox.push_back(vertexList[I]);
    }

    //phew
    constraints =  {0,1, 1,2, 2,3, 3,0, 0,2, 1,3,//top
                    4,5, 5,6, 6,7, 7,4, 4,6, 5,7,//bottom
                    0,3, 0,4, 4,7, 7,3, 3,4, 0,7,//left
                    1,5, 5,6, 6,2, 2,5, 1,6,     //right
                    2,6, 6,7, 7,3, 3,6, 3,6, 2,7,//near
                    0,1, 1,5, 5,4, 4,0, 4,1, 0,5,//far
                    0,6, 4,2, 1,7, 3,5};         //diagonals

    for(I = 0; I < constraints.size(); I+=2){
        c0 = vertexList[constraints[I+0]];
        c1 = vertexList[constraints[I+1]];
        disp = c1-c0;
        distances.push_back( sqrtf(disp*disp) );
    }
}

void SoftBody::resetOrientation(){
    Vec4 *realBox = (Vec4 *)vaoBoxPtr->getVertexBuffer()->data();
    Vec4 *fakeBox = (Vec4 *)vaoDummyBoxPtr->getVertexBuffer()->data();
    uint32_t size, I;
    size = vaoBoxPtr->getVertexBuffer()->size();
    Vec4 center = basis[BASIS_O];
    center.w = 0.0f;
    float randVal = float(rand())/float(RAND_MAX);

    for(I =0; I < size; I++){
        v4ASA(&realBox[I],&fakeBox[I],1,&center);
        realBox[I].y+= (3.0f + 8.0f*randVal);
        oldBox[I] = realBox[I];

    }
    updateBasis();
}

void SoftBody::cacheOrientation(){
    calculateAxis({1,0,0,0},axes+0);
    calculateAxis({0,1,0,0},axes+1);
    calculateAxis({0,0,1,0},axes+2);
}

void SoftBody::calculateAxis(Vec4 axis, CachedAxis *ca){
    uint32_t I;
    float min_t,max_t;
    Vec4 tri[3],
         baryCoord;

    std::vector<int> & indexList  = *vaoBoxPtr->getIndexBuffer();
    Vec4 * vertexList = (Vec4*)vaoBoxPtr->getVertexBuffer()->data();


    min_t =  99999999.0f;
    max_t = -99999999.0f;

    for(I = 0; I < indexList.size(); I+=3){
        tri[0] = vertexList[indexList[I+0]];
        tri[1] = vertexList[indexList[I+1]];
        tri[2] = vertexList[indexList[I+2]];
        //calculate intersection of a ray and a triangle
        baryCoord = MTHUTL::line_v_triangle(&basis[3],&axis,tri);

        if( MTHUTL::isValidBaryCoord(&baryCoord) ){
            if(baryCoord.w > max_t){
                max_t = baryCoord.w;
                ca->maxPoint.vIndex[0] = indexList[I+0];
                ca->maxPoint.vIndex[1] = indexList[I+1];
                ca->maxPoint.vIndex[2] = indexList[I+2];
                ca->maxPoint.baryCoord = baryCoord;
            }else{
                if(baryCoord.w < min_t){
                    min_t  = baryCoord.w;
                    ca->minPoint.vIndex[0] = indexList[I+0];
                    ca->minPoint.vIndex[1] = indexList[I+1];
                    ca->minPoint.vIndex[2] = indexList[I+2];
                    ca->minPoint.baryCoord = baryCoord;
                }
            }
        }
    }

    if( min_t ==  99999999.0f || max_t == -99999999.0f){
        printf("something is wrong here... \n");
    }
}
