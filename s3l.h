#ifndef S3L_H
#define S3L_H

#include <stdint.h>

typedef int16_t S3L_COORD;

typedef struct
{
  int16_t x;
  int16_t y;
} S3L_PixelInfo;

typedef struct
{
  int16_t steps;
  int16_t err;
  int16_t x;
  int16_t y;

  int16_t *majorCoord;
  int16_t *minorCoord;
  int16_t majorIncrement;
  int16_t minorIncrement;
  int16_t majorDiff;
  int16_t minorDiff; 
} S3L_BresenhamState;

static inline int16_t S3L_abs(int16_t value)
{
  return value >= 0 ? value : -1 * value;
}

void S3L_bresenhamInit(S3L_BresenhamState *state, int16_t x0, int16_t y0, int16_t x1, int16_t y1)
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

void S3L_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
  S3L_COORD
    tPointX, tPointY,    // top triangle point coords
    lPointX, lPointY,    // left triangle point coords
    rPointX, rPointY;    // right triangle point coords

  // Sort the points.

  #define handleLR(a,b)\
    if (x##a <= x##b)\
    {\
      lPointX = x##a; lPointY = y##a;\
      rPointX = x##b; rPointY = y##b;\
    }\
    else\
    {\
      lPointX = x##b; lPointY = y##b;\
      rPointX = x##a; rPointY = y##a;\
    }

  if (y0 <= y1)
  {
    if (y0 <= y2)
    {
      tPointX = x0;
      tPointY = y0;
      handleLR(1,2)
    }
    else
    {
      tPointX = x2;
      tPointY = y2;
      handleLR(0,1)
    }
  }
  else
  {
    if (y1 <= y2)
    {
      tPointX = x1;
      tPointY = y1;
      handleLR(0,2)
    }
    else
    {
      tPointX = x2;
      tPointY = y2;
      handleLR(0,1)
    }
  }

  // Now drive the triangle line by line.

  #undef handleLR

  S3L_COORD currentY = tPointY;

  /* We'll be using a slight modification of Bresenham line algorithm (a one
     that draws a _non-continous_ line). */

  #define initSide(s)\
    int16_t s##X = tPointX;\
    int16_t s##Dx = s##PointX - tPointX;\
    int16_t s##Dy = s##PointY - tPointY;\
    int16_t s##Inc = s##Dx >= 0 ? 1 : -1;\
    int16_t s##Err = 2 * s##Dx - s##Dy;\
    int16_t s##ErrAdd = 2 * S3L_abs(s##Dx);\
    int16_t s##ErrSub = 2 * S3L_abs(s##Dy);

  #define stepSide(s)\
    while (s##Err > 0)\
    {\
      s##X += s##Inc;\
      s##Err -= s##ErrSub;\
    }\
    s##Err += s##ErrAdd;

  initSide(r)
  initSide(l)

  S3L_PixelInfo p;

  while (currentY <= rPointY)
  {
    p.y = currentY;

    for (S3L_COORD x = lX; x <= rX; ++x)
    {
      p.x = x;
      S3L_PIXEL_FUNCTION(&p);
    }

    stepSide(r)
    stepSide(l)

    ++currentY;
  }

  #undef initSide
  #undef stepSide
  
}

#endif
