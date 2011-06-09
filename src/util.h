#ifndef UTIL
#define UTIL

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <list>

#if 1 /*unix*/
#include <GL/glut.h>
#endif

#define DEG2RAD 0.0174532925 //Degrees to Radians
#define RAD2DEG 57.295779579 //Radians to Degrees
#define NUM_KEYS 256

#define LARGE_CHANGE 1
#define MEDIUM_CHANGE 0.1
#define SMALL_CHANGE 0.01
#define X_SMALL_CHANGE 0.001

#define OBJ_ID 1
#define PART_ID 2
#define DOT_ID 3
#define OBJ2D_ID 4
#define MODEL_ID 5

#endif
