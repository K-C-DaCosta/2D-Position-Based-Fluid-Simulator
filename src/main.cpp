/*
Name: Khadeem DaCosta
Description:
    This is an implementation of the paper "position based fluids" in openCL.
*/

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <exception>
#include <thread>
#include <math.h>


#include "../include/general_io_util.h"
#include "../include/shader.h"
#include "../include/matrix.h"
#include "../include/shader_program.h"
#include "../include/vertex_array_object.h"
#include "../include/matrix_stack.h"
#include "../include/shape_util.h"
#include "../include/softbody.h"
#include "../include/math_util.h"
#include "../include/signed_distance_field.h"
#include "../include/texture.h"
#include "../include/sampler.h"
#include "../include/opencl_util.h"
#include "../include/stack.h"
#include "../include/glfw_util.h"

//PBF CONSTANTS
#define _H 20.0f
#define _REST_DENSITY 0.001f
#define _EPSILON 1.0f
#define _XSPH 0.01f
#define _SCORR_K -0.1f

//TABLE CONSTANTS
#define _TABLE_SIZE 20000
#define _MAX_PROBE 3
#define _MAX_CHAIN 7

float poly6(float rSqrd, float h);
CVec4 spikey(CVec4 r, float h);

const float INV_REST_DENSITY = 1.0f/_REST_DENSITY;
const float POLY6_COEF =  315.0f/(64.0f*M_PI*_H*_H*_H*_H*_H*_H*_H*_H*_H);
const float SPIKEY_COEF = -45.0f/(M_PI*_H*_H*_H*_H*_H*_H);
const float SCORR_INV_DENOM = 1.0f/poly6( powf(0.6f*_H,2.0f),_H);



typedef struct LineRec_{
    Vec4 p0;
    Vec4 p1;
}LineRec;

typedef struct CellIndex_{
    uint16_t i;
    uint16_t j;
    uint16_t k;
}CellIndex;

typedef struct Slot_{
    uint16_t recsFilled;
    uint16_t chainList[_MAX_CHAIN];
}Slot;

//64
typedef struct ParticleCL_{
    CVec4 p0;
    CVec4 p1;
    CVec4 vel;
    CVec4 DP;
    float density;
    float lambda;
    float constraint;
    float gradSquared;
}ParticleCL;

//A spaial hashing implementation (WIP)
class PHashTable{
public:

    PHashTable(float h){
        cellSize =10;
        invCellSize = 1.0f/cellSize;
    }

    void insert(ParticleCL * p, uint16_t pindex,std::vector<ParticleCL> & buffer){
        uint32_t key = resolvedHash(p,buffer);
        Slot & slot = table[key];
        if(slot.recsFilled < _MAX_CHAIN){
            slot.chainList[slot.recsFilled++] = pindex;
        }
    }

    void checkSlotIntegrety(std::vector<ParticleCL> & buffer){
        CellIndex slotIndex;
        CellIndex particleIndex;
        for(int K = 0; K < _TABLE_SIZE; K++){
            Slot& slot = table[K];
            if( slot.recsFilled > 0){
                getCellIndex((Vec4*)&buffer[slot.chainList[0]].p1,&slotIndex);
                for(int I = 1; I < slot.recsFilled; I++){
                    getCellIndex((Vec4*)&buffer[slot.chainList[I]].p1,&particleIndex);
                    if(particleIndex.i != slotIndex.i ||
                       particleIndex.j != slotIndex.j ||
                       particleIndex.k != slotIndex.k ){

                       std::cout << "The hashtable insertion algorithm is broken" << std::endl;
                    }
                }
            }
        }
    }

    uint32_t resolvedHash(ParticleCL *P,std::vector<ParticleCL> & buffer){
        /*This function does collision handling
         *to make sure no two cells in the virtual grid
         * map to the same slot in the hash table */

        uint32_t key = hash((Vec4*)&P->p1);
        Slot* slot = table+key;
        uint32_t count = 1;

        while(count < _MAX_PROBE && hasCollision(P,slot,buffer)  ){
            key = (key+count*count)%_TABLE_SIZE;
            count++;
            slot = table+key;
        }
        return key;
    }

