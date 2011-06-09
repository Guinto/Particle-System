#include "camera.h"

/**
 * This constructs the default camera.
 */
camera::camera() {
   pos = vec3d(0, 2, 5, 0, 0, 0);
   w = u = v = pnt3d(0, 0, 0);
   theta = 0;
   phi = 0;
   spin_speed = 0.25;
   cam_spin_flag = true;
   updateLookat();
}

/**
 * Sets up the camera with the lookat positions.
 */
void camera::updateLookat() {
   gluLookAt(pos.x, pos.y, pos.z, pos.px, pos.py, pos.pz, 0, 1, 0);
}

/**
 * Sets up the camera with the lookat positions.
 */
void camera::setFocus(pnt3d spot) {
   pos.px = spot.x;
   pos.py = spot.y;
   pos.pz = spot.z;
}

/**
 * Camera movement function.
 */
/*void camera::move(pnt3d dir) {
   getW();
   pos.x = pos.x - w.x; pos.px = pos.px - w.x;
   pos.y = pos.y - w.y; pos.py = pos.py - w.y;
   pos.z = pos.z - w.z; pos.pz = pos.pz - w.z;
}*/

/**
 * The update function for the camera to change the position of
 * the camera based upon the movemnt of the mouse (used in main).
 */
void camera::update(float dx, float dy) {
   theta += dx;
   phi -= dy;

   if (cam_spin_flag) {
      if (spin_speed > 10) {
         spin_speed = 10;
      }
      else if (spin_speed < -10) {
         spin_speed = -10;
      }
      theta += spin_speed;
   }

   if (phi > 89 || phi  < -89) {
      phi += dy;
   }

   if (theta <= -360 || theta >= 360) {
      theta = 0;
   }

   
   float r;
   r = pos.mag();
   pos.x = r * cos(phi * DEG2RAD) * cos(theta * DEG2RAD);
   pos.y = r * sin(phi * DEG2RAD);
   pos.z = r * cos(phi * DEG2RAD) * cos((90 - theta) * DEG2RAD);
}

/*void camera::getW() {
   w.x = - (pos.px - pos.x) / pos.mag2();
   w.y = - (pos.py - pos.y) / pos.mag2();
   w.z = - (pos.pz - pos.z) / pos.mag2();
}

void camera::getU() {
   getW();
   pnt3d up = pnt3d(0, 1, 0);
   pnt3d cp = up.crossProduct(w);
   u.x = cp.x / cp.mag();
   u.y = cp.y / cp.mag();
   u.z = cp.z / cp.mag();
}

void camera::getV() {
   v = u.crossProduct(w);
}*/
