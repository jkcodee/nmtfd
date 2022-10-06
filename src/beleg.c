#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dataStructures.h"
#include "numericalCalculation.h"
#include "model.h"

void writeBelegData(char *filename, int nx, int ny, double p[nx + 1][ny + 1], double ux[nx + 1][ny + 1], double uy[nx + 1][ny + 1], double dx, double dy);
void writeSpeedData(const char *filename, int nx, int ny, double ux[nx + 1][ny + 1], double uy[nx + 1][ny + 1], double dx, double speicherDx);
void writeVolumetricFlowRate(char *filename, Model *model, double VolumenstromTrapez);

int fileExists(const char *filename);
void print_usage();


int main(int argc, char *argv[])
{
	double width = 10.0;  // [m] Eingegebene Breite
	double height = 10.0; // [m] Eingegebene Höhe
	double dx = 0.1;	  // [m] Eingegebener Abstand zwischen Gitterpunkten

	int noOuputFlag = 0; // Wenn diese Flag gesetzt ist, werden nur die Volumenstromdaten gespeichert

	Model model;
	model.properties = getProperties(argc, argv);

	if(!setupModel(&model)) return 1;
	calculatePressureOfModel(&model);
	//calculateSpeedOfModel(&model);
	calculateSpeed(model.nx, model.ny, model.pressure, model.velocityX, model.velocityY, model.properties.dx);

	printf("Geschwindigkeiten Berechnet\n");

	printf("\n");

	double volumetricFlowLeftPipe = calculateVolumetricFlowRateTrapez(model.nx, model.ny, model.velocityX, model.velocityY, &(model.pipeLeft), model.properties.dx);
	double volumetricFlowRightPipe = calculateVolumetricFlowRateTrapez(model.nx, model.ny, model.velocityX, model.velocityY, &(model.pipeRight), model.properties.dx);

	writeVolumetricFlowRate("volumenstrom_Rohr_Links.data", &model, volumetricFlowLeftPipe);
	writeVolumetricFlowRate("volumenstrom_Rohr_Rechts.data", &model, volumetricFlowRightPipe);

	if (!model.properties.noOuputFlag)
	{
		writeBelegData("beleg.data", model.nx, model.ny, model.pressure, model.velocityX, model.velocityY, model.properties.dx, model.properties.dx);
		writeSpeedData("geschwindigkeit.data", model.nx, model.ny, model.velocityX, model.velocityY, model.properties.dx, 0.2);
	}

	freeModel(&model);
	printf("Programmende erreicht\n");

	return 0;
}

void print_usage()
{
	fprintf(stderr, "Nutzung:\n -w [Wert] Gesamtbreite\n -h [Wert] Gesamthöhe\n -d [Wert] Gitterabstand\n -o        Druckfeld nicht speichern\n");
}

int fileExists(const char *filename)
{
	FILE *file;
	if ((file = fopen(filename, "r")))
	{
		fclose(file);
		return 1;
	}
	return 0;
}

void writeBelegData(char *filename, int nx, int ny, double p[nx + 1][ny + 1], double ux[nx + 1][ny + 1], double uy[nx + 1][ny + 1], double dx, double dy)
{
	FILE *file = fopen(filename, "w");
	int i, j;
	double u;
	fprintf(file, "# i   j          x          y          p        u_x        u_y        |u|\n");
	fprintf(file, "#------------------------------------------------------------------------\n");
	for (i = 0; i <= nx; i++)
	{
		for (j = 0; j <= ny; j++)
		{
			u = sqrt(ux[i][j] * ux[i][j] + uy[i][j] * uy[i][j]);
			fprintf(file, "%3i %3i %10.3e %10.3e %10.3e %10.3e %10.3e %10.3e\n", i, j, (double)i * dx, (double)j * dy, p[i][j], ux[i][j], uy[i][j], u);
		}
		fprintf(file, "\n");
	}
	fclose(file);
}

void writeSpeedData(const char *filename, int nx, int ny, double ux[nx + 1][ny + 1], double uy[nx + 1][ny + 1], double dx, double speicherDx)
{
	FILE *file = fopen(filename, "w");

	int factor = round(speicherDx / dx);

	int i, j;
	double u;
	fprintf(file, "# i   j          x          y        u_x        u_y        |u|\n");
	fprintf(file, "#-------------------------------------------------------------\n");
	for (i = 0; i <= nx; i += factor)
	{
		for (j = 0; j <= ny; j += factor)
		{
			u = sqrt(ux[i][j] * ux[i][j] + uy[i][j] * uy[i][j]);
			fprintf(file, "%3i %3i %10.3e %10.3e %10.3e %10.3e %10.3e\n", i, j, (double)i * dx, (double)j * dx, ux[i][j], uy[i][j], u);
		}
		fprintf(file, "\n");
	}
	fclose(file);
}


void writeVolumetricFlowRate(char *filename, Model *model, double VolumenstromTrapez)
{

	int existsFlag = fileExists(filename);

	FILE *file = fopen(filename, "a");

	if (!existsFlag)
	{
		fprintf(file, "| G_Breite|  G_Tiefe|    dx| Vs_mit[l/h]| Vs_tra[l/h]\n");
		fprintf(file, "|---------|---------|------|------------|------------\n");
	}
	double gesamtbreite = model->nx * model->properties.dx;
	double gesamthoehe = model->ny * model->properties.dx;

	fprintf(file, "%9.2f ", gesamtbreite);
	fprintf(file, "%9.2f ", gesamthoehe);
	fprintf(file, "%6.2f ", model->properties.dx);
	fprintf(file, "%12.6f ", VolumenstromTrapez);
	fprintf(file, "\n");

	fclose(file);
}