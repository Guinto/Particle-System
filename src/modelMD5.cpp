#include "model.h"

/**
 * Used to compute W.
 */
void Quat_computeW (quat4_t q) {
   float t = 1.0f - (q[X] * q[X]) - (q[Y] * q[Y]) - (q[Z] * q[Z]);

   if (t < 0.0f) {
      q[W] = 0.0f;
   }
   else {
      q[W] = -sqrt (t);
   }
}

/**
 * Used to find the normal.
 */
void Quat_normalize (quat4_t q) {
   /* compute magnitude of the quaternion */
   float mag = sqrt ((q[X] * q[X]) + (q[Y] * q[Y])
                     + (q[Z] * q[Z]) + (q[W] * q[W]));

   /* check for bogus length, to protect against divide by zero */
   if (mag > 0.0f) {
      /* normalize it */
      float oneOverMag = 1.0f / mag;

      q[X] *= oneOverMag;
      q[Y] *= oneOverMag;
      q[Z] *= oneOverMag;
      q[W] *= oneOverMag;
   }
}

/**
 * Used to create multQuart.
 */
void Quat_multQuat (const quat4_t qa, const quat4_t qb, quat4_t out) {
   out[W] = (qa[W] * qb[W]) - (qa[X] * qb[X]) - (qa[Y] * qb[Y]) - (qa[Z] * qb[Z]);
   out[X] = (qa[X] * qb[W]) + (qa[W] * qb[X]) + (qa[Y] * qb[Z]) - (qa[Z] * qb[Y]);
   out[Y] = (qa[Y] * qb[W]) + (qa[W] * qb[Y]) + (qa[Z] * qb[X]) - (qa[X] * qb[Z]);
   out[Z] = (qa[Z] * qb[W]) + (qa[W] * qb[Z]) + (qa[X] * qb[Y]) - (qa[Y] * qb[X]);
}

/**
 * Used multiple Vec.
 */
void Quat_multVec (const quat4_t q, const vec3_t v, quat4_t out) {
   out[W] = - (q[X] * v[X]) - (q[Y] * v[Y]) - (q[Z] * v[Z]);
   out[X] =   (q[W] * v[X]) + (q[Y] * v[Z]) - (q[Z] * v[Y]);
   out[Y] =   (q[W] * v[Y]) + (q[Z] * v[X]) - (q[X] * v[Z]);
   out[Z] =   (q[W] * v[Z]) + (q[X] * v[Y]) - (q[Y] * v[X]);
}

/**
 * Used to rotate the points.
 */
void Quat_rotatePoint (const quat4_t q, const vec3_t in, vec3_t out) {
   quat4_t tmp, inv, final;

   inv[X] = -q[X]; inv[Y] = -q[Y];
   inv[Z] = -q[Z]; inv[W] =  q[W];

   Quat_normalize (inv);

   Quat_multVec (q, in, tmp);
   Quat_multQuat (tmp, inv, final);

   out[X] = final[X];
   out[Y] = final[Y];
   out[Z] = final[Z];
}

/**
 * More quaternion operations for skeletal animation.
 */

/**
 * Find the dot product.
 */
float Quat_dotProduct (const quat4_t qa, const quat4_t qb) {
   return ((qa[X] * qb[X]) + (qa[Y] * qb[Y]) + (qa[Z] * qb[Z]) + (qa[W] * qb[W]));
}

/**
 * Getting slerps.
 */
void Quat_slerp (const quat4_t qa, const quat4_t qb, float t, quat4_t out) {
   /* Check for out-of range parameter and return edge points if so */
   if (t <= 0.0) {
      memcpy (out, qa, sizeof(quat4_t));
      return;
   }

   if (t >= 1.0) {
      memcpy (out, qb, sizeof (quat4_t));
      return;
   }

   /* Compute "cosine of angle between quaternions" using dot product */
   float cosOmega = Quat_dotProduct (qa, qb);

   /* If negative dot, use -q1.  Two quaternions q and -q
      represent the same rotation, but may produce
      different slerp.  We chose q or -q to rotate using
      the acute angle. */
   float q1w = qb[W];
   float q1x = qb[X];
   float q1y = qb[Y];
   float q1z = qb[Z];

   if (cosOmega < 0.0f) {
      q1w = -q1w;
      q1x = -q1x;
      q1y = -q1y;
      q1z = -q1z;
      cosOmega = -cosOmega;
   }

   /* We should have two unit quaternions, so dot should be <= 1.0 */
   assert (cosOmega < 1.1f);

   /* Compute interpolation fraction, checking for quaternions
      almost exactly the same */
   float k0, k1;

   if (cosOmega > 0.9999f) {
      /* Very close - just use linear interpolation,
         which will protect againt a divide by zero */

      k0 = 1.0f - t;
      k1 = t;
   }
   else {
      /* Compute the sin of the angle using the
         trig identity sin^2(omega) + cos^2(omega) = 1 */
      float sinOmega = sqrt (1.0f - (cosOmega * cosOmega));

      /* Compute the angle from its sin and cosine */
      float omega = atan2 (sinOmega, cosOmega);

      /* Compute inverse of denominator, so we only have
         to divide once */
      float oneOverSinOmega = 1.0f / sinOmega;

      /* Compute interpolation parameters */
      k0 = sin ((1.0f - t) * omega) * oneOverSinOmega;
      k1 = sin (t * omega) * oneOverSinOmega;
   }

   /* Interpolate and return new quaternion */
   out[W] = (k0 * qa[3]) + (k1 * q1w);
   out[X] = (k0 * qa[0]) + (k1 * q1x);
   out[Y] = (k0 * qa[1]) + (k1 * q1y);
   out[Z] = (k0 * qa[2]) + (k1 * q1z);
}