    uint32_t resolvedNeighbourHash(int ni,int nj, std::vector<ParticleCL> &buffer){
        uint32_t key;
        Slot* slot;
        uint32_t count;

        count = 1;
        key = hash(ni,nj,0);
        slot = table+key;


        while(count < _MAX_PROBE &&  hasCollision(ni,nj,0,slot,buffer) ) {
            key = (key+count*count)%_TABLE_SIZE;
            count++;
            slot = table+key;
        }

        return key;
    }


    bool hasCollision(ParticleCL *srcParticle, Slot * destSlot, std::vector<ParticleCL> & buffer){
        CellIndex destCell;
        CellIndex srcCell;


        if(destSlot->recsFilled == 0){
            return false;
        }

        ParticleCL & destParticle = buffer[destSlot->chainList[0]];

        getCellIndex((Vec4*)&destParticle.p1,&destCell);
        getCellIndex((Vec4*)&srcParticle->p1,&srcCell);

        if( (destCell.i==srcCell.i) && (destCell.j == srcCell.j) && (destCell.k == srcCell.k) ){
            return false;
        }

        return true;
    }
    bool hasCollision(uint16_t ni,uint16_t nj,uint16_t nk, Slot * destSlot, std::vector<ParticleCL> & buffer){
        CellIndex destCell = {0,0,0};
        CellIndex srcCell = {ni,nj,nk};

        if(destSlot->recsFilled == 0){
            return false;
        }

        ParticleCL & destParticle = buffer[destSlot->chainList[0]];
        getCellIndex((Vec4*)&destParticle.p1,&destCell);

        if( (destCell.i==srcCell.i) && (destCell.j == srcCell.j) && (destCell.k == srcCell.k) ){
            return false;
        }

        return true;
    }

    void getCellIndex(Vec4 * v,CellIndex * ci){
        ci->i = v->x*invCellSize;
        ci->j = v->y*invCellSize;
        ci->k = v->z*invCellSize;
    }

    uint32_t hash(Vec4 * v){
        uint32_t i = v->x*invCellSize;
        uint32_t j = v->y*invCellSize;
        uint32_t k = v->z*invCellSize;
        const uint32_t p1 = 73856093;
        const uint32_t p2 = 19349663;
        const uint32_t p3 = 83492791;
        return ((i*p1)^(j*p2)^(k*p3))%_TABLE_SIZE;
    }

    uint32_t hash(uint32_t i, uint32_t j, uint32_t k){
        const uint32_t p1 = 73856093;
        const uint32_t p2 = 19349663;
        const uint32_t p3 = 83492791;
        return ((i*p1)^(j*p2)^(k*p3))%_TABLE_SIZE;
    }

    Slot& operator [](uint32_t k){
        return table[k];
    }
    void clearTable(){
         memset(table,0,sizeof(Slot)*_TABLE_SIZE);
    }
public:
    float cellSize;
    float invCellSize;
private:
    Slot table[_TABLE_SIZE];
};

//GLOBALS
MatrixStack matStack;
OpenCLInfoBlock oclInfoBlock;
GLFWInfoBlock glfwGlobalInfo;
std::vector<LineRec> lineList;

void print_particle(ParticleCL * p);

//branchless implementation of poly6 kernel
float poly6(float rSqrd, float h){
    float BASE = (h*h -rSqrd);
    float poly6result = POLY6_COEF*(BASE*BASE*BASE);
    float cmp = -BASE;
    uint32_t poly6int = *(uint32_t*)&poly6result;
    uint32_t mask = ((*(uint32_t*)&cmp)>>31)*0xffffffff; // MSB is 0  if rSqrd>HSQRD else is 1
    uint32_t result = mask&poly6int;
    return *(float*)&result;
}

