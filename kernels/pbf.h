#define POLY6_COEF 1.566681471060844711474949545698188251276712137757618167359f
#define SPIKEY_G_COEF -14.32394488f
#define _H 4.0f
#define EPSILON 0.00125f
#define REST_DENSITY 1.0f
#define GCOLS 512
#define GROWS 512

typedef struct Particle_{
    float4 x0;//position x0
    float4 x1;//position x1
    float4 vel;//velocity of particle
    float density;//density of particle
    float lambda;//read the paper "position based dynamics"
    float constraint;
    float padding[1];
}Particle;

typedef struct SphGrid_{
    unsigned short recsFilled;
    unsigned short buckets[7];
}SphGrid;

__constant const float POLY6_H = 1.0f/(_H*_H*_H*_H*_H*_H*_H*_H*_H);
__constant const float SPIKEY_H = 1.0f/(_H*_H*_H*_H*_H*_H);


float4 SpikeyGradient(float4 disp){
    float r = length(disp);
    return (r < EPSILON)? (0.0f,0.0f,0.0f,0.0f) : normalize(disp)*(SPIKEY_G_COEF*SPIKEY_H*pow(_H-r,2.0f));
}

float Poly6Kernel(float4 vr){
    float r = length(vr);
    return (r > _H) ? 0.0f : POLY6_COEF*POLY6_H*pow((_H*_H -r*r),3.0f);
}

float Poly6KernelAlt(float r){
    return (r > _H) ? 0.0f : POLY6_COEF*POLY6_H*pow((_H*_H -r*r),3.0f);
}





///OLD MAIN.CPP code

///*
//Name: Khadeem DaCosta
//Description:
//    This is an implementation of the paper "position based fluids" in openCL.
//*/

//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <iostream>
//#include <unistd.h>
//#include <stdint.h>
//#include <exception>
//#include <thread>
//#include <math.h>


//#include "../include/general_io_util.h"
//#include "../include/shader.h"
//#include "../include/matrix.h"
//#include "../include/shader_program.h"
//#include "../include/vertex_array_object.h"
//#include "../include/matrix_stack.h"
//#include "../include/shape_util.h"
//#include "../include/softbody.h"
//#include "../include/math_util.h"
//#include "../include/signed_distance_field.h"
//#include "../include/texture.h"
//#include "../include/sampler.h"
//#include "../include/opencl_util.h"
//#include "../include/stack.h"
//#include "../include/glfw_util.h"

//#define _H 4.5f
//#define _REST_DENSITY 1.0f
//#define _EPSILON 1.0f

////64
//typedef struct ParticleCL_{
//    Vec4 p0;
//    Vec4 p1;
//    Vec4 vel;
//    Vec4 DP;
//    float density;
//    float lambda;
//    float constraint;
//    float gradSquared;
//}ParticleCL;

////16
//typedef struct  SphGridCL_{
//    unsigned short recsFilled;
//    unsigned short buckets[7];
//}SphGridCL;

//// I create a matrix stack, like the one in fixed function version of OpenGL.
//MatrixStack matStack;
//OpenCLInfoBlock oclInfoBlock;
//GLFWInfoBlock glfwGlobalInfo;

//void print_particle(ParticleCL * p);


//float poly6(float r,float h){
//    float BASE = (h*h - r*r);
//    float poly6result = (315.0f/(64.0f*M_PI))*powf(h,-9.0f)*BASE*BASE*BASE;
//    return (r > h) ? 0.0f : poly6result;
//}

//Vec4 spikey(Vec4 r,float rmag, float h){
//    Vec4 spikeyResult = {0,0,0,0};
//    float scaleFactor = 1.0f;

//    if(rmag < 0.000125f){
//       return spikeyResult;
//    }

//    scaleFactor = (-45.0f/(M_PI*powf(h,6.0f)))*powf((h-rmag),2.0f);
//    scaleFactor/=rmag;
//    v4IS(&r,scaleFactor);
//    spikeyResult = r;

//    return spikeyResult;
//}

//void computeAttributes(std::vector<ParticleCL> & particleList, std::vector<SphGridCL> & grid){
//    int gx,gy;
//    uint32_t K,F;
//    int min_x,min_y,max_x,max_y;

//    float r,gradSquaredSum,poly6Sum;
//    Vec4 spikeySum = {0,0,0,0};
//    Vec4 disp,spikeyGrad;


//    //compute density, constraint and gradient^2 for P_k
//    for(K = 0; K < particleList.size(); K++){
//        ParticleCL & P = particleList[K];

