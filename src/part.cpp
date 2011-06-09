#include "part.h"

/**
 * Creates a random long between min & max.
 *
 */
float randomizerL(float min, float max, int decimal) {
   float d = pow(10, decimal); 
   if ((int)(max * d - min * d) == 0) {
      return 0;
   }
   return (rand()%(int)(max * d - min * d)) / d + min;
}

/**
 * The default particle constructor.
 */
part::part() {
   classId = DOT_ID;
   pos = pnt3d(0, 0, 0);
   vel = pnt3d((randomizerL(0, 100, 0) - 50) / 1500, 
               (randomizerL(0, 100, 0) - 50) / 1500, 
               (randomizerL(0, 100, 0) - 50) / 1500);
   force = pnt3d(-vel.x, -vel.y, -vel.z);
   mass = 30;
   calcAcc();
   rgb[0] = rgb[1] = rgb[2] = randomizerL(0, 90, 0)/100;
   end_rgb[0] = end_rgb[1] = end_rgb[2] = 0;
   color_interpolation_flag = false;
   size = randomizerL(1, 5, 0);
   end_size = 0;
   lifetime = randomizerL(30, 80, 0);
   getSizeStep();
   o = obj2d((char *)"lightsource", 1, false);
}

/**
 * Uses to create a unique particle.
 */
part::part(pnt3d p, pnt3d v, pnt3d f, float m, float rgb[3], 
      float e_rgb[3], bool cif, float g, float lt, float s, float es,
      char *tex, int frames, bool r, char *t) {
   classId = DOT_ID;
   pos = pnt3d(p.x, p.y, p.z);
   vel = pnt3d(v.x, v.y, v.z);
   force = pnt3d(f.x, f.y, f.z);
   mass = m;
   gravity = g;
   calcAcc();
   lifetime = lifetime = lt;
   this->rgb[0] = rgb[0]; this->rgb[1] = rgb[1]; this->rgb[2] = rgb[2];
   end_rgb[0] = e_rgb[0]; end_rgb[1] = e_rgb[1]; end_rgb[2] = e_rgb[2];
   color_interpolation_flag = cif;
   if (color_interpolation_flag) getColorStep();
   size = size = s;
   end_size = es;
   getSizeStep();
   o = obj2d(tex, frames, r);
   strcpy(type, t);
}

/**
 * Physics equation to find the current acceleration.
 */
void part::calcAcc() {
   if (mass == 0) {
      acc = pnt3d(0, 0, 0);
   }
   else {
      acc = force / mass;
   }
}

/**
 * Physics equation to find the current velocity.
 */
void part::calcVel() {
   vel = vel + (acc * dt);
}

/**
 * Physics equation to find the current position.
 */
void part::calcPos() {
   pos = pos + (vel * dt + acc * 0.5 * dt * dt);
}

/**
 * Gets the size step based on the begin / end size;
 */
void part::getColorStep() {
   color_step[0] = (rgb[0] - end_rgb[0]) / lifetime; 
   color_step[1] = (rgb[1] - end_rgb[1]) / lifetime; 
   color_step[2] = (rgb[2] - end_rgb[2]) / lifetime; 
}

/**
 * Gets the size step based on the begin / end size;
 */
void part::getSizeStep() {
   size_step = (size - end_size) / lifetime;
}

/**
 * Updates the position of a single particle.
 */
void part::update(float dt) {
   //calcAcc(); //Turn on if Force changes during animation
   this->dt = dt;
   calcVel(); 
   calcPos();
   lifetime -= (int)dt;
   size -= size_step * dt;
   if (color_interpolation_flag) {
      rgb[0] -= color_step[0] * dt;
      rgb[1] -= color_step[1] * dt;
      rgb[2] -= color_step[2] * dt;
   }
   if (!strcmp(type, "obj2d")) {
      o.update(dt);
   }
}

/**
 * Draws a single particle
 */
