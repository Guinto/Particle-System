#include "model.h"

char *readLine(FILE *mesh);

/**
 * Constructs the model based on the file provided.
 */
model::model(char *file) {
   classId = MODEL_ID;
   FILE *mesh;
   char *line;
   float num[3];

   mesh = fopen(file, "r");
   if (mesh == NULL) {
      perror(file);
      exit(EXIT_FAILURE);
   }
   
   while (1) {
      line = readLine(mesh);
      if (line == NULL) {
         break;
      }
      if (!strncmp(line, "#", 1)) {
         continue;
      }
      if (!strncmp(line, "Face", 4)) {
         break;;
      }  
      strtok(line, " "); strtok(NULL, " ");
      for (int i = 0; i < 3; i++) {
         sscanf(strtok(NULL, " "), "%f", &num[i]);
      }
      points.push_back(new pnt3d(num[0], num[1], num[2]));
   }
   resize();
}

/**
 * Converts model data to a string format.
 */
char* model::toString() {
   char *s;
   s = points[0]->toString();
   for (unsigned int i = 1; i < points.size(); i++) {
      sprintf(s, "%s %s,", s, points[i]->toString());
   }
   return s;
}

/**
 * Reads model data from a string.
 */
void model::readString(char *s) {
   char *token;
   points.clear();
   token = strtok(s, ",");
   pnt3d *insert;
   insert->readString(token);
   points.push_back(insert);
   for (int i = 0; token != NULL; i++) { 
      token = strtok(NULL, ",");
      if (token == NULL) {
         return;
      }

      pnt3d *insert;
      insert->readString(token);
      points.push_back(insert);
   }

   fprintf(stderr, "No mesh points\n");

}

/**
 * Sets the object to a standard size with the center at 0, 0, 0.
 */
void model::resize() {
   float largest[3]; 
   float smallest[3];
   float change;
   largest[0] = INT_MIN; largest[1] = INT_MIN; largest[2] = INT_MIN;
   smallest[0] = INT_MAX; smallest[1] = INT_MAX; smallest[2] = INT_MAX;

   for (unsigned int i = 0; i < points.size(); i++) {
      if (points[i]->x > largest[0]) {
        largest[0] = points[i]->x; 
      }
      if (points[i]->y > largest[1]) {
        largest[1] = points[i]->y; 
      }
      if (points[i]->z > largest[2]) {
        largest[2] = points[i]->z; 
      }
      if (points[i]->x < smallest[0]) {
        smallest[0] = points[i]->x; 
      }
      if (points[i]->y < smallest[1]) {
        smallest[1] = points[i]->y; 
      }
      if (points[i]->z < smallest[2]) {
        smallest[2] = points[i]->z; 
      }
   }
   float diff;
   diff = largest[0] - smallest[0];
   if (largest[1] - smallest[1] > diff) diff = largest[1] - smallest[1];
   if (largest[2] - smallest[2] > diff) diff = largest[2] - smallest[2];
   change = 10/diff;

   for (unsigned int i = 0; i < points.size(); i++) {
      points[i]->x -= (largest[0] + smallest[0]) / 2;
      points[i]->x *= change; 
      points[i]->y -= (largest[1] + smallest[1]) / 2;
      points[i]->y *= change;
      points[i]->z -= (largest[2] + smallest[2]) / 2;
      points[i]->z *= change;
   }
}

/** 
 * Draws the model.
 */
void model::draw() {
   glPushMatrix(); {
      glColor3f(1, 1, 1);
      glScalef(15, 15, 15);
      glBegin(GL_POINTS); {
         for (unsigned int i = 0; i < points.size(); i++) {
            glVertex3f(points[i]->x, points[i]->y, points[i]->z);
         }
      } glEnd();
   } glPopMatrix();
}

/**
 * Used to read one line from a file.
 */
char *readLine(FILE *mesh) {
   int size;
   char *line;
   char c;

   line = (char*)malloc(sizeof(char));
   size = 0;

   while (1) {
      c = fgetc(mesh);
      if (c == EOF) {
         return NULL;
      }
      if (c == '\n') {
         line = (char*)realloc(line, (size + 1) * sizeof(char));
         line[size++] = '\0';
         return line;
      }
      
      line = (char*)realloc(line, (size + 1) * sizeof(char));
      line[size++] = c;
   }
   return NULL;
}