//        //clear relevant accumulators
//        spikeySum = {0,0,0,0};
//        poly6Sum = 0;

//        //calculate grid iteration bounds
//        gx = P.p1.x/5.0f;
//        gy = P.p1.y/5.0f;

//        min_x = gx -1;
//        min_y = gy -1;
//        max_x = gx +1;
//        max_y = gy +1;

//        min_x = CLAMP(min_x,0,512);
//        min_y = CLAMP(min_y,0,512);
//        max_x = CLAMP(max_x,0,512);
//        max_y = CLAMP(max_y,0,512);

//        //check 9 neighbour cells of the grid
//        for(gy = min_y; gy < max_y; gy++){
//            for(gx = min_x; gx < max_x; gx++){
//                SphGridCL & cell = grid[GI(gx,gy,512)];
//                for(F = 0;  F < cell.recsFilled; F++){
//                    unsigned short npIndex = cell.buckets[F];
//                    ParticleCL & NP =particleList[npIndex];
//                    if(K != npIndex){
//                        v4Su(&disp,&NP.p1,&P.p1);
//                        r = sqrt(v4Dot(&disp,&disp));
//                        poly6Sum+=poly6(r,_H);
//                        spikeyGrad = spikey(disp,r,_H);
//                        v4IA(&spikeySum,&spikeyGrad);
//                    }
//                }
//            }
//        }
//        v4IS(&spikeySum,1.0f/_REST_DENSITY);
//        P.density = poly6Sum;
//        P.constraint = P.density/_REST_DENSITY - 1.0f;
//        P.gradSquared = v4Dot(&spikeySum,&spikeySum);
//    }

//    //compute lamba for each particle in the constrains
//    for(K = 0; K < particleList.size(); K++){
//        ParticleCL & P = particleList[K];
//        //clear relevant accumulators
//        gradSquaredSum = 0.0f;

//        //compute grid iteration bounds
//        gx = P.p1.x/5.0f;
//        gy = P.p1.y/5.0f;

//        min_x = gx -1;
//        min_y = gy -1;
//        max_x = gx +1;
//        max_y = gy +1;

//        min_x = CLAMP(min_x,0,512);
//        min_y = CLAMP(min_y,0,512);
//        max_x = CLAMP(max_x,0,512);
//        max_y = CLAMP(max_y,0,512);

//        //check 9 neighbour cells of the grid
//        for(gy = min_y; gy < max_y; gy++){
//            for(gx = min_x; gx < max_x; gx++){
//                SphGridCL & cell = grid[GI(gx,gy,512)];
//                for(F = 0;  F < cell.recsFilled; F++){
//                    unsigned short npIndex = cell.buckets[F];
//                    ParticleCL & NP =particleList[npIndex];
//                    if(K != npIndex){
//                        gradSquaredSum+= NP.gradSquared;
//                    }
//                }
//            }
//        }
//        P.lambda = -P.constraint/(gradSquaredSum +_EPSILON);
//    }

//    for(K = 0; K < particleList.size(); K++){
//        ParticleCL & P = particleList[K];

//        gx = P.p1.x/5.0f;
//        gy = P.p1.y/5.0f;

//        min_x = gx -1;
//        min_y = gy -1;
//        max_x = gx +1;
//        max_y = gy +1;

//        min_x = CLAMP(min_x,0,512);
//        min_y = CLAMP(min_y,0,512);
//        max_x = CLAMP(max_x,0,512);
//        max_y = CLAMP(max_y,0,512);

//        P.DP = {0,0,0,0};

//        //check 9 neighbour cells of the grid
//        for(gy = min_y; gy < max_y; gy++){
//            for(gx = min_x; gx < max_x; gx++){
//                SphGridCL & cell = grid[GI(gx,gy,512)];
//                for(F = 0;  F < cell.recsFilled; F++){
//                    unsigned short npIndex = cell.buckets[F];
//                    ParticleCL & NP =particleList[npIndex];
//                    if(K != npIndex){
//                        v4Su(&disp,&P.p1,&NP.p1);
//                        r = v4Mag(&disp);
//                        spikeyGrad = spikey(disp,r,_H);
//                        v4IAS(&P.DP,&spikeyGrad,P.lambda+NP.lambda);
//                    }
//                }
//            }
//        }
//        v4IS(&P.DP,1.0f/_REST_DENSITY);
//    }
//}


//int main(void){
//    GLFWwindow *rootWindow = NULL;
//    OpenCLBuffer<ParticleCL> particleBuffer(CL_MEM_COPY_HOST_PTR);
//    OpenCLBuffer<SphGridCL>  gridBuffer(CL_MEM_COPY_HOST_PTR);

