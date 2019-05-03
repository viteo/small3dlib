/*
  WIP

  Simple realtime 3D software rasterization library. It is fast, focused on
  resource-limited computers, located in a single C header file, with no
  dependencies, using only integer arithmetic.

  author: Miloslav Ciz
  license: CC0 1.0

  --------------------

  This work's goal is to never be encumbered by any exclusive intellectual
  property rights. The work is therefore provided under CC0 1.0 + additional
  WAIVER OF ALL INTELLECTUAL PROPERTY RIGHTS that waives the rest of
  intellectual property rights not already waived by CC0 1.0. The WAIVER OF ALL
  INTELLECTUAL PROPERTY RGHTS is as follows:

  Each contributor to this work agrees that they waive any exclusive rights,
  including but not limited to copyright, patents, trademark, trade dress,
  industrial design, plant varieties and trade secrets, to any and all ideas,
  concepts, processes, discoveries, improvements and inventions conceived,
  discovered, made, designed, researched or developed by the contributor either
  solely or jointly with others, which relate to this work or result from this
  work. Should any waiver of such right be judged legally invalid or
  ineffective under applicable law, the contributor hereby grants to each
  affected person a royalty-free, non transferable, non sublicensable, non
  exclusive, irrevocable and unconditional license to this right.

  --------------------

  CONVENTIONS:

  Angles are in S3L_Units, a full angle (2 pi) is S3L_FRACTIONS_PER_UNITs.

  We use row vectors.

  COORDINATE SYSTEMS:

  In 3D space, a left-handed coord. system is used. One spatial unit is split
  into S3L_FRACTIONS_PER_UNIT fractions (fixed point arithmetic).

     y ^
       |   _ 
       |   /| z
       |  /
       | /
  [0,0,0]-------> x

  Untransformed camera is placed at [0,0,0], looking forward along +z axis. The
  projection plane is centered at [0,0,0], stretrinch from
  -S3L_FRACTIONS_PER_UNIT to S3L_FRACTIONS_PER_UNIT horizontally (x),
  vertical size (y) depends on the camera aspect ratio. Camera FOV is defined
  by focal length.

           y ^
             |  _
             |  /| z
         ____|_/__
        |    |/   |
     -----[0,0,0]-|-----> x
        |____|____|
             |    
             |

  Coordinates of pixels on screen start typically at the top left, from [0,0].

  Rasterization rules are to be the same as in OpenGL or DirectX:

  - Pixel centers are at integer coordinates.
  - Pixel is rasterized if its center is inside the primitive (e.g. a triangle)
    OR
    if its center is exactly on the primitive's side, which is either left
    (no exactly horizontal and on the left side of triangle) or top (exactly
    horizontal and above the other two edges)

  These rules imply e.g.:

  - Triangles of points that lie on a single line are not rasterized.
  - A single "long" triangle can be rasterized as non-continuous.
*/

#ifndef S3L_H
#define S3L_H

#include <stdint.h>

typedef int32_t S3L_Unit; /**< Units of measurement in 3D space. There is
                               S3L_FRACTIONS_PER_UNIT in one spatial unit.
                               By dividing the unit into fractions we
                               effectively achieve fixed point arithmetic.
                               The number of fractions is a constant that
                               serves as 1.0 in floating point arithmetic
                               (normalization etc.). */
#define S3L_FRACTIONS_PER_UNIT 512 /**< How many fractions a spatial unit is
                                        split into. WARNING: if setting
                                        higher than 1024, you'll probably
                                        have to modify a sin table otherwise
                                        it will overflow. Also other things
                                        may overflow, so rather don't do it. */

#define S3L_NONZERO(value) ((value) != 0 ? (value) : 1) /**< prevents division
                                                             by zero */