void part::draw() {
   glPushMatrix();
      glColor3f(rgb[0], rgb[1], rgb[2]);
      glTranslatef(pos.x, pos.y, pos.z);
      glScalef(size, size, size);
      if (!strcmp(type, "cube")) {
         glutSolidCube(1.0);
      }
      else if (!strcmp(type, "obj2d")){
         o.draw();
      }
      else {
         //object.draw();
      }
   glPopMatrix();
}

/**
 * A general function to make simple changes.
 */
void particleSystem::general(char *value) {
   if (!strcmp(value, "cif")) cif = !cif;
   else if (!strcmp(value, "is_moving")) is_moving = false;
   else if (!strcmp(value, "retract_flag")) retract_flag = !retract_flag;
   else if (!strcmp(value, "points_flag")) points_flag = !points_flag;
   else if (!strcmp(value, "random_normals")) {
      is_random_normal = !is_random_normal;
   }
   else if (!strcmp(value, "is_line")) is_line = !is_line;
   else if (!strcmp(value, "is_point")) is_points = !is_points;
   else if (!strcmp(value, "switch_mesh")) createPoints();
   else if (!strcmp(value, "type")) switchType();
}

/**
 * A general up function to dynamically change the corrisponding
 * one inside main.
 */
void particleSystem::up(char *value) {
   if (!strcmp(value, "freq")) upFreq();
   else if (!strcmp(value, "source_x")) upSX();
   else if (!strcmp(value, "source_y")) upSY();
   else if (!strcmp(value, "source_z")) upSZ();
   else if (!strcmp(value, "size_variance")) upSV();
   else if (!strcmp(value, "color_variance")) upCV();
   else if (!strcmp(value, "lifetime_variance")) upLTV();
   else if (!strcmp(value, "velocity_variance")) upVV();
   else if (!strcmp(value, "position_x_variance")) upPVX();
   else if (!strcmp(value, "position_y_variance")) upPVY();
   else if (!strcmp(value, "position_z_variance")) upPVZ();
   else if (!strcmp(value, "velocity_x")) upVX();
   else if (!strcmp(value, "velocity_y")) upVY();
   else if (!strcmp(value, "velocity_z")) upVZ();
   else if (!strcmp(value, "force_x")) upFX();
   else if (!strcmp(value, "force_y")) upFY();
   else if (!strcmp(value, "force_z")) upFZ();
   else if (!strcmp(value, "gravity")) upGV();
   else if (!strcmp(value, "mass")) upM();
   else if (!strcmp(value, "lifetime")) upLT();
   else if (!strcmp(value, "size")) upS();
   else if (!strcmp(value, "end_size")) upES();
   else if (!strcmp(value, "red")) upR();
   else if (!strcmp(value, "green")) upG();
   else if (!strcmp(value, "blue")) upB();
   else if (!strcmp(value, "end_red")) upER();
   else if (!strcmp(value, "end_green")) upEG();
   else if (!strcmp(value, "end_blue")) upEB();
   else if (!strcmp(value, "mesh_size")) upMS();
}

/**
 * A general down function to dynamically change the corrisponding
 * one inside main.
 */