//    std::string pbfSrc;
//    cl_program pbfProg = NULL;
//    cl_kernel pbfAttributesKern = NULL;
//    cl_kernel pbfComputeForcesKern = NULL;
//    cl_kernel pbfAdvanceKern = NULL;


//    cl_command_queue queue = NULL;

//    gridBuffer.resize(512*512);
//    gridBuffer.zero();
//    particleBuffer.resize(64);

//    int x = 20;
//    int y = 20;
//    for(ParticleCL & p: particleBuffer.getHostBuffer()){
//        Vec4 randPos = { float ( x ) ,float(y), 0.0f, 1.0f};
//        p.p0 = randPos;//randPos;
//        p.p1 = randPos;
//        p.vel = {0,0,0,0};
//        p.DP = {0,0,0,0};
//        p.density = 0.0f;
//        p.lambda = 0.0f;

//        if(x > 300){
//            y+=5;
//            x=20;
//        }
//        x+=5;
//    }
////    particleBuffer[0].p1 = particleBuffer[0].p0 = {60,300,0,1};
////    particleBuffer[1].p1 = particleBuffer[1].p0 = {400,300,0,1};



//    try{
////        CLU::setupOCL(&oclInfoBlock);
//        GLFWU::setupGLFW(&glfwGlobalInfo,"Position Based Fluids (GPGPU) Demo - Khadeem DaCosta",800,600);
//        printf("\n\n\n\n\n\n");
////        particleBuffer.createServerBuffer(oclInfoBlock);
////        gridBuffer.createServerBuffer(oclInfoBlock);
////        queue = CLU::createCommandQueue(oclInfoBlock);

////        pbfSrc = GIOUtil::loadTextFile("./kernels/pbf.cl");
////        pbfProg = CLU::createProgram(oclInfoBlock,pbfSrc);
////        CLU::buildProgram(oclInfoBlock,pbfProg);
////        pbfAttributesKern = CLU::createKernel(oclInfoBlock,pbfProg,"pbfComputeAttributes");
////        pbfComputeForcesKern = CLU::createKernel(oclInfoBlock,pbfProg,"pbfComputeForces");
////        pbfAdvanceKern = CLU::createKernel(oclInfoBlock,pbfProg,"pbfAdvance");
//    }catch(char const *  err){
//        std::cout << err << std::endl;
//        return 0;
//    }catch(OCLException err){
//        printf("%s\n",err.what());
//        std::cout << CLU::getProgramBuildInfo(pbfProg,oclInfoBlock.deviceIds[0],CL_PROGRAM_BUILD_LOG) << std::endl;
//        return 0;
//    }

////    //setting kernel arguments
////    particleBuffer.setKernelArg(pbfAttributesKern,0);
////    gridBuffer.setKernelArg(pbfAttributesKern,1);

////    particleBuffer.setKernelArg(pbfComputeForcesKern,0);
////    gridBuffer.setKernelArg(pbfComputeForcesKern,1);

////    particleBuffer.setKernelArg(pbfAdvanceKern,0);

////    //Tell opencl the datasize im working with
////    size_t globalWorkSize [] = {particleBuffer.size()};
////    size_t localWorkSize [] = { 64 };




//    /* Loop until the user closes the window */
//    glClearColor(0,0.0,0,0);
//    glEnable(GL_DEPTH_TEST);

//    ShaderProgram prog("./shaders/generic_vert.glsl","./shaders/generic_frag.glsl");
//    prog.useProgram();

//    matStack.getPerspMat().setOrthographic(0,800,-400,400,0,600);
//    //matStack.getPerspMat().setPerspective(10,800.0/600.0,1.0f,300.0f);


//    GLuint blockData,ubIndex = glGetUniformBlockIndex(prog.getID(),"MatrixBlock");OPENGL_ERROR();
//    //create VBO with target GL_UNIFORM_BUFFER containing all matrix data
//    glGenBuffers(1,&blockData);OPENGL_ERROR();
//    glBindBuffer(GL_UNIFORM_BUFFER,blockData);OPENGL_ERROR();
//    glBufferData(GL_UNIFORM_BUFFER,sizeof(CMAT44)*4,matStack.getKeyMatricies(),GL_DYNAMIC_DRAW);OPENGL_ERROR();
//    glBindBufferBase(GL_UNIFORM_BUFFER,ubIndex,blockData);OPENGL_ERROR();



//    //VAO* sphere = new VAO(VAO_CUBE|VAO_DELETE_DATA,GL_STATIC_DRAW);


