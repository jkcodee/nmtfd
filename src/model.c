#include "model.h"

int setupModel(Model *model)
{
	printf("%f.2\n",model->properties.width);
	double width = model->properties.width;  	// [m] Eingegebene Breite
	double height = model->properties.height; 	// [m] Eingegebene Höhe
	double dx = model->properties.dx;	  		// [m] Eingegebener Abstand zwischen Gitterpunkten


	// Distance between pipe
	double rohrAbstand = 2.; // [m] Rohrabstand
	double rohrTiefe = 2.;   // [m] Rohrtiefe
	double rohrBreite = 0.2; // [m] Rohrbreite
	double rohrHoehe = 0.2;  // [m] Rohrhöhe

	//double breiteLinks  = 4.; // [m] Breite nach links vom linken Rohr bis zur adiabaten Wand
	//double breiteRechts = 4.; // [m] Breite nach rechts vom rechten Rohr bis zur adiabaten Wand
	//double tiefe        = 4.; // [m] Tiefe vom Rohr bis zur adiabaten Wand

	double Wx = width;  // [m] Gesamtbreite
	double Wy = height; // [m] Gesamthöhe

	int cancleFlag = 0;
	double breiteRest = Wx - (2 * rohrBreite) - rohrAbstand;
	double hoeheRest = Wy - rohrTiefe - rohrHoehe;
	if (breiteRest <= 0)
	{
		printf("Gebiet ist in der Breite zu schmall\n");
		cancleFlag = 1;
	}
	if (hoeheRest <= 0)
	{
		printf("Gebiet ist in der Tiefe zu klein\n");
		cancleFlag = 1;
	}
	if (cancleFlag)
		return 0;


	double breiteLinks = breiteRest / 2.0;  // [m] Breite nach links vom linken Rohr bis zur adiabaten Wand
	double breiteRechts = breiteRest / 2.0; // [m] Breite nach rechts vom rechten Rohr bis zur adiabaten Wand
	double tiefe = hoeheRest;				// [m] Tiefe vom Rohr bis zur adiabaten Wand

	model->maxIterations = 100000;
	model->minDelta = 0.00000001; // [bar] DELTA

	printf("Breite: %6.1fm\n", Wx);
	printf("Höhe: %6.1fm\n\n", Wy);

	model->area_count = 11;
	model->areas = malloc(model->area_count * sizeof(Area));

	int nx = round(Wx / dx); // Anzahl Segmente in x Richtung
	int ny = round(Wy / dx); // Anzahl Segmente in y Richtung

	model->nx = nx;
	model->ny = ny;

	printf("Anzahl Segemente x-Richtung: %3i\n", nx);
	printf("Anzahl Segemente y-Richtung: %3i\n\n", ny);

	// Definition der Geometrie

	Area gesamt;
	gesamt.begX = 0;
	gesamt.endX = nx;
	gesamt.begY = 0;
	gesamt.endY = ny;

	// berechne Gitterpunkte der Rohre
	Area rohrLinks; // linkes rohr
	rohrLinks.begX = round((breiteLinks) / dx);
	rohrLinks.endX = round((breiteLinks + rohrBreite) / dx);
	rohrLinks.begY = round((rohrTiefe) / dx);
	rohrLinks.endY = round((rohrTiefe + rohrHoehe) / dx);
	rohrLinks.pressureFunction = &pressureConstant;
	model->pipeLeft = rohrLinks;

	Area rohrRechts; // rechtes rohr
	rohrRechts.begX = round((breiteLinks + rohrBreite + rohrAbstand) / dx);
	rohrRechts.endX = round((breiteLinks + 2. * rohrBreite + rohrAbstand) / dx);
	rohrRechts.begY = rohrLinks.begY;
	rohrRechts.endY = rohrLinks.endY;
	rohrRechts.pressureFunction = &pressureConstant;
	model->pipeRight = rohrRechts;

	// innere Berechnungsgebiete

	// Gebiet über Rohren
	Area gebiet1;
	setArea(&gebiet1, 1, nx - 1, 1, rohrLinks.begY - 1, &pressureInside);
	model->areas[0] = gebiet1;


	// links vom linken Rohr
	Area gebiet2;
	setArea(&gebiet2, 1, rohrLinks.begX - 1, rohrLinks.begY, rohrLinks.endY, &pressureInside);
	model->areas[1] = gebiet2;

	// zwischen beiden Rohren
	Area gebiet3;
	setArea(&gebiet3, rohrLinks.endX + 1, rohrRechts.begX - 1, rohrRechts.begY, rohrLinks.endY, &pressureInside);
	model->areas[2] = gebiet3;

	// rechts vom rechten Rohr
	Area gebiet4;
	setArea(&gebiet4, rohrRechts.endX + 1, nx - 1, rohrRechts.begY, rohrRechts.endY, &pressureInside);
	model->areas[3] = gebiet4;

	// unter Rohren
	Area gebiet5;
	setArea(&gebiet5, 1, nx - 1, rohrLinks.endY + 1, ny - 1, &pressureInside);
	model->areas[4] = gebiet5;

	// Randgebiete
	Area gebietOben;
	setArea(&gebietOben, 0, nx, 0, 0, &pressureConstant);
	model->areas[5] = gebietOben;

	Area gebietLinks;
	setArea(&gebietLinks, 0, 0, 1, ny - 1, &pressureLeft);
	model->areas[6] = gebietLinks;

	Area gebietRechts;
	setArea(&gebietRechts, nx, nx, 1, ny - 1, &pressureRight);
	model->areas[7] = gebietRechts;

	Area gebietUnten;
	setArea(&gebietUnten, 1, nx - 1, ny, ny, &pressureBottom);
	model->areas[8] = gebietUnten;

	Area gebietEckeLinksUnten;
	setArea(&gebietEckeLinksUnten, 0, 0, ny, ny, &pressureBottomLeft);
	model->areas[9] = gebietEckeLinksUnten;

	Area gebietEckeRechtsUnten;
	setArea(&gebietEckeRechtsUnten, nx, nx, ny, ny, &pressureBottomRight);
	model->areas[10] = gebietEckeRechtsUnten;

	int elements = (nx + 1) * (ny + 1);

	double *p = malloc(sizeof(double) * elements);
	double *ux = malloc(sizeof(double) * elements);
	double *uy = malloc(sizeof(double) * elements);
	if (!p || !ux || !uy)
	{
		free(p);
		free(ux);
		free(uy);
		printf("Error allocating memory\n");
		return 0;
	}
	model->pressure = p;
	model->velocityX = ux;
	model->velocityY = uy;

	double pB = 1.; // [bar] Druck Oberfläche
	double p1 = 2.; // [bar] Druck Rohr Quelle
	double p2 = 0.; // [bar] Druck Rohr Senke

	setPressure(&gesamt, nx, ny, p, pB);	 // Startdruck aller Knoten
	setPressure(&rohrLinks, nx, ny, p, p1);  // Startdruck linkes Rohr
	setPressure(&rohrRechts, nx, ny, p, p2); // Startdruck rechtes Rohr

	printf("Initialized pressure\n");


	return 1;
}

