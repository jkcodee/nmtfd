#include "numericalCalculation.h"

double absoluteVelocity(int nx, int ny, int i, int j, double ux[nx + 1][ny + 1], double uy[nx + 1][ny + 1])
{
	return sqrt(ux[i][j] * ux[i][j] + uy[i][j] * uy[i][j]);
}

double calculatePressure(Area *area, int nx, int ny, double p[nx + 1][ny + 1], PressureFunction pressureFunction)
{
	double p_alt, delta = 0.;
	int i, j;
	for (i = area->begX; i <= area->endX; i++)
	{
		for (j = area->begY; j <= area->endY; j++)
		{
			p_alt = p[i][j];
			p[i][j] = (pressureFunction)(i, j, nx, ny, p);
			delta += fabs(p[i][j] - p_alt);
		}
	}
	return delta;
}

double pressureInside(int i, int j, int nx, int ny, double p[nx + 1][ny + 1])
{
	return 0.25 * (p[i + 1][j] + p[i - 1][j] + p[i][j + 1] + p[i][j - 1]);
}
double pressureLeft(int i, int j, int nx, int ny, double p[nx + 1][ny + 1])
{
	return 0.2 * (-p[i + 1][j] + 4 * p[i + 1][j] + p[i][j - 1] + p[i][j + 1]);
}
double pressureRight(int i, int j, int nx, int ny, double p[nx + 1][ny + 1])
{
	return 0.2 * (-p[i - 2][j] + 4 * p[i - 1][j] + p[i][j - 1] + p[i][j + 1]);
}
double pressureBottom(int i, int j, int nx, int ny, double p[nx + 1][ny + 1])
{
	return 0.2 * (-p[i][j - 2] + 4 * p[i][j - 1] + p[i - 1][j] + p[i + 1][j]);
}
double pressureBottomLeft(int i, int j, int nx, int ny, double p[nx + 1][ny + 1])
{
	const double fraction = 1. / 6.;
	return fraction * (4. * p[i + 1][j] - p[i + 2][j] + 4. * p[i][j - 1] - p[i][j - 2]);
}
double pressureBottomRight(int i, int j, int nx, int ny, double p[nx + 1][ny + 1])
{
	const double fraction = 1. / 6.;
	return fraction * (4. * p[i - 1][j] - p[i - 2][j] + 4. * p[i][j - 1] - p[i][j - 2]);
}
double pressureConstant(int i, int j, int nx, int ny, double p[nx + 1][ny + 1])
{
	return p[i][j];
}

