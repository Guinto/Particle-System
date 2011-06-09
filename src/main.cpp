#include "util.h"
#include "obj.h"
#include "part.h"
#include "camera.h"
#include "ground.h"
#include <time.h>

//Window globals
int GW;
int GH;

//World
camera cam;
float lastx, lasty; //Mouse drag

//Global vars
long start_loop, mouse_loop;
std::vector<drawable*> objs;
std::vector<pnt3d*> points;
float d_time;
bool draw_arrows;
bool draw_bounding_box;
bool follow;

//Screen Saver
std::vector<int> functions;
int ss_choice = 0;
float screen_saver_time = 0;
bool is_screen_saver = false;

//Config settings
float mouse_sensitivity = 10;

//Keyboard
bool key_states[NUM_KEYS] = {false};

//Flags
bool full_screen_flag = false;

void initScene();
void drawScene();
void resetSystem();
void setScreenSaver();
void display();
void reshape(int w, int h);
void loop(int n);
void screenSaver(float dt);
char* readLine(FILE *file);

/**
 * Initializes the scene (used in main).
 */
void initScene() {
   glutSetCursor(GLUT_CURSOR_NONE);
   glutWarpPointer(GW / 2, GH / 2);

   srand(time(NULL));
   resetSystem();
   setScreenSaver();

   cam = camera();
   objs.push_back(new ground());
   objs.push_back(new particleSystem(pnt3d(0, 0, 0)));
   glutTimerFunc(d_time, loop, d_time);
}

/**
 * Draws the scene (used in display).
 */
void drawScene() {
   glMatrixMode(GL_MODELVIEW);
   glEnable(GL_DEPTH_TEST);

   glPushMatrix(); {
      cam.updateLookat();
      for (unsigned int i = 0; i < objs.size(); i++) {
         objs[i]->draw();
         if (draw_arrows) {
            glDisable(GL_DEPTH_TEST);
            objs[i]->drawArrows();
            glEnable(GL_DEPTH_TEST);
         }
         if (draw_bounding_box) {
            objs[i]->drawBoundingBox();
         }
         if (follow && objs[i]->classId == PART_ID) {
            particleSystem *temp = (particleSystem*)objs[i];
            cam.setFocus(temp->pos);
         }
      }
   } glPopMatrix();
}

/**
 * Displays the scene.
 */
void display() {
   glFlush();
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60, ((float)GW)/GH, 0.05, 2000);

   drawScene();
   glutSwapBuffers();
}

/**
 * Converts pixel to world.
 */
float p2wx(int xp) {
   return 2 * ((float)(2 * GW * xp) / (GH * (GW - 1)) - ((float) GW / GH));
}

/**
 * Converts pixel to world.
 */
float p2wy(int yp) {
   return 2 * (((float)(2 * yp) / (GH - 1)) - 1);
}

/**
 * Inverts y.
 */
int g2py(int yp) {
   return (GH - 1) - yp;
}

/**
 * The resizing of the window function.
 */
void reshape(int  w, int h) {
   GW = w;
   GH = h;
   glViewport(0, 0, GW, GH);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glutPostRedisplay();
}

float dx, dy; // Declared outside for efficiency
/**
 * Updates the camera based on the mouse movement.
 */
void mouseMove(int x, int y) {
   dx = mouse_sensitivity * (p2wx(x) - lastx);
   dy = mouse_sensitivity * (p2wy(g2py(y)) - lasty);
   //dy = mouse_sensitivity * (p2wy(y) - lasty);
   
   cam.update(cam.cam_spin_flag ? 0 : dx, dy);

   //lastx = p2wx(x);
   lasty = p2wy(g2py(y));
   lasty = p2wy(y);
   glutPostRedisplay();
}

/**
 * This allows for everything to be reset.
 */
void resetSystem() {
   d_time = 10;
   draw_arrows = false;
   draw_bounding_box = false;
   follow = false;
   for (int i = 0; i < NUM_KEYS; i++) {
      key_states[i] = false;
   }
}

