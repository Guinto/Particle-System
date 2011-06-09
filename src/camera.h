#ifndef CAM
#define CAM

#include "util.h"
#include "obj.h"

class camera {
   public:
      vec3d pos;
      pnt3d w, u, v;
      float theta;
      float phi;
      float spin_speed;

      // Constructor
      camera();

      // Methods
      void update(float dx, float dy);
      void updateLookat();
      void setFocus(pnt3d spot);
      void move(pnt3d dir);
      void getW(); void getU(); void getV();

      // Flags
      bool cam_spin_flag;
};

#endif