//    GLuint vao,vbo;
//    glGenVertexArrays(1,&vao);
//    glBindVertexArray(vao);
//    glGenBuffers(1,&vbo);

//    glBindBuffer(GL_ARRAY_BUFFER,vbo);

//    glBufferData(GL_ARRAY_BUFFER,
//                 sizeof(ParticleCL)*particleBuffer.size(),
//                 particleBuffer.getHostBuffer().data(),
//                 GL_DYNAMIC_DRAW);OPENGL_ERROR();

//    glVertexAttribPointer(LAYOUT_VERTEX,4,GL_FLOAT,GL_FALSE,48,(void*)16);OPENGL_ERROR();

//    glEnableVertexAttribArray(LAYOUT_VERTEX);OPENGL_ERROR();


//    glPointSize(2);

//    rootWindow = glfwGlobalInfo.windowBlocks.getHead()->data.window;
//    while (!glfwWindowShouldClose(rootWindow) ){
//        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//        glfwPollEvents();

//        for(ParticleCL & P : particleBuffer.getHostBuffer()){
//            //P.vel.y+=0.01f;
//            v4ASA(&P.p1,&P.vel,1,&P.p0);// p1 = vel*dt + p0
//        }

//        //initalize the grid
//        memset(gridBuffer.getHostBuffer().data(),0,sizeof(SphGridCL)*512*512);

//        for(uint32_t K = 0; K < particleBuffer.size(); K++){
//            int gx,gy;
//            ParticleCL & P = particleBuffer[K];
//            gx = P.p1.x/5.0f;
//            gy = P.p1.y/5.0f;
//            SphGridCL & gridCell =  gridBuffer[GI(gx,gy,512)];
//            gridCell.buckets[gridCell.recsFilled++] = K;
//        }

//        for(int iters = 0; iters  < 1; iters++){
//            //compute density , constraints, lambdas
//            computeAttributes(particleBuffer.getHostBuffer(),gridBuffer.getHostBuffer());
//            //update p1 by DP
//            for(ParticleCL & P : particleBuffer.getHostBuffer()){
//                v4IA(&P.p1,&P.DP);
//            }
//        }
//        particleBuffer[0].p0 = particleBuffer[0].p1 = {glfwGlobalInfo.mousePos.x,glfwGlobalInfo.mousePos.y,0,1.0f};

//        //update velocity
//        for(ParticleCL & P: particleBuffer.getHostBuffer()){
//            P.p1.x = CLAMP(P.p1.x,0,800);
//            P.p1.y = CLAMP(P.p1.y,0,600);
//            v4Su(&P.vel,&P.p1,&P.p0);
//            P.p0 = P.p1;
//        }



//        //draw particles

//        prog.useProgram();
//        glBindVertexArray(vao);
//        glDrawArrays(GL_POINTS,0,particleBuffer.size());
//        glUseProgram(0);


//        //update buffer objects

//        glBindBuffer(GL_UNIFORM_BUFFER,blockData);OPENGL_ERROR();
//        glBufferData(GL_UNIFORM_BUFFER,sizeof(CMAT44)*4,matStack.getKeyMatricies(),GL_DYNAMIC_DRAW);OPENGL_ERROR();


//        //upload particle info to opengl
//        glBindBuffer(GL_ARRAY_BUFFER,vbo);
//        glBufferData(GL_ARRAY_BUFFER,
//                     sizeof(ParticleCL)*particleBuffer.size(),
//                     particleBuffer.getHostBuffer().data(),
//                     GL_DYNAMIC_DRAW);OPENGL_ERROR();

//        glVertexAttribPointer(LAYOUT_VERTEX,4,GL_FLOAT,GL_FALSE,48+16,(void*)16);OPENGL_ERROR();



//        GLFWU::resolveEvents(matStack);
//        std::this_thread::sleep_for(std::chrono::milliseconds(1));

//        glfwSwapBuffers(rootWindow);
//    }

//    clReleaseContext(oclInfoBlock.context);
//    glfwTerminate();
//    return 0;
//}

//void print_particle(ParticleCL * p){
//    printf("x0  : [ %f, %f , %f ,%f] \n",p->p0.x ,p->p0.y ,p->p0.z ,p->p0.w);
//    printf("x1  : [ %f, %f , %f ,%f] \n",p->p1.x ,p->p1.y ,p->p1.z ,p->p1.w);
//    printf("vel : [ %f, %f , %f ,%f] \n",p->vel.x,p->vel.y,p->vel.z,p->vel.w);
//    printf("density: [%f]\n",p->density);
//    printf("lambda:  [%f]\n",p->lambda);
//}