/**
 * Saves the system.
 */
void save() {
   FILE *file;
   file = fopen("save", "w");
   for (unsigned int i = 0; i < objs.size(); i++) {
      fputs(objs[i]->toString(), file);
      fputc('\n', file);
   }
   fclose(file);
}

/**
 * Loads the system.
 */
void load() {
   FILE *file;
   file = fopen("save", "r");

   char *s;
   s = (char*)malloc(5012 * sizeof(char));
   for (unsigned int i = 0; i < objs.size(); i++) {
      s = readLine(file); 
      if (s == NULL) {
         break;
      }
      objs[i]->readString(s);
   }
   fclose(file);
}

/**
 * Reads in a line.
 */
char* readLine(FILE *file) {
   char *line;
   char c;
   int size = 0;
   line = (char*)malloc(sizeof(char));

   while (1) {
      c = fgetc(file);
      if (c == EOF) {
         return NULL;
      }
      line = (char*)realloc(line, (size + 2) * sizeof(char));
      if (c != '\n') {
         line[size++] = c;
      }
      else {
         line[size] = '\0';
         return line;
      }
   }
   return NULL;
}

/**
 * Changes particle values.
 */
void general(char *value) {
   for (unsigned int i = 0; i < objs.size(); i++) {
      if (objs[i]->classId == PART_ID) {
         particleSystem *temp = (particleSystem*)objs[i];
         if (!strcmp(value, "reset")) {
            temp->resetSystem(temp->init_pos);
            cam.spin_speed = 0.25;
            resetSystem();
         }
         else if (!strcmp(value, "screen_saver")) {
            is_screen_saver = !is_screen_saver;
         }
         else {
            temp->general(value);
         }
      }
   }
}

/**
 * Increases particle values.
 */
void up(char *value) {
   for (unsigned int i = 0; i < objs.size(); i++) {
      if (objs[i]->classId == PART_ID) {
         particleSystem *temp = (particleSystem*)objs[i];
         temp->up(value);
      }
   }
}

/**
 * Decreases particle size.
 */
void down(char *value) {
   for (unsigned int i = 0; i < objs.size(); i++) {
      if (objs[i]->classId == PART_ID) {
         particleSystem *temp = (particleSystem*)objs[i];
         temp->down(value);
      }
   }
}

/**
 * Functionality for keys that are held down.
 */
