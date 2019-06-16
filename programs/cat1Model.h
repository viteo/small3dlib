#ifndef CAT1_MODEL_H
#define CAT1_MODEL_H

#define CAT1_VERTEX_COUNT 59
const S3L_Unit cat1Vertices[CAT1_VERTEX_COUNT * 3] = {
    289,   327,  1473,        // 0
    540,  1038,  1979,        // 3
    392,  1897,  2253,        // 6
    321,  -201,  1127,        // 9
    325,  -806,   910,        // 12
    325, -1445,   411,        // 15
    285, -1351,   112,        // 18
    259,  -876,   528,        // 21
    175,  -350,   409,        // 24
    245,  -384,  -433,        // 27
    306,   938, -1369,        // 30
    282,  1307, -1321,        // 33
    165,   553, -1503,        // 36
    353,     7, -1013,        // 39
    411, -1222,  -218,        // 42
    260,   451,  -836,        // 45
    297, -1459,  -615,        // 48
    254, -1133,  -495,        // 51
    273,   344,   336,        // 54
    294,   761, -1783,        // 57
    174, -1204,  -229,        // 60
    124,  -806,   910,        // 63
    124, -1429,   433,        // 66
   -209,   469,  1497,        // 69
     -9,   655,  -807,        // 72
    277,   997,  1976,        // 75
    226,  1888,  2216,        // 78
    365,  1781,  2015,        // 81
    404,  1057,  1757,        // 84
     62,   596,  1249,        // 87
      0,   548,   145,        // 90
   -321,  -163,  1268,        // 93
   -257,  -349,  1550,        // 96
   -253, -1255,  1737,        // 99
   -212, -1409,  1475,        // 102
   -259,  -624,  1342,        // 105
   -175,  -435,   743,        // 108
   -245,  -392,  -588,        // 111
   -462,   793, -1370,        // 114
   -566,  1137, -1218,        // 117
   -129,  1139, -1544,        // 120
   -128,  1136, -1756,        // 123
    -58,   770, -2070,        // 126
    -25,   591, -2061,        // 129
   -190,   503, -1409,        // 132
   -347,   -61,  -964,        // 135
   -182, -1396, -1457,        // 138
   -260,   520,  -841,        // 141
    -22, -1223, -1865,        // 144
    -61, -1157, -1629,        // 147
   -273,   320,   184,        // 150
   -385,   650, -1689,        // 153
   -124,  1109, -1264,        // 156
     -2,   469, -1595,        // 159
      0,  -233, -1107,        // 162
     44, -1369, -1459,        // 165
      0,  -183,  1349,        // 168
    -56,  -349,  1550,        // 171
    -52, -1176,  1700         // 174
}; // cat1Vertices

