#pragma once

#include "render.h"

void saveColorer(FILE *fp, Colorer const *c);
void saveSphere(FILE *fp, Sphere const *s);
void saveScene(FILE *fp, Sphere const *scene, unsigned count);
void saveSceneC(FILE *fp, Sphere const *scene, unsigned count);
void loadColorer(FILE *fp, Colorer *c);
void loadSphere(FILE *fp, Sphere *s);
void loadScene(FILE *fp, Sphere *scene, unsigned count);
Sphere *loadSceneAC(FILE *fp, unsigned *pcount);
