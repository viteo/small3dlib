/*
  WIP simple realtime 3D rasterization-based library.

  author: Miloslav Ciz
  license: CC0 1.0
*/

#ifndef S3L_H
#define S3L_H

#include <stdint.h>

typedef int16_t S3L_Unit;
#define S3L_FRACTIONS_PER_UNIT 1024
typedef int16_t S3L_ScreenCoord;

typedef struct
{
  S3L_Unit x;
  S3L_Unit y;
  S3L_Unit z;
} S3L_Vec3;

typedef struct
{
  S3L_ScreenCoord x;           ///< Screen X coordinate.
  S3L_ScreenCoord y;           ///< Screen Y coordinate.

  S3L_Unit barycentric0; /**< Barycentric coord A (corresponds to 1st vertex).
                              Together with B and C coords these serve to
                              locate the pixel on a triangle and interpolate
                              values between it's three points. The sum of the
                              three coordinates will always be exactly
                              S3L_FRACTIONS_PER_UNIT. */
  S3L_Unit barycentric1; ///< Baryc. coord B (corresponds to 2nd vertex).
  S3L_Unit barycentric2; ///< Baryc. coord C (corresponds to 3rd vertex).
} S3L_PixelInfo;

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
} S3L_BresenhamState;

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

// general helper functions:

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

void S3L_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
  int16_t x2, int16_t y2, S3L_DrawConfig config)
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

  p.barycentric0 = 0;
  p.barycentric1 = 0;
  p.barycentric2 = 0;

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

  S3L_ScreenCoord splitY; // Y at which one side (L or R) changes
  S3L_ScreenCoord endY;   // bottom Y of the whole triangle
  int splitOnLeft;  // whether split happens on L or R

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

  #define initSide(v,p1,p2, down)\
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

  while (currentY <= endY)
  {
    if (currentY == splitY)
    {
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

    // draw the line

    S3L_Unit tMax = rX - lX;
    tMax = tMax != 0 ? tMax : 1; // prevent division by zero

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

#endif
