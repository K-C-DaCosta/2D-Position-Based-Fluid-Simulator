#include "../include/shape_util.h"
#include "../include/matrix.h"
#include "../include/math_util.h"
/*
    Description: A utility to generate vertex data for spheres,planes and other geometry
    Name:Khadeem DaCosta
    Student#:500590228
*/


void ShapeUtil::createSphere(float radius,int zStep,int yStep,
                             std::vector<int>   *iB,
                             std::vector<CVec4> *vB,
                             std::vector<CVec4> *nB){
    uint32_t I,vbIndex;

    int degY,degZ;

    float sin0,cos0,
          rad,
          cx,cy;

    std::vector<CVec4> line0,
                       line1,
                       line2;

    CMAT44 rot;

    vbIndex = 0;

    //this only needs to be computed once.
    for(degZ = -90; degZ <= 90; degZ+=zStep){
        rad = TO_RADIANS(degZ);//convert to radians
        sin0 = sinf(rad);
        cos0 = cosf(rad);
        cx = cos0*radius;
        cy = sin0*radius;
        line0.push_back( CVec4(cx,cy,0.0f,1.0f) );
    }

    line1.resize(line0.size());
    line2.resize(line0.size());

    for(degY = 0; degY < 360; degY+= yStep){
        rad = TO_RADIANS(degY);
        rot.setNRotation(CVec4(0,1,0,0),rad);
        for(I = 0; I < line0.size(); I++ ){
            line1[I] = rot*line0[I];
        }

        rad = TO_RADIANS(degY+yStep);
        rot.setNRotation(CVec4(0,1,0,0),rad);
        for(I = 0; I < line0.size(); I++ ){
            line2[I] =rot*line0[I];
        }

        for(I = 0; I < line0.size()-1; I++ ){
            /*
            l11-----l21       i+1-----i+1
             |       |   ->    |       |
             |       |   ->    |       |
            l10-----l20       i+0-----i+0
            */

            //insert sphere vertexes into buffer
            vB->push_back(line1[I+0]);
            vB->push_back(line2[I+0]);
            vB->push_back(line1[I+1]);
            vB->push_back(line2[I+1]);

//            //insert normal attribues
            nB->push_back(line1[I+0].getNormal());
            nB->push_back(line2[I+0].getNormal());
            nB->push_back(line1[I+1].getNormal());
            nB->push_back(line2[I+1].getNormal());

            //For flat shading
//            nB->push_back(line1[I+0].getNormal() );
//            nB->push_back(line1[I+0].getNormal() );
//            nB->push_back(line1[I+0].getNormal() );
//            nB->push_back(line1[I+0].getNormal() );

            //calcate indexes for points on sphere
            iB->push_back(vbIndex + 0);
            iB->push_back(vbIndex + 1);
            iB->push_back(vbIndex + 2);

            iB->push_back(vbIndex + 1);
            iB->push_back(vbIndex + 3);
            iB->push_back(vbIndex + 2);

            vbIndex+=4;
        }
    }
}

void ShapeUtil::createPlane(uint32_t n, CVec4 p, CVec4 q, CVec4 o,
                            std::vector<int>   *iB,
                            std::vector<CVec4> *vB,
                            std::vector<CVec4> *nB){
    uint32_t i,j;
    CMAT44 tmat;
    CVec4 sp;
    tmat.setPlaneTransform(&p,&q,&o);
    float invN = 1.0f/n;


    //generate geometry

    vB->reserve(n*n);


    for(j = 0; j < n; j++){
        for(i = 0; i < n; i++){
            sp = {float(i)*invN,float(j)*invN,0,1.0f};
            sp = tmat*sp;
//            sp.sy( sp.y() + 0.5f*sinf(0.3f*sp.x()) + 2.5f*cosf(0.2*sp.z())  );
            vB->push_back(sp);
        }
    }

    //here I index the geometry
    for(j = 0; j < n-1; j++){
        for(i = 0; i < n-1; i++){
            iB->push_back(GI(i+0,j+0,n) );
            iB->push_back(GI(i+1,j+1,n) );
            iB->push_back(GI(i+0,j+1,n) );
            iB->push_back(GI(i+0,j+0,n) );
            iB->push_back(GI(i+1,j+0,n) );
            iB->push_back(GI(i+1,j+1,n) );
        }
    }

    nB->resize( vB->size() );
    CVec4 d0,d1,norm,a,b,c;
    int ia,ib,ic;
    std::vector<int>   & iBr = *iB;
    std::vector<CVec4> & vBr = *vB;
    std::vector<CVec4> & nBr = *nB;

    for(i = 0; i < iBr.size() ; i+= 3){
        ia = iBr[i+0];
        ib = iBr[i+1];
        ic = iBr[i+2];
        a = vBr[ia];
        b = vBr[ib];
        c = vBr[ic];
        d0 = b-a;
        d1 = c-b;
        norm=d0^d1;//d0 cross d1
        norm *= 1.0f/sqrtf(norm*norm);
        nBr[ia] = norm;
        nBr[ib] = norm;
        nBr[ic] = norm;
    }
}

