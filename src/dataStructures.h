#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>

typedef double (*PressureFunction)(int i, int j, int nx, int ny, double p[nx + 1][ny + 1]);

typedef struct Area_s
{
	int begX;
	int endX;
	int begY;
	int endY;
	PressureFunction pressureFunction;
} Area;

typedef struct Properties_s{
	double width;
	double height;
	double dx;
	char noOuputFlag;
} Properties;

int countNodesOfArea(Area *area);
void setArea(Area *area, int begX, int endX, int begY, int endY, PressureFunction pressureFunction);
void setPressure(Area *area, int nx, int ny, double p[nx + 1][ny + 1], double pInit);
Properties getProperties(int argc, char *argv[]);

#endif