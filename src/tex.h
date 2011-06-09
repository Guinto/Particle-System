/**
 * Texture functions Provided by Zoe Wood F2010.
 */

#ifndef TEX
#define TEX

#include "util.h"
#include "SOIL.h"
#include <string>

typedef struct Image {
   unsigned long sizeX;
   unsigned long sizeY;
   char *data;
} Image;

int exists(char* image_file);
int imageLoad(char *filename, Image *image);
GLuint loadTexture(char *image_file, bool *does_exist);

#endif