void ShapeUtil::createCone(std::vector<int> *iB, std::vector<CVec4> *vB, std::vector<CVec4> *normal){
//not yet implemented
}

void ShapeUtil::createCube(float l,float r,float t, float b,float n,float f,
                               std::vector<int> *iB, std::vector<CVec4> *vB, std::vector<CVec4> *nB){
    uint32_t I;
    CVec4 center ( (l+r)*0.5,(t+b)*0.5, (n+f)*0.5 ,1);

    vB->push_back(CVec4(l,t,f,1));//0
    vB->push_back(CVec4(r,t,f,1));//1
    vB->push_back(CVec4(r,t,n,1));//2
    vB->push_back(CVec4(l,t,n,1));//3
    vB->push_back(CVec4(l,b,f,1));//4
    vB->push_back(CVec4(r,b,f,1));//5
    vB->push_back(CVec4(r,b,n,1));//6
    vB->push_back(CVec4(l,b,n,1));//7

    //calculate ""normals"" (they arent calculated right but I just want this to work for now)
    for(I = 0; I < vB->size(); I++){
        nB->push_back( ((*vB)[I] - center).getNormal() );
    }

    int il[36] = {2,1,0,0,3,2,7,6,2,7,2,3,6,1,2,6,5,1,7,6,4,6,5,4,4,5,1,4,1,0,7,4,0,7,0,3};
    for(I = 0; I < 36; I++){
        iB->push_back(il[I]);
    }
}

void ShapeUtil::createCylinder(uint32_t n, float radius, float height, CVec4 centerDisp, std::vector<int> *iB, std::vector<CVec4> *vB, std::vector<CVec4> *nB){
    float rad,angleIncrement;
    uint32_t I,indexCount,capIndexOffset;//,centerCapIndex;
    CMAT44 rot;
    CVec4 p;
    std::vector<CVec4> line0,lineL,lineR;


    rot.setNRotation({0,1,0,0},-TO_RADIANS(90));
    angleIncrement = (2.0f*M_PI)/float(n);
    rad = 0;


    //compute circle on xy-plane and rotate it into the yz-plane
    for(I = 0; I <= n; I++){
        p = { radius*cosf(rad), radius*sinf(rad),0.0f,1.0f };
        p = rot*p;
        line0.push_back(p);
        rad+=angleIncrement;
    }

    //compute curve (right)
    for(I = 0; I < line0.size(); I++){
        p = line0[I] +  CVec4(height*0.5,0,0,0);
        lineR.push_back(p);
    }

    //compute curve (left)
    for(I = 0; I < line0.size(); I++){
        p = line0[I] + CVec4(-height*0.5,0,0,0);
        lineL.push_back(p);
    }

    //compute final vertex buffer and index everything
    indexCount = 0;
    for(I = 0; I < line0.size()-1; I++){
        vB->push_back(lineL[I+0]);
        vB->push_back(lineR[I+0]);
        vB->push_back(lineR[I+1]);
        vB->push_back(lineL[I+1]);

        iB->push_back(indexCount + 0);
        iB->push_back(indexCount + 1);
        iB->push_back(indexCount + 2);

        iB->push_back(indexCount + 2);
        iB->push_back(indexCount + 3);
        iB->push_back(indexCount + 0);

        indexCount+=4;
    }

//    compute normals for the curved part of cylinder
    CVec4 LV = {-0.5f*height,0.0f,0.0f,1.0f},
          RV = { 0.5f*height,0.0f,0.0f,1.0f},
          DISP,
          NORM,
          PARM,
          P;
    float proj;

    for(I = 0; I < vB->size(); I++){
        P = (*vB)[I];
        PARM = LV-P;
        DISP = RV-LV;
        proj = (DISP*PARM)/(DISP*DISP);
        proj = CLAMP(proj,0.0f,1.0f);
        NORM = P - (DISP*proj + LV);
        nB->push_back(NORM);
    }

    /*store the index were the cap vertexes begin
     *so I can compute the rest of the normals*/
    capIndexOffset = iB->size();

//    //compute left cap
//    centerCapIndex = indexCount++;
//    vB->push_back( CVec4( -0.5f*height,0,0,1)  );
//    for(I = 0; I < lineL.size()-1; I++){
//        vB->push_back(lineL[I+0]);
//        vB->push_back(lineL[I+1]);

//        iB->push_back(centerCapIndex);
//        iB->push_back(indexCount + 0 );
//        iB->push_back(indexCount + 1 );
//        indexCount+=2;
//    }

//    //compute right cap
//    centerCapIndex = indexCount++;
//    vB->push_back( CVec4( 0.5f*height,0,0,1)  );
//    for(I = 0; I < lineR.size()-1; I++){
//        vB->push_back(lineR[I+0]);
//        vB->push_back(lineR[I+1]);
//        iB->push_back(centerCapIndex);
//        iB->push_back(indexCount + 1 );
//        iB->push_back(indexCount + 0 );
//        indexCount+=2;
//    }


    //hacky attempt at computing normals (causes facets)
    nB->resize( vB->size() );
    CVec4 d0,d1,norm,a,b,c;
    int ia,ib,ic;
    std::vector<int>   & iBr = *iB;
    std::vector<CVec4> & vBr = *vB;
    std::vector<CVec4> & nBr = *nB;

    for(I = capIndexOffset; I < iBr.size() ; I+= 3){
        ia = iBr[I+0];
        ib = iBr[I+1];
        ic = iBr[I+2];
        a = vBr[ia];
        b = vBr[ib];
        c = vBr[ic];
        d0 = b-a;
        d1 = c-b;
        norm=d0^d1;//d0 cross d1
        norm *= 1.0f/sqrtf(norm*norm);
        //This causes facets. Interpolation is basically disabled because all normals are the same
        nBr[ia] = norm;
        nBr[ib] = norm;
        nBr[ic] = norm;
    }
}