void particleSystem::down(char *value) {
   if (!strcmp(value, "freq")) downFreq();
   else if (!strcmp(value, "source_x")) downSX();
   else if (!strcmp(value, "source_y")) downSY();
   else if (!strcmp(value, "source_z")) downSZ();
   else if (!strcmp(value, "size_variance")) downSV();
   else if (!strcmp(value, "color_variance")) downCV();
   else if (!strcmp(value, "lifetime_variance")) downLTV();
   else if (!strcmp(value, "velocity_variance")) downVV();
   else if (!strcmp(value, "position_x_variance")) downPVX();
   else if (!strcmp(value, "position_y_variance")) downPVY();
   else if (!strcmp(value, "position_z_variance")) downPVZ();
   else if (!strcmp(value, "velocity_x")) downVY();
   else if (!strcmp(value, "velocity_z")) downVZ();
   else if (!strcmp(value, "force_x")) downFX();
   else if (!strcmp(value, "force_y")) downFY();
   else if (!strcmp(value, "force_z")) downFZ();
   else if (!strcmp(value, "gravity")) downGV();
   else if (!strcmp(value, "mass")) downM();
   else if (!strcmp(value, "lifetime")) downLT();
   else if (!strcmp(value, "size")) downS();
   else if (!strcmp(value, "end_size")) downES();
   else if (!strcmp(value, "red")) downR();
   else if (!strcmp(value, "green")) downG();
   else if (!strcmp(value, "blue")) downB();
   else if (!strcmp(value, "end_red")) downER();
   else if (!strcmp(value, "end_green")) downEG();
   else if (!strcmp(value, "end_blue")) downEB();
   else if (!strcmp(value, "mesh_size")) downMS();
}
/** Increase Mass. */
void particleSystem::upM() { 
   (dm < 300) ? dm += LARGE_CHANGE : dm += LARGE_CHANGE * 5; 
} 
/** Decrease Mass. */
void particleSystem::downM() { 
   (dm < 300) ? dm -= LARGE_CHANGE : dm -= LARGE_CHANGE * 5; 
} 
/** Increase Life Time */
void particleSystem::upLT() { 
   (dlt < 300) ? dlt += LARGE_CHANGE : dlt += LARGE_CHANGE * 5; 
} 
/** Decrease Life Time */
void particleSystem::downLT() { 
   if (dlt < -dltv) return;
   (dlt < 300) ? dlt -= LARGE_CHANGE : dlt -= LARGE_CHANGE * 5; 
} 
/** Increase Frequency */
void particleSystem::upFreq() { 
   if (frequency > 2) {
      frequency -= MEDIUM_CHANGE * 5; 
   } 
   else if (frequency > 0.04) {
      frequency -= SMALL_CHANGE;
   }
}
/** Decrease Frequency */
void particleSystem::downFreq() { 
   if (frequency < 2) {
      frequency += SMALL_CHANGE; 
   } 
   else if (frequency < 50) {
      frequency += MEDIUM_CHANGE * 5;
   }
}
/** Increase Gravity */
void particleSystem::upGV() { (dgv > -20) ? dgv += X_SMALL_CHANGE : 0; } 
/** Decrease Gravity */
void particleSystem::downGV() { (dgv < 20) ? dgv -= X_SMALL_CHANGE : 0; } 
/** Increase Size */
void particleSystem::upS() { (ds < 4) ? ds += SMALL_CHANGE : 0; } 
/** Decrease Size */
void particleSystem::downS() { (ds > 0.02) ? ds -= SMALL_CHANGE : 0; }
/** Increase Size End */
void particleSystem::upES() { (des < 40) ? des += SMALL_CHANGE : 0; } 
/** Decrease Size End */
void particleSystem::downES() { (des > 0.1) ? des -= SMALL_CHANGE : 0; }
/** Increase Red */
void particleSystem::upR() { (dr < 1) ? dr += SMALL_CHANGE : 0; } 
/** Decrease Red */
void particleSystem::downR() { (dr > -1) ? dr -= SMALL_CHANGE : 0; } 
/** Increase Green */
void particleSystem::upG() { (dg < 1) ? dg += SMALL_CHANGE : 0; } 
/** Decrease Green */
void particleSystem::downG() { (dg > -1) ? dg -= SMALL_CHANGE : 0; } 
/** Increase Blue */
void particleSystem::upB() { (db < 1) ? db += SMALL_CHANGE : 0; } 
/** Decrease Blue */
void particleSystem::downB() { (db > -1) ? db -= SMALL_CHANGE : 0; } 
/** Increase Red End*/
void particleSystem::upER() { 
   (der < 1) ? der += SMALL_CHANGE : 0; cif = true; 
} 
/** Decrease Red End*/
void particleSystem::downER() { 
   (der > 0) ? der -= SMALL_CHANGE : 0; cif = true; 
} 
/** Increase Green End*/
void particleSystem::upEG() { 
   (deg < 1) ? deg += SMALL_CHANGE : 0; cif = true; 
} 
/** Decrease Green End*/
void particleSystem::downEG() { 
   (deg > 0) ? deg -= SMALL_CHANGE : 0; cif = true; 
} 
/** Increase Blue End*/
void particleSystem::upEB() { 
   (deb < 1) ? deb += SMALL_CHANGE : 0; cif = true; 
} 
/** Decrease Blue End*/
void particleSystem::downEB() { 
   (deb > 0) ? deb -= SMALL_CHANGE : 0; cif = true; 
} 
/** Increase Color Variance */
void particleSystem::upCV() { (dcv < 100) ? dcv += MEDIUM_CHANGE * 3 : 0; } 
/** Decrease Color Variance */
void particleSystem::downCV() { (dcv > 1) ? dcv -= MEDIUM_CHANGE * 3: 0; } 
/** Increase Size Variance */
void particleSystem::upSV() { (dsv < 2) ? dsv += SMALL_CHANGE : 0; } 
/** Decrease Size Variance */
void particleSystem::downSV() { (dsv > 0.01) ? dsv -= SMALL_CHANGE : 0; } 
/** Increase LifeTime Variance */
void particleSystem::upLTV() { (dltv < 300) ? dltv += LARGE_CHANGE : 0; }
/** Decrease LifeTime Variance */
void particleSystem::downLTV() { (dltv > 0.1) ? dltv -= LARGE_CHANGE : 0; } 
/** Increase X Point Variance  */
void particleSystem::upPVX() { dpv.x += SMALL_CHANGE; }
/** Decrease X Point Variance  */
void particleSystem::downPVX() { (dpv.x > 0) ? dpv.x -= SMALL_CHANGE : 0; } 
/** Increase Y Point Variance  */
void particleSystem::upPVY() { dpv.y += SMALL_CHANGE; }
/** Decrease Y Point Variance  */
void particleSystem::downPVY() { (dpv.y > 0) ? dpv.y -= SMALL_CHANGE : 0; } 
/** Increase Z Point Variance  */
void particleSystem::upPVZ() { dpv.z += SMALL_CHANGE; }
/** Decrease Z Point Variance */
void particleSystem::downPVZ() { (dpv.z > 0) ? dpv.z -= SMALL_CHANGE : 0; } 
/** Increase X Velocity Base */
void particleSystem::upVX() { velocity_base.x += X_SMALL_CHANGE; }
/** Decrease X Velocity Base */
void particleSystem::downVX() { velocity_base.x -= X_SMALL_CHANGE; } 
/** Increase Y Velocity Base */
void particleSystem::upVY() { velocity_base.y += X_SMALL_CHANGE; }
/** Decrease Y Velocity Base */
void particleSystem::downVY() { velocity_base.y -= X_SMALL_CHANGE; } 
/** Increase Z Velocity Base */
void particleSystem::upVZ() { velocity_base.z += X_SMALL_CHANGE; }
/** Decrease Z Velocity Base */
void particleSystem::downVZ() { velocity_base.z -= X_SMALL_CHANGE; } 
/** Increase X Force */
void particleSystem::upFX() { df.x += X_SMALL_CHANGE; }
/** Decrease X Force */
void particleSystem::downFX() { df.x -= X_SMALL_CHANGE; } 
/** Increase Y Force */
void particleSystem::upFY() { df.y += X_SMALL_CHANGE; }
/** Decrease Y Force */
void particleSystem::downFY() { df.y -= X_SMALL_CHANGE; } 
/** Increase Z Force */
void particleSystem::upFZ() { df.z += X_SMALL_CHANGE; }
/** Decrease Z Force */
void particleSystem::downFZ() { df.z -= X_SMALL_CHANGE; } 
/** Increase Velocity Variance */
void particleSystem::upVV() { 
   (dvv < 300) ? dvv += LARGE_CHANGE : dvv += LARGE_CHANGE * 5; 
}
/** Decrease Velocity Variance */
void particleSystem::downVV() { 
   if (dvv < 1) return;
   (dvv < 300) ? dvv -= LARGE_CHANGE : dvv -= LARGE_CHANGE * 5; 
}
/** Increase Mesh Size */
void particleSystem::upMS() { mesh_size += SMALL_CHANGE; }
/** Decrease Mesh Size */
void particleSystem::downMS() { mesh_size -= SMALL_CHANGE; } 

