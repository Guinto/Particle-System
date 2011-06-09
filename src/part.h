#ifndef PART
#define PART

#include "obj.h"
#include "obj2d.h"
#include "model.h"
#include "errno.h"

class part: public obj {
public:
	part(); 
   part(pnt3d p, pnt3d v, pnt3d f, float m, float rgb[3], float e_rgb[3], 
         bool cif, float g, float lt, float s, float es, char *tex, 
         int frames, bool r, char *type);

	// Overriden methods
	void draw();
	void update(float dt);

   // Other methods
   void calcAcc();
   void calcVel();
   void calcPos();
   void getSizeStep();
   void getColorStep();

	pnt3d pos;
	pnt3d vel;
   pnt3d acc;
   pnt3d force;
   float mass;
   float gravity;
   float rgb[3];
   float end_rgb[3];
   float color_step[3];
   bool color_interpolation_flag; // 1 = use end_rgb / 0 = dont use end_rgb
   float lifetime;
   float size;
   float end_size;
   float size_step;
   float dt;
   obj2d o;
   int frames;
   char type[64];
};

class particleSystem: public obj {
public:
	particleSystem() { }
   particleSystem(pnt3d s);

	// Overriden methods
	void draw();
	void update(float dt);
   char* toString();
   void readString(char *s);

   // Particle System methods
   void drawStrips(); void drawPoints();
   void resetSystem(pnt3d s); 
   void createParticle(pnt3d souce, pnt3d source_force);
   bool dist_cmp(part *a, part *b);
   void addParticle();
   void createPoints();
   void getSourceForce();
   void setModels();
   void switchType();
   void upSX(); void downSX();
   void upSY(); void downSY();
   void upSZ(); void downSZ();

   // Particle Methods
   void general(char*);
   void up(char*); void down(char*);
   void upFreq(); void downFreq();
   void upGV(); void downGV();
   void upM(); void downM();
   void upLT(); void downLT();
   void upS(); void downS();
   void upES(); void downES();
   void upR(); void downR();
   void upG(); void downG();
   void upB(); void downB();
   void upER(); void downER();
   void upEG(); void downEG();
   void upEB(); void downEB();
   void upCV(); void downCV();
   void upSV(); void downSV();
   void upLTV(); void downLTV();
   void upPVX(); void downPVX();
   void upPVY(); void downPVY();
   void upPVZ(); void downPVZ();
   void upVX(); void downVX();
   void upVY(); void downVY();
   void upVZ(); void downVZ();
   void upFX(); void downFX();
   void upFY(); void downFY();
   void upFZ(); void downFZ();
   void upVV(); void downVV();
   void upMS(); void downMS();

   // For Source
	pnt3d init_pos;
	pnt3d old_pos; // Used for calculating the source force
   pnt3d source_force;
   std::vector<pnt3d*> points;
   std::list<part*> particles;
   std::vector<char*> models;
   std::list<part*>::iterator it;
   float timer;
   float frequency;
   model mesh;
   float mesh_size;

   // Source Flags
   bool is_moving;
   bool points_flag;
   char type[64];

   // For creating different systems
   pnt3d p, v, f; // position, velocity, force
   float m, g, lt, s, es; // mass, gravity, lifetime, size, end_size
   float rgb[3]; // red, green, blue values
   float e_rgb[3]; // ending red, green, blue values
   char *tex; // texture
   int frames; // num of frames for animated texture
   bool r; // randomizes the 2d obj vectors if set
   bool cif; // 1 = use end_rgb / 0 = dont use end_rgb
   bool retract_flag; // Sets force to -vel

   // For changing different systems
   pnt3d velocity_base; // Base velocity
   pnt3d dpv; // Initial position variance
   pnt3d df; // Force vector
   float dm, dgv, dlt, ds, des; // mass, gravity, lifetime, size, size_end
   float dr, dg, db; // red, green, blue (base color)
   float der, deg, deb; // ending red, green, blue (base color)
   float dcv, dsv; // color/size variance (varies the base color/size)
   float dltv; // liftime variance (varies the base liftime)
   float dvv; // velocity variance (varies the base velocity)
   bool is_random_normal; // sets 2d objest to random normals
   bool is_line; // line_strip == true / triangle_strip == false
   bool is_points; // points == true / strip == false
};

#endif