#define S3L_SIN_TABLE_LENGTH 128
static const S3L_Unit S3L_sinTable[S3L_SIN_TABLE_LENGTH] =
{
  /* 511 was chosen here as a highest number that doesn't overflow during
     compilation for S3L_FRACTIONS_PER_UNIT == 1024 */

  (0*S3L_FRACTIONS_PER_UNIT)/511, (6*S3L_FRACTIONS_PER_UNIT)/511, 
  (12*S3L_FRACTIONS_PER_UNIT)/511, (18*S3L_FRACTIONS_PER_UNIT)/511, 
  (25*S3L_FRACTIONS_PER_UNIT)/511, (31*S3L_FRACTIONS_PER_UNIT)/511, 
  (37*S3L_FRACTIONS_PER_UNIT)/511, (43*S3L_FRACTIONS_PER_UNIT)/511, 
  (50*S3L_FRACTIONS_PER_UNIT)/511, (56*S3L_FRACTIONS_PER_UNIT)/511, 
  (62*S3L_FRACTIONS_PER_UNIT)/511, (68*S3L_FRACTIONS_PER_UNIT)/511, 
  (74*S3L_FRACTIONS_PER_UNIT)/511, (81*S3L_FRACTIONS_PER_UNIT)/511, 
  (87*S3L_FRACTIONS_PER_UNIT)/511, (93*S3L_FRACTIONS_PER_UNIT)/511, 
  (99*S3L_FRACTIONS_PER_UNIT)/511, (105*S3L_FRACTIONS_PER_UNIT)/511, 
  (111*S3L_FRACTIONS_PER_UNIT)/511, (118*S3L_FRACTIONS_PER_UNIT)/511, 
  (124*S3L_FRACTIONS_PER_UNIT)/511, (130*S3L_FRACTIONS_PER_UNIT)/511, 
  (136*S3L_FRACTIONS_PER_UNIT)/511, (142*S3L_FRACTIONS_PER_UNIT)/511, 
  (148*S3L_FRACTIONS_PER_UNIT)/511, (154*S3L_FRACTIONS_PER_UNIT)/511, 
  (160*S3L_FRACTIONS_PER_UNIT)/511, (166*S3L_FRACTIONS_PER_UNIT)/511, 
  (172*S3L_FRACTIONS_PER_UNIT)/511, (178*S3L_FRACTIONS_PER_UNIT)/511, 
  (183*S3L_FRACTIONS_PER_UNIT)/511, (189*S3L_FRACTIONS_PER_UNIT)/511, 
  (195*S3L_FRACTIONS_PER_UNIT)/511, (201*S3L_FRACTIONS_PER_UNIT)/511, 
  (207*S3L_FRACTIONS_PER_UNIT)/511, (212*S3L_FRACTIONS_PER_UNIT)/511, 
  (218*S3L_FRACTIONS_PER_UNIT)/511, (224*S3L_FRACTIONS_PER_UNIT)/511, 
  (229*S3L_FRACTIONS_PER_UNIT)/511, (235*S3L_FRACTIONS_PER_UNIT)/511, 
  (240*S3L_FRACTIONS_PER_UNIT)/511, (246*S3L_FRACTIONS_PER_UNIT)/511, 
  (251*S3L_FRACTIONS_PER_UNIT)/511, (257*S3L_FRACTIONS_PER_UNIT)/511, 
  (262*S3L_FRACTIONS_PER_UNIT)/511, (268*S3L_FRACTIONS_PER_UNIT)/511, 
  (273*S3L_FRACTIONS_PER_UNIT)/511, (278*S3L_FRACTIONS_PER_UNIT)/511, 
  (283*S3L_FRACTIONS_PER_UNIT)/511, (289*S3L_FRACTIONS_PER_UNIT)/511, 
  (294*S3L_FRACTIONS_PER_UNIT)/511, (299*S3L_FRACTIONS_PER_UNIT)/511, 
  (304*S3L_FRACTIONS_PER_UNIT)/511, (309*S3L_FRACTIONS_PER_UNIT)/511, 
  (314*S3L_FRACTIONS_PER_UNIT)/511, (319*S3L_FRACTIONS_PER_UNIT)/511, 
  (324*S3L_FRACTIONS_PER_UNIT)/511, (328*S3L_FRACTIONS_PER_UNIT)/511, 
  (333*S3L_FRACTIONS_PER_UNIT)/511, (338*S3L_FRACTIONS_PER_UNIT)/511, 
  (343*S3L_FRACTIONS_PER_UNIT)/511, (347*S3L_FRACTIONS_PER_UNIT)/511, 
  (352*S3L_FRACTIONS_PER_UNIT)/511, (356*S3L_FRACTIONS_PER_UNIT)/511, 
  (361*S3L_FRACTIONS_PER_UNIT)/511, (365*S3L_FRACTIONS_PER_UNIT)/511, 
  (370*S3L_FRACTIONS_PER_UNIT)/511, (374*S3L_FRACTIONS_PER_UNIT)/511, 
  (378*S3L_FRACTIONS_PER_UNIT)/511, (382*S3L_FRACTIONS_PER_UNIT)/511, 
  (386*S3L_FRACTIONS_PER_UNIT)/511, (391*S3L_FRACTIONS_PER_UNIT)/511, 
  (395*S3L_FRACTIONS_PER_UNIT)/511, (398*S3L_FRACTIONS_PER_UNIT)/511, 
  (402*S3L_FRACTIONS_PER_UNIT)/511, (406*S3L_FRACTIONS_PER_UNIT)/511, 
  (410*S3L_FRACTIONS_PER_UNIT)/511, (414*S3L_FRACTIONS_PER_UNIT)/511, 
  (417*S3L_FRACTIONS_PER_UNIT)/511, (421*S3L_FRACTIONS_PER_UNIT)/511, 
  (424*S3L_FRACTIONS_PER_UNIT)/511, (428*S3L_FRACTIONS_PER_UNIT)/511, 
  (431*S3L_FRACTIONS_PER_UNIT)/511, (435*S3L_FRACTIONS_PER_UNIT)/511, 
  (438*S3L_FRACTIONS_PER_UNIT)/511, (441*S3L_FRACTIONS_PER_UNIT)/511, 
  (444*S3L_FRACTIONS_PER_UNIT)/511, (447*S3L_FRACTIONS_PER_UNIT)/511, 
  (450*S3L_FRACTIONS_PER_UNIT)/511, (453*S3L_FRACTIONS_PER_UNIT)/511, 
  (456*S3L_FRACTIONS_PER_UNIT)/511, (459*S3L_FRACTIONS_PER_UNIT)/511, 
  (461*S3L_FRACTIONS_PER_UNIT)/511, (464*S3L_FRACTIONS_PER_UNIT)/511, 
  (467*S3L_FRACTIONS_PER_UNIT)/511, (469*S3L_FRACTIONS_PER_UNIT)/511, 
  (472*S3L_FRACTIONS_PER_UNIT)/511, (474*S3L_FRACTIONS_PER_UNIT)/511, 
  (476*S3L_FRACTIONS_PER_UNIT)/511, (478*S3L_FRACTIONS_PER_UNIT)/511, 
  (481*S3L_FRACTIONS_PER_UNIT)/511, (483*S3L_FRACTIONS_PER_UNIT)/511, 
  (485*S3L_FRACTIONS_PER_UNIT)/511, (487*S3L_FRACTIONS_PER_UNIT)/511, 
  (488*S3L_FRACTIONS_PER_UNIT)/511, (490*S3L_FRACTIONS_PER_UNIT)/511, 
  (492*S3L_FRACTIONS_PER_UNIT)/511, (494*S3L_FRACTIONS_PER_UNIT)/511, 
  (495*S3L_FRACTIONS_PER_UNIT)/511, (497*S3L_FRACTIONS_PER_UNIT)/511, 
  (498*S3L_FRACTIONS_PER_UNIT)/511, (499*S3L_FRACTIONS_PER_UNIT)/511, 
  (501*S3L_FRACTIONS_PER_UNIT)/511, (502*S3L_FRACTIONS_PER_UNIT)/511, 
  (503*S3L_FRACTIONS_PER_UNIT)/511, (504*S3L_FRACTIONS_PER_UNIT)/511, 
  (505*S3L_FRACTIONS_PER_UNIT)/511, (506*S3L_FRACTIONS_PER_UNIT)/511, 
  (507*S3L_FRACTIONS_PER_UNIT)/511, (507*S3L_FRACTIONS_PER_UNIT)/511, 
  (508*S3L_FRACTIONS_PER_UNIT)/511, (509*S3L_FRACTIONS_PER_UNIT)/511, 
  (509*S3L_FRACTIONS_PER_UNIT)/511, (510*S3L_FRACTIONS_PER_UNIT)/511, 
  (510*S3L_FRACTIONS_PER_UNIT)/511, (510*S3L_FRACTIONS_PER_UNIT)/511, 
  (510*S3L_FRACTIONS_PER_UNIT)/511, (510*S3L_FRACTIONS_PER_UNIT)/511
};

