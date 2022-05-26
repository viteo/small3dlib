#ifndef PLANT_MODEL_H
#define PLANT_MODEL_H

#define PLANT_VERTEX_COUNT 8
const S3L_Unit plantVertices[PLANT_VERTEX_COUNT * 3] = {
  -1604, -1546,     0,        // 0
   1604, -1546,     0,        // 3
  -1604,  2293,     0,        // 6
   1604,  2293,     0,        // 9
      0, -1546, -1604,        // 12
      0, -1546,  1604,        // 15
      0,  2293, -1604,        // 18
      0,  2293,  1604         // 21
}; // plantVertices

#define PLANT_TRIANGLE_COUNT 4
const S3L_Index plantTriangleIndices[PLANT_TRIANGLE_COUNT * 3] = {
      1,     2,     0,        // 0
      5,     6,     4,        // 3
      1,     3,     2,        // 6
      5,     7,     6         // 9
}; // plantTriangleIndices

#define PLANT_UV_COUNT 8
const S3L_Unit plantUVs[PLANT_UV_COUNT * 2] = {
    512,   512,         // 0
      0,     0,         // 2
      0,   512,         // 4
    512,   512,         // 6
      0,     0,         // 8
      0,   512,         // 10
    512,     0,         // 12
    512,     0          // 14
}; // plantUVs

#define PLANT_UV_INDEX_COUNT 4
const S3L_Index plantUVIndices[PLANT_UV_INDEX_COUNT * 3] = {
      0,     1,     2,        // 0
      3,     4,     5,        // 3
      0,     6,     1,        // 6
      3,     7,     4         // 9
}; // plantUVIndices

S3L_Model3D plantModel;

void plantModelInit(void)
{
  S3L_model3DInit(
    plantVertices,
    PLANT_VERTEX_COUNT,
    plantTriangleIndices,
    PLANT_TRIANGLE_COUNT,
    &plantModel);
}

#endif // guard