/**
 * Moves the Source.
 */

/** Increase X Source */
void particleSystem::upSX() { 
   is_moving = true;
   old_pos = pos;
   pos.x += MEDIUM_CHANGE; 
}
/** Decrease X Source  */
void particleSystem::downSX() { 
   is_moving = true;
   old_pos = pos;
   pos.x -= MEDIUM_CHANGE;
} 
/** Increase Y Source  */
void particleSystem::upSY() { 
   is_moving = true;
   old_pos = pos;
   pos.y += MEDIUM_CHANGE; 
}
/** Decrease Y Source  */
void particleSystem::downSY() { 
   is_moving = true;
   old_pos = pos;
   pos.y -= MEDIUM_CHANGE; 
}
/** Increase Z Source  */
void particleSystem::upSZ() { 
   is_moving = true;
   old_pos = pos;
   pos.z += MEDIUM_CHANGE; 
}
/** Decrease Z Source */
void particleSystem::downSZ() { 
   is_moving = true;
   old_pos = pos;
   pos.z -= MEDIUM_CHANGE; 
}

/**
 * Switches the type of particle that is emitted.
 */
void particleSystem::switchType() {
   if (!strcmp(type, "cube")) strcpy(type, "obj2d");
   else if (!strcmp(type, "obj2d")) strcpy(type, "cube");
}