//branchless implementation of spikey kernel
CVec4 spikey(CVec4 r, float h){
    float rmag = r.mag();
    float fcmp0 = rmag - 0.00125f ; //MSB 1 if rmag < C
    float fcmp1 =  h-rmag; //MSB 1 if rmag > h
    float fspikey = SPIKEY_COEF*fcmp1*fcmp1/rmag;
    uint32_t mask0 = ((*(uint32_t*)&fcmp0)>>31)*0xffffffff;
    uint32_t mask1 = ((*(uint32_t*)&fcmp1)>>31)*0xffffffff;
    uint32_t ispikey = *(uint32_t*)&fspikey;
    uint32_t iresult = (~(mask0|mask1))&ispikey;   //  (~a.~b)  ->  ~(a+b)
    float scaleFactor = *(float*)&iresult;
    r*=scaleFactor;
    return r;
}

void computePBF(std::vector<ParticleCL> & particleList, PHashTable & htable){
    uint16_t K;

    int32_t  ci,cj, // cell index
             I,J,
             ni,nj;

    uint32_t nkey,M,
             count;

    float poly6Accum,
          spikeyAccum,
          Scorr,
          countAvg = 0,
          globalAvg = 0.0f;

    CVec4 disp,
          DPAccum;
    Slot *slot;

    static double t0 = glfwGetTime();


    for(M = 0; M < particleList.size(); M++){
        ParticleCL & Pi = particleList[M];

        ci = Pi.p1.x()*htable.invCellSize;
        cj = Pi.p1.y()*htable.invCellSize;

        poly6Accum = 0.0f;
        spikeyAccum = 0.0f;
        countAvg =0.0f;

        for(I = 0; I < 3; I++){
            for(J = 0; J < 3; J++){
                ni = (ci-1)+I;
                nj = (cj-1)+J;
                ni = (ni < 0)?0 : ni;
                nj = (nj < 0)?0 : nj;
                nkey = htable.resolvedNeighbourHash(ni,nj,particleList);
                slot = &htable[nkey];
                count = 0;
                for(K = 0; K < slot->recsFilled; K++){
                    //summation starts here
                    ParticleCL & Pj = particleList[slot->chainList[K]];
                    disp = Pi.p1 - Pj.p1;
                    poly6Accum+= poly6(disp.magSquared(),_H);
                    spikeyAccum+= (spikey(disp,_H)*INV_REST_DENSITY).magSquared();

                    lineList.push_back({Pi.p1.x() ,Pi.p1.y() ,0.0f,1.0f,
                                        Pj.p1.x() ,Pj.p1.y() ,0.0f,1.0f,});

                    count++;
                }


                countAvg+=count;
            }
        }
        globalAvg += countAvg;
        Pi.density = poly6Accum;
        Pi.constraint = Pi.density*INV_REST_DENSITY -1.0f;
        Pi.lambda = -(Pi.constraint)/(spikeyAccum +_EPSILON);
    }


    if(glfwGetTime() - t0 > 0.2){
        t0 = glfwGetTime();
        printf("Average count = %.3f  p num = %ld\n",globalAvg/float(particleList.size()),particleList.size() );
    }

    for(ParticleCL & Pi: particleList){
        ci = Pi.p1.x()*htable.invCellSize;
        cj = Pi.p1.y()*htable.invCellSize;
        DPAccum = {0,0,0,0};

        for(I = 0; I < 3; I++){
            for(J = 0; J < 3; J++){
                ni = (ci-1)+I;
                nj = (cj-1)+J;
                ni = (ni < 0)?0 : ni;
                nj = (nj < 0)?0 : nj;
                nkey = htable.resolvedNeighbourHash(ni,nj,particleList);
                slot = &htable[nkey];
                for(K = 0; K < slot->recsFilled; K++){
                    //summation starts here
                    ParticleCL & Pj = particleList[slot->chainList[K]];
                    disp = Pi.p1 - Pj.p1;
                    Scorr = powf(poly6(disp.magSquared(),_H)*SCORR_INV_DENOM,4.0f)*_SCORR_K;
                    DPAccum+= spikey(disp,_H)*(Pi.lambda + Pj.lambda + Scorr);
                }
            }
        }
        DPAccum*=INV_REST_DENSITY;
        Pi.DP = DPAccum;
    }
}