double calculateVolumetricFlowRateTrapez(int nx, int ny, double ux[nx + 1][ny + 1], double uy[nx + 1][ny + 1], Area *rohr, double dx)
{
    int i = 0, j = 0;
    int it = 0;

	double l = 100.; // Länge Rohr in cm


    double A = 0.0;
    double VolumenstromTrapez = 0.0;

	// 0.5*u[0] + 0.5*u[n]
	double u1 = 0.5 * (absoluteVelocity(nx, ny, rohr->begX, rohr->begY, ux, uy) + absoluteVelocity(nx, ny, rohr->begX, rohr->begY, ux, uy)); // für obere Rohrkante
	double u2 = 0.5 * (absoluteVelocity(nx, ny, rohr->begX, rohr->endY, ux, uy) + absoluteVelocity(nx, ny, rohr->begX, rohr->endY, ux, uy)); // für untere Rohrkante

	// Geschwindigkeit x Richtung
	for (i = rohr->begX + 1; i <= rohr->endX - 1; i++)
	{
		it++;
		u1 += absoluteVelocity(nx, ny, i, rohr->begY, ux, uy);
		u2 += absoluteVelocity(nx, ny, i, rohr->endY, ux, uy);
	}

	u1 *= 100.; // Umrechnung in cm
	u2 *= 100.; // Umrechnung in cm

	double b = (it + 1) * dx; // Rohrbreite
	A = l * b;

	// Volumenstrom von der oberen Rohrfläche
	VolumenstromTrapez += A * u1 / (it + 2);

	// Volumenstrom von der unteren Rohrfläche
	VolumenstromTrapez += A * u2 / (it + 2);

	// 0.5*u[0] + 0.5*u[n]
	u1 = 0.5 * (absoluteVelocity(nx, ny, rohr->begX, rohr->begY, ux, uy) + absoluteVelocity(nx, ny, rohr->begX, rohr->endY, ux, uy)); // für linke Rohrkante
	u2 = 0.5 * (absoluteVelocity(nx, ny, rohr->endX, rohr->begY, ux, uy) + absoluteVelocity(nx, ny, rohr->endX, rohr->endY, ux, uy)); // für rechte Rohrkante
	it = 0;
	// Geschwindigkeit y Richtung
	for (j = rohr->begY + 1; j <= rohr->endY - 1; j++)
	{
		it++;
		u1 += absoluteVelocity(nx, ny, rohr->begX, j, ux, uy);
		u2 += absoluteVelocity(nx, ny, rohr->endX, j, ux, uy);
	}

	u1 *= 100.; // Umrechnung in cm
	u2 *= 100.; // Umrechnung in cm

	double h = (it + 1) * dx; // Rohrhöhe
	A = l * h;

	// Volumenstrom von der linke  Rohrfläche
	VolumenstromTrapez += A * u1 / (it + 2);

	// Volumenstrom von der rechte Rohrfläche
	VolumenstromTrapez += A * u2 / (it + 2);

	VolumenstromTrapez *= 3600; // zu l/h

    return VolumenstromTrapez;
}

void calculateSpeed(int nx, int ny, double p[nx + 1][ny + 1], double ux[nx + 1][ny + 1], double uy[nx + 1][ny + 1], double dx)
{
	int i, j;
	const double k = pow(10, -12); //  m²
	const double eta = 0.001;	  // Pa*s

	// Berechne Geschwindigkeitskomponenten
	for (i = 1; i < nx; i++)
	{
		for (j = 1; j < ny; j++)
		{
			ux[i][j] = -(k / eta) * ((p[i + 1][j] - p[i - 1][j])) / (dx * 2.);
			uy[i][j] = -(k / eta) * ((p[i][j + 1] - p[i][j - 1])) / (dx * 2.);
		}
	}

	// linker und rechter Rand
	for (j = 1; j < ny; j++)
	{
		// linker Rand
		ux[0][j] = 0.;
		uy[0][j] = -(k / eta) * ((p[0][j + 1] - p[0][j - 1])) / (dx * 2.);
		// rechter Rand
		ux[nx][j] = 0.;
		uy[nx][j] = -(k / eta) * ((p[nx][j + 1] - p[nx][j - 1])) / (dx * 2.);
	}

	// oberer Rand und unterer Rand
	for (i = 1; i < nx; i++)
	{
		// oberer Rand
		ux[i][0] = -(k / eta) * (p[i + 1][0] - p[i - 1][0]) / (dx * 2.);
		uy[i][0] = -(k / eta) * (-p[i][2] + 4. * p[i][1] - 3. * p[i][0]) / (2. * dx); //-(k/eta) * ((p[i][1]-p[i][0])) / (dx * 2.);
		// unterer Rand
		ux[i][ny] = -(k / eta) * (p[i + 1][ny] - p[i - 1][ny]) / (dx * 2.);
		uy[i][ny] = 0.;
	}

	// Eckknoten
	// oben links
	ux[0][0] = 0.;
	uy[0][0] = -(k / eta) * (-p[0][2] + 4. * p[0][1] - 3. * p[0][0]) / (2. * dx);

	// oben rechts
	ux[nx][0] = 0.;
	uy[nx][0] = -(k / eta) * (-p[nx][2] + 4. * p[nx][1] - 3. * p[nx][0]) / (2. * dx);

	// unten links
	ux[0][ny] = 0.;
	uy[0][ny] = 0.;

	// unten rechts
	ux[nx][ny] = 0.;
	uy[nx][ny] = 0.;
}