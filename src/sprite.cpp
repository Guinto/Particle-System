#include "sprite.h"

/**
 * Default constructor for the sprite class.
 */
sprite::sprite() {
   is_looping = true;
   is_playing = true;
   id = 0;
   frame = 0;
   seconds_per_frame = 8;
}

/**
 * Loads the textures for the sprite to play.
 */
void sprite::load(char *tex, int num_frames) {
   bool does_exist = false;
   char texture[64];
   GLuint loaded_id;

   if (num_frames < 1) {
      return;
   }

   if (num_frames == 1) {
      sprintf(texture, "textures/%s", tex);
      id = loadTexture(texture, &does_exist);
      return;
   }

   for (int i = 0; i < num_frames; i++) {
      if (does_exist) {
         indexes.push_back(loaded_id + i);
      }
      else {
         sprintf(texture, "textures/%s/%s%03d", tex, tex, i);
         loaded_id = loadTexture(texture, &does_exist);
         indexes.push_back(loaded_id); 
      }
   }
   id = indexes[frame];
}

/**
 * Updates the frame of the animated texture.
 */
void sprite::update(float dt) {
   if (indexes.size() <= 1) { 
      return;
   }

   if (time >= seconds_per_frame) {
      if ((unsigned int)frame >= indexes.size() - 1) {
         frame = 0;
      }
      else {
         frame++;
      }
      id = indexes[frame];
      time = 0;
   }

   if (is_playing) {
      time += dt;
   }
}

/**
 * Plays the animated texture.
 */
void sprite::play() {
   is_playing = true;
}

/**
 * Pauses the animated texture.
 */
void sprite::pause() {
   is_playing = false;
}
