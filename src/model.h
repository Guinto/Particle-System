#ifndef MODEL
#define MODEL

#include "util.h"
#include "obj.h"
#include "string.h"
#include "limits.h"

class model: public obj {
   public:
      model() { }
      model(char *file);

      // Overriden functions
      void draw();
      char* toString();
      void readString(char *s);

      // Other functions
      void resize();

      std::vector<pnt3d*> points;
};

#endif