/**
 * Load an MD5 model from file.
 */
int model::ReadMD5Model (const char *filename, struct md5_model_t *mdl) {
   FILE *fp;
   char buff[512];
   int version;
   int curr_mesh = 0;
   unsigned int i;

   fp = fopen (filename, "rb");
   if (!fp) {
      fprintf (stderr, "Error: couldn't open \"%s\"!\n", filename);
      return 0;
   }

   while (!feof (fp)) {
      /* Read whole line */
      errno = 0;
      if (fgets (buff, sizeof (buff), fp) == NULL){
         if (errno != 0) {
            perror("fgets");
         }
      }

      if (sscanf (buff, " MD5Version %d", &version) == 1) {
         if (version != 10) {
            /* Bad version */
            fprintf (stderr, "Error: bad model version\n");
            fclose (fp);
            return 0;
         }
      }
      else if (sscanf (buff, " numJoints %d", &mdl->num_joints) == 1) {
         if (mdl->num_joints > 0) {
            /* Allocate memory for base skeleton joints */
            mdl->baseSkel = (struct md5_joint_t *)
            calloc (mdl->num_joints, sizeof (struct md5_joint_t));
         }
      }
      else if (sscanf (buff, " numMeshes %d", &mdl->num_meshes) == 1) {
         if (mdl->num_meshes > 0) {
            /* Allocate memory for meshes */
            mdl->meshes = (struct md5_mesh_t *)
            calloc (mdl->num_meshes, sizeof (struct md5_mesh_t));
         }
      }
      else if (strncmp (buff, "joints {", 8) == 0) {
         /* Read each joint */
         for (i = 0; (int)i < mdl->num_joints; ++i) {
            struct md5_joint_t *joint = &mdl->baseSkel[i];

            /* Read whole line */
            errno = 0;
            if (fgets (buff, sizeof (buff), fp) == NULL) {
               if (errno != 0) {
                  perror("fgets");
               }
            }

            if (sscanf (buff, "%s %d ( %f %f %f ) ( %f %f %f )",
                     joint->name, &joint->parent, &joint->pos[0],
                     &joint->pos[1], &joint->pos[2], &joint->orient[0],
                     &joint->orient[1], &joint->orient[2]) == 8) {
               /* Compute the w component */
               Quat_computeW (joint->orient);
            }
         }
      }
      else if (strncmp (buff, "mesh {", 6) == 0) {
         struct md5_mesh_t *mesh = &mdl->meshes[curr_mesh];
         int vert_index = 0;
         int tri_index = 0;
         int weight_index = 0;
         float fdata[4];
         int idata[3];

         while ((buff[0] != '}') && !feof (fp)) {
            /* Read whole line */
            errno = 0;
            if (fgets (buff, sizeof (buff), fp) == NULL) {
               if (errno != 0) {
                  perror("fgets");
               }
            }

            if (strstr (buff, "shader ")) {
               int quote = 0, j = 0;

               /* Copy the shader name without the quote marks */
               for (i = 0; i < sizeof (buff) && (quote < 2); ++i) {
                  if (buff[i] == '\"') {
                     quote++;
                  }

                  if ((quote == 1) && (buff[i] != '\"')) {
                     mesh->shader[j] = buff[i];
                     j++;
                  }
               }
            }
            else if (sscanf (buff, " numverts %d", &mesh->num_verts) == 1) {
               if (mesh->num_verts > 0) {
                  /* Allocate memory for vertices */
                  mesh->vertices = (struct md5_vertex_t *)
                  malloc (sizeof (struct md5_vertex_t) * mesh->num_verts);
               }

               if (mesh->num_verts > max_verts) {
                  max_verts = mesh->num_verts;
               }
            }
            else if (sscanf (buff, " numtris %d", &mesh->num_tris) == 1) {
               if (mesh->num_tris > 0) {
                  /* Allocate memory for triangles */
                  mesh->triangles = (struct md5_triangle_t *)
                  malloc (sizeof (struct md5_triangle_t) * mesh->num_tris);
               }

               if (mesh->num_tris > max_tris) {
                  max_tris = mesh->num_tris;
               }
            }
            else if (sscanf(buff, " numweights %d", &mesh->num_weights) == 1) {
               if (mesh->num_weights > 0) {
                  /* Allocate memory for vertex weights */
                  mesh->weights = (struct md5_weight_t *)
                  malloc (sizeof (struct md5_weight_t) * mesh->num_weights);
               }
            }
            else if (sscanf(buff, " vert %d ( %f %f ) %d %d", &vert_index,
                  &fdata[0], &fdata[1], &idata[0], &idata[1]) == 5) {
               /* Copy vertex data */
               mesh->vertices[vert_index].st[0] = fdata[0];
               mesh->vertices[vert_index].st[1] = fdata[1];
               mesh->vertices[vert_index].start = idata[0];
               mesh->vertices[vert_index].count = idata[1];
            }
            else if (sscanf(buff, " tri %d %d %d %d", &tri_index,
                  &idata[0], &idata[1], &idata[2]) == 4) {
               /* Copy triangle data */
               mesh->triangles[tri_index ].index[0] = idata[0];
               mesh->triangles[tri_index ].index[1] = idata[1];
               mesh->triangles[tri_index ].index[2] = idata[2];
            }
            else if (sscanf (buff, " weight %d %d %f ( %f %f %f )",
                  &weight_index, &idata[0], &fdata[3],
                  &fdata[0], &fdata[1], &fdata[2]) == 6) {
               /* Copy vertex data */
               mesh->weights[weight_index].joint  = idata[0];
               mesh->weights[weight_index].bias   = fdata[3];
               mesh->weights[weight_index].pos[0] = fdata[0];
               mesh->weights[weight_index].pos[1] = fdata[1];
               mesh->weights[weight_index].pos[2] = fdata[2];
            }
         }
         curr_mesh++;
      }
   }
   fclose (fp);

   return 1;
}

