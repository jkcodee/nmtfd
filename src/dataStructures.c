
#include "dataStructures.h"

int countNodesOfArea(Area *area)
{
	return (area->endX - area->begX + 1) * (area->endY - area->begY + 1);
}

void setArea(Area *area, int begX, int endX, int begY, int endY, PressureFunction pressureFunction)
{
	area->begX = begX;
	area->endX = endX;
	area->begY = begY;
	area->endY = endY;
	area->pressureFunction = pressureFunction;
}

void setPressure(Area *area, int nx, int ny, double p[nx + 1][ny + 1], double pInit)
{
	int i, j;
	for (i = area->begX; i <= area->endX; i++)
	{
		for (j = area->begY; j <= area->endY; j++)
		{
			p[i][j] = pInit;
		}
	}
}

Properties getProperties(int argc, char *argv[])
{
	Properties properties = {2, 10.0, 0.1, 0};
	int option;
	while ((option = getopt(argc, argv, "w:h:d:o")) != -1)
	{
		switch (option)
		{
		case 'w': // Set Width
			properties.width = atof(optarg);
			break;
		case 'h': // Set Height
			properties.height = atof(optarg);
			break;
		case 'd': // Set dx
			properties.dx = atof(optarg);
			break;
		case 'o':
			properties.noOuputFlag = 1;
			break;
		default:
			print_usage();
			exit(EXIT_FAILURE);
		}
	}
	return properties;
}