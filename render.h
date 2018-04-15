#pragma once

#include "vec.h"

typedef struct {
	Vec ambi, diff, spec;
} Colorer;

typedef struct {
	Vec p;
	float r;
	Colorer c;
} Sphere;

typedef struct {
	Vec o, d;
} Ray;

typedef struct {
	float t;
	Vec n, p;
	Colorer c;
} HitResult;

float SphereIntersect(Ray const *r, Sphere const *s);
void SphereCalcHitResult(HitResult *res, Ray const *r, Sphere const *s);
int SceneHit(HitResult *res, Ray const *r, Sphere const *scene, unsigned count);