/**
 * Sets the vector with random models.
 */
void particleSystem::setModels() {
   models.push_back((char*)"models/bunny.orig.m");
   models.push_back((char*)"models/cessna_color.m");
   models.push_back((char*)"models/dragon10k.m");
   models.push_back((char*)"models/fandisk_10k.m");
   models.push_back((char*)"models/gameguy_noinfo_20k.m");
   models.push_back((char*)"models/gargoyle_2k.m");
   models.push_back((char*)"models/test1.m");
   models.push_back((char*)"models/test_simp1.m");
   models.push_back((char*)"models/tyra_100k.m");
}

/**
 * Constructer for the particle system.
 */
particleSystem::particleSystem(pnt3d s) {
   classId = PART_ID;
   init_pos = s;
   resetSystem(s);
   setModels();
   createPoints();
}

/**
 * Resets the particle system to default settings.
 */
void particleSystem::resetSystem(pnt3d s) {
   /* SOURCE */
   frequency = 1;
   pos = old_pos = s;
   
   /* PARITCLES */
   velocity_base = pnt3d(0, 0, 0);
   dpv = pnt3d(0, 0, 0);
   df = pnt3d(0, 0, 0);
   dm = 0; dgv = 0; dlt = 55; ds = 0.1; des = 0;
   dr = dg = db = 0;
   der = deg = deb = 0;
   dcv = 40; dsv = 0.4; 
   dltv = 25; dvv = 100;

   /* MESH */
   mesh_size = 0.2;
   points.push_back(new pnt3d(0, 0, 0));

   // Flags
   cif = false;
   retract_flag = true;
   points_flag = false;
   is_random_normal = false;
   is_line = true;
   is_points = true;
   strcpy(type, "cube");
}

/**
 * Creates a set of points where the particles will be emitted.
 */
void particleSystem::createPoints() {
   mesh = model(models[rand()%models.size()]);
   points = mesh.points;
}

/**
 * Creates particle.
 * @param s_f The force that comes from the movement of the source.
 */
