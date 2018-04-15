#include <math.h>
#include <string.h>
#include "render.h"

float SphereIntersect(Ray const *r, Sphere const *s)
{
	Vec op;
	vsub(op, s->p, r->o);
	float b = vdot(op, r->d);
	float det = b * b + s->r * s->r - vdot(op, op);
	if (det < 0.f)
		return 0.f;
	det = sqrt(det);
	float t, eps = 2e-4f;
        return (t = b - det) > eps ? t : ((t = b + det) > eps ? t : 0.f);
}

void SphereCalcHitResult(HitResult *res, Ray const *r, Sphere const *s)
{
	Vec dist;
	vsmul(dist, res->t, r->d);
	vadd(res->p, r->o, dist);
	vsub(res->n, res->p, s->p);
	vnorm(res->n);

	memcpy(&res->c, &s->c, sizeof(res->c));
}

int SceneHit(HitResult *res, Ray const *r, Sphere const *scene, unsigned count)
{
	int id = -1;
	for (int i = 0; i < count; i++) {
		float t = SphereIntersect(r, &scene[i]);
		if (t != 0.f && t < res->t) {
			id = i;
			res->t = t;
		}
	}
	if (id != -1)
		SphereCalcHitResult(res, r, &scene[id]);
	return id;
}