#define S3L_SIN_TABLE_UNIT_STEP\
  (S3L_FRACTIONS_PER_UNIT / (S3L_SIN_TABLE_LENGTH * 4))

typedef int16_t S3L_ScreenCoord;
typedef uint16_t S3L_Index;

/**
  Vector that consists of four scalars and can represent homogenous
  coordinates, but is generally also used as Vec3 and Vec2.
*/

typedef struct
{
  S3L_Unit x;
  S3L_Unit y;
  S3L_Unit z;
  S3L_Unit w;
} S3L_Vec4;

#define S3L_writeVec4(v)\
  printf("Vec4: %d %d %d %d\n",(v.x),(v.y),(v.z),(v.w))

static inline void S3L_initVec4(S3L_Vec4 *v)
{
  v->x = 0; v->y = 0; v->z = 0; v->w = S3L_FRACTIONS_PER_UNIT;
}

typedef S3L_Unit S3L_Mat4[4][4]; /**< 4x4 matrix, used mostly for 3D
                                      transforms. The indexing is this:
                                      matrix[column][row]. */
#define S3L_writeMat4(m)\
  printf("Mat4:\n  %d %d %d %d\n  %d %d %d %d\n  %d %d %d %d\n  %d %d %d %d\n"\
   ,(m)[0][0],(m)[1][0],(m)[2][0],(m)[3][0],\
    (m)[0][1],(m)[1][1],(m)[2][1],(m)[3][1],\
    (m)[0][2],(m)[1][2],(m)[2][2],(m)[3][2],\
    (m)[0][3],(m)[1][3],(m)[2][3],(m)[3][3])

/**
  Initializes a 4x4 matrix to identity.
*/
static inline void S3L_initMat4(S3L_Mat4 *m)
{
  #define M(x,y) (*m)[x][y]
  #define S S3L_FRACTIONS_PER_UNIT

  M(0,0) = S; M(1,0) = 0; M(2,0) = 0; M(3,0) = 0; 
  M(0,1) = 0; M(1,1) = S; M(2,1) = 0; M(3,1) = 0; 
  M(0,2) = 0; M(1,2) = 0; M(2,2) = S; M(3,2) = 0; 
  M(0,3) = 0; M(1,3) = 0; M(2,3) = 0; M(3,3) = S; 

  #undef M
  #undef S
}

/**
  Multiplies a vector by a matrix with normalization by S3L_FRACTIONS_PER_UNIT.
  Result is stored in the input vector.
*/

