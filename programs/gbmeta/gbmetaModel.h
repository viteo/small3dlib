#ifndef GBMETA_MODEL_H
#define GBMETA_MODEL_H

#define GBMETA_VERTEX_COUNT 12
const S3L_Unit gbmetaVertices[GBMETA_VERTEX_COUNT * 3] = {
  -2732,  1591,  -512,        // 0
  -2732, -1591,  -512,        // 3
  -2732,  1591,   511,        // 6
  -2732, -1591,   511,        // 9
   2732,  1591,  -511,        // 12
   2732, -1591,  -511,        // 15
   2732,  1591,   512,        // 18
   2732, -1591,   512,        // 21
   3198, -1591,     0,        // 24
  -3198,  1591,     0,        // 27
   3198,  1591,     0,        // 30
  -3198, -1591,     0         // 33
}; // gbmetaVertices

#define GBMETA_TRIANGLE_COUNT 20
const S3L_Index gbmetaTriangleIndices[GBMETA_TRIANGLE_COUNT * 3] = {
     11,     3,     1,        // 0
      2,     7,     3,        // 3
     10,     5,     8,        // 6
      8,     5,     7,        // 9
      9,     3,    11,        // 12
      4,     1,     5,        // 15
      0,    11,     1,        // 18
     10,     6,     4,        // 21
      6,     8,     7,        // 24
      3,     5,     1,        // 27
      0,     6,     2,        // 30
      9,     0,     2,        // 33
      2,     6,     7,        // 36
     10,     4,     5,        // 39
      9,     2,     3,        // 42
      4,     0,     1,        // 45
      0,     9,    11,        // 48
      6,    10,     8,        // 51
      3,     7,     5,        // 54
      0,     4,     6         // 57
}; // gbmetaTriangleIndices

#define GBMETA_UV_COUNT 28
const S3L_Unit gbmetaUVs[GBMETA_UV_COUNT * 2] = {
    261,    37,         // 0
    278,    55,         // 2
    278,    18,         // 4
    451,    82,         // 6
     56,   287,         // 8
    451,   287,         // 10
    501,   297,         // 12
    451,   501,         // 14
    501,   501,         // 16
    496,    37,         // 18
    479,    18,         // 20
    479,    55,         // 22
    501,    82,         // 24
    501,   287,         // 26
    451,   297,         // 28
     57,   501,         // 30
     57,   297,         // 32
      7,   501,         // 34
     12,    36,         // 36
     29,    55,         // 38
     29,    17,         // 40
     56,    82,         // 42
      6,   287,         // 44
    230,    17,         // 46
    230,    55,         // 48
    247,    36,         // 50
      7,   297,         // 52
      6,    82          // 54
}; // gbmetaUVs

#define GBMETA_UV_INDEX_COUNT 20
const S3L_Index gbmetaUVIndices[GBMETA_UV_INDEX_COUNT * 3] = {
      0,     1,     2,        // 0
      3,     4,     5,        // 3
      6,     7,     8,        // 6
      9,    10,    11,        // 9
     12,     5,    13,        // 12
     14,    15,     7,        // 15
     16,    17,    15,        // 18
     18,    19,    20,        // 21
     21,    22,     4,        // 24
      1,    10,     2,        // 27
     23,    19,    24,        // 30
     25,    23,    24,        // 33
      3,    21,     4,        // 36
      6,    14,     7,        // 39
     12,     3,     5,        // 42
     14,    16,    15,        // 45
     16,    26,    17,        // 48
     21,    27,    22,        // 51
      1,    11,    10,        // 54
     23,    20,    19         // 57
}; // gbmetaUVIndices

S3L_Model3D gbmetaModel;

void gbmetaModelInit()
{
  S3L_initModel3D(
    gbmetaVertices,
    GBMETA_VERTEX_COUNT,
    gbmetaTriangleIndices,
    GBMETA_TRIANGLE_COUNT,
    &gbmetaModel);
}

#endif // guard