#define CAT1_TRIANGLE_COUNT 114
const S3L_Index cat1TriangleIndices[CAT1_TRIANGLE_COUNT * 3] = {
      9,    13,    14,        // 0
     16,    14,    17,        // 3
     30,    15,    18,        // 6
     29,    18,     0,        // 9
     15,    13,     9,        // 12
     18,    15,     9,        // 15
      9,     8,    18,        // 18
     28,    29,     0,        // 21
      0,     1,    28,        // 24
     27,    28,     2,        // 27
      1,     2,    28,        // 30
      8,     3,    18,        // 33
      0,    18,     3,        // 36
      8,     7,     3,        // 39
      3,     7,     4,        // 42
      5,     4,     7,        // 45
      6,     5,     7,        // 48
     11,    40,    10,        // 51
     43,    12,    19,        // 54
     19,    42,    43,        // 57
     41,    42,    19,        // 60
     19,    10,    41,        // 63
     41,    10,    40,        // 66
     19,    12,    10,        // 69
     24,    10,    15,        // 72
     10,    12,    15,        // 75
     12,    13,    15,        // 78
     10,    52,    11,        // 81
     11,    52,    40,        // 84
     24,    52,    10,        // 87
     43,    53,    12,        // 90
     13,    53,    54,        // 93
     17,    20,    16,        // 96
      9,    14,    20,        // 99
     16,    20,    14,        // 102
     54,    20,    17,        // 105
      9,    20,    54,        // 108
      1,    26,     2,        // 111
     22,     4,     5,        // 114
     21,     3,     4,        // 117
      5,     6,    22,        // 120
      7,    22,     6,        // 123
      8,    21,     7,        // 126
      3,    56,     0,        // 129
     56,     8,    36,        // 132
     14,    13,    17,        // 135
     54,    17,    13,        // 138
     37,    46,    45,        // 141
     48,    49,    46,        // 144
     30,    47,    24,        // 147
     29,    50,    30,        // 150
     47,    37,    45,        // 153
     50,    37,    47,        // 156
     37,    50,    36,        // 159
     28,    23,    29,        // 162
     23,    28,    25,        // 165
     27,    26,    28,        // 168
     25,    28,    26,        // 171
     36,    50,    31,        // 174
     23,    31,    50,        // 177
     36,    31,    35,        // 180
     31,    32,    35,        // 183
     33,    35,    32,        // 186
     34,    35,    33,        // 189
     39,    38,    40,        // 192
     43,    51,    44,        // 195
     51,    43,    42,        // 198
     41,    51,    42,        // 201
     51,    41,    38,        // 204
     41,    40,    38,        // 207
     51,    38,    44,        // 210
     24,    47,    38,        // 213
     38,    47,    44,        // 216
     44,    47,    45,        // 219
     38,    39,    52,        // 222
     39,    40,    52,        // 225
     24,    38,    52,        // 228
     43,    44,    53,        // 231
     54,    44,    45,        // 234
     49,    48,    55,        // 237
     37,    55,    46,        // 240
     48,    46,    55,        // 243
     54,    49,    55,        // 246
     37,    54,    55,        // 249
      2,    26,    27,        // 252
      0,    25,     1,        // 255
     32,    58,    33,        // 258
     32,    56,    57,        // 261
     33,    58,    34,        // 264
     58,    35,    34,        // 267
     35,    56,    36,        // 270
     31,    23,    56,        // 273
     56,    23,     0,        // 276
     37,     8,     9,        // 279
     54,    37,     9,        // 282
     46,    49,    45,        // 285
     54,    45,    49,        // 288
     30,    24,    15,        // 291
     29,    30,    18,        // 294
     13,    12,    53,        // 297
      1,    25,    26,        // 300
     22,    21,     4,        // 303
     21,    56,     3,        // 306
      7,    21,    22,        // 309
      8,    56,    21,        // 312
     30,    50,    47,        // 315
     29,    23,    50,        // 318
     54,    53,    44,        // 321
      0,    23,    25,        // 324
     32,    57,    58,        // 327
     32,    31,    56,        // 330
     58,    57,    35,        // 333
     35,    57,    56,        // 336
     37,    36,     8         // 339
}; // cat1TriangleIndices

#define CAT1_UV_COUNT 90
const S3L_Unit cat1UVs[CAT1_UV_COUNT * 2] = {
    191,   369,         // 0
    142,   329,         // 2
    159,   509,         // 4
    109,   504,         // 6
    140,   472,         // 8
    289,   243,         // 10
    148,   264,         // 12
    296,   277,         // 14
    419,   232,         // 16
    438,   278,         // 18
    341,   371,         // 20
    479,   146,         // 22
    506,   150,         // 24
    460,    66,         // 26
    486,    66,         // 28
    421,   337,         // 30
    416,   428,         // 32
    454,   421,         // 34
    394,   498,         // 36
    368,   493,         // 38
     98,   169,         // 40
     67,   155,         // 42
     96,   207,         // 44
     16,   251,         // 46
     81,   270,         // 48
     64,   239,         // 50
      7,   216,         // 52
     50,   153,         // 54
    164,   242,         // 56
    117,   163,         // 58
     76,   276,         // 60
    127,   350,         // 62
    135,   117,         // 64
    153,    92,         // 66
    114,    86,         // 68
    211,   167,         // 70
    157,    64,         // 72
    154,   202,         // 74
    478,    75,         // 76
    412,   488,         // 78
    464,   418,         // 80
    354,     5,         // 82
    341,    39,         // 84
    374,    29,         // 86
    402,    96,         // 88
    346,   161,         // 90
    449,   100,         // 92
    435,   343,         // 94
    389,   371,         // 96
    340,   347,         // 98
    337,   366,         // 100
    191,   369,         // 102
    159,   509,         // 104
    142,   329,         // 106
    109,   504,         // 108
    140,   472,         // 110
    148,   264,         // 112
    296,   277,         // 114
    341,   371,         // 116
    420,   278,         // 118
    490,   154,         // 120
    421,   337,         // 122
    416,   428,         // 124
    454,   421,         // 126
    394,   498,         // 128
    368,   493,         // 130
     98,   170,         // 132
     96,   207,         // 134
     64,   239,         // 136
     81,   270,         // 138
    135,   117,         // 140
    114,    86,         // 142
    153,    92,         // 144
    211,   167,         // 146
    157,    64,         // 148
    412,   488,         // 150
    464,   418,         // 152
    354,     5,         // 154
    374,    29,         // 156
    341,    39,         // 158
    402,    96,         // 160
    435,   172,         // 162
    346,   161,         // 164
    438,   277,         // 166
    205,   362,         // 168
    203,   341,         // 170
    166,   366,         // 172
    204,   363,         // 174
    449,   100,         // 176
    336,   366          // 178
}; // cat1UVs