void S3L_vec4Xmat4(S3L_Vec4 *v, S3L_Mat4 *m)
{
  S3L_Vec4 vBackup;

  vBackup.x = v->x;  
  vBackup.y = v->y;  
  vBackup.z = v->z;  
  vBackup.w = v->w;  

  // TODO: try alternative operation orders to optimize

  #define dot(col)\
    (vBackup.x * (*m)[col][0]) / S3L_FRACTIONS_PER_UNIT +\
    (vBackup.y * (*m)[col][1]) / S3L_FRACTIONS_PER_UNIT +\
    (vBackup.z * (*m)[col][2]) / S3L_FRACTIONS_PER_UNIT +\
    (vBackup.w * (*m)[col][3]) / S3L_FRACTIONS_PER_UNIT

  v->x = dot(0);
  v->y = dot(1);
  v->z = dot(2);
  v->w = dot(3);

  #undef dot
}

// general helper functions

static inline int16_t S3L_abs(int16_t value)
{
  return value >= 0 ? value : -1 * value;
}

static inline int16_t S3L_min(int16_t v1, int16_t v2)
{
  return v1 >= v2 ? v2 : v1;
}

static inline int16_t S3L_max(int16_t v1, int16_t v2)
{
  return v1 >= v2 ? v1 : v2;
}

static inline S3L_Unit S3L_wrap(S3L_Unit value, S3L_Unit mod)
{
  return value >= 0 ? (value % mod) : (mod + (value % mod) - 1);
}

static inline S3L_Unit S3L_nonZero(S3L_Unit value)
{
  return value != 0 ? value : 1;
}

/**
  Multiplies two matrices with normalization by S3L_FRACTIONS_PER_UNIT. Result
  is stored in the first matrix.
*/
void S3L_mat4Xmat4(S3L_Mat4 *m1, S3L_Mat4 *m2)
{
  S3L_Mat4 mat1;

  for (uint16_t row = 0; row < 4; ++row)
    for (uint16_t col = 0; col < 4; ++col)
      mat1[col][row] = (*m1)[col][row];

  for (uint16_t row = 0; row < 4; ++row)
    for (uint16_t col = 0; col < 4; ++col)
    {
      (*m1)[col][row] = 0;

      for (uint16_t i = 0; i < 4; ++i)
        (*m1)[col][row] +=
          (mat1[i][row] * (*m2)[col][i]) / S3L_FRACTIONS_PER_UNIT;
    }
}

S3L_Unit S3L_sin(S3L_Unit x)
{
  x = S3L_wrap(x / S3L_SIN_TABLE_UNIT_STEP,S3L_SIN_TABLE_LENGTH * 4);
  int8_t positive = 1;

  if (x < S3L_SIN_TABLE_LENGTH)
    x = x;
  else if (x < S3L_SIN_TABLE_LENGTH * 2)
    x = S3L_SIN_TABLE_LENGTH * 2 - x - 1;
  else if (x < S3L_SIN_TABLE_LENGTH * 3)
  {
    x = x - S3L_SIN_TABLE_LENGTH * 2;
    positive = 0;
  }
  else
  {
    x = S3L_SIN_TABLE_LENGTH - (x - S3L_SIN_TABLE_LENGTH * 3) - 1;
    positive = 0;
  }

  return positive ? S3L_sinTable[x] : -1 * S3L_sinTable[x];
}

static inline S3L_Unit S3L_cos(S3L_Unit x)
{
  return S3L_sin(x - S3L_FRACTIONS_PER_UNIT / 4);
}

void S3L_makeTranslationMat(
  S3L_Unit offsetX, S3L_Unit offsetY, S3L_Unit offsetZ, S3L_Mat4 *m)
{
  #define M(x,y) (*m)[x][y]
  #define S S3L_FRACTIONS_PER_UNIT

  M(0,0) = S; M(1,0) = 0; M(2,0) = 0; M(3,0) = 0; 
  M(0,1) = 0; M(1,1) = S; M(2,1) = 0; M(3,1) = 0; 
  M(0,2) = 0; M(1,2) = 0; M(2,2) = S; M(3,2) = 0; 
  M(0,3) = offsetX; M(1,3) = offsetY; M(2,3) = offsetZ; M(3,3) = S;

  #undef M
  #undef S
}

/**
  Makes a scaling matrix. DON'T FORGET: scale of 1.0 is set with
  S3L_FRACTIONS_PER_UNIT!
*/
void S3L_makeScaleMatrix(
  S3L_Unit scaleX, S3L_Unit scaleY, S3L_Unit scaleZ, S3L_Mat4 *m)
{
  #define M(x,y) (*m)[x][y]

  M(0,0) = scaleX; M(2,0) = 0;     M(3,0) = 0; 
  M(0,1) = 0;      M(1,1) = scaleY; M(2,1) = 0; M(3,1) = 0; 
  M(0,2) = 0;      M(1,2) = 0;     M(2,2) = scaleZ; M(3,2) = 0; 
  M(0,3) = 0;      M(1,3) = 0;     M(2,3) = 0; M(3,3) = S3L_FRACTIONS_PER_UNIT; 

  #undef M
}