void addParticles(int x0,int y0, std::vector<ParticleCL> & particleBuffer){
    for(int y = 0; y <130 ; y+=10){
        for(int x = 0; x < 130; x+=10){
            ParticleCL p ;
            CVec4 randPos = { float(x+x0) ,float(y+y0), 0.0f, 1.0f};
            p.p0 = randPos;//randPos;
            p.p1 = randPos;
            p.vel = {0,0,0,0};
            p.DP =  {0,0,0,0};
            p.density = 0.0f;
            p.lambda  = 0.0f;
            particleBuffer.push_back(p);
        }
    }
}


int main(void){
    GLFWwindow *rootWindow = NULL;
    std::vector<ParticleCL> particleBuffer;


    //Vec4 test[2] = ;
    lineList.push_back({20,20,0,1,100,100,0,1});

    PHashTable pht(_H);

    glfwGlobalInfo.mousePos.x = 800;


    GLFWU::setupGLFW(&glfwGlobalInfo,"Position Based Fluids (GPGPU) Demo - Khadeem DaCosta",800,600);


    /* Loop until the user closes the window */
    glClearColor(0,0.0,0,0);
    glEnable(GL_DEPTH_TEST);

    ShaderProgram prog("./shaders/generic_vert.glsl","./shaders/generic_frag.glsl");
    prog.useProgram();

    matStack.getPerspMat().setOrthographic(0,800,-400,400,0,600);
    //matStack.getPerspMat().setPerspective(10,800.0/600.0,1.0f,300.0f);


    GLuint blockData,ubIndex = glGetUniformBlockIndex(prog.getID(),"MatrixBlock");OPENGL_ERROR();
    //create VBO with target GL_UNIFORM_BUFFER containing all matrix data
    glGenBuffers(1,&blockData);OPENGL_ERROR();
    glBindBuffer(GL_UNIFORM_BUFFER,blockData);OPENGL_ERROR();
    glBufferData(GL_UNIFORM_BUFFER,sizeof(CMAT44)*4,matStack.getKeyMatricies(),GL_DYNAMIC_DRAW);OPENGL_ERROR();
    glBindBufferBase(GL_UNIFORM_BUFFER,ubIndex,blockData);OPENGL_ERROR();



    GLuint vaoLines, vboLines;
    glGenVertexArrays(1,&vaoLines);
    glBindVertexArray(vaoLines);
    glGenBuffers(1,&vboLines);
    glBindBuffer(GL_ARRAY_BUFFER,vboLines);
    glBufferData(GL_ARRAY_BUFFER,sizeof(LineRec)*lineList.size(),lineList.data(),GL_DYNAMIC_DRAW);
    glVertexAttribPointer(LAYOUT_VERTEX,4,GL_FLOAT,GL_FALSE,0,(void*)0);
    glEnableVertexAttribArray(LAYOUT_VERTEX);





    GLuint vao,vbo;
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);
    glGenBuffers(1,&vbo);

    glBindBuffer(GL_ARRAY_BUFFER,vbo);

    glBufferData(GL_ARRAY_BUFFER,sizeof(ParticleCL)*particleBuffer.size(),particleBuffer.data(),GL_DYNAMIC_DRAW);OPENGL_ERROR();
    glVertexAttribPointer(LAYOUT_VERTEX,4,GL_FLOAT,GL_FALSE,64,(void*)16);OPENGL_ERROR();
    glEnableVertexAttribArray(LAYOUT_VERTEX);OPENGL_ERROR();

    std::cout << "Particle size: " << particleBuffer.size()  << std::endl;

    double t0 = glfwGetTime();

    glPointSize(2);
    rootWindow = glfwGlobalInfo.windowBlocks.getHead()->data.window;

    const int ITERATIONS = 4;
    const float STEPS = 1.0f;
    const float DT = (1.0f/STEPS);
    float time = 0;

    while (!glfwWindowShouldClose(rootWindow) ){
         int tw,th;
         glfwGetWindowSize(rootWindow,&tw,&th);
         matStack.getPerspMat().setOrthographic(0,tw,-400,400,0,th);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glfwPollEvents();

        if(glfwGlobalInfo.mouseBuffer[GLFW_MOUSE_BUTTON_LEFT]){
            Vec2 & mouse = glfwGlobalInfo.mousePos;
            addParticles(mouse.x,mouse.y,particleBuffer);
        }

        for(int K = 0; K < int(STEPS); K++){

            for(ParticleCL & P : particleBuffer){
                P.vel+={0,0.05,0,0};
                P.p1 = P.vel*DT + P.p0;// p1 = vel*dt + p0
            }

            pht.clearTable();
            for(uint32_t I  = 0; I < particleBuffer.size(); I++){
                ParticleCL & P  = particleBuffer[I];
                pht.insert(&P,I,particleBuffer);
            }
            //pht.checkSlotIntegrety(particleBuffer);

            //update p1 by DP
            for(int I = 0; I < ITERATIONS;I++){
                lineList.clear();
                computePBF(particleBuffer,pht);
                for(ParticleCL & P : particleBuffer){
                    P.p1+=P.DP;
                    P.p1.sx(CLAMP(P.p1.x(),50.0f,800));
                    P.p1.sy(CLAMP(P.p1.y(),50.0f,590-3.0f*sin(P.p1.x()*0.1+time)));
                }
            }

            //particleBuffer[0].p0 = particleBuffer[0].p1 = {glfwGlobalInfo.mousePos.x,glfwGlobalInfo.mousePos.y,0,1.0f};

            //update velocity
            for(ParticleCL & P: particleBuffer){
                P.vel = (P.p1-P.p0)*STEPS;
                P.p0 = P.p1;
            }
        }
        time+=0.1f;

        prog.useProgram();
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(LAYOUT_VERTEX,4,GL_FLOAT,GL_FALSE,80,(void*)16);
        glDrawArrays(GL_POINTS,0,particleBuffer.size());
        glUseProgram(0);

        if(glfwGlobalInfo.keyBuffer[GLFW_KEY_SPACE]){
            prog.useProgram();
            glBindVertexArray(vaoLines);
            glBindBuffer(GL_ARRAY_BUFFER,vboLines);
            glVertexAttribPointer(LAYOUT_VERTEX,4,GL_FLOAT,GL_FALSE,0,0);
            glDrawArrays(GL_LINES,0,lineList.size()*2);
            glUseProgram(0);
        }


        //update buffer objects

        glBindBuffer(GL_UNIFORM_BUFFER,blockData);OPENGL_ERROR();
        glBufferData(GL_UNIFORM_BUFFER,sizeof(CMAT44)*4,matStack.getKeyMatricies(),GL_DYNAMIC_DRAW);OPENGL_ERROR();

        //upload particle info to opengl
        if( (glfwGetTime() - t0)  > 0.03){
            t0 = glfwGetTime();

            glBindBuffer(GL_ARRAY_BUFFER,vbo);
            glBufferData(GL_ARRAY_BUFFER,
                         sizeof(ParticleCL)*particleBuffer.size(),
                         particleBuffer.data(),
                         GL_DYNAMIC_DRAW);OPENGL_ERROR();

//            glVertexAttribPointer(LAYOUT_VERTEX,4,GL_FLOAT,GL_FALSE,80,(void*)16);OPENGL_ERROR();



            glBindBuffer(GL_ARRAY_BUFFER,vboLines);
            glBufferData(GL_ARRAY_BUFFER,sizeof(LineRec)*lineList.size(),
                         lineList.data(),
                         GL_DYNAMIC_DRAW);
//            glVertexAttribPointer(LAYOUT_VERTEX,4,GL_FLOAT,GL_FALSE,0,(void*)0);OPENGL_ERROR();
        }
        lineList.clear();
        GLFWU::resolveEvents(matStack);
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
        glfwSwapBuffers(rootWindow);

    }

    clReleaseContext(oclInfoBlock.context);
    glfwTerminate();
    return 0;
}