/**
 * Load an MD5 animation from file.
 */
int model::ReadMD5Anim (const char *filename, struct md5_anim_t *anim)
{
   FILE *fp = NULL;
   char buff[512];
   struct joint_info_t *jointInfos = NULL;
   struct baseframe_joint_t *baseFrame = NULL;
   float *animFrameData = NULL;
   int version;
   int numAnimatedComponents;
   int frame_index;
   int i;

   fp = fopen (filename, "rb");
   if (!fp)
      {
         fprintf (stderr, "error: couldn't open \"%s\"!\n", filename);
         return 0;
      }

   while (!feof (fp))
      {
         /* Read whole line */
         if (fgets (buff, sizeof (buff), fp) == NULL) {
            if (errno != 0) {
               perror("fgets");
            }
         }

         if (sscanf (buff, " MD5Version %d", &version) == 1)
            {
               if (version != 10)
                  {
                     /* Bad version */
                     fprintf (stderr, "Error: bad animation version\n");
                     fclose (fp);
                     return 0;
                  }
            }
         else if (sscanf (buff, " numFrames %d", &anim->num_frames) == 1)
            {
               /* Allocate memory for skeleton frames and bounding boxes */
               if (anim->num_frames > 0)
                  {
                     anim->skelFrames = (struct md5_joint_t **)
                        malloc (sizeof (struct md5_joint_t*) * anim->num_frames);
                     anim->bboxes = (struct md5_bbox_t *)
                        malloc (sizeof (struct md5_bbox_t) * anim->num_frames);
                  }
            }
         else if (sscanf (buff, " numJoints %d", &anim->num_joints) == 1)
            {
               if (anim->num_joints > 0)
                  {
                     for (i = 0; i < anim->num_frames; ++i)
                        {
                           /* Allocate memory for joints of each frame */
                           anim->skelFrames[i] = (struct md5_joint_t *)
                              malloc (sizeof (struct md5_joint_t) * anim->num_joints);
                        }

                     /* Allocate temporary memory for building skeleton frames */
                     jointInfos = (struct joint_info_t *)
                        malloc (sizeof (struct joint_info_t) * anim->num_joints);

                     baseFrame = (struct baseframe_joint_t *)
                        malloc (sizeof (struct baseframe_joint_t) * anim->num_joints);
                  }
            }
         else if (sscanf (buff, " frameRate %d", &anim->frameRate) == 1)
            {
               /*
                 printf ("md5anim: animation's frame rate is %d\n", anim->frameRate);
               */
            }
         else if (sscanf (buff, " numAnimatedComponents %d", &numAnimatedComponents) == 1)
            {
               if (numAnimatedComponents > 0)
                  {
                     /* Allocate memory for animation frame data */
                     animFrameData = (float *)malloc (sizeof (float) * numAnimatedComponents);
                  }
            }
         else if (strncmp (buff, "hierarchy {", 11) == 0)
            {
               for (i = 0; i < anim->num_joints; ++i)
                  {
                     /* Read whole line */
                     errno = 0;
                     if (fgets (buff, sizeof (buff), fp) == NULL) {
                        if (errno != 0) {
                           perror("fgets");
                        }
                     }

                     /* Read joint info */
                     sscanf (buff, " %s %d %d %d", jointInfos[i].name, &jointInfos[i].parent,
                             &jointInfos[i].flags, &jointInfos[i].startIndex);
                  }
            }
         else if (strncmp (buff, "bounds {", 8) == 0)
            {
               for (i = 0; i < anim->num_frames; ++i)
                  {
                     /* Read whole line */
                     errno = 0;
                     if (fgets (buff, sizeof (buff), fp) == NULL) {
                        if (errno != 0) {
                           perror("fgets");
                        }
                     }

                     /* Read bounding box */
                     sscanf (buff, " ( %f %f %f ) ( %f %f %f )",
                             &anim->bboxes[i].min[0], &anim->bboxes[i].min[1],
                             &anim->bboxes[i].min[2], &anim->bboxes[i].max[0],
                             &anim->bboxes[i].max[1], &anim->bboxes[i].max[2]);
                  }
            }
         else if (strncmp (buff, "baseframe {", 10) == 0)
            {
               for (i = 0; i < anim->num_joints; ++i)
                  {
                     /* Read whole line */
                     errno = 0;
                     if (fgets (buff, sizeof (buff), fp) == NULL) {
                        if (errno != 0) {
                           perror("fgets");
                        }
                     }

                     /* Read base frame joint */
                     if (sscanf (buff, " ( %f %f %f ) ( %f %f %f )",
                                 &baseFrame[i].pos[0], &baseFrame[i].pos[1],
                                 &baseFrame[i].pos[2], &baseFrame[i].orient[0],
                                 &baseFrame[i].orient[1], &baseFrame[i].orient[2]) == 6)
                        {
                           /* Compute the w component */
                           Quat_computeW (baseFrame[i].orient);
                        }
                  }
            }
         else if (sscanf (buff, " frame %d", &frame_index) == 1)
            {
               /* Read frame data */
               for (i = 0; i < numAnimatedComponents; ++i)
                  errno = 0;
                  if (fscanf (fp, "%f", &animFrameData[i]) == EOF) {
                     if (errno != 0) {
                        perror("fscanf");
                     }
                  }

               /* Build frame skeleton from the collected data */
               BuildFrameSkeleton (jointInfos, baseFrame, animFrameData,
                                   anim->skelFrames[frame_index], anim->num_joints);
            }
      }

   fclose (fp);

   /* Free temporary data allocated */
   if (animFrameData)
      free (animFrameData);

   if (baseFrame)
      free (baseFrame);

   if (jointInfos)
      free (jointInfos);

   return 1;
}

