#ifndef OBJ2D
#define OBJ2D

#include "util.h"
#include "sprite.h"
#include "camera.h"

extern camera cam;

class obj2d : public obj {
   public:
      obj2d() {}
      obj2d(char *t, int frames, bool is_random_normal);

      // Overriden methods
      void draw();
      void update(float dt);

      // Other methods
      void rotateToNormal();
      void rotateToCamera();

      pnt3d direction;
      sprite texture;

};

#endif
