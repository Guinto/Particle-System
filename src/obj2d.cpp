#include "obj2d.h"

/**
 * 2D object constructor that should be used.
 */
obj2d::obj2d(char *t, int frames, bool is_random_normal) {
   classId = OBJ2D_ID;
   pos = pnt3d(0, 0, 0);
   if (is_random_normal) {
      direction = pnt3d(rand()%360, rand()%360, rand()%360);
   }
   else {
      direction = pnt3d(0, 0, 0);
   }
   texture.load(t, frames);
}

/**
 * Rotates the 2d object to face the normal.
 */
void obj2d::rotateToNormal() {
   glRotatef(direction.x, 1, 0, 0);
   glRotatef(direction.y, 0, 1, 0);
   glRotatef(direction.z, 0, 0, 1);
}

/**
 * rotates the object towards the camera to create a billboard effect.
 */
void obj2d::rotateToCamera() {
   pnt3d def = pnt3d(0, 0, -1);
   pnt3d path = pnt3d(cam.pos.x, cam.pos.y, cam.pos.z - 1);
   path = path.normalize(path);
   pnt3d cross = def.crossProduct(path);
   float radians = def.dotProduct(path);
   radians = RAD2DEG * acos(radians/(def.mag() * path.mag()));
   glRotatef(radians, cross.x, cross.y, cross.z);
}

/**
 * Draws the specified 2d object.
 */
void obj2d::draw() {
   glPushMatrix(); {
      glTranslatef(pos.x, pos.y, pos.z); 
      rotateToNormal();
      rotateToCamera();

      glEnable (GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
      glEnable(GL_CULL_FACE);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texture.id);

      glBegin(GL_POLYGON); {
         glTexCoord2f(0, 1);
         glVertex3f(-1, 1, 0);
         glTexCoord2f(1, 1);
         glVertex3f(1, 1, 0);
         glTexCoord2f(1, 0);
         glVertex3f(1, -1, 0);
         glTexCoord2f(0, 0);
         glVertex3f(-1, -1, 0);
      } glEnd();

      glDisable(GL_BLEND);
      glDisable(GL_CULL_FACE);
      glDisable(GL_TEXTURE_2D);
   } glPopMatrix();
}

/**
 * The update function for a 2d object.
 */
void obj2d::update(float dt) {
   texture.update(dt);
}