/**
 * Free resources allocated for the model.
 */
void model::FreeModel (struct md5_model_t *mdl)
{
   int i;

   if (mdl->baseSkel)
      {
         free (mdl->baseSkel);
         mdl->baseSkel = NULL;
      }

   if (mdl->meshes)
      {
         /* Free mesh data */
         for (i = 0; i < mdl->num_meshes; ++i)
            {
               if (mdl->meshes[i].vertices)
                  {
                     free (mdl->meshes[i].vertices);
                     mdl->meshes[i].vertices = NULL;
                  }

               if (mdl->meshes[i].triangles)
                  {
                     free (mdl->meshes[i].triangles);
                     mdl->meshes[i].triangles = NULL;
                  }

               if (mdl->meshes[i].weights)
                  {
                     free (mdl->meshes[i].weights);
                     mdl->meshes[i].weights = NULL;
                  }
            }

         free (mdl->meshes);
         mdl->meshes = NULL;
      }
}

float Dot (vec3_t a,vec3_t b)
{
   return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}


float Vector3DLength (float x, float y, float z) {
   return sqrt(x*x + y*y + z*z);
}

/**
 * Prepare a mesh for drawing.  Compute mesh's final vertex positions
 * given a skeleton.  Put the vertices in vertex arrays.
 */
void model::PrepareMesh (const struct md5_mesh_t *mesh,
                         const struct md5_joint_t *skeleton)
{
   int i, j, k;
   //vec3_t v1, v2;

   /* Setup vertex indices */
   for (k = 0, i = 0; i < mesh->num_tris; ++i) 
      {
         for (j = 0; j < 3; ++j, ++k)
            vertexIndices[k] = mesh->triangles[i].index[j];
      }

   /* Setup vertices */
   for (i = 0; i < mesh->num_verts; ++i)
      {
         vec3_t finalVertex = { 0.0f, 0.0f, 0.0f };

         /* Calculate final vertex to draw with weights */
         for (j = 0; j < mesh->vertices[i].count; ++j)
            {
               const struct md5_weight_t *weight
                  = &mesh->weights[mesh->vertices[i].start + j];
               const struct md5_joint_t *joint
                  = &skeleton[weight->joint];

               /* Calculate transformed vertex for this weight */
               vec3_t wv;
               Quat_rotatePoint (joint->orient, weight->pos, wv);

               /* The sum of all weight->bias should be 1.0 */
               finalVertex[0] += (joint->pos[0] + wv[0]) * weight->bias;
               finalVertex[1] += (joint->pos[1] + wv[1]) * weight->bias;
               finalVertex[2] += (joint->pos[2] + wv[2]) * weight->bias;
            }

         vertexArray[i][0] = finalVertex[0];
         vertexArray[i][1] = finalVertex[1];
         vertexArray[i][2] = finalVertex[2];

         texArray[i][0] = mesh->vertices[i].st[0];
         texArray[i][1] = mesh->vertices[i].st[1];
      }
}