/**
  Makes a rotation matrix. For the rotation conventions (meaning, order, units)
  see the appropriate structure comments.
*/
void S3L_makeRotationMatrix(
  S3L_Unit aroundX, S3L_Unit aroundY, S3L_Unit aroundZ, S3L_Mat4 *m)
{
  S3L_Unit sx = S3L_sin(aroundX);
  S3L_Unit sy = S3L_sin(aroundY);
  S3L_Unit sz = S3L_sin(aroundZ);

  S3L_Unit cx = S3L_cos(aroundX);
  S3L_Unit cy = S3L_cos(aroundY);
  S3L_Unit cz = S3L_cos(aroundZ);

  #define M(x,y) (*m)[x][y]
  #define S S3L_FRACTIONS_PER_UNIT

  M(0,0) = (cy * cz) / S + (sy * sx * sz) / (S * S);
  M(1,0) = (cx * sz) / S;
  M(2,0) = (cy * sx * sz) / (S * S) - (cz * sy) / S;
  M(3,0) = 0;

  M(0,1) = (cz * sy * sx) / (S * S) - (cy * sz) / S;
  M(1,1) = (cx * cz) / S;
  M(2,1) = (cy * cz * sx) / (S * S) + (sy * sz) / S;
  M(3,1) = 0;

  M(0,2) = (cx * sy) / S;
  M(1,2) = -1 * sx;
  M(2,2) = (cy * cx) / S;
  M(3,2) = 0;

  M(0,3) = 0;
  M(1,3) = 0;
  M(2,3) = 0;
  M(3,3) = S3L_FRACTIONS_PER_UNIT;

  #undef M
  #undef S 
}

typedef struct
{
  S3L_Vec4 translation;
  S3L_Vec4 rotation; /**< Euler angles. Rortation is applied in this order:
                          1. z = around z (roll) CW looking along z+
                          2. x = around x (pitch) CW looking along x+
                          3. y = around y (yaw) CW looking along y+ */
  S3L_Vec4 scale;
} S3L_Transform3D;

static inline void S3L_initTransoform3D(S3L_Transform3D *t)
{
  S3L_initVec4(&(t->translation));
  S3L_initVec4(&(t->rotation));
  t->scale.x = S3L_FRACTIONS_PER_UNIT;
  t->scale.y = S3L_FRACTIONS_PER_UNIT;
  t->scale.z = S3L_FRACTIONS_PER_UNIT;
}

typedef struct
{
  uint16_t resolutionX;
  uint16_t resolutionY;
  S3L_Unit focalLength;       ///< Defines the field of view (FOV).
  S3L_Transform3D transform;
} S3L_Camera;

static inline void S3L_initCamera(S3L_Camera *c)
{
  c->resolutionX = 128;
  c->resolutionY = 64;
  c->focalLength = S3L_FRACTIONS_PER_UNIT;
  S3L_initTransoform3D(&(c->transform));
}

typedef struct
{
  S3L_ScreenCoord x;          ///< Screen X coordinate.
  S3L_ScreenCoord y;          ///< Screen Y coordinate.

  S3L_Unit barycentric0; /**< Barycentric coord 0 (corresponds to 1st vertex).
                              Together with 1 and 2 coords these serve to
                              locate the pixel on a triangle and interpolate
                              values between it's three points. The sum of the
                              three coordinates will always be exactly
                              S3L_FRACTIONS_PER_UNIT. */
  S3L_Unit barycentric1; ///< Baryc. coord 1 (corresponds to 2nd vertex).
  S3L_Unit barycentric2; ///< Baryc. coord 2 (corresponds to 3rd vertex).
  S3L_Index triangleID;
} S3L_PixelInfo;

static inline void S3L_initPixelInfo(S3L_PixelInfo *p)
{
  p->x = 0;
  p->y = 0;
  p->barycentric0 = S3L_FRACTIONS_PER_UNIT;
  p->barycentric1 = 0;
  p->barycentric2 = 0;
  p->triangleID = 0;
}

#define S3L_BACKFACE_CULLING_NONE 0
#define S3L_BACKFACE_CULLING_CW 1
#define S3L_BACKFACE_CULLING_CCW 2

#define S3L_MODE_TRIANGLES 0
#define S3L_MODE_LINES 1
#define S3L_MODE_POINTS 2

typedef struct
{
  int backfaceCulling;
  int mode;
} S3L_DrawConfig;

void S3L_PIXEL_FUNCTION(S3L_PixelInfo *pixel); // forward decl

typedef struct
{
  int16_t steps;
  int16_t err;
  S3L_ScreenCoord x;
  S3L_ScreenCoord y;

  int16_t *majorCoord;
  int16_t *minorCoord;
  int16_t majorIncrement;
  int16_t minorIncrement;
  int16_t majorDiff;
  int16_t minorDiff; 
} S3L_BresenhamState; ///< State of drawing a line with Bresenham algorithm.

/**
  Returns a value interpolated between the three triangle vertices based on
  barycentric coordinates.
*/
static inline S3L_Unit S3L_interpolateBarycentric(
  S3L_Unit value0, S3L_Unit value1, S3L_Unit value2,
  S3L_Unit barycentric0, S3L_Unit barycentric1, S3L_Unit barycentric2)
{
  return
    (
      (value0 * barycentric0) +
      (value1 * barycentric1) +
      (value2 * barycentric2)
    ) / S3L_FRACTIONS_PER_UNIT;
}

