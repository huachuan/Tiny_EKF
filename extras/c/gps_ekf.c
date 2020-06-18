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
#include <gpsdata.h>
#include "tiny_ekf.h"

static inline unsigned long long
ps_tsc(void) {
	unsigned long a, d, c;

	__asm__ __volatile__("rdtsc" : "=a" (a), "=d" (d), "=c" (c) : :);

	return ((unsigned long long)d << 32) | (unsigned long long)a;
}

int 
main(int argc, char ** argv)
{    
    // Do generic EKF initialization
    ekf_t ekf;
    ekf_init(&ekf, Nsta, Mobs);

    // Do local initialization
    init(&ekf);

    // Skip CSV header

    // Make a place to store the data from the file and the output of the EKF
    	double SV_Pos[4][3];
    	double SV_Rho[4];
    	double Pos_KF[25][3];

    // Open output CSV file and write header
    //fprintf(ofp,"X,Y,Z\n");

    	int j, k;
	unsigned long long start, end;

	start = ps_tsc();
    // Loop till no more data
    	for (j=0; j<25; ++j) {
		readdata(j, SV_Pos, SV_Rho);

       	 	model(&ekf, SV_Pos);

        	ekf_step(&ekf, SV_Rho);

        // grab positions, ignoring velocities
        	for (k=0; k<3; ++k)
            		Pos_KF[j][k] = ekf.x[2*k];
    	}

    // Compute means of filtered positions
    	double mean_Pos_KF[3] = {0, 0, 0};
   	for (j=0; j<25; ++j) 
        	for (k=0; k<3; ++k)
            		mean_Pos_KF[k] += Pos_KF[j][k];
    	for (k=0; k<3; ++k)
        	mean_Pos_KF[k] /= 25;


    // Dump filtered positions minus their means
   // for (j=0; j<25; ++j) {
        //printf("%f,%f,%f\n", Pos_KF[j][0]-mean_Pos_KF[0], Pos_KF[j][1]-mean_Pos_KF[1], Pos_KF[j][2]-mean_Pos_KF[2]);
        //printf("%f %f %f\n", Pos_KF[j][0], Pos_KF[j][1], Pos_KF[j][2]);
    //}
	end = ps_tsc();
	printf("cycles per 100000 loop: %llu, %llu\n", (end-start), (end-start)/25);
    return 0;
}
