/* gps_ekf: TinyEKF test case using You Chong's GPS example:
 * 
 *   http://www.mathworks.com/matlabcentral/fileexchange/31487-extended-kalman-filter-ekf--for-gps
 * 
 * Reads file gps.csv of satellite data and writes file ekf.csv of mean-subtracted estimated positions.
 *
 *
 * References:
 *
 * 1. R G Brown, P Y C Hwang, "Introduction to random signals and applied 
 * Kalman filtering : with MATLAB exercises and solutions",1996
 *
 * 2. Pratap Misra, Per Enge, "Global Positioning System Signals, 
 * Measurements, and Performance(Second Edition)",2006
 * 
 * Copyright (C) 2015 Simon D. Levy
 *
 * MIT License
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include "tiny_ekf.h"
#include "tinyekf_config.h"
// positioning interval
const double T = 1;
void 
blkfill(ekf_t * ekf, const double * a, int off)
{
    off *= 2;

    ekf->Q[off]   [off]   = a[0]; 
    ekf->Q[off]   [off+1] = a[1];
    ekf->Q[off+1] [off]   = a[2];
    ekf->Q[off+1] [off+1] = a[3];
}

void 
init(ekf_t * ekf)
{
    // Set Q, see [1]
    const double Sf    = 36;
    const double Sg    = 0.01;
    const double sigma = 5;         // state transition variance
    const double Qb[4] = {Sf*T+Sg*T*T*T/3, Sg*T*T/2, Sg*T*T/2, Sg*T};
    const double Qxyz[4] = {sigma*sigma*T*T*T/3, sigma*sigma*T*T/2, sigma*sigma*T*T/2, sigma*sigma*T};

    blkfill(ekf, Qxyz, 0);
    blkfill(ekf, Qxyz, 1);
    blkfill(ekf, Qxyz, 2);
    blkfill(ekf, Qb,   3);

    // initial covariances of state noise, measurement noise
    double P0 = 10;
    double R0 = 36;

    int i;

    for (i=0; i<8; ++i)
        ekf->P[i][i] = P0;

    for (i=0; i<4; ++i)
        ekf->R[i][i] = R0;

    // position
    ekf->x[0] = -2.168816181271560e+006;
    ekf->x[2] =  4.386648549091666e+006;
    ekf->x[4] =  4.077161596428751e+006;

    // velocity
    ekf->x[1] = 0;
    ekf->x[3] = 0;
    ekf->x[5] = 0;

    // clock bias
    ekf->x[6] = 3.575261153706439e+006;

    // clock drift
    ekf->x[7] = 4.549246345845814e+001;
}

void 
model(ekf_t * ekf, double SV[4][3])
{ 

    int i, j;

    for (j=0; j<8; j+=2) {
        ekf->fx[j] = ekf->x[j] + T * ekf->x[j+1];
        ekf->fx[j+1] = ekf->x[j+1];
    }

    for (j=0; j<8; ++j)
        ekf->F[j][j] = 1;

    for (j=0; j<4; ++j)
        ekf->F[2*j][2*j+1] = T;

    double dx[4][3];

    for (i=0; i<4; ++i) {
        ekf->hx[i] = 0;
        for (j=0; j<3; ++j) {
            double d = ekf->fx[j*2] - SV[i][j];
            dx[i][j] = d;
            ekf->hx[i] += d*d;
        }
        ekf->hx[i] = pow(ekf->hx[i], 0.5) + ekf->fx[6];
    }

    for (i=0; i<4; ++i) {
        for (j=0; j<3; ++j) 
            ekf->H[i][j*2]  = dx[i][j] / ekf->hx[i];
        ekf->H[i][6] = 1;
    }   
}

void 
readdata(double input[16], double SV_Pos[4][3], double SV_Rho[4])
{
    int i, j, itr = 0;
    for (i=0; i<4; ++i)
        for (j=0; j<3; ++j) {
            SV_Pos[i][j] = input[itr++];
        }

    for (j=0; j<4; ++j) {
        SV_Rho[j] = input[itr++];
    }
}
static inline unsigned long long
ps_tsc(void) {
	unsigned long a, d, c;

	__asm__ __volatile__("rdtsc" : "=a" (a), "=d" (d), "=c" (c) : :);

	return ((unsigned long long)d << 32) | (unsigned long long)a;
}

double *
ekf_fn(ekf_t * p_ekf, double data_input[16])
{    
    // Do generic EKF initialization
    //ekf_t ekf;
     ekf_t ekf = *p_ekf;
    //ekf_init(&ekf, Nsta, Mobs);
    //static double Pos_dump[3];
    // Do local initialization
    //init(&ekf);

    // Skip CSV header

    // Make a place to store the data from the file and the output of the EKF
    	double SV_Pos[4][3];
    	double SV_Rho[4];
    	static double Pos_KF[3];

    // Open output CSV file and write header
    //fprintf(ofp,"X,Y,Z\n");

    	int k;
	unsigned long long start, end;

	start = ps_tsc();
    // Loop till no more data
    	//for (j=0; j<1; ++j) {
		readdata(data_input, SV_Pos, SV_Rho);

       	 	model(&ekf, SV_Pos);

        	ekf_step(&ekf, SV_Rho);

        // grab positions, ignoring velocities
        	for (k=0; k<3; ++k)
            		Pos_KF[k] = ekf.x[2*k];
    	//}

    // Compute means of filtered positions
    //	double mean_Pos_KF[3] = {0, 0, 0};
   /*	for (j=0; j<25; ++j) 
        	for (k=0; k<3; ++k)
            		mean_Pos_KF[k] += Pos_KF[j][k];*/
    /*	for (k=0; k<3; ++k)
        	mean_Pos_KF[k] /= 25;*/

    // Dump filtered positions minus their means
   	//for (j=0; j<25; ++j) {
        //printf("%f,%f,%f\n", Pos_KF[j][0]-mean_Pos_KF[0], Pos_KF[j][1]-mean_Pos_KF[1], Pos_KF[j][2]-mean_Pos_KF[2]);
     /*   Pos_dump[0] = Pos_KF[itr][0]-mean_Pos_KF[0];
	Pos_dump[1] = Pos_KF[itr][1]-mean_Pos_KF[1];
	Pos_dump[2] = Pos_KF[itr][2]-mean_Pos_KF[2];
        printf("%f %f %f\n", Pos_KF[j][0], Pos_KF[j][1], Pos_KF[j][2]);
    	//}*/
	printf("\n %f %f %f\n", Pos_KF[0], Pos_KF[1], Pos_KF[2]);
	end = ps_tsc();
	printf("cycles per 100000 loop: %llu\n", (end-start));
    	//return Pos_dump;
	return Pos_KF;
}
