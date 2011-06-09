#include "ground.h"

void ground::drawFloor() {
   int size = 100;
   min = pnt3d(-size, -0.1, -size);
   min = pnt3d(size, 0.1, size);
   pnt3d draw;

   glPushMatrix(); {
      glColor3f(rgb[0], rgb[1], rgb[2]);
      glBegin(GL_LINES); {
         /* Verticle Lines */
         draw = pnt3d(pos.x - size / 2, pos.y, pos.z - size / 2);
         for (int i = 0; i < size; i++) {
            glVertex3f(draw.x, draw.y, draw.z);
            draw.z += size;
            glVertex3f(draw.x, draw.y, draw.z);
            draw.z -= size;
            draw.x++;
         }
         /* Horizontal Lines */
         draw = pnt3d(pos.x - size / 2, pos.y, pos.z - size / 2);
         for (int i = 0; i < size; i++) {
            glVertex3f(draw.x, draw.y, draw.z);
            draw.x += size;
            glVertex3f(draw.x, draw.y, draw.z);
            draw.x -= size;
            draw.z++;
         }
      } glEnd();
   } glPopMatrix();
}

void ground::setRGB(float new_rgb[]) {
   rgb[0] = new_rgb[0];
   rgb[1] = new_rgb[1];
   rgb[2] = new_rgb[2];
}

void ground::draw() {
   glPushMatrix(); {
      drawFloor();
   } glPopMatrix();
}

void ground::update(float dt) {
   /* TODO change when needed */
}