#define CAT1_UV_INDEX_COUNT 114
const S3L_Index cat1UVIndices[CAT1_UV_INDEX_COUNT * 3] = {
      0,     1,     2,        // 0
      3,     2,     4,        // 3
      5,     6,     7,        // 6
      8,     7,     9,        // 9
      6,     1,     0,        // 12
      7,     6,     0,        // 15
      0,    10,     7,        // 18
     11,     8,     9,        // 21
      9,    12,    11,        // 24
     13,    11,    14,        // 27
     12,    14,    11,        // 30
     10,    15,     7,        // 33
      9,     7,    15,        // 36
     10,    16,    15,        // 39
     15,    16,    17,        // 42
     18,    17,    16,        // 45
     19,    18,    16,        // 48
     20,    21,    22,        // 51
     23,    24,    25,        // 54
     25,    26,    23,        // 57
     27,    26,    25,        // 60
     25,    22,    27,        // 63
     27,    22,    21,        // 66
     25,    24,    22,        // 69
     28,    22,     6,        // 72
     22,    24,     6,        // 75
     24,     1,     6,        // 78
     22,    29,    20,        // 81
     20,    29,    21,        // 84
     28,    29,    22,        // 87
     23,    30,    24,        // 90
      1,    30,    31,        // 93
     32,    33,    34,        // 96
     35,    36,    33,        // 99
     34,    33,    36,        // 102
     37,    33,    32,        // 105
     35,    33,    37,        // 108
     12,    38,    14,        // 111
     39,    17,    18,        // 114
     40,    15,    17,        // 117
     41,    42,    43,        // 120
     44,    43,    42,        // 123
     45,    46,    44,        // 126
     15,    47,     9,        // 129
     48,    49,    50,        // 132
      2,     1,     4,        // 135
     31,     4,     1,        // 138
     51,    52,    53,        // 141
     54,    55,    52,        // 144
      5,    56,    28,        // 147
      8,    57,     5,        // 150
     56,    51,    53,        // 153
     57,    51,    56,        // 156
     51,    57,    58,        // 159
     11,    59,     8,        // 162
     59,    11,    60,        // 165
     13,    38,    11,        // 168
     60,    11,    38,        // 171
     58,    57,    61,        // 174
     59,    61,    57,        // 177
     58,    61,    62,        // 180
     61,    63,    62,        // 183
     64,    62,    63,        // 186
     65,    62,    64,        // 189
     66,    67,    21,        // 192
     23,    68,    69,        // 195
     68,    23,    26,        // 198
     27,    68,    26,        // 201
     68,    27,    67,        // 204
     27,    21,    67,        // 207
     68,    67,    69,        // 210
     28,    56,    67,        // 213
     67,    56,    69,        // 216
     69,    56,    53,        // 219
     67,    66,    29,        // 222
     66,    21,    29,        // 225
     28,    67,    29,        // 228
     23,    69,    30,        // 231
     31,    69,    53,        // 234
     70,    71,    72,        // 237
     73,    72,    74,        // 240
     71,    74,    72,        // 243
     37,    70,    72,        // 246
     73,    37,    72,        // 249
     14,    38,    13,        // 252
      9,    60,    12,        // 255
     63,    75,    64,        // 258
     63,    47,    76,        // 261
     77,    78,    79,        // 264
     78,    80,    79,        // 267
     80,    81,    82,        // 270
     61,    59,    47,        // 273
     47,    59,    83,        // 276
     84,    49,    85,        // 279
     86,    87,    85,        // 282
     52,    55,    53,        // 285
     31,    53,    55,        // 288
      5,    28,     6,        // 291
      8,     5,     7,        // 294
      1,    24,    30,        // 297
     12,    60,    38,        // 300
     39,    40,    17,        // 303
     40,    47,    15,        // 306
     44,    46,    43,        // 309
     45,    81,    46,        // 312
      5,    57,    56,        // 315
      8,    59,    57,        // 318
     31,    30,    69,        // 321
      9,    59,    60,        // 324
     63,    76,    75,        // 327
     63,    61,    47,        // 330
     78,    88,    80,        // 333
     80,    88,    81,        // 336
     84,    89,    49         // 339
}; // cat1UVIndices

S3L_Model3D cat1Model = 
{
  .vertices = cat1Vertices,
  .vertexCount = CAT1_VERTEX_COUNT,
  .triangles = cat1TriangleIndices,
  .triangleCount = CAT1_TRIANGLE_COUNT,
  .customTransformMatrix = 0
};

#endif // guard
