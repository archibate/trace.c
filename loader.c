#include <stdio.h>
#include <stdlib.h>
#include "loader.h"

void saveColorer(FILE *fp, Colorer const *c)
{
	fprintf(fp, "%f %f %f %f %f %f %f %f %f", vcomma(c->ambi), vcomma(c->diff), vcomma(c->spec));
}

void saveSphere(FILE *fp, Sphere const *s)
{
	fprintf(fp, "%f %f %f %f ", s->r, vcomma(s->p));
	saveColorer(fp, &s->c);
}

void saveScene(FILE *fp, Sphere const *scene, unsigned count)
{
	for (int i = 0; i < count; i++) {
		saveSphere(fp, &scene[i]);
		fprintf(fp, "\n");
	}
}

void saveSceneC(FILE *fp, Sphere const *scene, unsigned count)
{
	fprintf(fp, "%d\n", count);
	saveScene(fp, scene, count);
}

void loadColorer(FILE *fp, Colorer *c)
{
	fscanf(fp, "%f %f %f %f %f %f %f %f %f", vrefcom(c->ambi), vrefcom(c->diff), vrefcom(c->spec));
}

void loadSphere(FILE *fp, Sphere *s)
{
	fscanf(fp, "%f %f %f %f ", &s->r, vrefcom(s->p));
	loadColorer(fp, &s->c);
}

void loadScene(FILE *fp, Sphere *scene, unsigned count)
{
	for (int i = 0; i < count; i++) {
		loadSphere(fp, &scene[i]);
	}
}

Sphere *loadSceneAC(FILE *fp, unsigned *pcount)
{
	unsigned count;
	fscanf(fp, "%d", &count);
	Sphere *scene = (Sphere *) malloc(count * sizeof(Sphere));
	loadScene(fp, scene, count);
	*pcount = count;
	return scene;
}
