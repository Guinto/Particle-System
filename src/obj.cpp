#include "obj.h"

/**
 * Draws the arrows for moving position.
 */
void obj::drawArrows() {
   glPushMatrix(); {
      glColor3f(1, 0, 0);
      glTranslatef(pos.x + 0.8, pos.y, pos.z);
      glScalef(1.6, 0.1, 0.1);
      glutSolidCube(1.0);
   } glPopMatrix();
   glPushMatrix(); {
      glColor3f(0, 1, 0);
      glTranslatef(pos.x, pos.y + 0.8, pos.z);
      glScalef(0.1, 1.6, 0.1);
      glutSolidCube(1.0);
   } glPopMatrix();
   glPushMatrix(); {
      glColor3f(0, 0, 1);
      glTranslatef(pos.x, pos.y, pos.z + 0.8);
      glScalef(0.1, 0.1, 1.6);
      glutSolidCube(1.0);
   } glPopMatrix();
}

/**
 * Draws the bounding box of the object.
 */
void obj::drawBoundingBox() {
   glPushMatrix(); {
      glColor3f(1, 1, 1);
      glTranslatef(pos.x, pos.y, pos.z);
      glScalef(max.x - min.x, max.y - min.y, max.z - min.z);
      glutWireCube(1.0);
   } glPopMatrix();

}

/**
 * Draws the default 3d Object.
 */
void obj::draw() {
   glPushMatrix(); {
      glTranslatef(pos.x, pos.y, pos.z);
      glutSolidCube(1.0);
#if 0
      for (int i = 0; i < surface.size(); i++) { //Loop through surfaces
      glBegin(GL_POLYGON); {
         /* Loop through verts */
         for (int j = 0; j < surfaces[i].verts.size(); j++) {
            glVertex3f(surface[i].verts[j].x, surface[i].verts[j].y, 
                  surface[i].verts[j].z);
         }
      } glEnd();
#endif
   } glPopMatrix();
}

/**
 * Updates a 3d object.
 */
void obj::update(float dt) {
   /* TODO fill when objects have stuff */
}

/**
 * Creates a string for specific obj variables.
 */
char* obj::toString() {
   return pos.toString();
}

/**
 * Reads a string for specific obj variables.
 */
void obj::readString(char *s) {
   pos.readString(s);
}

/**
 * Normalizes a point.
 */
pnt3d pnt3d::normalize(pnt3d self) {
   return self / self.mag();
}

/**
 * Finds the dot product of two points.
 */
float pnt3d::dotProduct(pnt3d o) {
   return x * o.x + y * o.y + z * o.z;
}

/**
 * Finds the cross product between two points.
 */
pnt3d pnt3d::crossProduct(pnt3d o) {
   return pnt3d(y * o.z - z * o.y,
                z * o.x - x * o.z,
                x * o.y - y * o.x);
}

/**
 * Finds the distance between 2 points.
 */
float pnt3d::distBetween(pnt3d o) {
   return sqrt((o.x - x) * (o.x - x) + (o.y - y) * (o.y - y) +
         (o.z - z) * (o.z - z));
}

/**
 * Finds the distance between point and vecotr.
 */
float pnt3d::distBetweenV(vec3d o) {
   return sqrt((o.x - x) * (o.x - x) + (o.y - y) * (o.y - y) +
         (o.z - z) * (o.z - z));
}

/**
 * Creates a string for the pnt3d.
 */
char* pnt3d::toString() {
   char *s;
   s = (char*)malloc(64 * sizeof(char));
   sprintf(s, "%f %f %f", x, y, z);
   return s;
}

/**
 * Reads a string for the pnt3d.
 */
void pnt3d::readString(char *s) {
   char *token;
   char *saveptr;
   token = strtok_r(s, " ", &saveptr); sscanf(token, "%f", &x);
   token = strtok_r(NULL, " ", &saveptr); sscanf(token, "%f", &y);
   token = strtok_r(NULL, " ", &saveptr); sscanf(token, "%f", &z);
}