void model::AllocVertexArrays ()
{
   vertexArray = (vec3_t *)malloc (sizeof (vec3_t) * max_verts); texArray = (vec2_t *)malloc (sizeof (vec2_t) * max_verts);
   vertexIndices = (GLuint *)malloc (sizeof (GLuint) * max_tris * 3);
}

void model::FreeVertexArrays ()
{
   if (vertexArray)
      {
         free (vertexArray);
         vertexArray = NULL;
      }

   if (texArray) {
      free (texArray);
      texArray = NULL;
   }

   if (vertexIndices)
      {
         free (vertexIndices);
         vertexIndices = NULL;
      }
}

/**
 * Draw the skeleton as lines and points (for joints).
 */
void model::DrawSkeleton (const struct md5_joint_t *skeleton, int num_joints)
{
   int i;

   /* Draw each joint */
   glPointSize (5.0f);
   glColor3f (1.0f, 0.0f, 0.0f);
   glBegin (GL_POINTS);
   for (i = 0; i < num_joints; ++i)
      glVertex3fv (skeleton[i].pos);
   glEnd ();
   glPointSize (1.0f);

   /* Draw each bone */
   glColor3f (0.0f, 1.0f, 0.0f);
   glBegin (GL_LINES);
   for (i = 0; i < num_joints; ++i)
      {
         if (skeleton[i].parent != -1)
            {
               glVertex3fv (skeleton[skeleton[i].parent].pos);
               glVertex3fv (skeleton[i].pos);
            }
      }
   glEnd();
}

void model::changeAnimation(const char *animfile)
{
   if(animfile)
      {
         if (!ReadMD5Anim (animfile, &md5anim))
            {
               FreeAnim (&md5anim);
            }
         else
            {
               animInfo.curr_frame = 0;
               animInfo.next_frame = 1;
               
               animInfo.last_time = 0;
               animInfo.max_time = 1.0 / md5anim.frameRate;
		      
               /* Allocate memory for animated skeleton */
               skeleton = (struct md5_joint_t *)
                  malloc (sizeof (struct md5_joint_t) * md5anim.num_joints);

               animated = 1;
            }
      }
}

void model::init (const char *filename, const char *animfile)
{
   /* Initialize OpenGL context */
   //glClearColor (1.0f, 1.0f, 1.0f, 1.0f);
   // glShadeModel (GL_SMOOTH);
   // glEnable (GL_DEPTH_TEST);

   /* Load MD5 model file */
   if (!ReadMD5Model (filename, &md5file))
      exit (EXIT_FAILURE);

   AllocVertexArrays ();
   skeleton = md5file.baseSkel;
   PrepareMesh (&md5file.meshes[0], skeleton);

   /* Load MD5 animation file */
   if (animfile) {
      if (!ReadMD5Anim (animfile, &md5anim)) {
         FreeAnim (&md5anim);
      }
      else {
         animInfo.curr_frame = 0;
         animInfo.next_frame = 1;
         
         animInfo.last_time = 0;
         animInfo.max_time = 1.0 / md5anim.frameRate;
               
         /* Allocate memory for animated skeleton */
         skeleton = (struct md5_joint_t *)
            malloc (sizeof (struct md5_joint_t) * md5anim.num_joints);
         
         animated = 1;
      }
   }
   
   // if (!animated)
   //    printf ("init: no animation loaded.\n");
}


/**
 * Check if an animation can be used for a given model.  Model's
 * skeleton and animation's skeleton must match.
 */
int CheckAnimValidity (const struct md5_model_t *mdl,
                       const struct md5_anim_t *anim)
{
   int i;

   /* md5mesh and md5anim must have the same number of joints */
   if (mdl->num_joints != anim->num_joints)
      return 0;

   /* We just check with frame[0] */
   for (i = 0; i < mdl->num_joints; ++i)
      {
         /* Joints must have the same parent index */
         if (mdl->baseSkel[i].parent != anim->skelFrames[0][i].parent)
            return 0;

         /* Joints must have the same name */
         if (strcmp (mdl->baseSkel[i].name, anim->skelFrames[0][i].name) != 0)
            return 0;
      }

   return 1;
}

/**
 * Build skeleton for a given frame data.
 */
