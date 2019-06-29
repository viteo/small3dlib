/*
  Example program for small3dlib -- a GTA-like game demo.

  author: Miloslav Ciz
  license: CC0 1.0
*/

#include "Pokitto.h"
#include <stdio.h>

Pokitto::Core pokitto;

#define S3L_FLAT 0
#define S3L_STRICT_NEAR_CULLING 0
#define S3L_PERSPECTIVE_CORRECTION 2
#define S3L_SORT 0
#define S3L_STENCIL_BUFFER 0
#define S3L_Z_BUFFER 2

#define S3L_PIXEL_FUNCTION drawPixel

#define S3L_RESOLUTION_X 110
#define S3L_RESOLUTION_Y 88

#include "small3dlib.h"

#include "cityModel.h"
#include "cityTexture.h"
#include "carModel.h"

#define MAX_VELOCITY 700
#define ACCELERATION 600
#define TURN_SPEED 200
#define FRICTION 600

S3L_Model3D models[2];

const uint8_t collisionMap[8 * 10] =
{
  1,1,1,1,1,1,1,1,
  1,1,1,1,0,0,0,1,
  1,1,1,1,0,1,0,1,
  2,2,1,0,0,0,0,3,
  1,2,1,0,1,1,3,1,
  2,0,0,0,1,1,3,3,
  1,0,1,0,0,1,1,1,
  1,0,0,0,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1
};

S3L_Scene scene;

inline uint8_t sampleTexture(int32_t u, int32_t v)
{
  uint32_t index = v * CITY_TEXTURE_WIDTH + u;

  return cityTexture[index];
}

uint32_t previousTriangle = -1;
S3L_Vec4 uv0, uv1, uv2;

void drawPixel(S3L_PixelInfo *p)
{
  if (p->triangleID != previousTriangle)
  {
    const S3L_Index *uvIndices;
    const S3L_Unit *uvs;

    if (p->modelIndex == 0)
    {
      uvIndices = cityUVIndices;
      uvs = cityUVs;
    }
    else
    {
      uvIndices = carUVIndices;
      uvs = carUVs;
    }

    S3L_getIndexedTriangleValues(p->triangleIndex,uvIndices,uvs,2,&uv0,&uv1,&uv2);

    previousTriangle = p->triangleID;
  }

  S3L_Unit uv[2];

  uv[0] = S3L_interpolateBarycentric(uv0.x,uv1.x,uv2.x,p->barycentric);
  uv[1] = S3L_interpolateBarycentric(uv0.y,uv1.y,uv2.y,p->barycentric);

  uint8_t *buf = pokitto.display.screenbuffer;

  buf += p->y * 110;
  buf += p->x;
  *buf = sampleTexture(uv[0] >> 2,uv[1] >> 2);
}

static inline void draw()
{
  S3L_newFrame();
  S3L_drawScene(scene);
}

static inline uint8_t collision(S3L_Vec4 worldPosition)
{
  worldPosition.x /= S3L_FRACTIONS_PER_UNIT;
  worldPosition.z /= -S3L_FRACTIONS_PER_UNIT;    

  uint16_t index = worldPosition.z * 8 + worldPosition.x;

  return collisionMap[index];
}

static inline void handleCollision(S3L_Vec4 *pos, S3L_Vec4 previousPos)
{
  S3L_Vec4 newPos = *pos;
  newPos.x = previousPos.x;
    
  if (collision(newPos))
  {
    newPos = *pos;
    newPos.z = previousPos.z;

    if (collision(newPos))
      newPos = previousPos;
  }

  *pos = newPos;
}

