/*
  Example program for small3dlib, testing a high-poly offline model.

  author: Miloslav Ciz
  license: CC0 1.0
*/

#include <stdio.h>

#define S3L_STRICT_NEAR_CULLING 0

#if TEXTURES
  #define S3L_PERSPECTIVE_CORRECTION 2
#else
  #define S3L_PERSPECTIVE_CORRECTION 0
#endif

#define S3L_NEAR (S3L_FRACTIONS_PER_UNIT / 5)

#define S3L_Z_BUFFER 1

#define S3L_PIXEL_FUNCTION drawPixel

#define S3L_RESOLUTION_X 800
#define S3L_RESOLUTION_Y 600

#include "../small3dlib.h"

#include "alligatorModel.h"

S3L_Unit normals[ALLIGATOR_VERTEX_COUNT * 3];

uint8_t frameBuffer[S3L_RESOLUTION_X * S3L_RESOLUTION_Y * 3];

S3L_Scene scene;

S3L_Vec4 teleportPoint;

uint32_t pixels[S3L_RESOLUTION_X * S3L_RESOLUTION_Y];

void clearScreen()
{
  memset(frameBuffer,255,S3L_RESOLUTION_X * S3L_RESOLUTION_Y * 3 * sizeof(uint8_t));
}

void saveImage(char *fileName)
{
  printf("saving image file: %s\n",fileName);

  FILE *f = fopen(fileName,"w");
  
  fprintf(f,"P3\n%d %d\n255\n",S3L_RESOLUTION_X,S3L_RESOLUTION_Y);

  for (int i = 0; i < S3L_RESOLUTION_X * S3L_RESOLUTION_Y * 3; i += 3)
    fprintf(f,"%d %d %d\n",frameBuffer[i],frameBuffer[i + 1],frameBuffer[i + 2]);

  fclose(f);
}

uint32_t previousTriangle = 1000;
S3L_Vec4 n0, n1, n2, toLight;

void drawPixel(S3L_PixelInfo *p)
{
  if (p->triangleID != previousTriangle)
  {
    S3L_getIndexedTriangleValues(
      p->triangleIndex,
      scene.models[p->modelIndex].triangles,
      normals,3,&n0,&n1,&n2);

    previousTriangle = p->triangleID;
  }

  S3L_Vec4 normal;

  normal.x = S3L_interpolateBarycentric(n0.x,n1.x,n2.x,p->barycentric);
  normal.y = S3L_interpolateBarycentric(n0.y,n1.y,n2.y,p->barycentric);
  normal.z = S3L_interpolateBarycentric(n0.z,n1.z,n2.z,p->barycentric);

  S3L_normalizeVec3(&normal);

  S3L_Unit shading = 
    (S3L_dotProductVec3(normal,toLight) + S3L_FRACTIONS_PER_UNIT) / 2;

  shading = S3L_interpolate(shading,0,p->depth,32 * S3L_FRACTIONS_PER_UNIT);

  int index = (p->y * S3L_RESOLUTION_X + p->x) * 3;

  frameBuffer[index] = S3L_clamp(S3L_interpolateByUnitFrom0(200,shading),0,255);
  frameBuffer[index + 1] = S3L_clamp(S3L_interpolateByUnitFrom0(255,shading),0,255);
  frameBuffer[index + 2] = S3L_clamp(S3L_interpolateByUnitFrom0(150,shading),0,255); 
}

int main()
{
  S3L_setVec4(&toLight,10,-10,-10,0);

  S3L_normalizeVec3(&toLight);

  alligatorModelInit();
 
  S3L_computeModelNormals(alligatorModel,normals,0);

  S3L_initScene(&alligatorModel,1,&scene);

  scene.camera.transform.translation.z = -8 * S3L_FRACTIONS_PER_UNIT;
  scene.camera.transform.translation.x = 9 * S3L_FRACTIONS_PER_UNIT;
  scene.camera.transform.translation.y = 6 * S3L_FRACTIONS_PER_UNIT;

  S3L_lookAt(scene.models[0].transform.translation,&(scene.camera.transform));

  clearScreen();
  S3L_newFrame();
  S3L_drawScene(scene);

  saveImage("allligator.ppm");

  return 0;
}
