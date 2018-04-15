 #include <math.h>   // smallpt, a Path Tracer by Kevin Beason, 2008
 #include <stdlib.h> // Make : g++ -O3 -fopenmp smallpt.cpp -o smallpt
 #include <stdio.h>  //        Remove "-fopenmp" for g++ version < 4.2

 struct Vec {        // Usage: time ./smallpt 5000 && xv image.ppm
   double x, y, z;                  // position, also color (r,g,b)
   Vec(double x_=0, double y_=0, double z_=0){ x=x_; y=y_; z=z_; }
   Vec operator+(const Vec &b) const { return Vec(x+b.x,y+b.y,z+b.z); }
   Vec operator-(const Vec &b) const { return Vec(x-b.x,y-b.y,z-b.z); }
   Vec operator*(double b) const { return Vec(x*b,y*b,z*b); }
   Vec mult(const Vec &b) const { return Vec(x*b.x,y*b.y,z*b.z); }
   Vec& norm(){ return *this = *this * (1/sqrt(x*x+y*y+z*z)); }
   double dot(const Vec &b) const { return x*b.x+y*b.y+z*b.z; } // cross:
   Vec operator%(Vec&b){return Vec(y*b.z-z*b.y,z*b.x-x*b.z,x*b.y-y*b.x);}
 };
 struct Ray { Vec o, d; Ray(Vec o_, Vec d_) : o(o_), d(d_) {} };
 enum Refl_t { DIFF, SPEC, REFR };  // material types, used in radiance()
 struct Sphere {
   double rad;       // radius
   Vec p, e, c;      // position, emission, color
   Refl_t refl;      // reflection type (DIFFuse, SPECular, REFRactive)
   Sphere(double rad_, Vec p_, Vec e_, Vec c_, Refl_t refl_):
     rad(rad_), p(p_), e(e_), c(c_), refl(refl_) {}
   double intersect(const Ray &r) const { // returns distance, 0 if nohit
     Vec op = p-r.o; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
     double t, eps=1e-4, b=op.dot(r.d), det=b*b-op.dot(op)+rad*rad;
     if (det<0) return 0; else det=sqrt(det);
     return (t=b-det)>eps ? t : ((t=b+det)>eps ? t : 0);
   }
 };

 Sphere spheres[] = {//Scene: radius, position, emission, color, material 
   Sphere(1e5, Vec( 1e5+1,40.8,81.6), Vec(),Vec(.75,.25,.25),DIFF),//Left 
   Sphere(1e5, Vec(-1e5+99,40.8,81.6),Vec(),Vec(.25,.25,.75),DIFF),//Rght 
   Sphere(1e5, Vec(50,40.8, 1e5),     Vec(),Vec(.75,.75,.75),DIFF),//Back 
   Sphere(1e5, Vec(50,40.8,-1e5+170), Vec(),Vec(),           DIFF),//Frnt 
   Sphere(1e5, Vec(50, 1e5, 81.6),    Vec(),Vec(.75,.75,.75),DIFF),//Botm 
   Sphere(1e5, Vec(50,-1e5+81.6,81.6),Vec(),Vec(.75,.75,.75),DIFF),//Top 
   Sphere(16.5,Vec(27,16.5,47),       Vec(),Vec(1,1,1)*.999, SPEC),//Mirr 
   Sphere(16.5,Vec(73,16.5,78),       Vec(),Vec(1,1,1)*.999, REFR),//Glas 
   Sphere(600, Vec(50,681.6-.27,81.6),Vec(12,12,12),  Vec(), DIFF) //Lite 
 }; 

#define vcomma(v) (v).x, (v).y, (v).z
int main() {
	printf("%d\n", sizeof(spheres) / sizeof(spheres[0]));
	for (auto sphere: spheres) {
		printf("%f %f %f %f ", sphere.rad, vcomma(sphere.p), vcomma(sphere.e));
		if (sphere.refl == DIFF) {
			// diff spec
			printf("%f %f %f 0 0 0\n", vcomma(sphere.c));
		} else if (sphere.refl == SPEC) {
			// diff spec
			printf("0 0 0 %f %f %f\n", vcomma(sphere.c));
		} else if (sphere.refl == REFR) {
			// diff spec
			printf("0 0 0 %f %f %f\n", vcomma(sphere.c));
		} else {
			return -1;
		}
	}
}