void particleSystem::createParticle(pnt3d src, pnt3d s_f) {
   /* Physics */
   if (points_flag) {
      int r = rand()%points.size();
      p = pnt3d(src.x + points[r]->x * mesh_size +
            (randomizerL(0, dpv.x, 4) - dpv.x / 2),
                src.y + points[r]->y * mesh_size + 
            (randomizerL(0, dpv.y, 4) - dpv.y / 2),
                src.z + points[r]->z * mesh_size + 
            (randomizerL(0, dpv.z, 4) - dpv.z / 2));
   }
   else {
      p = pnt3d(src.x + (randomizerL(0, dpv.x, 4) - dpv.x / 2), 
                src.y + (randomizerL(0, dpv.y, 4) - dpv.y / 2), 
                src.z + (randomizerL(0, dpv.z, 4) - dpv.z / 2));
   }
   v = pnt3d(s_f.x + velocity_base.x + (randomizerL(0, dvv, 4) - dvv / 2) / 1500,
             s_f.y + velocity_base.y + (randomizerL(0, dvv, 4) - dvv / 2) / 1500,
             s_f.z + velocity_base.z + (randomizerL(0, dvv, 4) - dvv / 2) / 1500);
   g = dgv;
   if (retract_flag) { f = pnt3d(-v.x, -v.y + g, -v.z); }
   else { f = pnt3d(df.x, df.y + g, df.z); }
   m = 30 + dm;

   /* Color */
   float color_base = randomizerL(50 - dcv, 50 + dcv, 4) / 100;
   rgb[0] = color_base + dr;
   rgb[1] = color_base + dg;
   rgb[2] = color_base + db;
   e_rgb[0] = der;
   e_rgb[1] = deg;
   e_rgb[2] = deb;

   /* Size */
   lt = randomizerL(0, dltv, 4) + dlt;
   s = randomizerL(0, dsv, 5) + ds;
   max = pnt3d((dsv + ds) / 2, (dsv + ds) / 2, (dsv + ds) / 2);
   min = pnt3d(-(dsv + ds) / 2, -(dsv + ds) / 2, -(dsv + ds) / 2);
   es = des;

   /* Obj */
   tex = (char*)"lightsource";
   frames = 1;
   r = is_random_normal;
}

/**
 * Used for setting to a string.
 */
char* particleSystem::toString() {
   char *s;
   s = (char*)malloc(5012 * sizeof(char));
   sprintf(s, "%s, %s, %s, %s, %f %f %f %d %s %s %d %d %d %d %s, %s, %s, \
         %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %d %d %d", 
         init_pos.toString(), pos.toString(), old_pos.toString(), 
         source_force.toString(), timer, frequency, 
         mesh_size, points_flag, type, tex, frames, r, cif, retract_flag,
         velocity_base.toString(), dpv.toString(), df.toString(), dm, dgv, 
         dlt, ds, des, dr, dg, db, der, deg, deb, dcv, dsv, dltv, dvv, 
         is_random_normal, is_line, is_points);
   return s;
}

/**
 * Used for setting to a string.
 */