void freeModel(Model *model)
{
	free(model->areas);
	free(model->velocityX);
	free(model->velocityY);
	free(model->pressure);
}

void calculatePressureOfModel(Model *model)
{
	double minDelta = model->minDelta;
	int maxIterations = model->maxIterations;
	double delta = 0.;
	double oldDelta = 0.;
	double dDelta = 0;
	int it = 0;
	int area_iterator = 0;
	int nodes = countNodesOfModel(model);
	Area currentArea;
	do
	{
		oldDelta = delta;
		delta = 0.;
		it++;
		for(area_iterator = 0; area_iterator < model->area_count; area_iterator++)
		{
			currentArea = model->areas[area_iterator];
			delta += calculatePressure(&currentArea, model->nx, model->ny, model->pressure, currentArea.pressureFunction);
		}
		dDelta = fabs(oldDelta - delta);
		delta /= nodes;
		if (it % 1000 == 0)
			printf("%i : %10.3e, %10.3e\n", it, dDelta, delta);
	} while (delta > minDelta && maxIterations > it);
	printf("Druckfeld nach %i Iterationen berechnet.\n", it);
}

int countNodesOfModel(Model *model)
{
	int nodes = 0;
	int area_count = model->area_count;
	int i = 0;
	for(; i < area_count; i++)
	{
		nodes += countNodesOfArea(&(model->areas[i]));
	}
}