void keyActions() {
   /* GENERAL */
   if (key_states['K']) {
      (d_time < 30) ? d_time += MEDIUM_CHANGE : 0;
   }
   else if (key_states['k']) {
      if (d_time > 20) { d_time -= MEDIUM_CHANGE * 3; }
      else if (d_time > 3) { d_time -= MEDIUM_CHANGE; }
   }

   /* CAMERA */
   if (key_states['='] || key_states['+']) {
      cam.spin_speed += 0.03;
   }
   if (key_states['-'] || key_states['_']) {
      cam.spin_speed -= 0.03;
   }

   /* PARTICLES */
   if (key_states['|'] || key_states['\\']) { general((char*)"reset"); }
   if (key_states['m']) { up((char*)"source_x"); }
   if (key_states['M']) { down((char*)"source_x"); }
   if (key_states[',']) { up((char*)"source_y"); }
   if (key_states['<']) { down((char*)"source_y"); }
   if (key_states['.']) { up((char*)"source_z"); }
   if (key_states['>']) { down((char*)"source_z"); }
   if (key_states['a']) { up((char*)"freq"); }
   if (key_states['A']) { down((char*)"freq"); }
   if (key_states['s']) { up((char*)"size_variance"); }
   if (key_states['S']) { down((char*)"size_variance"); }
   if (key_states['c']) { up((char*)"color_variance"); }
   if (key_states['C']) { down((char*)"color_variance"); }
   if (key_states['l']) { up((char*)"lifetime_variance"); }
   if (key_states['L']) { down((char*)"lifetime_variance"); }
   if (key_states['v']) { up((char*)"velocity_variance"); }
   if (key_states['V']) { down((char*)"velocity_variance"); }
   if (key_states['u']) { up((char*)"position_x_variance"); }
   if (key_states['U']) { down((char*)"position_x_variance"); }
   if (key_states['i']) { up((char*)"position_y_variance"); }
   if (key_states['I']) { down((char*)"position_y_variance"); }
   if (key_states['o']) { up((char*)"position_z_variance"); }
   if (key_states['O']) { down((char*)"position_z_variance"); }
   if (key_states['x']) { up((char*)"velocity_x"); }
   if (key_states['X']) { down((char*)"velocity_x"); }
   if (key_states['y']) { up((char*)"velocity_y"); }
   if (key_states['Y']) { down((char*)"velocity_y"); }
   if (key_states['z']) { up((char*)"velocity_z"); }
   if (key_states['Z']) { down((char*)"velocity_z"); }
   if (key_states['p']) { up((char*)"force_x"); }
   if (key_states['P']) { down((char*)"force_x"); }
   if (key_states['[']) { up((char*)"force_y"); }
   if (key_states['{']) { down((char*)"force_y"); }
   if (key_states[']']) { up((char*)"force_z"); }
   if (key_states['}']) { down((char*)"force_z"); }
   if (key_states['j']) { up((char*)"gravity"); }
   if (key_states['J']) { down((char*)"gravity"); }
   if (key_states['q']) { up((char*)"mass"); }
   if (key_states['Q']) { down((char*)"mass"); }
   if (key_states['w']) { up((char*)"lifetime"); }
   if (key_states['W']) { down((char*)"lifetime"); }
   if (key_states['e']) { up((char*)"end_size"); }
   if (key_states['E']) { down((char*)"end_size"); }
   if (key_states['d']) { up((char*)"size"); }
   if (key_states['D']) { down((char*)"size"); }
   if (key_states['r']) { up((char*)"red"); }
   if (key_states['R']) { down((char*)"red"); }
   if (key_states['g']) { up((char*)"green"); }
   if (key_states['G']) { down((char*)"green"); }
   if (key_states['b']) { up((char*)"blue"); }
   if (key_states['B']) { down((char*)"blue"); }
   if (key_states['t']) { up((char*)"end_red"); }
   if (key_states['T']) { down((char*)"end_red"); }
   if (key_states['h']) { up((char*)"end_green"); }
   if (key_states['H']) { down((char*)"end_green"); }
   if (key_states['n']) { up((char*)"end_blue"); }
   if (key_states['N']) { down((char*)"end_blue"); }
   if (key_states[';']) { up((char*)"mesh_size"); }
   if (key_states[':']) { down((char*)"mesh_size"); }
}

/**
 * Sets the functions desired for the screen saver mode.
 */
void setScreenSaver() {
   for (int i = 65; i <= 90; i++) {
      if (i == 'K' || i == 'M' || i == 'W' || i == 'A') continue;
      functions.push_back(i);   
   }
   for (int i = 97; i <= 122; i++) {
      if (i == 'k' || i == 'm') continue;
      functions.push_back(i);   
   }
   functions.push_back('|');
   functions.push_back('[');
   functions.push_back('{');
   functions.push_back(']');
   functions.push_back('}');
}

/**
 * Screen saver mode randomly goes through all functions.
 */
void screenSaver(float dt) {
   if (screen_saver_time > rand()%100 + 50) {
      key_states[functions[ss_choice]] = false;
      ss_choice = rand()%functions.size();
      key_states[functions[ss_choice]] = true; 
      screen_saver_time = 0;
   }
   screen_saver_time += dt;
}



/**
 * The keyboard up function that recognizes when a key is released.
 */