/**
  Interpolated between two values, v1 and v2, in the same ratio as t is to
  tMax. Does NOT prevent zero division.
*/
static inline int16_t S3L_interpolate(int16_t v1, int16_t v2, int16_t t,
  int16_t tMax)
{
  return v1 + ((v2 - v1) * t) / tMax;
}

/**
  Same as S3L_interpolate but with v1 = 0. Should be faster.
*/
static inline int16_t S3L_interpolateFrom0(int16_t v2, int16_t t, int16_t tMax)
{
  return (v2 * t) / tMax;
}

void S3L_bresenhamInit(S3L_BresenhamState *state, int16_t x0, int16_t y0,
  int16_t x1, int16_t y1)
{
  int16_t dx = x1 - x0;
  int16_t dy = y1 - y0;

  int16_t absDx = S3L_abs(dx);
  int16_t absDy = S3L_abs(dy);

  if (absDx >= absDy)
  {
    state->majorCoord = &(state->x);
    state->minorCoord = &(state->y);

    state->minorDiff = 2 * absDy;
    state->majorDiff = 2 * absDx;
    state->err = 2 * dy - dx;
  
    state->majorIncrement = dx >= 0 ? 1 : -1;
    state->minorIncrement = dy >= 0 ? 1 : -1;

    state->steps = absDx;
  }
  else
  {
    state->majorCoord = &(state->y);
    state->minorCoord = &(state->x);

    state->minorDiff = 2 * absDx;
    state->majorDiff = 2 * absDy;
    state->err = 2 * dx - dy;

    state->majorIncrement = dy >= 0 ? 1 : -1;
    state->minorIncrement = dx >= 0 ? 1 : -1;

    state->steps = absDy;
  }

  state->x = x0;
  state->y = y0;
}

int S3L_bresenhamStep(S3L_BresenhamState *state)
{
  state->steps--;

  (*state->majorCoord) += state->majorIncrement;

  if (state->err > 0)
  {
    (*state->minorCoord) += state->minorIncrement;
    state->err -= state->majorDiff;
  }

  state->err += state->minorDiff;

  return state->steps >= 0;
}