int main()
{  
  pokitto.begin();
  pokitto.setFrameRate(60);
  pokitto.display.load565Palette(cityPalette);

  pokitto.display.bgcolor = 78;

  cityModelInit();
  carModelInit();

  models[0] = cityModel;
  models[1] = carModel;

  S3L_initScene(models,2,&scene);

  S3L_setTransform3D(1909,16,-3317,0,-510,0,512,512,512,&(models[1].transform));

  S3L_Vec4 carDirection;

  S3L_initVec4(&carDirection);
  
  scene.camera.transform.translation.y = S3L_FRACTIONS_PER_UNIT / 2;
  scene.camera.transform.rotation.x = -S3L_FRACTIONS_PER_UNIT / 16;

  int16_t velocity = 0;

  int16_t previousTime = 0;

  while (pokitto.isRunning())
  {
    if (pokitto.update())
    {
      models[1].transform.rotation.y += models[1].transform.rotation.z; // overturn the car for the rendering

      draw();

      models[1].transform.rotation.y -= models[1].transform.rotation.z; // turn the car back for the physics

      int16_t timeNow = pokitto.getTime();
      int16_t frameDiffMs = timeNow - previousTime;
      previousTime = timeNow;

      int16_t step = (velocity * frameDiffMs) / 1024;                             
      int16_t stepFriction = (FRICTION * frameDiffMs) / 1024;                     
      int16_t stepRotation = TURN_SPEED * frameDiffMs * S3L_max(0,velocity - 200) / (MAX_VELOCITY * 1024);
      int16_t stepVelocity = S3L_nonZero((ACCELERATION * frameDiffMs) / 1024);

      if (stepRotation == 0 && S3L_abs(velocity) >= 200)
        stepRotation = 10;

      if (velocity < 0)
        stepRotation *= -1;

      if (pokitto.leftBtn())
      {
        models[1].transform.rotation.y += stepRotation;
        models[1].transform.rotation.z =
          S3L_min(S3L_abs(velocity) / 64, models[1].transform.rotation.z + 1);
      }
      else if (pokitto.rightBtn())
      {
        models[1].transform.rotation.y -= stepRotation;
        models[1].transform.rotation.z =
          S3L_max(-S3L_abs(velocity) / 64, models[1].transform.rotation.z - 1);
      }
      else
        models[1].transform.rotation.z = (models[1].transform.rotation.z * 3) / 4;

      S3L_rotationToDirections(models[1].transform.rotation,S3L_FRACTIONS_PER_UNIT,&carDirection,0,0);

      S3L_Vec4 previousCarPos = models[1].transform.translation;
      S3L_Vec4 previousCamPos = scene.camera.transform.translation;

      int16_t friction = 0;

      if (pokitto.upBtn())
        velocity = S3L_min(MAX_VELOCITY,velocity + (velocity < 0 ? (2 * stepVelocity) : stepVelocity));
      else if (pokitto.downBtn())
        velocity = S3L_max(-MAX_VELOCITY,velocity - (velocity > 0 ? (2 * stepVelocity) : stepVelocity));
      else
        friction = 1;

      models[1].transform.translation.x += (carDirection.x * step) / S3L_FRACTIONS_PER_UNIT;
      models[1].transform.translation.z += (carDirection.z * step) / S3L_FRACTIONS_PER_UNIT;

      uint8_t coll = collision(models[1].transform.translation);

      if (coll != 0)
      {
        if (coll == 1)
        {
          handleCollision(&(models[1].transform.translation),previousCarPos);
          friction = 2;
        }
        else if (coll == 2)
        {
          // teleport the car
          models[1].transform.translation.x += 5 * S3L_FRACTIONS_PER_UNIT;
          models[1].transform.translation.z += 2 * S3L_FRACTIONS_PER_UNIT;
        }
        else
        {
          // teleport the car
          models[1].transform.translation.x -= 5 * S3L_FRACTIONS_PER_UNIT;
          models[1].transform.translation.z -= 2 * S3L_FRACTIONS_PER_UNIT;
        }
      }

      if (velocity > 0)
        velocity = S3L_max(0,velocity - stepFriction * friction);
      else
        velocity = S3L_min(0,velocity + stepFriction * friction);

      S3L_Unit cameraDistance =
        S3L_interpolate(S3L_FRACTIONS_PER_UNIT / 2,(3 * S3L_FRACTIONS_PER_UNIT) / 4,S3L_abs(velocity),MAX_VELOCITY);

      scene.camera.transform.translation.x =
        scene.models[1].transform.translation.x - (carDirection.x * cameraDistance) / S3L_FRACTIONS_PER_UNIT;

      scene.camera.transform.translation.z =
        scene.models[1].transform.translation.z - (carDirection.z * cameraDistance) / S3L_FRACTIONS_PER_UNIT;

      scene.camera.transform.rotation.y = models[1].transform.rotation.y;
    }
  }

  return 0;
}
