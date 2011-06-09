#ifndef OBJ
#define OBJ

#include "util.h"

typedef struct vec3d {
   vec3d():x(0), y(0), z(0), px(0), py(0), pz(0) {}
   vec3d(float in_x, float in_y, float in_z, float in_px, float in_py, float in_pz):
      x(in_x), y(in_y), z(in_z), px(in_px), py(in_py), pz(in_pz) {}
   float mag() { return sqrt(x * x + y * y + z * z); }
   float magP() { return sqrt(px * px + py * py + pz * pz); }
   float mag2() { return sqrt((x - px) * (x - px) + 
                              (y - py) * (y - py) +
                              (z - pz) * (z - pz)); }

   float x;
   float y;
   float z;
   float px;
   float py;
   float pz;
} vec3d;

typedef struct pnt3d {
   float x;
   float y;
   float z;
   pnt3d():x(0), y(0), z(0) {};
   pnt3d(float in_x, float in_y, float in_z): x(in_x), y(in_y), z(in_z) {};
   float mag() {return sqrt(x * x + y * y + z * z);}
   pnt3d operator* (float f){ return pnt3d(x * f, y * f, z * f); }
   pnt3d operator/ (float f){ return pnt3d(x / f, y / f, z / f); }
   pnt3d operator+ (pnt3d p){ return pnt3d(x + p.x, y + p.y, z + p.z); }
   pnt3d operator- (pnt3d p){ return pnt3d(x - p.x, y - p.y, z - p.z); }
   void print() { printf("%f, %f, %f\n", x, y, z); }
   void norm() { float m = mag(); x /= m; y /= m; z /= m; }
   pnt3d normalize(pnt3d o);
   float dotProduct(pnt3d o);
   pnt3d crossProduct(pnt3d o);
   float distBetween(pnt3d o);
   float distBetweenV(vec3d o);
   char* toString();
   void readString(char *s);
} pnt3d;

class drawable {
   public:
      int index; //Individual id
      int classId;
      virtual ~drawable() {}
      virtual void draw() = 0;
      virtual void drawArrows() = 0;
      virtual void drawBoundingBox() = 0;
      virtual void update(float dt) = 0;
      virtual char* toString() = 0;
      virtual void readString(char *s) = 0;
      virtual float getX() { return -1; }
      virtual float getY() { return -1; }
      virtual float getZ() { return -1; }
};

class obj : public drawable {
   public:
      obj() { 
         pos = pnt3d(0, 0, 0); 
         min = pnt3d(-1, -1, -1); 
         max = pnt3d(1, 1, 1); 
      }
      virtual ~obj() {}

      // Overriden methods
      virtual void draw();
      virtual void drawArrows();
      virtual void drawBoundingBox();
      virtual void update(float dt);
      virtual char* toString();
      virtual void readString(char *s);

      pnt3d pos;
      pnt3d min;
      pnt3d max;
};

#endif