void S3L_drawTriangle(
  S3L_ScreenCoord x0, S3L_ScreenCoord y0,
  S3L_ScreenCoord x1, S3L_ScreenCoord y1,
  S3L_ScreenCoord x2, S3L_ScreenCoord y2,
  S3L_DrawConfig config,
  S3L_Index triangleID)
{
  if (config.backfaceCulling != S3L_BACKFACE_CULLING_NONE)
  {
    int cw = // matrix determinant
      x0 * y1 + y0 * x2 + x1 * y2 - y1 * x2 - y0 * x1 - x0 * y2 > 0;

    if ((config.backfaceCulling == S3L_BACKFACE_CULLING_CW && !cw) ||
        (config.backfaceCulling == S3L_BACKFACE_CULLING_CCW && cw))
      return;
  }

  S3L_PixelInfo p;
  S3L_initPixelInfo(&p);
  p.triangleID = triangleID;

  // point mode

  if (config.mode == S3L_MODE_POINTS)
  {
    p.x = x0; p.y = y0; p.barycentric0 = S3L_FRACTIONS_PER_UNIT;
    p.barycentric1 = 0; p.barycentric2 = 0;
    S3L_PIXEL_FUNCTION(&p);

    p.x = x1; p.y = y1; p.barycentric0 = 0;
    p.barycentric1 = S3L_FRACTIONS_PER_UNIT; p.barycentric2 = 0;
    S3L_PIXEL_FUNCTION(&p);

    p.x = x2; p.y = y2; p.barycentric0 = 0;
    p.barycentric1 = 0; p.barycentric2 = S3L_FRACTIONS_PER_UNIT;
    S3L_PIXEL_FUNCTION(&p);
 
    return;
  }

  // line mode

  if (config.mode == S3L_MODE_LINES)
  {
    S3L_BresenhamState line;
    S3L_Unit lineLen;

    #define drawLine(p1,p2)\
      S3L_bresenhamInit(&line,x##p1,y##p1,x##p2,y##p2);\
      p.barycentric0 = 0;\
      p.barycentric1 = 0;\
      p.barycentric2 = 0;\
      lineLen = S3L_nonZero(line.steps);\
      do\
      {\
        p.x = line.x; p.y = line.y;\
        p.barycentric##p1 = S3L_interpolateFrom0(\
          S3L_FRACTIONS_PER_UNIT,line.steps,lineLen);  \
        p.barycentric##p2 = S3L_FRACTIONS_PER_UNIT - p.barycentric##p1;\
        S3L_PIXEL_FUNCTION(&p);\
      } while (S3L_bresenhamStep(&line));
   
    drawLine(0,1)
    drawLine(2,0)
    drawLine(1,2)

    #undef drawLine

    return;
  }

  // triangle mode

  S3L_ScreenCoord
    tPointX, tPointY,     // top triangle point coords
    lPointX, lPointY,     // left triangle point coords
    rPointX, rPointY;     // right triangle point coords

  S3L_Unit *barycentric0; // bar. coord that gets higher from L to R
  S3L_Unit *barycentric1; // bar. coord that gets higher from R to L
  S3L_Unit *barycentric2; // bar. coord that gets higher from bottom up

  // Sort the points.

  #define handleLR(t,a,b)\
    int16_t aDx = x##a - x##t;\
    int16_t bDx = x##b - x##t;\
    int16_t aDy = S3L_nonZero(y##a - y##t);\
    int16_t bDy = S3L_nonZero(y##b - y##t);\
    if ((aDx << 4) / aDy < (bDx << 4) / bDy)\
    /*if (x##a <= x##b)*/\
    {\
      lPointX = x##a; lPointY = y##a;\
      rPointX = x##b; rPointY = y##b;\
      barycentric0 = &p.barycentric##b;\
      barycentric1 = &p.barycentric##a;\
    }\
    else\
    {\
      lPointX = x##b; lPointY = y##b;\
      rPointX = x##a; rPointY = y##a;\
      barycentric0 = &p.barycentric##a;\
      barycentric1 = &p.barycentric##b;\
    }

  if (y0 <= y1)
  {
    if (y0 <= y2)
    {
      tPointX = x0;
      tPointY = y0;
      barycentric2 = &p.barycentric0;
      handleLR(0,1,2)
    }
    else
    {
      tPointX = x2;
      tPointY = y2;
      barycentric2 = &p.barycentric2;
      handleLR(2,0,1)
    }
  }
  else
  {
    if (y1 <= y2)
    {
      tPointX = x1;
      tPointY = y1;
      barycentric2 = &p.barycentric1;
      handleLR(1,0,2)
    }
    else
    {
      tPointX = x2;
      tPointY = y2;
      barycentric2 = &p.barycentric2;
      handleLR(2,0,1)
    }
  }

  // Now draw the triangle line by line.

  #undef handleLR

  S3L_ScreenCoord splitY; // Y of the vertically middle point of the triangle
  S3L_ScreenCoord endY;   // bottom Y of the whole triangle
  int splitOnLeft;        // whether splitY happens on L or R side

  if (rPointY <= lPointY)
  {
    splitY = rPointY;
    splitOnLeft = 0;
    endY = lPointY;
  }
  else
  {
    splitY = lPointY;
    splitOnLeft = 1;
    endY = rPointY;
  }

  S3L_ScreenCoord currentY = tPointY;

  /* We'll be using a slight modification of Bresenham line algorithm (a one
     that draws a _non-continous_ line). */

  int16_t
    /* triangle side:
    left     right */
    lX,      rX,        // current x position
    lDx,     rDx,       // dx (end point - start point)
    lDy,     rDy,       // dy (end point - start point)
    lInc,    rInc,      // direction in which to increment (1 or -1)
    lErr,    rErr,      // current error (Bresenham)
    lErrAdd, rErrAdd,   // error value to add in each Bresenham cycle
    lErrSub, rErrSub;   // error value to substract when moving in x direction

  S3L_Unit
    lSideUnitStep, rSideUnitStep,
    lSideUnitPos,  rSideUnitPos;

  int16_t helperDxAbs;

          /* init side for the algorithm, params:
             v - which side (l or r)
             p1 - point from (t, l or r)
             p2 - point to (t, l or r)
             down - whether going top-down or bottom-up */
  #define initSide(v,p1,p2,down)\
    v##X = p1##PointX;\
    v##Dx = p2##PointX - p1##PointX;\
    v##Dy = p2##PointY - p1##PointY;\
    v##SideUnitStep = S3L_FRACTIONS_PER_UNIT / (v##Dy != 0 ? v##Dy : 1);\
    v##SideUnitPos = 0;\
    if (!down)\
    {\
      v##SideUnitPos = S3L_FRACTIONS_PER_UNIT;\
      v##SideUnitStep *= -1;\
    }\
    helperDxAbs = S3L_abs(v##Dx);\
    v##Inc = v##Dx >= 0 ? 1 : -1;\
    v##Err = 2 * helperDxAbs - v##Dy;\
    v##ErrAdd = 2 * helperDxAbs;\
    v##ErrSub = 2 * v##Dy;\
    v##ErrSub = v##ErrSub != 0 ? v##ErrSub : 1; /* don't allow 0, could lead
                                                   to an infinite substracting
                                                   loop */
  #define stepSide(s)\
    while (s##Err > 0)\
    {\
      s##X += s##Inc;\
      s##Err -= s##ErrSub;\
    }\
    s##Err += s##ErrAdd;

  initSide(r,t,r,1)
  initSide(l,t,l,1)

  while (currentY <= endY)  // draw the triangle from top to bottom
  {
    if (currentY == splitY) // reached a vertical split of the triangle?
    {                       // then reinit one side
      if (splitOnLeft)
      {
        initSide(l,l,r,0);

        S3L_Unit *tmp = barycentric0;
        barycentric0 = barycentric2;
        barycentric2 = tmp;

        rSideUnitPos = S3L_FRACTIONS_PER_UNIT - rSideUnitPos;
        rSideUnitStep *= -1;
      }
      else
      {
        initSide(r,r,l,0);

        S3L_Unit *tmp = barycentric1;
        barycentric1 = barycentric2;
        barycentric2 = tmp;

        lSideUnitPos = S3L_FRACTIONS_PER_UNIT - lSideUnitPos;
        lSideUnitStep *= -1;
      }
    }

    p.y = currentY;

    // draw the horizontal line

    S3L_Unit tMax = rX - lX;
    tMax = S3L_NONZERO(tMax); // prevent division by zero

    S3L_Unit t1 = 0;
    S3L_Unit t2 = tMax;

    for (S3L_ScreenCoord x = lX; x <= rX; ++x)
    {
      *barycentric0 = S3L_interpolateFrom0(rSideUnitPos,t1,tMax);
      *barycentric1 = S3L_interpolateFrom0(lSideUnitPos,t2,tMax);
      *barycentric2 = S3L_FRACTIONS_PER_UNIT - *barycentric0 - *barycentric1;

      p.x = x;
      S3L_PIXEL_FUNCTION(&p);

      ++t1;
      --t2;
    }

    stepSide(r)
    stepSide(l)

    lSideUnitPos += lSideUnitStep;
    rSideUnitPos += rSideUnitStep;

    ++currentY;
  }

  #undef initSide
  #undef stepSide
}

static inline void S3L_rotate2DPoint(S3L_Unit *x, S3L_Unit *y, S3L_Unit angle)
{
  if (angle < S3L_SIN_TABLE_UNIT_STEP)
    return; // no visible rotation

  S3L_Unit angleSin = S3L_sin(angle);
  S3L_Unit angleCos = S3L_cos(angle);

  S3L_Unit xBackup = *x;

  *x =
    (angleCos * (*x)) / S3L_FRACTIONS_PER_UNIT -
    (angleSin * (*y)) / S3L_FRACTIONS_PER_UNIT;

  *y =
    (angleSin * xBackup) / S3L_FRACTIONS_PER_UNIT +
    (angleCos * (*y)) / S3L_FRACTIONS_PER_UNIT;
}

void S3L_makeWorldMatrix(S3L_Transform3D worldTransform, S3L_Mat4 *m)
{
  S3L_makeScaleMatrix(
    worldTransform.scale.x,
    worldTransform.scale.y,
    worldTransform.scale.z,
    m
  );

  S3L_Mat4 t;

  S3L_makeRotationMatrix(
    worldTransform.rotation.x,
    worldTransform.rotation.y,
    worldTransform.rotation.z,
    &t);

  S3L_mat4Xmat4(m,&t);

  S3L_makeTranslationMat(
    worldTransform.translation.x,
    worldTransform.translation.y,
    worldTransform.translation.z,
    &t);

  S3L_mat4Xmat4(m,&t);
}

void S3L_makeCameraMatrix(S3L_Transform3D cameraTransform, S3L_Mat4 *m)
{
  S3L_makeTranslationMat(
    -1 * cameraTransform.translation.x,
    -1 * cameraTransform.translation.y,
    -1 * cameraTransform.translation.z,
    m);
}

static inline void S3L_mapCameraToScreen(S3L_Vec4 point, S3L_Camera *camera,
  S3L_ScreenCoord *screenX, S3L_ScreenCoord *screenY)
{
  uint16_t halfW = camera->resolutionX >> 1; // TODO: precompute earlier? 
  uint16_t halfH = camera->resolutionY >> 1;

  *screenX = halfW + (point.x * halfW) / point.z;
  *screenY = halfH - (point.y * halfW) / point.z;
  // ^ S3L_FRACTIONS_PER_UNIT cancel out
}

void S3L_drawModel(
  const S3L_Unit coords[],
  const S3L_Index triangleVertexIndices[],
  uint16_t triangleCount,
  S3L_Transform3D modelTransform,
  S3L_Camera camera,
  S3L_DrawConfig config)
{
  S3L_Index triangleIndex = 0;
  S3L_Index coordIndex = 0;

  S3L_ScreenCoord sX0, sY0, sX1, sY1, sX2, sY2;
  S3L_Vec4 pointModel;
  S3L_Unit indexIndex;

  pointModel.w = S3L_FRACTIONS_PER_UNIT; // has to be "1.0" for translation

  S3L_Mat4 mat1, mat2;

  S3L_makeWorldMatrix(modelTransform,&mat1);
  S3L_makeCameraMatrix(camera.transform,&mat2);
  S3L_mat4Xmat4(&mat1,&mat2);

  while (triangleIndex < triangleCount)
  {
    #define mapCoords(n)\
      indexIndex = triangleVertexIndices[coordIndex] * 3;\
      pointModel.x = coords[indexIndex];\
      ++indexIndex; /* TODO: put into square brackets? */\
      pointModel.y = coords[indexIndex];\
      ++indexIndex;\
      pointModel.z = coords[indexIndex];\
      ++coordIndex;\
      S3L_vec4Xmat4(&pointModel,&mat1);\
      S3L_mapCameraToScreen(pointModel,&camera,&sX##n,&sY##n);

    mapCoords(0)
    mapCoords(1)
    mapCoords(2)

    S3L_drawTriangle(sX0,sY0,sX1,sY1,sX2,sY2,config,triangleIndex);

    ++triangleIndex;
  }
}

#endif