void particleSystem::readString(char *s) {
   char *token;
   int temp;
   token = strtok(s, ","); init_pos.readString(token);
   token = strtok(NULL, ","); pos.readString(token);
   token = strtok(NULL, ","); old_pos.readString(token);
   token = strtok(NULL, ","); source_force.readString(token);
   token = strtok(NULL, " "); sscanf(token, "%f", &timer);
   token = strtok(NULL, " "); sscanf(token, "%f", &frequency);
   token = strtok(NULL, " "); sscanf(token, "%f", &mesh_size);
   token = strtok(NULL, " "); sscanf(token, "%d", &temp);
   points_flag = (bool)temp;
   token = strtok(NULL, " "); strcpy(type, token);
   token = strtok(NULL, " "); tex = token;
   token = strtok(NULL, " "); sscanf(token, "%d", &frames);
   token = strtok(NULL, " "); sscanf(token, "%d", &temp);
   r = (bool)temp;
   token = strtok(NULL, " "); sscanf(token, "%d", &temp);
   cif = (bool)temp;
   token = strtok(NULL, " "); sscanf(token, "%d", &temp);
   retract_flag = (bool)temp;
   token = strtok(NULL, ","); velocity_base.readString(token);
   token = strtok(NULL, ","); dpv.readString(token);
   token = strtok(NULL, ","); df.readString(token);
   token = strtok(NULL, " "); sscanf(token, "%f", &dm);
   token = strtok(NULL, " "); sscanf(token, "%f", &dgv);
   token = strtok(NULL, " "); sscanf(token, "%f", &dlt);
   token = strtok(NULL, " "); sscanf(token, "%f", &ds);
   token = strtok(NULL, " "); sscanf(token, "%f", &des);
   token = strtok(NULL, " "); sscanf(token, "%f", &dr);
   token = strtok(NULL, " "); sscanf(token, "%f", &dg);
   token = strtok(NULL, " "); sscanf(token, "%f", &db);
   token = strtok(NULL, " "); sscanf(token, "%f", &der);
   token = strtok(NULL, " "); sscanf(token, "%f", &deg);
   token = strtok(NULL, " "); sscanf(token, "%f", &deb);
   token = strtok(NULL, " "); sscanf(token, "%f", &dcv);
   token = strtok(NULL, " "); sscanf(token, "%f", &dsv);
   token = strtok(NULL, " "); sscanf(token, "%f", &dltv);
   token = strtok(NULL, " "); sscanf(token, "%f", &dvv);
   token = strtok(NULL, " "); sscanf(token, "%d", &temp);
   is_random_normal = (bool)temp;
   token = strtok(NULL, " "); sscanf(token, "%d", &temp);
   is_line = (bool)temp;
   token = strtok(NULL, " "); sscanf(token, "%d", &temp);
   is_points = (bool)temp;
}

/**
 * Get's the force from the movement of the source.
 */
void particleSystem::getSourceForce() {
   if (is_moving) source_force = old_pos - pos;
   else source_force = pnt3d(0, 0, 0);
}

/**
 * Updates all particles in system.
 */
void particleSystem::update(float dt) {
   timer += dt;
   if (frequency < 50) {
      if (timer > frequency) {
         for (int i = 0; i < (int)(timer / frequency); i++) {
            getSourceForce();
            createParticle(pos, source_force);
            addParticle();
         }
         timer = 0;
      }
   }
   else {
      timer = 0;
   }
   for (it = particles.begin(); it != particles.end(); it++) {
      (*it)->update(dt);
   }
}

/**
 * Adds a new particle to the system.
 */
void particleSystem::addParticle() {
   part *item = new part(p, v, f, m, rgb, e_rgb, cif, g, lt, s, es, tex, 
         frames, r, type);
   if (!strcmp(type, "obj2d")) {
      particles.push_front(item);
   }
   else {
      particles.push_back(item);
   }
}

/**
 * Helps the draw function to draw strips.
 */
void particleSystem::drawStrips() {
   glBegin((is_line) ? GL_LINE_STRIP : GL_TRIANGLE_STRIP); {
      for (it = particles.begin(); it != particles.end(); it++) {
          if ((*it)->lifetime > 0) {
             glColor3f((*it)->rgb[0], (*it)->rgb[1], (*it)->rgb[2]);
             glVertex3f((*it)->pos.x, (*it)->pos.y, (*it)->pos.z);
          } 
          else {
             particles.erase(it++);
          }
      }
   } glEnd();
}

/**
 * Helps the draw function to draw individual points.
 */
void particleSystem::drawPoints() {
   for (it = particles.begin(); it != particles.end(); it++) {
       if ((*it)->lifetime > 0) {
          (*it)->draw();
       } 
       else {
          particles.erase(it++);
       }
   }
}

/**
 * Draws all particles in system.
 */
void particleSystem::draw() {
   if (particles.size() != 0) {
      glPushMatrix(); {
         (is_points) ? drawPoints() : drawStrips();
      } glPopMatrix();
   }
}

/**
 * Comparison that sorts from farthest to closest to camera.
 */
bool particleSystem::dist_cmp(part *a, part *b) {
   if (a->pos.distBetweenV(cam.pos) > b->pos.distBetweenV(cam.pos)) {
      return 1;
   }
   if (a->pos.distBetweenV(cam.pos) < b->pos.distBetweenV(cam.pos)) {
      return -1;
   }
   return 0;
}
