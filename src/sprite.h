#ifndef SPRITE
#define SPRITE

#include "util.h"
#include "tex.h"

class sprite {
   public:
      sprite();

      void update(float dt);
      void load(char *tex, int num_frames);
      void play();
      void pause();

      std::vector<GLuint> indexes;
      int id; // Current texture id
      int frame; // Current frame of the animation
      float seconds_per_frame;
      float time; // Used with seconds_per_frame
      
      // Flags
      bool is_looping;
      bool is_playing;
};

#endif
