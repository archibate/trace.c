#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>
#include <math.h>
#include <omp.h>
#include "render.h"
#include "loader.h"
#include "config.h"

Sphere *scene;
unsigned count;

void radianceBasic(Ray const *ray, Vec *pcol);
void radiance(Ray const *ray, Vec *pcol, int depth);

static inline float rand48() {
    return (float) rand() / (float) RAND_MAX;
}

int main(int argc, char **argv)
{
	const char *infile = argc > 1 ? argv[1] : "scene.dat";
	const char *sizeinfo = argc > 2 ? argv[2] : NULL;
	const char *outfile = argc > 3 ? argv[3] : "image.out";

	FILE *fin = fopen(infile, "r");
	if (!fin) {
		fprintf(stderr, "cannot fopen(%s)\n", infile);
		return -1;
	}
	scene = loadSceneAC(fin, &count);
	fclose(fin);

#if defined(SAVE_PPM)
	FILE *fp = fopen(outfile, "w+");
#elif defined(SAVE_BMP)
	FILE *fp = fopen(outfile, "wb+");
#endif

	int nx = IMGSIZEX, ny = IMGSIZEY;

#ifdef RAD_SPP
	int samps = RAD_SPP;
	if (sizeinfo)
		sscanf(sizeinfo, "%dx%ds%d", &nx, &ny, &samps);
#else
	if (sizeinfo)
		sscanf(sizeinfo, "%dx%d", &nx, &ny);
#endif
	int imgSize = nx * ny * 3;

#if defined(SAVE_PPM)
#if SAVE_PPM == 6
#define FORMAT "%c%c%c"
	fprintf(fp, "P6\n");

#elif SAVE_PPM == 3
#define FORMAT "%d %d %d\n"
	fprintf(fp, "P3\n");

#else
#error invalid SAVE_PPM
#endif
	fprintf(fp, "%d %d\n255\n", nx, ny);

#elif defined(SAVE_BMP)
#if SAVE_BMP == 24
#else
#error invalid SAVE_BMP
#endif

	struct bmp_fhdr {
		uint32_t bfSize;
		uint16_t bfReserved1;
		uint16_t bfReserved2;
		uint32_t bfOffBits;
	} bfh;
	struct bmp_ihdr {
		uint32_t biSize;
		uint32_t biWidth;
		uint32_t biHeight;
		uint16_t biPlanes;
		uint16_t biBitCount;
		uint32_t biCompression;
		uint32_t biSizeImage;
		uint32_t biXPelsPerPeter;
		uint32_t biYPelsPerPeter;
		uint32_t biClrUsed;
		uint32_t biClrImportant;
	} bih;

	int hdrSize = 2 + sizeof(bfh) + sizeof(bih);
	bfh.bfSize = hdrSize + imgSize;
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfOffBits = hdrSize;

	bih.biSize = sizeof(bih);
	bih.biWidth = nx;
	bih.biHeight = -ny;
	bih.biPlanes = 1;
	bih.biBitCount = SAVE_BMP;
	bih.biCompression = 0; // BI_RGB
	bih.biSizeImage = imgSize;
	bih.biXPelsPerPeter = 0;
	bih.biYPelsPerPeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	fprintf(fp, "BM");
	fwrite(&bfh, sizeof(bfh), 1, fp);
	fwrite(&bih, sizeof(bih), 1, fp);
#endif

	unsigned char *buf = (unsigned char *) malloc(imgSize);

#pragma omp parallel for schedule(dynamic, 1)
	for (int y = 0; y < ny; y++) {
		fprintf(stderr, "\rRendering %5.2f%%", 100.0 * y / ny);

		unsigned char *p = buf + y * nx * 3;

		Ray ray = {
			.o = {0.f, 0.f, -20.f}, // camera
		};

		for (int x = 0; x < nx; x++) {
			Vec col;
#ifdef RAD_SPP
			vclr(col);
			for (int sy = 0; sy < 2; sy++) {
				for (int sx = 0; sx < 2; sx++) {
					Vec clr;
					vclr(clr);
					for (int i = 0; i < samps; i++) {

						float r1 = 2.f * rand48();
						float dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
						float r2 = 2.f * rand48();
						float dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);

						float rx = ((dx + .5f + sx) * .5f + x) / nx - .5f;
						float ry = .5f - ((dy + .5f + sy) * .5f + y) / ny;
#else
						float rx = (float) x / nx - .5f;
						float ry = .5f - (float) y / ny;
#endif

						vinit(ray.d, rx, ry, 1.f / .2f); // fov
						vnorm(ray.d);

#ifndef RAD_SPP
						radianceBasic(&ray, &col);
#else
						Vec cl;
						radiance(&ray, &cl, 0);
						vsmul(cl, .25f / samps, cl);
						vadd(clr, clr, cl);
					}
#define clamp025(x) clamp((x), 0.f, .25f)
					vinit(clr, clamp025(clr.x), clamp025(clr.y), clamp025(clr.z));
					vadd(col, col, clr);
				}
			}
#endif

			p[x * 3 + 0] = toInt(col.z);
			p[x * 3 + 1] = toInt(col.y);
			p[x * 3 + 2] = toInt(col.x);
		}
	}

	fprintf(stderr, "\rRendering 100.00%%\n");

