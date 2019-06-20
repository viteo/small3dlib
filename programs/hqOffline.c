/*
  Example program for small3dlib. This is an offline (non-realtime) program
  which creates an animation of a scene with more complex shaders. The
  animation is output in image files (PPM format).

  author: Miloslav Ciz
  licene: CC0 1.0
*/

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
#include "grassNormalTexture.h"
#include "sandTexture.h"
#include "sandNormalTexture.h"
#include "treeModel.h"
#include "treeTexture.h"

uint8_t frameBuffer[S3L_RESOLUTION_X * S3L_RESOLUTION_Y * 3];

int frame = 0;

#define GRID_W 16
#define GRID_H 16

int8_t heightMap[GRID_W * GRID_H] =
{
#define e -1
  e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,
  e,0,0,0,0,1,0,0,1,1,1,0,0,0,0,e,
  e,0,0,0,0,1,0,1,1,1,1,1,0,0,0,e,
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

float interpolate(float a, float b, float t)
{
  return a * (1.0 - t) + b * t;
}

// 0, 1, 2 left for trees
#define ISLAND_MODEL_INDEX 3
#define WATER_MODEL_INDEX 4   // must be last, for transparency
#define MODELS_TOTAL (WATER_MODEL_INDEX + 1)

#define GRID_TRIANGLES ((GRID_W - 1) * (GRID_H - 1) * 2)

S3L_Unit terrainVertices[GRID_W * GRID_H * 3];
S3L_Unit terrainNormals[GRID_W * GRID_H * 3];

S3L_Unit waterVertices[GRID_W * GRID_H * 3];
S3L_Unit waterNormals[GRID_W * GRID_H * 3];

S3L_Index gridTriangles[GRID_TRIANGLES * 3];

S3L_Unit treeNormals[TREE_VERTEX_COUNT * 3];

S3L_Model3D models[MODELS_TOTAL];
S3L_Scene scene;

int previousTriangle = -1;

S3L_Vec4 toLightDirection;

S3L_Vec4 n0, n1, n2, v0, v1, v2;

S3L_Unit uv0[2], uv1[2], uv2[2];

void sampleTexture(uint8_t *texture, int w, int h, float x, float y, uint8_t color[3])
{
  // we do linear interpolation of the samples

  x = fmod(x,1.0);
  y = fmod(y,1.0);

  if (x < 0)
    x = 1.0 + x;

  if (y < 0)
    y = 1.0 + y;

  x *= w;
  y *= h;

  int intX0 = x;

  float xFract = x - intX0;

  int intY0 = y;

  float yFract = y - intY0;

  int intX1 = (intX0 + 1) % w;
  int intY1 = (intY0 + 1) % h;

  int index;
  int maxIndex = w * h * 3 - 1;

  uint8_t c0[3], c1[3], c2[3], c3[3];

  #define getColor(n,i0,i1)\
    index = S3L_clamp((intY##i0 * w + intX##i1) * 3,0,maxIndex);\
    c##n[0] = texture[index];\
    c##n[1] = texture[index + 1];\
    c##n[2] = texture[index + 2];\

  getColor(0,0,0);
  getColor(1,0,1);
  getColor(2,1,0);
  getColor(3,1,1);

  #undef getColor

  color[0] = interpolate(interpolate(c0[0],c1[0],xFract),interpolate(c2[0],c3[0],xFract),yFract);
  color[1] = interpolate(interpolate(c0[1],c1[1],xFract),interpolate(c2[1],c3[1],xFract),yFract);
  color[2] = interpolate(interpolate(c0[2],c1[2],xFract),interpolate(c2[2],c3[2],xFract),yFract);

}

void drawPixel(S3L_PixelInfo *p)
{
  int16_t color[3];

  float u, v;

  float diffuseIntensity, specularIntensity, specularPower;

  S3L_Unit *normals;

  switch (p->modelIndex)
  {
    case 0:
    case 1:
    case 2:
            normals = treeNormals; break;

    case ISLAND_MODEL_INDEX:
            normals = terrainNormals; break;

    case WATER_MODEL_INDEX:
    default:
            normals = waterNormals; break;
  } 

  if (p->triangleID != previousTriangle)
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

    if (p->modelIndex != WATER_MODEL_INDEX &&
        p->modelIndex != ISLAND_MODEL_INDEX)
    {
      index = treeUVIndices[p->triangleIndex * 3] * 2;

      uv0[0] = treeUVs[index];
      index++;
      uv0[1] = treeUVs[index];

      index = treeUVIndices[p->triangleIndex * 3 + 1] * 2;

      uv1[0] = treeUVs[index];
      index++;
      uv1[1] = treeUVs[index];

      index = treeUVIndices[p->triangleIndex * 3 + 2] * 2;

      uv2[0] = treeUVs[index];
      index++;
      uv2[1] = treeUVs[index];
    }

    previousTriangle = p->triangleID;
  }

  S3L_correctBarycentricCoords(p->barycentric);

  S3L_Vec4 position;
  S3L_Vec4 normal;
  S3L_Vec4 toCameraDirection;
  S3L_Vec4 reflected;
  S3L_Unit blend = 0;

  position.x = S3L_interpolateBarycentric(v0.x,v1.x,v2.x,p->barycentric);
  position.y = S3L_interpolateBarycentric(v0.y,v1.y,v2.y,p->barycentric);
  position.z = S3L_interpolateBarycentric(v0.z,v1.z,v2.z,p->barycentric);

  normal.x = S3L_interpolateBarycentric(n0.x,n1.x,n2.x,p->barycentric);
  normal.y = S3L_interpolateBarycentric(n0.y,n1.y,n2.y,p->barycentric);
  normal.z = S3L_interpolateBarycentric(n0.z,n1.z,n2.z,p->barycentric);

  toCameraDirection.x = scene.camera.transform.translation.x - position.x;
  toCameraDirection.y = scene.camera.transform.translation.y - position.y; 
  toCameraDirection.z = scene.camera.transform.translation.z - position.z;
  S3L_normalizeVec3(&toCameraDirection);

  if (p->modelIndex == WATER_MODEL_INDEX)
  {
    diffuseIntensity = 0.6;
    specularIntensity = 0.8;
    specularPower = 40.0;

    float dist, dx, dy;

    // create wavy normal map for water

    dist = position.x + position.z + frame * 5;
    normal.x += S3L_sin(dist) / 8;
    normal.z += S3L_cos(dist) / 8;

    dist = position.x - 2 * position.z + frame * 10;
    normal.x += S3L_sin(dist) / 16;
    normal.z += S3L_cos(dist) / 16;
  }
  else if (p->modelIndex == ISLAND_MODEL_INDEX)
  {
    diffuseIntensity = 0.5;
    specularIntensity = 0.7;
    specularPower = 10.0;

    u = position.x / ((float) S3L_FRACTIONS_PER_UNIT * 2);
    v = position.z / ((float) S3L_FRACTIONS_PER_UNIT * 2);
 
    uint8_t textureNormal[3];
    uint8_t textureNormal2[3];

    sampleTexture(sandNormalTexture,SANDNORMAL_TEXTURE_WIDTH,SANDNORMAL_TEXTURE_HEIGHT,u,v,textureNormal);
    sampleTexture(grassNormalTexture,GRASSNORMAL_TEXTURE_WIDTH,GRASSNORMAL_TEXTURE_HEIGHT,u / 2,v / 2,textureNormal2);

    blend = S3L_clamp(position.y * 4 - S3L_FRACTIONS_PER_UNIT,0,S3L_FRACTIONS_PER_UNIT);

    textureNormal[0] = S3L_interpolateByUnit(textureNormal[0],textureNormal2[0],blend);
    textureNormal[1] = S3L_interpolateByUnit(textureNormal[1],textureNormal2[1],blend);
    textureNormal[2] = S3L_interpolateByUnit(textureNormal[2],textureNormal2[2],blend);

    normal.x += (((int16_t) textureNormal[0]) - 128);
    normal.z += (((int16_t) textureNormal[1]) - 128);
  }
  else // tree
  {
    diffuseIntensity = 0.6;
    specularIntensity = 0.2;
    specularPower = 20.0;

    u = S3L_interpolateBarycentric(uv0[0],uv1[0],uv2[0],p->barycentric) / ((float) S3L_FRACTIONS_PER_UNIT);
    v = S3L_interpolateBarycentric(uv0[1],uv1[1],uv2[1],p->barycentric) / ((float) S3L_FRACTIONS_PER_UNIT);
  }

  S3L_normalizeVec3(&normal);
  S3L_reflect(toLightDirection,normal,&reflected);
 
  float diffuse = 0.5 - (S3L_dotProductVec3(toLightDirection,normal) / ((float) S3L_FRACTIONS_PER_UNIT)) * 0.5;
  float specular = 0.5 + (S3L_dotProductVec3(reflected,toCameraDirection) / ((float) S3L_FRACTIONS_PER_UNIT)) * 0.5;
  float fog = (p->depth / ((float) S3L_FRACTIONS_PER_UNIT * 20));
 
  if (fog > 1.0)
    fog = 1.0;

  float light = 0.9 * fog + diffuseIntensity * diffuse + specularIntensity * pow(specular,specularPower);

  int index = (p->y * S3L_RESOLUTION_X + p->x) * 3;

  if (p->modelIndex == WATER_MODEL_INDEX)
  {
    S3L_Unit waterDepth = (p->previousZ - p->depth) / 2;

    float transparency = waterDepth / ((float) (S3L_FRACTIONS_PER_UNIT / 3));

    transparency = transparency > 1.0 ? 1.0 : transparency;

    if (transparency < 0.2)
      transparency = transparency + 1.0 - transparency / 0.2;

    uint8_t previousColor[3];

    previousColor[0] = frameBuffer[index];
    previousColor[1] = frameBuffer[index + 1];
    previousColor[2] = frameBuffer[index + 2];

    float fresnel = 0.5 + (S3L_dotProductVec3(toCameraDirection,normal) / ((float) S3L_FRACTIONS_PER_UNIT)) * 0.5;

    color[0] = interpolate(150,0,fresnel);
    color[1] = interpolate(230,10,fresnel);
    color[2] = interpolate(255,100,fresnel);

    color[0] = interpolate(previousColor[0],color[0] * light,transparency);
    color[1] = interpolate(previousColor[1],color[1] * light,transparency);
    color[2] = interpolate(previousColor[2],color[2] * light,transparency);
  }
  else if (p->modelIndex == ISLAND_MODEL_INDEX)
  {
    uint8_t textureColor[3];
    uint8_t textureColor2[3];

    sampleTexture(sandTexture,SAND_TEXTURE_WIDTH,SAND_TEXTURE_HEIGHT,u,v,textureColor);
    sampleTexture(grassTexture,GRASS_TEXTURE_WIDTH,GRASS_TEXTURE_HEIGHT,u / 2,v / 2,textureColor2);

    textureColor[0] = S3L_interpolateByUnit(textureColor[0],textureColor2[0],blend);
    textureColor[1] = S3L_interpolateByUnit(textureColor[1],textureColor2[1],blend);
    textureColor[2] = S3L_interpolateByUnit(textureColor[2],textureColor2[2],blend);

    color[0] = textureColor[0] * light;
    color[1] = textureColor[1] * light;
    color[2] = textureColor[2] * light;
  }
  else // tree
  {
    uint8_t textureColor[3];

    sampleTexture(treeTexture,TREE_TEXTURE_WIDTH,TREE_TEXTURE_HEIGHT,u,v,textureColor);

    color[0] = textureColor[0] * light;
    color[1] = textureColor[1] * light;
    color[2] = textureColor[2] * light;
  }

  frameBuffer[index] = S3L_clamp(color[0],0,255);
  frameBuffer[index + 1] = S3L_clamp(color[1],0,255);
  frameBuffer[index + 2] = S3L_clamp(color[2],0,255);
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

       waterVertices[i] = terrainVertices[i];
       waterVertices[i + 1] = 0;
       waterVertices[i + 2] = terrainVertices[i + 2];

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

  S3L_computeModelNormals(models[WATER_MODEL_INDEX],waterNormals,0);
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

  treeModelInit();

  models[0] = treeModel;
  models[1] = treeModel;
  models[2] = treeModel;

  S3L_Unit scale = S3L_FRACTIONS_PER_UNIT / 4;

  S3L_setTransform3D(0,1.2 * S3L_FRACTIONS_PER_UNIT,-1.5 * S3L_FRACTIONS_PER_UNIT,0,0,0,scale,scale,scale,&(models[0].transform));
  S3L_setTransform3D(0.95 * S3L_FRACTIONS_PER_UNIT,1.3 * S3L_FRACTIONS_PER_UNIT,0,0,0,0,scale,scale * 1.3,scale,&(models[1].transform));
  S3L_setTransform3D(-2 * S3L_FRACTIONS_PER_UNIT,0.8 * S3L_FRACTIONS_PER_UNIT,1.5 * S3L_FRACTIONS_PER_UNIT,0,0,0,scale,scale,scale,&(models[2].transform));

  S3L_initModel3D(
    terrainVertices,
    GRID_W * GRID_H,
    gridTriangles,
    GRID_TRIANGLES,  
    &(models[ISLAND_MODEL_INDEX]));

  S3L_computeModelNormals(models[ISLAND_MODEL_INDEX],terrainNormals,0);
  S3L_computeModelNormals(treeModel,treeNormals,0);

  S3L_initModel3D(
    waterVertices,
    GRID_W * GRID_H,
    gridTriangles,
    GRID_TRIANGLES,  
    &(models[WATER_MODEL_INDEX]));

  S3L_initScene(models,MODELS_TOTAL,&scene);

  char fileName[] = "test00.ppm";

  S3L_Transform3D transform0, transform1;
  S3L_Vec4 target0, target1;

  S3L_initTransoform3D(&transform0);
  S3L_initTransoform3D(&transform1);

  target0 = scene.models[0].transform.translation;
  target1 = scene.models[2].transform.translation;

  transform0.translation.x = -2 * S3L_FRACTIONS_PER_UNIT;
  transform0.translation.y = 5 * S3L_FRACTIONS_PER_UNIT;
  transform0.translation.z = -14 * S3L_FRACTIONS_PER_UNIT;

  transform0.rotation.x = -S3L_FRACTIONS_PER_UNIT / 12;
  transform1.rotation.y = S3L_FRACTIONS_PER_UNIT / 8;

  transform1.translation.x = 5 * S3L_FRACTIONS_PER_UNIT;
  transform1.translation.y = 6 * S3L_FRACTIONS_PER_UNIT;
  transform1.translation.z = 3 * S3L_FRACTIONS_PER_UNIT;

  transform1.rotation.x = transform0.rotation.x;
  transform1.rotation.y = transform0.rotation.y;

  int frames = 100;
 
  for (int i = 0; i < frames; ++i) // render the frames
  {
    animateWater();

    float t = i / ((float) frames);

    scene.camera.transform.translation.x = interpolate(transform0.translation.x,transform1.translation.x,t);
    scene.camera.transform.translation.y = interpolate(transform0.translation.y,transform1.translation.y,t);
    scene.camera.transform.translation.z = interpolate(transform0.translation.z,transform1.translation.z,t);

    scene.camera.transform.rotation.x = interpolate(transform0.rotation.x,transform1.rotation.x,t);
    scene.camera.transform.rotation.y = interpolate(transform0.rotation.y,transform1.rotation.y,t);

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
