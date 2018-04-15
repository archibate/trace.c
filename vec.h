#ifndef _VEC_H
#define	_VEC_H

typedef struct {
	float x, y, z; // position, also color (r,g,b)
} Vec;

#define vinit(v, a, b, c) do { (v).x = (a); (v).y = (b); (v).z = (c); } while (0)
#define vcomma(v) (v).x, (v).y, (v).z
#define vrefcom(v) &(v).x, &(v).y, &(v).z
#define vassign(a, b) vinit(a, (b).x, (b).y, (b).z)
#define vclr(v) vinit(v, 0.f, 0.f, 0.f)
#define vadd(v, a, b) vinit(v, (a).x + (b).x, (a).y + (b).y, (a).z + (b).z)
#define vsub(v, a, b) vinit(v, (a).x - (b).x, (a).y - (b).y, (a).z - (b).z)
#define vsadd(v, a, b) do { float __k = (a); vinit(v, (b).x + __k, (b).y + __k, (b).z + __k); } while (0)
#define vssub(v, a, b) do { float __k = (a); vinit(v, (b).x - __k, (b).y - __k, (b).z - __k); } while (0)
#define vmul(v, a, b) vinit(v, (a).x * (b).x, (a).y * (b).y, (a).z * (b).z)
#define vsmul(v, a, b) do { float __k = (a); vinit(v, __k * (b).x, __k * (b).y, __k * (b).z); } while (0)
#define vdot(a, b) ((a).x * (b).x + (a).y * (b).y + (a).z * (b).z)
#define vnorm(v) do { float __l = 1.f / sqrt(vdot(v, v)); vsmul(v, __l, v); } while (0)
#define vxcross(v, a, b) vinit(v, (a).y * (b).z - (a).z * (b).y, (a).z * (b).x - (a).x * (b).z, (a).x * (b).y - (a).y * (b).x)
#define vfilter(v) ((v).x > (v).y && (v).x > (v).z ? (v).x : (v).y > (v).z ? (v).y : (v).z)
#define viszero(v) (((v).x == 0.f) && ((v).x == 0.f) && ((v).z == 0.f))
#define vreflect(v, d, n) do { \
	vsmul(v, -2 * vdot((n), (d)), (n)); \
	vadd(v, (d), v); \
} while (0)

#ifndef GPU_KERNEL
#define clamp(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))
#define max(x, y) ((x) > (y) ? (x) : (y))
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

#define toInt(x) ((int)(pow(clamp(x, 0.f, 1.f), 1.f / 2.2f) * 255.f + .5f))

// Rendering flags
#define RFLAGS_DISABLE_DIFFUSE_PATH 1

#endif	/* _VEC_H */