void ShapeUtil::createCircle(uint32_t n, float radius,float normFactor, CVec4 disp, std::vector<int> *iB, std::vector<CVec4> *vB, std::vector<CVec4> *nB, std::vector<CVec4> *uvB){
    float rad,angleIncrement,u,v;
    uint32_t I,indexCount;
    CMAT44 rot;
    CVec4 p;
    std::vector<CVec4> line0;

    rot.setNRotation({0,1,0,0},-TO_RADIANS(90));
    angleIncrement = (2.0f*M_PI)/float(n);
    rad = 0;

    line0.push_back(CVec4(0,0,0,1));
    //compute circle on xy-plane and rotate it into the yz-plane
    for(I = 0; I <= n; I++){
        p = { radius*cosf(rad), radius*sinf(rad),0.0f,1.0f };
        //p = rot*p;
        line0.push_back(p);
        rad+=angleIncrement;
    }

    //submit and index vertex data to the final buffer
    indexCount = 0;


    for(I = 0; I < line0.size()-1; I++){
        vB->push_back(line0[I]);
        vB->push_back(line0[I+1]);

        iB->push_back(0);
        iB->push_back(indexCount+0);
        iB->push_back(indexCount+1);
        indexCount+=2;
    }

    for(I = 0; I < vB->size(); I++){
        u = MTHUTL::map(vB[0][I].y(),-radius,radius,0,1);
        v = MTHUTL::map(vB[0][I].z(),-radius,radius,0,1);
        uvB->push_back(CVec4(u,v,0,0));
    }

    //hacky attempt at computing normals (causes facets)
    nB->resize( vB->size() );
    CVec4 d0,d1,norm,a,b,c;
    int ia,ib,ic;
    std::vector<int>   & iBr = *iB;
    std::vector<CVec4> & vBr = *vB;
    std::vector<CVec4> & nBr = *nB;

    for(I = 0; I < iBr.size() ; I+= 3){
        ia = iBr[I+0];
        ib = iBr[I+1];
        ic = iBr[I+2];
        a = vBr[ia];
        b = vBr[ib];
        c = vBr[ic];
        d0 = b-a;
        d1 = c-b;
        norm=d0^d1;//d0 cross d1
        norm *= 1.0f/sqrtf(norm*norm)*normFactor;
        //This causes facets. Interpolation is basically disabled because all normals are the same
        nBr[ia] = norm;
        nBr[ib] = norm;
        nBr[ic] = norm;
    }

}

void ShapeUtil::computeNormals(std::vector<int> &iBr, std::vector<CVec4> &vBr, std::vector<CVec4> &nBr){
    //fast but causes facets
    CVec4 d0,d1,norm,a,b,c;
    int ia,ib,ic;
    for(int i = 0; i < int(iBr.size()) ; i+= 3){
        ia = iBr[i+0];
        ib = iBr[i+1];
        ic = iBr[i+2];
        a = vBr[ia];
        b = vBr[ib];
        c = vBr[ic];
        d0 = b-a;
        d1 = c-b;
        norm=d0^d1;//d0 cross d1
        norm *= 1.0f/sqrtf(norm*norm);
        nBr[ia] = norm;
        nBr[ib] = norm;
        nBr[ic] = norm;
    }
}