//float poly6(float rSqrd,float h){//r is the distance squared
//    float BASE = (h*h-rSqrd);
//    float poly6result = POLY6_COEF*(BASE*BASE*BASE);
//    return (rSqrd > h*h)? 0.0f: poly6result;
//}

//CVec4 spikey(CVec4 r, float h){
//    float rmag = r.normalize();
//    if(rmag < 0.00125f || rmag  > h){
//       return {0,0,0,0};
//    }
//    float scaleFactor = SPIKEY_COEF*powf(h-rmag,2.0f);
//    r*=scaleFactor;
//    return r;
//}

//void computeNeighbours(std::vector<ParticleCL> &particleList, std::vector<Neighbours> & neighbourList){
//    uint32_t K,F;
//    CVec4 disp;
//    for(K = 0; K < particleList.size(); K++){
//        ParticleCL & P  = particleList[K];
//        Neighbours & PNL = neighbourList[K];
//        for(F = 0; F < particleList.size(); F++){
//            ParticleCL & NP = particleList[F];
//            if(K!=F){
//               disp = P.p1-NP.p1;
//               if(disp*disp < (_H*_H) && PNL.recsFilled < 64){
//                    PNL.buckets[PNL.recsFilled] = F;
//                    PNL.recsFilled++;
//               }
//            }
//        }
//    }
//}

