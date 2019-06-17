#ifndef CAR_MODEL_H
#define CAR_MODEL_H

#define CAR_VERTEX_COUNT 12
const S3L_Unit carVertices[CAR_VERTEX_COUNT * 3] = {
   -159,    27,  -121,        // 0
   -119,   205,   -88,        // 3
   -119,   205,    88,        // 6
     47,    27,  -121,        // 9
      7,   205,   -88,        // 12
      7,   205,    88,        // 15
   -152,   116,   159,        // 18
     40,   116,   159,        // 21
   -159,   103,   267,        // 24
   -144,    27,   267,        // 27
     32,    27,   267,        // 30
     47,   103,   267         // 33
}; // carVertices

#define CAR_TRIANGLE_COUNT 18
const S3L_Index carTriangleIndices[CAR_TRIANGLE_COUNT * 3] = {
      4,     3,     5,        // 0
      2,     7,     6,        // 3
      1,     0,     4,        // 6
      7,     5,     3,        // 9
      2,     4,     5,        // 12
      2,     0,     1,        // 15
      9,     6,     8,        // 18
      7,     8,     6,        // 21
      3,     4,     0,        // 24
      8,    10,     9,        // 27
      7,     3,    10,        // 30
      0,     6,     9,        // 33
      6,     0,     2,        // 36
     10,    11,     7,        // 39
      2,     5,     7,        // 42
      2,     1,     4,        // 45
      7,    11,     8,        // 48
      8,    11,    10         // 51
}; // carTriangleIndices

#define CAR_UV_COUNT 24
const S3L_Unit carUVs[CAR_UV_COUNT * 2] = {
    451,   476,         // 0
    459,   509,         // 2
    422,   477,         // 4
    422,   476,         // 6
    409,   451,         // 8
    409,   476,         // 10
    451,   476,         // 12
    484,   476,         // 14
    451,   451,         // 16
    409,   492,         // 18
    422,   451,         // 20
    422,   477,         // 22
    459,   509,         // 24
    451,   476,         // 26
    398,   509,         // 28
    409,   492,         // 30
    398,   493,         // 32
    397,   476,         // 34
    484,   451,         // 36
    386,   451,         // 38
    386,   476,         // 40
    398,   509,         // 42
    398,   493,         // 44
    397,   451          // 46
}; // carUVs

#define CAR_UV_INDEX_COUNT 18
const S3L_Index carUVIndices[CAR_UV_INDEX_COUNT * 3] = {
      0,     1,     2,        // 0
      3,     4,     5,        // 3
      6,     7,     8,        // 6
      9,     2,     1,        // 9
      3,     8,    10,        // 12
     11,    12,    13,        // 15
     14,    15,    16,        // 18
      4,    17,     5,        // 21
     18,     8,     7,        // 24
     17,    19,    20,        // 27
      9,     1,    21,        // 30
     12,    15,    14,        // 33
     15,    12,    11,        // 36
     21,    22,     9,        // 39
      3,    10,     4,        // 42
      3,     6,     8,        // 45
      4,    23,    17,        // 48
     17,    23,    19         // 51
}; // carUVIndices

S3L_Model3D carModel;

void carModelInit()
{
  S3L_initModel3D(
    carVertices,
    CAR_VERTEX_COUNT,
    carTriangleIndices,
    CAR_TRIANGLE_COUNT,
    &carModel);
}

#endif // guard