#ifdef FORMAT
	for (unsigned char *p = buf; p < buf + imgSize; p += 3) {
		fprintf(fp, FORMAT, p[2], p[1], p[0]);
	}
#else
	fwrite(buf, imgSize, 1, fp);
#endif

	fclose(fp);
	free(buf);
	free(scene);

	return 0;
}

void radiance(Ray const *ray, Vec *pcol, int depth)
{
	HitResult res;
	res.t = 1e5f;
	if (-1 == SceneHit(&res, ray, scene, count)) {
		vclr(*pcol);
		return;
	}

	// ambient
	vassign(*pcol, res.c.ambi);

	if (++depth > 5) {
		return;
	} else if (depth > 5) {
		float p = clamp(max(vfilter(res.c.diff), vfilter(res.c.spec)), 0.f, 1.f);
		if (rand48() >= p)
			return;
		vsmul(res.c.diff, 1.f / p, res.c.diff);
		vsmul(res.c.spec, 1.f / p, res.c.spec);
	}

	// specular
	float dn = vdot(ray->d, res.n);

	Ray rad;
	vassign(rad.o, res.p);

	Vec clr;

	if (!viszero(res.c.spec)) {
		vsmul(rad.d, -2 * dn, res.n);
		vadd(rad.d, ray->d, rad.d);
		radiance(&rad, &clr, depth);
		vmul(clr, clr, res.c.spec);
		vadd(*pcol, *pcol, clr);
	}

	// diffuse
	if (!viszero(res.c.diff)) {
		float r1 = 2 * M_PI * rand48();
		float r2 = rand48();
		float r2s = sqrt(r2);

		Vec w;
		vsmul(w, dn < 0.f ? 1.f : -1.f, res.n);
		if (fabsf(w.x) > .1f) {
			vinit(rad.d, -w.z, 0.f, w.x);
		} else {
			vinit(rad.d, 0.f, w.z, -w.y);
		}
		vnorm(rad.d);
		Vec v;
		vxcross(v, w, rad.d);

		vsmul(rad.d, cos(r1) * r2s, rad.d);
		vsmul(v, sin(r1) * r2s, v);
		vsmul(w, sqrt(1 - r2), w);

		vadd(rad.d, rad.d, v);
		vadd(rad.d, rad.d, w);

		radiance(&rad, &clr, depth);
		vmul(clr, clr, res.c.diff);
		vadd(*pcol, *pcol, clr);
	}
}

void radianceBasic(Ray const *ray, Vec *pcol)
{
	Vec col;
	HitResult res;
	res.t = 1e5f;
	if (-1 == SceneHit(&res, ray, scene, count)) {
		vclr(*pcol);
	} else {
		vassign(col, res.c.ambi);
		Vec ldir;
		Vec lpos = {0.f, 19.f, -20.f};
		vsub(ldir, lpos, res.p);
		vnorm(ldir);
		float diff = vdot(res.n, ldir);
		if (diff > 0) {
			Vec dif;
			vassign(dif, res.c.diff);
			vsmul(dif, diff, dif);
			vadd(col, col, dif);
		}
		Vec vdir;
		vsub(vdir, ray->o, res.p);
		vnorm(vdir);
		Vec rrdir;
		vreflect(rrdir, ldir, res.n);
		float vdr = -vdot(vdir, rrdir);
		if (vdr > 0) {
			float spec = pow(vdr, 16);
			Vec spc;
			vassign(spc, res.c.spec);
			vsmul(spc, spec, spc);
			vadd(col, col, spc);
		}

		vsmul(*pcol, .68f, col);
	}
}
