#ifndef NUMERICAL_CALCULATION_H
#define NUMERICAL_CALCULATION_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dataStructures.h"

double absoluteVelocity(int nx, int ny, int i, int j, double ux[nx + 1][ny + 1], double uy[nx + 1][ny + 1]);

double calculatePressure(Area *area, int nx, int ny, double p[nx + 1][ny + 1], PressureFunction pressureFunction);

// Calculation specification for the different areas
double pressureInside(int i, int j, int nx, int ny, double p[nx + 1][ny + 1]);
double pressureLeft(int i, int j, int nx, int ny, double p[nx + 1][ny + 1]);
double pressureRight(int i, int j, int nx, int ny, double p[nx + 1][ny + 1]);
double pressureBottom(int i, int j, int nx, int ny, double p[nx + 1][ny + 1]);
double pressureBottomLeft(int i, int j, int nx, int ny, double p[nx + 1][ny + 1]);
double pressureBottomRight(int i, int j, int nx, int ny, double p[nx + 1][ny + 1]);
double pressureConstant(int i, int j, int nx, int ny, double p[nx + 1][ny + 1]);

double calculateVolumetricFlowRateTrapez(int nx, int ny, double ux[nx + 1][ny + 1], double uy[nx + 1][ny + 1], Area *rohr, double dx);

void calculateSpeed(int nx, int ny, double p[nx + 1][ny + 1], double ux[nx + 1][ny + 1], double uy[nx + 1][ny + 1], double dx);

#endif