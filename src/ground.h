#ifndef GRND
#define GRND

#include "util.h"
#include "obj.h"

class ground: public obj {
   public:
      ground() {
         pos = pnt3d(0, -5, 0);
         rgb[0] = rgb[1] = rgb[2] = 1;
      }
      ground(float new_rgb[]) {
         pos = pnt3d(0, 0, 0);
         rgb[0] = new_rgb[0];
         rgb[1] = new_rgb[1];
         rgb[2] = new_rgb[2];
      }

      //Overriden methods
      void draw();
      void update(float dt);
      char* toString();

      //Other methods
      void setRGB(float new_rgb[]);
      void drawFloor();

      float rgb[3];
};

#endif
