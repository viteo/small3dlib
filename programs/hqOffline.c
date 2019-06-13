#define S3L_RESOLUTION_X 1280
#define S3L_RESOLUTION_Y 1024

#define S3L_PIXEL_FUNCTION drawPixel

#define S3L_SORT 0
#define S3L_Z_BUFFER 1

#include "../small3dlib.h"
#include <stdio.h>

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
S3L_Index terrainTriangles[GRID_TRIANGLES * 3];
S3L_Unit terrainNormals[GRID_W * GRID_H * 3];

#define MODELS 1

S3L_Model3D models[MODELS];
S3L_Scene scene;

int previousTriangle = -1;

S3L_Vec4 lightDirection;

S3L_Vec4 n0, n1, n2;

void drawPixel(S3L_PixelInfo *p)
{
  if (p->triangleIndex != previousTriangle)
  {
    int index = scene.models[p->modelIndex].triangles[p->triangleIndex * 3] * 3;

    n0.x = terrainNormals[index];
    index++;
    n0.y = terrainNormals[index];
    index++;
    n0.z = terrainNormals[index];

    index = scene.models[p->modelIndex].triangles[p->triangleIndex * 3 + 1] * 3;

    n1.x = terrainNormals[index];
    index++;
    n1.y = terrainNormals[index];
    index++;
    n1.z = terrainNormals[index];
 
    index = scene.models[p->modelIndex].triangles[p->triangleIndex * 3 + 2] * 3;

    n2.x = terrainNormals[index];
    index++;
    n2.y = terrainNormals[index];
    index++;
    n2.z = terrainNormals[index];
  }

  S3L_Vec4 normal;

  normal.x = S3L_interpolateBarycentric(n0.x, n1.x, n2.x,
    p->barycentric[0], p->barycentric[1], p->barycentric[2]);

  normal.y = S3L_interpolateBarycentric(n0.y, n1.y, n2.y,
    p->barycentric[0], p->barycentric[1], p->barycentric[2]);

  normal.z = S3L_interpolateBarycentric(n0.z, n1.z, n2.z,
    p->barycentric[0], p->barycentric[1], p->barycentric[2]);

  S3L_normalizeVec3(&normal);

 
  uint8_t light = 127 - 127 * (S3L_dotProductVec3(lightDirection,normal) / ((float) S3L_FRACTIONS_PER_UNIT));

  uint8_t color[3];

  color[0] = light;
  color[1] = S3L_clamp(p->depth / 64,0,255);
  color[2] = light;

  int index = (p->y * S3L_RESOLUTION_X + p->x) * 3;

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

      terrainTriangles[i + 0] = indices[0];
      terrainTriangles[i + 1] = indices[1];
      terrainTriangles[i + 2] = indices[2];

      terrainTriangles[i + 3] = indices[2];
      terrainTriangles[i + 4] = indices[1];
      terrainTriangles[i + 5] = indices[3];

      i += 6; 
    }
}

void clearFrameBuffer()
{
  memset(frameBuffer,0,S3L_RESOLUTION_X * S3L_RESOLUTION_Y * 3 * sizeof(uint8_t));
}

void saveImage(char *fileName)
{
  printf("saving image file: %s",fileName);

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
    terrainTriangles,
    GRID_TRIANGLES,  
    &(models[MODELS - 1]));

  S3L_computeModelNormals(models[MODELS - 1],terrainNormals,0);

  S3L_initScene(models,MODELS,&scene);

  scene.camera.transform.translation.x = 4 * S3L_FRACTIONS_PER_UNIT;
  scene.camera.transform.translation.y = 6 * S3L_FRACTIONS_PER_UNIT;
  scene.camera.transform.translation.z = -7 * S3L_FRACTIONS_PER_UNIT;
  scene.camera.transform.rotation.x = -S3L_FRACTIONS_PER_UNIT / 8;
  scene.camera.transform.rotation.y = -S3L_FRACTIONS_PER_UNIT / 8;

  clearFrameBuffer();

  S3L_newFrame();

  S3L_drawScene(scene);

  saveImage("test.ppm");

  return 0;
}