void keyboardUp(unsigned char key, int x, int y) {
   if (key == 'm' || key == 'M' || key == ',' || key == '<' ||
         key == '.' || key == '>') {
      key_states['m'] = false; key_states['M'] = false;
      key_states[','] = false; key_states['<'] = false;
      key_states['.'] = false; key_states['>'] = false;
      general((char*)"is_moving");
   }
   else if (key >= 65 && key <= 90) {
      key_states[key + 32] = false;
   }
   else if (key >= 97 && key <= 122) {
      key_states[key - 32] = false;
   }
   else if (key == '[' || key == '{') {
      key_states['['] = false;
      key_states['{'] = false;
   }
   else if (key == ']' || key == '}') {
      key_states[']'] = false;
      key_states['}'] = false;
   }
   else if (key == ';' || key == ':') {
      key_states[';'] = false;
      key_states[':'] = false;
   }
   key_states[key] = false;
}

/**
 * The keyboard function that defines keys to be used.
 */
void keyboard(unsigned char key, int x, int y) {
   switch(key) {
      /* GENERAL */
      case 27:
         exit(EXIT_SUCCESS);
         break;
      case 'f': case 'F':
         full_screen_flag = !full_screen_flag;
         if (full_screen_flag) {
            glutFullScreen();
         }
         else {
            GW = 1080; GH = 720;
            glutReshapeWindow(GW, GH);
         }
         break;
      case '2': case '@':
         draw_arrows = !draw_arrows;
         break;
      case '3': case '#':
         draw_bounding_box = !draw_bounding_box;
         break;
      case '4': case '$':
         follow = !follow;
         if (!follow) { cam.setFocus(pnt3d(0, 0, 0)); }
         break;
      case '9':
         load();
         break;
      case '(': 
         save();
         break;


      /* CAMERA */
      case ' ':
         cam.cam_spin_flag = !cam.cam_spin_flag;
         break;

      /* PARTICLES */
      case '/': case '?':
         general((char*)"cif");
         break;
      case '\'': case '\"':
         general((char*)"retract_flag");
         break;
      case '1': case '!':
         objs.push_back(new particleSystem(pnt3d(rand() % 10 - 5,
                                                 rand() % 10 - 5,
                                                 rand() % 10 - 5)));
         break;
      case '5': case '%':
         general((char*)"points_flag");
         break;
      case '6': case '^':
         general((char*)"switch_mesh");
         break;
      case '7': case '&':
         general((char*)"type");
         break;
      case '0': case ')':
         general((char*)"screen_saver");
         break;

      default:
         key_states[key] = true;
         break;
   }
}

/**
 * The loop for the program.
 */
void loop(int n) {
   keyActions();
   long old_start_loop = start_loop;
   start_loop = (long) glutGet(GLUT_ELAPSED_TIME);
   float dt = (float) (start_loop - old_start_loop);
   dt /= d_time;
   
   if (is_screen_saver) {
      screenSaver(dt);
   }

   glutWarpPointer(GW / 2, GH / 2); //Cursor At Center of Screen
   /* Allows movement */
   lastx = p2wx(GW / 2);
   lasty = p2wy(g2py(GH / 2));
   //lasty = p2wy(GH / 2);

   /* Updates all objects in world */
   for (unsigned int i = 0; i < objs.size(); i++) {
      objs[i]->update(dt);
   }

   glutPostRedisplay();
   glutTimerFunc(d_time, loop, d_time);
}

/**
 * The main function.
 */
int main(int argc, char *argv[]) {
   //Set up the window
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   GW = 1080; GH = 720;
   glutInitWindowSize(GW, GH);
   glutInitWindowPosition(300, 50);
   glutCreateWindow("Particle System");
   glClearColor(0.0, 0.0, 0.0, 1.0);

   glutPassiveMotionFunc( mouseMove );
   glutDisplayFunc( display );
   glutReshapeFunc( reshape );
   glutKeyboardFunc ( keyboard );
   glutKeyboardUpFunc ( keyboardUp );
   glutIgnoreKeyRepeat(1);

   initScene();

   glutMainLoop();
}