void model::BuildFrameSkeleton (const struct joint_info_t *jointInfos,
                                const struct baseframe_joint_t *baseFrame,
                                const float *animFrameData,
                                struct md5_joint_t *skelFrame,
                                int num_joints)
{
   int i;

   for (i = 0; i < num_joints; ++i)
      {
         const struct baseframe_joint_t *baseJoint = &baseFrame[i];
         vec3_t animatedPos;
         quat4_t animatedOrient;
         int j = 0;

         memcpy (animatedPos, baseJoint->pos, sizeof (vec3_t));
         memcpy (animatedOrient, baseJoint->orient, sizeof (quat4_t));

         if (jointInfos[i].flags & 1) /* Tx */
            {
               animatedPos[0] = animFrameData[jointInfos[i].startIndex + j];
               ++j;
            }

         if (jointInfos[i].flags & 2) /* Ty */
            {
               animatedPos[1] = animFrameData[jointInfos[i].startIndex + j];
               ++j;
            }

         if (jointInfos[i].flags & 4) /* Tz */
            {
               animatedPos[2] = animFrameData[jointInfos[i].startIndex + j];
               ++j;
            }

         if (jointInfos[i].flags & 8) /* Qx */
            {
               animatedOrient[0] = animFrameData[jointInfos[i].startIndex + j];
               ++j;
            }

         if (jointInfos[i].flags & 16) /* Qy */
            {
               animatedOrient[1] = animFrameData[jointInfos[i].startIndex + j];
               ++j;
            }

         if (jointInfos[i].flags & 32) /* Qz */
            {
               animatedOrient[2] = animFrameData[jointInfos[i].startIndex + j];
               ++j;
            }

         /* Compute orient quaternion's w value */
         Quat_computeW (animatedOrient);

         /* NOTE: we assume that this joint's parent has
            already been calculated, i.e. joint's ID should
            never be smaller than its parent ID. */
         struct md5_joint_t *thisJoint = &skelFrame[i];

         int parent = jointInfos[i].parent;
         thisJoint->parent = parent;
         strcpy (thisJoint->name, jointInfos[i].name);

         /* Has parent? */
         if (thisJoint->parent < 0)
            {
               memcpy (thisJoint->pos, animatedPos, sizeof (vec3_t));
               memcpy (thisJoint->orient, animatedOrient, sizeof (quat4_t));
            }
         else
            {
               struct md5_joint_t *parentJoint = &skelFrame[parent];
               vec3_t rpos; /* Rotated position */

               /* Add positions */
               Quat_rotatePoint (parentJoint->orient, animatedPos, rpos);
               thisJoint->pos[0] = rpos[0] + parentJoint->pos[0];
               thisJoint->pos[1] = rpos[1] + parentJoint->pos[1];
               thisJoint->pos[2] = rpos[2] + parentJoint->pos[2];

               /* Concatenate rotations */
               Quat_multQuat (parentJoint->orient, animatedOrient, thisJoint->orient);
               Quat_normalize (thisJoint->orient);
            }
      }
}

/**
 * Free resources allocated for the animation.
 */
void model::FreeAnim (struct md5_anim_t *anim)
{
   int i;

   if (anim->skelFrames)
      {
         for (i = 0; i < anim->num_frames; ++i)
            {
               if (anim->skelFrames[i])
                  {
                     free (anim->skelFrames[i]);
                     anim->skelFrames[i] = NULL;
                  }
            }

         free (anim->skelFrames);
         anim->skelFrames = NULL;
      }

   if (anim->bboxes)
      {
         free (anim->bboxes);
         anim->bboxes = NULL;
      }
}

/**
 * Smoothly interpolate two skeletons
 */
void model::InterpolateSkeletons (const struct md5_joint_t *skelA,
                                  const struct md5_joint_t *skelB,
                                  int num_joints, float interp,
                                  struct md5_joint_t *out)
{
   int i;

   // printf("interpolating\n");

   for (i = 0; i < num_joints; ++i)
      {
         /* Copy parent index */
         out[i].parent = skelA[i].parent;

         /* Linear interpolation for position */
         out[i].pos[0] = skelA[i].pos[0] + interp * (skelB[i].pos[0] - skelA[i].pos[0]);
         out[i].pos[1] = skelA[i].pos[1] + interp * (skelB[i].pos[1] - skelA[i].pos[1]);
         out[i].pos[2] = skelA[i].pos[2] + interp * (skelB[i].pos[2] - skelA[i].pos[2]);

         /* Spherical linear interpolation for orientation */
         Quat_slerp (skelA[i].orient, skelB[i].orient, interp, out[i].orient);
      }
}

/**
 * Perform animation related computations.  Calculate the current and
 * next frames, given a delta time.
 */
void model::Animate (struct md5_anim_t *anim,
                     struct anim_info_t *animInfo, double dt)
{
   int maxFrames = anim->num_frames - 1;

   animInfo->last_time += dt;

   /* move to next frame */
   if (animInfo->last_time >= animInfo->max_time)
      {
         animInfo->curr_frame++;
         animInfo->next_frame++;
         animInfo->last_time = 0.0;

         if (animInfo->curr_frame > maxFrames)
            animInfo->curr_frame = 0;

         if (animInfo->next_frame > maxFrames)
            animInfo->next_frame = 0;
      }
}

