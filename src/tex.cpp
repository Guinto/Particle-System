/**
 * Texture Functions provided by Zoe Wood F2010.
 */

#include "tex.h"

char texture_names [5012][64];
int size = 0;

/**
 * Checks if the texture is already loaded.
 */
int exists(char *image_file) {
   for (int i = 0; i < size; i++) {
      if (!strcmp(texture_names[i], image_file)) {
         return i + 1;
      }
   }
   return -1;
}

/**
 * Loads the texture to be used.
 */
GLuint loadTexture(char *image_file, bool *does_exist) {
   char image_file_png[64];
   char image_file_jpg[64];
   int id = exists(image_file);

   if (id != -1) {
      *does_exist = true;
      return id;
   }

   strcpy(image_file_png, image_file);
   strcpy(image_file_jpg, image_file);
   /* Try with .jpg */
   strcat(image_file_jpg, ".jpg");
   id = SOIL_load_OGL_texture(image_file_jpg, 0, 0, 0);

   /* Try with .png if .jpg doesn't work */
   if (id == 0) {
      strcat(image_file_png, image_file);
      id = SOIL_load_OGL_texture(image_file_png, 0, 0, 0);
   }

   if (id == 0) {
      fprintf(stderr, "Error loading: %s\n", image_file);
   }
   else {
      strcpy(texture_names[size++], image_file);
   }
   return id;
}
