#define S3L_RESOLUTION_X 800
#define S3L_RESOLUTION_Y 600

#define S3L_PIXEL_FUNCTION drawPixel

#define S3L_PERSPECTIVE_CORRECTION 1

#define S3L_SORT 0
#define S3L_Z_BUFFER 1

#include "../small3dlib.h"
#include <stdio.h>
#include <math.h>

uint8_t frameBuffer[S3L_RESOLUTION_X * S3L_RESOLUTION_Y * 3];

#define GRID_W 16
#define GRID_H 16

int8_t heightMap[GRID_W * GRID_H] =
{
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0,
  0,0,0,0,0,1,0,1,3,3,1,1,0,0,0,0,
  0,0,0,1,1,1,1,3,3,4,2,1,1,0,0,0,
  0,0,0,0,1,1,3,4,4,6,5,2,1,1,0,0,
  0,0,1,2,2,3,4,4,4,6,6,4,3,1,0,0,
  0,1,2,4,5,5,6,6,6,6,6,5,5,3,1,0,
  0,0,2,4,6,7,8,7,7,6,6,6,6,6,2,0,
  0,0,3,4,7,8,8,9,8,7,6,6,6,6,2,0,
  0,0,0,2,4,7,7,7,7,6,6,6,4,3,0,0,
  0,0,0,1,3,6,6,6,6,6,6,6,2,0,0,0,
  0,0,0,0,3,6,6,6,6,6,6,2,0,0,0,0,
  0,0,0,1,1,2,3,5,5,5,2,0,0,0,0,0,
  0,0,1,2,0,0,2,4,4,2,2,0,0,0,0,0,
  0,0,0,0,0,0,1,3,3,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

#define GRID_TRIANGLES ((GRID_W - 1) * (GRID_H - 1) * 2)

S3L_Unit terrainVertices[GRID_W * GRID_H * 3];
S3L_Unit terrainNormals[GRID_W * GRID_H * 3];

S3L_Unit waterVertices[GRID_W * GRID_H * 3];
S3L_Unit waterNormals[GRID_W * GRID_H * 3];

S3L_Index gridTriangles[GRID_TRIANGLES * 3];

#define MODELS 2

S3L_Model3D models[MODELS];
S3L_Scene scene;

int previousTriangle = -1;

S3L_Vec4 lightDirection;

S3L_Vec4 n0, n1, n2;

void drawPixel(S3L_PixelInfo *p)
{
  S3L_Unit *normals = p->modelIndex == 0 ? terrainNormals : waterNormals;

  if (p->triangleIndex != previousTriangle)
  {
    int index = scene.models[p->modelIndex].triangles[p->triangleIndex * 3] * 3;

    n0.x = normals[index];
    index++;
    n0.y = normals[index];
    index++;
    n0.z = normals[index];

    index = scene.models[p->modelIndex].triangles[p->triangleIndex * 3 + 1] * 3;

    n1.x = normals[index];
    index++;
    n1.y = normals[index];
    index++;
    n1.z = normals[index];
 
    index = scene.models[p->modelIndex].triangles[p->triangleIndex * 3 + 2] * 3;

    n2.x = normals[index];
    index++;
    n2.y = normals[index];
    index++;
    n2.z = normals[index];
  }

  S3L_Vec4 normal;

  normal.x = S3L_interpolateBarycentric(n0.x, n1.x, n2.x,
    p->barycentric[0], p->barycentric[1], p->barycentric[2]);

  normal.y = S3L_interpolateBarycentric(n0.y, n1.y, n2.y,
    p->barycentric[0], p->barycentric[1], p->barycentric[2]);

  normal.z = S3L_interpolateBarycentric(n0.z, n1.z, n2.z,
    p->barycentric[0], p->barycentric[1], p->barycentric[2]);

  S3L_normalizeVec3(&normal);
 
  float light = 0.5 - (S3L_dotProductVec3(lightDirection,normal) / ((float) S3L_FRACTIONS_PER_UNIT)) * 0.5;

  uint8_t color[3];

  int index = (p->y * S3L_RESOLUTION_X + p->x) * 3;

  if (p->modelIndex == MODELS - 1)
  {
    S3L_Unit waterDepth = p->previousZ - p->depth;

    float transparency = waterDepth / ((float) (S3L_FRACTIONS_PER_UNIT / 3));

    transparency = transparency > 1.0 ? 1.0 : transparency;
  
    float transparency2 = 1.0 - transparency;

    uint8_t previousColor[3];

    previousColor[0] = frameBuffer[index];
    previousColor[1] = frameBuffer[index + 1];
    previousColor[2] = frameBuffer[index + 2];

    color[0] = transparency2 * previousColor[0] + transparency * 100 * light;
    color[1] = transparency2 * previousColor[1] + transparency * 100 * light;
    color[2] = transparency2 * previousColor[2] + transparency * 255 * light;
  }
  else
  {
    color[0] = 255 * light;
    color[1] = 100 * light;
    color[2] = 50 * light;
  }

/*
color[0] = S3L_clamp(127 + normal.x / 4,0,255);
color[1] = S3L_clamp(127 + normal.y / 4,0,255);
color[2] = S3L_clamp(127 + normal.z / 4,0,255);
*/

  frameBuffer[index] = color[0];
  frameBuffer[index + 1] = color[1];
  frameBuffer[index + 2] = color[2];
}

void createGeometry()
{
  int i = 0;

  for (int y = 0; y < GRID_H; ++y)
    for (int x = 0; x < GRID_W; ++x)
     {
       terrainVertices[i] = (x - GRID_W / 2) * S3L_FRACTIONS_PER_UNIT;
       terrainVertices[i + 1] = heightMap[i / 3] * S3L_FRACTIONS_PER_UNIT / 4;
       terrainVertices[i + 2] = (y - GRID_H / 2) * S3L_FRACTIONS_PER_UNIT;

       waterVertices[i] = terrainVertices[i] * 2;
       waterVertices[i + 1] = 0;
       waterVertices[i + 2] = terrainVertices[i + 2] * 2;

       i += 3;
     }

  i = 0;

  for (int y = 0; y < GRID_H - 1; ++y)
    for (int x = 0; x < GRID_W - 1; ++x)
    {
      S3L_Index indices[4];

      indices[0] = y * GRID_W + x;
      indices[1] = indices[0] + 1;
      indices[2] = indices[0] + GRID_W;
      indices[3] = indices[2] + 1;

      gridTriangles[i + 0] = indices[0];
      gridTriangles[i + 1] = indices[1];
      gridTriangles[i + 2] = indices[2];

      gridTriangles[i + 3] = indices[2];
      gridTriangles[i + 4] = indices[1];
      gridTriangles[i + 5] = indices[3];

      i += 6; 
    }
}

void animateWater(int t)
{
  for (int i = 1; i < GRID_W * GRID_H * 3; i += 3)
    waterVertices[i] = S3L_FRACTIONS_PER_UNIT / 2 + sin(i) * S3L_FRACTIONS_PER_UNIT / 4;

  S3L_computeModelNormals(models[MODELS - 1],waterNormals,0);
}

void clearFrameBuffer()
{
  memset(frameBuffer,0,S3L_RESOLUTION_X * S3L_RESOLUTION_Y * 3 * sizeof(uint8_t));
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

int main()
{
  createGeometry(); 

  lightDirection.x = 10;
  lightDirection.y = 10;
  lightDirection.z = 10;
  lightDirection.w = 0;

  S3L_normalizeVec3(&lightDirection);

  S3L_initModel3D(
    terrainVertices,
    GRID_W * GRID_H,
    gridTriangles,
    GRID_TRIANGLES,  
    &(models[0]));

  S3L_computeModelNormals(models[0],terrainNormals,0);

  S3L_initModel3D(
    waterVertices,
    GRID_W * GRID_H,
    gridTriangles,
    GRID_TRIANGLES,  
    &(models[MODELS - 1]));

  S3L_initScene(models,MODELS,&scene);

  animateWater(0);

  char fileName[] = "test00.ppm";
  
  for (int i = 0; i < 20; ++i)
  {
    scene.camera.transform.translation.x = i * S3L_FRACTIONS_PER_UNIT / 16;
    scene.camera.transform.translation.y = 8 * S3L_FRACTIONS_PER_UNIT;
    scene.camera.transform.translation.z = -10 * S3L_FRACTIONS_PER_UNIT;

    S3L_Vec4 target;
    
    target.x = 0;
    target.y = 0;
    target.z = 0;

    S3L_lookAt(scene.camera.transform.translation,target,&scene.camera.transform);

    clearFrameBuffer();

    S3L_newFrame();

    S3L_drawScene(scene);

    fileName[4] = '0' + (i / 10);
    fileName[5] = '0' + (i % 10);

    saveImage(fileName);
  }

  return 0;
}