void model::findBound() {
   float maxx, maxy, maxz, minx, miny, minz;
   minx = maxx = vertexArray[1][0];
   miny = maxy = vertexArray[1][1];
   minz = maxz = vertexArray[1][2];

   for (int i = 0; i < max_verts; ++i)
      {
         if (vertexArray[i][0] < minx)
            minx = vertexArray[i][0];
         else if (vertexArray[i][0] > maxx) 
            maxx = vertexArray[i][0];

         if (vertexArray[i][1] < miny)
            miny = vertexArray[i][1];
         else if (vertexArray[i][1] > maxy) 
            maxy = vertexArray[i][1];

         if (vertexArray[i][2] < minz)
            minz = vertexArray[i][2];
         else if (vertexArray[i][2] > maxz) 
            maxz = vertexArray[i][2];
      }

   /*xmax = maxx * scale;  
   xmin = minx * scale;
   zmax = maxy * scale;
   zmin = miny * scale;
   ymax = maxz * scale;
   ymin = minz * scale;
   float tMax = max(abs((xmax) - (xmin)), abs((ymax) - (ymin)));
   radius = max(abs(tMax), abs((zmax) - (zmin)));
   fx = 0;   
   fy = ymax;
   fz = 0;*/
   //printf("AABB: X[%f %f] Y[%f %f] Z[%f %f]\n",xmin,xmax,ymin,ymax,zmin,zmax);
}
// animates based on velocity
void model::draw(float velmagn) {
   static double curent_time = 0;
   static double last_time = 0;
   float constant = 20; // constant multiplied with dt and velocity to adjust animation speed
   double dtorg, dt;

   last_time = curent_time;
   curent_time = (double)glutGet (GLUT_ELAPSED_TIME) / 1000.0;

   glPushMatrix();
   glScalef(scale,scale,scale);
   glRotatef (-90.0f, 1.0, 0.0, 0.0);

   if (animated)
      {
         dtorg = curent_time - last_time;
         dt =  constant * velmagn * (curent_time - last_time);

         /* Calculate current and next frames */
         Animate (&md5anim, &animInfo, dt);

         /* Interpolate skeletons between two frames */
         InterpolateSkeletons (md5anim.skelFrames[animInfo.curr_frame],
                               md5anim.skelFrames[animInfo.next_frame],
                               md5anim.num_joints,
                               animInfo.last_time * md5anim.frameRate,
                               skeleton);
      }
   else
      {
         // No animation, use bind-pose skeleton
         skeleton = md5file.baseSkel;
      }

   /* Draw skeleton */
   // DrawSkeleton (skeleton, md5file.num_joints);

   /*glBindTexture(GL_TEXTURE_2D, texture.id);
   glEnable(GL_TEXTURE_2D);

   glColor3f (1.0f, 1.0f, 1.0f);

   if (!models){
      glColor3f (0.0f, 0.0f, 0.0f);
      if (classId == TREE_ID) glColor3f (1.0f, 1.0f, 1.0f);
   } else {
      if (classId == TREE_ID) glColor3f (0.0f, 0.0f, 0.0f);
   }*/
   glEnableClientState (GL_VERTEX_ARRAY);

   glEnableClientState (GL_TEXTURE_COORD_ARRAY);

   /* Draw each mesh of the model */

   for (int i = 0; i < md5file.num_meshes; ++i)
      {
         PrepareMesh (&md5file.meshes[i], skeleton);

         glVertexPointer (3, GL_FLOAT, 0, vertexArray);
         glTexCoordPointer (2, GL_FLOAT, 0, texArray);

         glDrawElements (GL_TRIANGLES, md5file.meshes[i].num_tris * 3,
                         GL_UNSIGNED_INT, vertexIndices);
      }

   glDisableClientState (GL_VERTEX_ARRAY);
   glPopMatrix();
}

void model::draw() {
   static double curent_time = 0;
   static double last_time = 0;
   //float constant = 20; // constant multiplied with dt and velocity to adjust animation speed
   //double dtorg, dt;

   last_time = curent_time;
   curent_time = (double)glutGet (GLUT_ELAPSED_TIME) / 1000.0;

   glPushMatrix();
   glScalef(scale,scale,scale);
   glRotatef (-90.0f, 1.0, 0.0, 0.0);

  /* if (animated)
      {
         dtorg = curent_time - last_time;
         dt =  constant * velmagn * (curent_time - last_time);*/

         /* Calculate current and next frames */
         //Animate (&md5anim, &animInfo, dt);

         /* Interpolate skeletons between two frames */
        /* InterpolateSkeletons (md5anim.skelFrames[animInfo.curr_frame],
                               md5anim.skelFrames[animInfo.next_frame],
                               md5anim.num_joints,
                               animInfo.last_time * md5anim.frameRate,
                               skeleton);
      }
   else
      {*/
         // No animation, use bind-pose skeleton
         skeleton = md5file.baseSkel;
      //}

   /* Draw skeleton */
   // DrawSkeleton (skeleton, md5file.num_joints);

   /*glBindTexture(GL_TEXTURE_2D, texture.id);
   glEnable(GL_TEXTURE_2D);

   glColor3f (1.0f, 1.0f, 1.0f);
     if (!models){
      glColor3f (0.0f, 0.0f, 0.0f);
      if (classId == TREE_ID) glColor3f (1.0f, 1.0f, 1.0f);
   } else {
      if (classId == TREE_ID) glColor3f (0.0f, 0.0f, 0.0f);
   }*/

   glEnableClientState (GL_VERTEX_ARRAY);

   glEnableClientState (GL_TEXTURE_COORD_ARRAY);

   /* Draw each mesh of the model */

   for (int i = 0; i < md5file.num_meshes; ++i)
      {
         PrepareMesh (&md5file.meshes[i], skeleton);

         glVertexPointer (3, GL_FLOAT, 0, vertexArray);
         glTexCoordPointer (2, GL_FLOAT, 0, texArray);

         glDrawElements (GL_TRIANGLES, md5file.meshes[i].num_tris * 3,
                         GL_UNSIGNED_INT, vertexIndices);
      }

   glDisableClientState (GL_VERTEX_ARRAY);
   glPopMatrix();
}