//void computeXSPH(std::vector<ParticleCL> & pl, std::vector<Neighbours> &nl){
//    uint32_t K,F;
//    CVec4 vij,disp;
//    CVec4 vectorAccum;
//    for(K = 0; K < pl.size(); K++){
//        ParticleCL & Pi = pl[K];
//        Neighbours & PiNL = nl[K];
//        vectorAccum = {0,0,0,0};
//        for(F = 0; F < PiNL.recsFilled;F++){
//            ParticleCL & Pj = pl[PiNL.buckets[F]];
//            disp = Pi.p1 - Pj.p1;
//            vij = Pi.vel-Pj.vel;
//            vectorAccum += vij*poly6(disp*disp,_H);
//        }
//        Pi.vel+=vectorAccum*0.01f;
//    }
//}

//void computePBF(std::vector<ParticleCL> & pl, std::vector<Neighbours> &nl){
//    uint32_t K,F;
//    CVec4 disp;

//    float scalarAccum;
//    CVec4 vectorAccum;
//    float gradAccum;

//    for(K = 0; K < pl.size(); K++){
//        ParticleCL & Pi = pl[K];
//        Neighbours & PiNL = nl[K];
//        //compute density and C_i
//        scalarAccum = 0.0f;
//        gradAccum = 0.0f;
//        for(F = 0; F < PiNL.recsFilled;F++){
//            ParticleCL & Pj = pl[PiNL.buckets[F]];
//            disp = Pi.p1-Pj.p1;
//            scalarAccum += poly6(disp*disp,_H);
//            gradAccum+= (spikey(disp,_H)*INV_REST_DENSITY).magSquared();
//        }
//        Pi.density = scalarAccum;
//        Pi.constraint = Pi.density*INV_REST_DENSITY -1.0f;
//        Pi.lambda = -Pi.constraint/(gradAccum+_EPSILON);
//    }

//    //compute DP
//    for(K = 0; K < pl.size(); K++){
//        ParticleCL & Pi = pl[K];
//        Neighbours & PiNL = nl[K];
//        vectorAccum = {0,0,0,0};
//        for(F = 0; F < PiNL.recsFilled;F++){
//            ParticleCL & Pj = pl[PiNL.buckets[F]];
//            disp = Pi.p1-Pj.p1;
//            float Scorr = powf(poly6(disp.magSquared(),_H)*SCORR_INV_DENOM,4.0f)*_SCORR_K;
//            vectorAccum +=  spikey(disp,_H)*(Pi.lambda+Pj.lambda+Scorr);
//        }
//        Pi.DP = vectorAccum*INV_REST_DENSITY;
//    }
//}
