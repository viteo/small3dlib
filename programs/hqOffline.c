#define S3L_RESOLUTION_X 800
#define S3L_RESOLUTION_Y 600

#define S3L_PIXEL_FUNCTION drawPixel

#define S3L_PERSPECTIVE_CORRECTION 1

#define S3L_STRICT_NEAR_CULLING 0

#define S3L_SORT 0
#define S3L_Z_BUFFER 1

#include "../small3dlib.h"
#include <stdio.h>
#include <math.h>

#include "grassTexture.h"

uint8_t frameBuffer[S3L_RESOLUTION_X * S3L_RESOLUTION_Y * 3];

int frame = 0;

#define GRID_W 16
#define GRID_H 16

int8_t heightMap[GRID_W * GRID_H] =
{
#define e -1
  e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,
  e,0,0,0,0,1,0,0,1,1,1,0,0,0,0,e,
  e,0,0,0,0,1,0,1,2,1,1,1,0,0,0,e,
  e,0,0,1,1,1,1,3,2,1,1,1,1,0,0,e,
  e,0,0,0,1,1,2,4,3,2,1,2,1,1,0,e,
  e,0,1,2,2,2,2,4,4,2,2,2,2,1,0,e,
  e,1,2,2,3,3,6,6,6,3,6,3,5,3,1,e,
  e,0,2,2,3,7,8,7,7,6,6,6,6,6,2,e,
  e,0,3,3,3,8,8,9,8,7,2,3,6,6,2,e,
  e,0,0,2,3,4,7,7,7,6,1,1,4,3,0,e,
  e,0,0,1,3,6,3,5,6,6,3,1,2,0,0,e,
  e,0,0,0,3,3,3,6,6,6,6,1,0,0,0,e,
  e,0,0,1,1,2,3,5,5,5,2,0,0,0,0,e,
  e,0,1,2,0,0,2,4,4,2,2,0,0,0,0,e,
  e,0,0,0,0,0,1,3,3,0,0,0,0,0,0,e,
  e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e
#undef e
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

S3L_Vec4 toLightDirection;

S3L_Vec4 n0, n1, n2, v0, v1, v2;

void sampleTexture(uint8_t *texture, int w, int h, float x, float y, char color[3])
{
  x = fmod(x,1.0);
  y = fmod(y,1.0);

  int intX = x * w;
  int intY = y * h;

  int index = S3L_clamp((intY * w + intX) * 3,0,w * h - 1);

  color[0] = texture[index];
  color[1] = texture[index + 1];
  color[2] = texture[index + 2];
}

void drawPixel(S3L_PixelInfo *p)
{
  S3L_Unit *normals = p->modelIndex == 0 ? terrainNormals : waterNormals;

  if (p->triangleIndex != previousTriangle)
  {
    int index = scene.models[p->modelIndex].triangles[p->triangleIndex * 3] * 3;

    n0.x = normals[index];
    v0.x = scene.models[p->modelIndex].vertices[index];
    index++;
    n0.y = normals[index];
    v0.y = scene.models[p->modelIndex].vertices[index];
    index++;
    n0.z = normals[index];
    v0.z = scene.models[p->modelIndex].vertices[index];

    index = scene.models[p->modelIndex].triangles[p->triangleIndex * 3 + 1] * 3;

    n1.x = normals[index];
    v1.x = scene.models[p->modelIndex].vertices[index];
    index++;
    n1.y = normals[index];
    v1.y = scene.models[p->modelIndex].vertices[index];
    index++;
    n1.z = normals[index];
    v1.z = scene.models[p->modelIndex].vertices[index];
 
    index = scene.models[p->modelIndex].triangles[p->triangleIndex * 3 + 2] * 3;

    n2.x = normals[index];
    v2.x = scene.models[p->modelIndex].vertices[index];
    index++;
    n2.y = normals[index];
    v2.y = scene.models[p->modelIndex].vertices[index];
    index++;
    n2.z = normals[index];
    v2.z = scene.models[p->modelIndex].vertices[index];
  }

  S3L_Vec4 position;

  position.x = S3L_interpolateBarycentric(v0.x,v1.x,v2.x,p->barycentric[0],p->barycentric[1],p->barycentric[2]);
  position.y = S3L_interpolateBarycentric(v0.y,v1.y,v2.y,p->barycentric[0],p->barycentric[1],p->barycentric[2]);
  position.z = S3L_interpolateBarycentric(v0.z,v1.z,v2.z,p->barycentric[0],p->barycentric[1],p->barycentric[2]);

  S3L_Vec4 normal;

  normal.x = S3L_interpolateBarycentric(n0.x, n1.x, n2.x,
    p->barycentric[0], p->barycentric[1], p->barycentric[2]);

  normal.y = S3L_interpolateBarycentric(n0.y, n1.y, n2.y,
    p->barycentric[0], p->barycentric[1], p->barycentric[2]);

  normal.z = S3L_interpolateBarycentric(n0.z, n1.z, n2.z,
    p->barycentric[0], p->barycentric[1], p->barycentric[2]);

if (p->modelIndex == MODELS - 1)
{

float dist, dx, dy;

dist = position.x + position.z + frame * 5;
normal.x += S3L_sin(dist / 2) / 8;
normal.y += S3L_cos(dist / 2) / 8;

dist = position.x - 2 * position.z + frame * 10;
normal.x += S3L_sin(dist) / 64;
normal.y += S3L_cos(dist) / 64;



}

  S3L_normalizeVec3(&normal);

  S3L_Vec4 reflected;

  S3L_Vec4 toCameraDirection;

  toCameraDirection.x = scene.camera.transform.translation.x - position.x;
  toCameraDirection.y = scene.camera.transform.translation.y - position.y; 
  toCameraDirection.z = scene.camera.transform.translation.z - position.z;

  S3L_normalizeVec3(&toCameraDirection);

  S3L_reflect(toLightDirection,normal,&reflected);
 
  float diffuse = 0.5 - (S3L_dotProductVec3(toLightDirection,normal) / ((float) S3L_FRACTIONS_PER_UNIT)) * 0.5;
  float specular = 0.5 + (S3L_dotProductVec3(reflected,toCameraDirection) / ((float) S3L_FRACTIONS_PER_UNIT)) * 0.5;
  float fog = (p->depth / ((float) S3L_FRACTIONS_PER_UNIT * 20));
 
  if (fog > 1.0)
    fog = 1.0;

  float light = 0.3 * fog + 0.6 * diffuse + 0.5 * pow(specular,20.0);

  uint8_t color[3];

  int index = (p->y * S3L_RESOLUTION_X + p->x) * 3;

  if (p->modelIndex == MODELS - 1)
  {
    S3L_Unit waterDepth = (p->previousZ - p->depth);

    float transparency = waterDepth / ((float) (S3L_FRACTIONS_PER_UNIT / 3));

    transparency = transparency > 1.0 ? 1.0 : transparency;
  
    float transparency2 = 1.0 - transparency;

    uint8_t previousColor[3];

    previousColor[0] = frameBuffer[index];
    previousColor[1] = frameBuffer[index + 1];
    previousColor[2] = frameBuffer[index + 2];


float fresnel = 0.5 + (S3L_dotProductVec3(toCameraDirection,normal) / ((float) S3L_FRACTIONS_PER_UNIT)) * 0.5;
float fresnel2 = 1.0 - fresnel;

color[0] = fresnel2 * 150 + fresnel * 0;
color[1] = fresnel2 * 230 + fresnel * 10;
color[2] = fresnel2 * 255 + fresnel * 100;

    color[0] = S3L_clamp(transparency2 * previousColor[0] + transparency * color[0] * light,0,255);
    color[1] = S3L_clamp(transparency2 * previousColor[1] + transparency * color[1] * light,0,255);
    color[2] = S3L_clamp(transparency2 * previousColor[2] + transparency * color[2] * light,0,255);




  }
  else
  {

char textureColor[3];

sampleTexture(grassTexture,GRASS_TEXTURE_WIDTH,GRASS_TEXTURE_HEIGHT,
position.x / ((float) S3L_FRACTIONS_PER_UNIT),
position.z / ((float) S3L_FRACTIONS_PER_UNIT),textureColor);

    color[0] = S3L_clamp(textureColor[0] * light,0,255);
    color[1] = S3L_clamp(textureColor[1] * light,0,255);
    color[2] = S3L_clamp(textureColor[2] * light,0,255);
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
       terrainVertices[i + 1] = (heightMap[i / 3] - 1) * S3L_FRACTIONS_PER_UNIT / 4;
       terrainVertices[i + 2] = (y - GRID_H / 2) * S3L_FRACTIONS_PER_UNIT;

       waterVertices[i] = terrainVertices[i] * 8;
       waterVertices[i + 1] = 0;
       waterVertices[i + 2] = terrainVertices[i + 2] * 8;

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

void animateWater()
{
  for (int i = 1; i < GRID_W * GRID_H * 3; i += 3)
    waterVertices[i] = S3L_FRACTIONS_PER_UNIT / 4 + sin(frame * 0.2) * S3L_FRACTIONS_PER_UNIT / 4;

  S3L_computeModelNormals(models[MODELS - 1],waterNormals,0);
}

void clearFrameBuffer()
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

int main()
{
  createGeometry(); 

  toLightDirection.x = 10;
  toLightDirection.y = 10;
  toLightDirection.z = 10;
  toLightDirection.w = 0;

  S3L_normalizeVec3(&toLightDirection);

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

  char fileName[] = "test00.ppm";
  
  for (int i = 0; i < 20; ++i)
  {
    animateWater();

    scene.camera.transform.translation.x = -i * S3L_FRACTIONS_PER_UNIT / 4;
    scene.camera.transform.translation.y = 5 * S3L_FRACTIONS_PER_UNIT;
    scene.camera.transform.translation.z = -8 * S3L_FRACTIONS_PER_UNIT + i * S3L_FRACTIONS_PER_UNIT / 4;

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

    frame++;
  }

  return 0;
}