void model::drawNormal() {
   static double curent_time = 0;
   static double last_time = 0;
   //float constant = 20; // constant multiplied with dt and velocity to adjust animation speed
   double dtorg, dt;

   last_time = curent_time;
   curent_time = (double)glutGet (GLUT_ELAPSED_TIME) / 1000.0;

   glPushMatrix();
   glScalef(scale,scale,scale);
   glRotatef (-90.0f, 1.0, 0.0, 0.0);

   //if (animated)
   //   {
         dtorg = curent_time - last_time;
         dt =  (curent_time - last_time);

         /* Calculate current and next frames */
         Animate (&md5anim, &animInfo, dt);

         /* Interpolate skeletons between two frames */
         InterpolateSkeletons (md5anim.skelFrames[animInfo.curr_frame],
                               md5anim.skelFrames[animInfo.next_frame],
                               md5anim.num_joints,
                               animInfo.last_time * md5anim.frameRate,
                               skeleton);
/**     
	 }
   else
      {**/
         // No animation, use bind-pose skeleton
         //skeleton = md5file.baseSkel;
      //}

   /* Draw skeleton */
   // DrawSkeleton (skeleton, md5file.num_joints);

   /*glBindTexture(GL_TEXTURE_2D, texture.id);
   glEnable(GL_TEXTURE_2D);

   glColor3f (1.0f, 1.0f, 1.0f);
   if (!models){
      glColor3f (0.0f, 0.0f, 0.0f);
      if (classId == TREE_ID) glColor3f (1.0f, 1.0f, 1.0f);
   } else {
      if (classId == TREE_ID) glColor3f (0.0f, 0.0f, 0.0f);
   }*/
   glEnableClientState (GL_VERTEX_ARRAY);

   glEnableClientState (GL_TEXTURE_COORD_ARRAY);

   /* Draw each mesh of the model */

   for (int i = 0; i < md5file.num_meshes; ++i)
      {
         PrepareMesh (&md5file.meshes[i], skeleton);

         glVertexPointer (3, GL_FLOAT, 0, vertexArray);
         glTexCoordPointer (2, GL_FLOAT, 0, texArray);

         glDrawElements (GL_TRIANGLES, md5file.meshes[i].num_tris * 3,
                         GL_UNSIGNED_INT, vertexIndices);
      }

   glDisableClientState (GL_VERTEX_ARRAY);
   glPopMatrix();
}

void model::drawWireframe(){
   /*glColor3f((glowBrightness+wireFrameBrightness)*(edgeColor.x+((float)(rand()%5)/40.0)),
                           (glowBrightness+wireFrameBrightness)*(edgeColor.y+((float)(rand()%5)/40.0)),
                           (glowBrightness+wireFrameBrightness)*(edgeColor.z+((float)(rand()%5)/40.0)));
   glBegin(GL_LINE_LOOP);
   glVertex3f(xmin,ymin,zmin);
   glVertex3f(xmin,ymin,zmax);
   glVertex3f(xmin,ymax,zmax);
   glVertex3f(xmin,ymax,zmin);
   glEnd();
   glBegin(GL_LINE_LOOP);
   glVertex3f(xmax,ymin,zmin);
   glVertex3f(xmax,ymin,zmax);
   glVertex3f(xmax,ymax,zmax);
   glVertex3f(xmax,ymax,zmin);
   glEnd();
   glBegin(GL_LINES);
   glVertex3f(xmax,ymin,zmin);
   glVertex3f(xmin,ymin,zmin);
   glVertex3f(xmax,ymax,zmax);
   glVertex3f(xmin,ymax,zmax);
   glVertex3f(xmax,ymin,zmax);
   glVertex3f(xmin,ymin,zmax);
   glVertex3f(xmax,ymax,zmin);
   glVertex3f(xmin,ymax,zmin);
   glEnd(); */
}
