#include "./kernels/pbf.h"

__kernel void pbfAdvance(__global Particle* pBuf){
    const int GLOBAL_ID = get_global_id(0);
    __global Particle* kp; //kernel particle
    kp = pBuf+GLOBAL_ID;

    kp->x1.x = clamp(kp->x1.x,0.0f,800.0f);
    kp->x1.y = clamp(kp->x1.y,0.0f,600.0f);



    kp->x1+=kp->vel;
    kp->x1.y+=0.1f;
    kp->vel = ((kp->x1)-(kp->x0));
    kp->x0 = kp->x1;
}



__kernel void pbfComputeForces(__global Particle* pBuf, __global SphGrid* gBuf){
    const int GLOBAL_ID = get_global_id(0);
    __global Particle* kp; //kernel particle
    __global Particle* np; //neighbour particle
    __global SphGrid* gridCell;

    unsigned int K, gx, gy;  // particles position in the grid
    float4 min,max;
    float4 grad = (0,0,0,0);
    float4 disp = (0,0,0,0);
    float4 DP = (0,0,0,0);
    float Scorr = 1.0f;

    kp = pBuf+GLOBAL_ID;

    //clamp particle position  so that it stays inside the screen
    kp->x1.x = clamp(kp->x1.x,0.0f,800.0f);
    kp->x1.y = clamp(kp->x1.y,0.0f,600.0f);


    //find cell index
    min.x = (kp->x1.x/5.0f)-1.0f;
    min.y = (kp->x1.y/5.0f)-1.0f;
    max.x = min.x+2.0f;
    max.y = min.y+2.0f;

    min.x = clamp(min.x,0.0f,GCOLS-1.0f);
    min.y = clamp(min.y,0.0f,GROWS-1.0f);
    max.x = clamp(max.x,0.0f,GCOLS-1.0f);
    max.y = clamp(max.y,0.0f,GROWS-1.0f);

    int maxY = (unsigned short)max.y;
    int maxX = (unsigned short)max.x;

    //Interpolate fluid displacement
    for(gy = (unsigned short)min.y; gy < maxY; gy++){
        for(gx = (unsigned short)min.x; gx < maxX; gx++){
            gridCell = gBuf+(gy*GCOLS + gx);
            for(K = 0; K < gridCell->recsFilled; K++){
                np = &pBuf[gridCell->buckets[K]];
                disp = (np->x1)-(kp->x1);
                Scorr = -pow(Poly6Kernel(disp)/Poly6KernelAlt(0.1f*_H),4.0f)*0.1f;
                DP+= SpikeyGradient(disp)*(np->lambda+kp->lambda+Scorr);
            }
        }
    }
    kp->vel=DP;
}

__kernel void pbfComputeAttributes (__global Particle* pBuf, __global SphGrid* gBuf){
    const int GLOBAL_ID = get_global_id(0);
    __global Particle* kp;  //kernel particle
    __global Particle* np; //neighbour particle
    __global SphGrid* gridCell;

    unsigned int K, gx,gy;  // particles position in the grid
    float4 min,max;
    float4 grad = (0,0,0,0);
    float4 disp = (0,0,0,0);
    float spikeySum = 0.0f;
    float polySum = 0.0f;

    kp = pBuf+GLOBAL_ID;

    //clear particle attributes
    kp->density = 0.0f;
    kp->lambda = 0.0f;

    //neighbour search
    min.x = (kp->x1.x/5.0f)-1.0f;
    min.y = (kp->x1.y/5.0f)-1.0f;
    max.x = min.x+2.0f;
    max.y = min.y+2.0f;

    min.x = clamp(min.x,0.0f,511.0f);
    min.y = clamp(min.y,0.0f,511.0f);

    max.x = clamp(max.x,0.0f,511.0f);
    max.y = clamp(max.y,0.0f,511.0f);

    int maxY = (unsigned short)max.y;
    int maxX = (unsigned short)max.x;

    //interpolate neighbourhood
    for(gy = (unsigned short)min.y; gy < maxY; gy++){
        for(gx = (unsigned short)min.x; gx < maxX; gx++){
            gridCell = gBuf+(gy*512 + gx);
            for(K = 0; K < gridCell->recsFilled; K++){
                np = &pBuf[gridCell->buckets[K]];
                disp = (np->x1)-(kp->x1);
                polySum+=Poly6Kernel(disp);
                grad = SpikeyGradient(disp);
                spikeySum+= dot(grad,grad);
            }
        }
    }

    //compute particle attributes
    kp->density = polySum;
    kp->constraint = (polySum/REST_DENSITY)-1.0f;
    kp->lambda = -kp->constraint/(spikeySum+EPSILON);
}
