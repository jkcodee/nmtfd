#ifndef MODEL_H
#define MODEL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dataStructures.h"
#include "numericalCalculation.h"

typedef struct Model_s
{
	// discrete segments in x direction (horizontal)
	int nx; 
	// discrete segments in y direction (vertical)
	int ny;

	double *pressure;
	double *velocityX;
	double *velocityY;

	Properties properties;

	int area_count;
	Area *areas;
	Area pipeLeft;
	Area pipeRight;

	// For Pressure Calculation
	int maxIterations;
	double minDelta;
} Model;

int setupModel(Model *model);
void freeModel(Model *model);
int countNodesOfModel(Model *model);
void calculatePressureOfModel(Model *model);

#endif