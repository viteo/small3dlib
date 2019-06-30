/*
  Some basic tests for small3dlib.

  author: Miloslav Ciz
  license: CC0 1.0
*/

#include <stdio.h>
#define S3L_PIXEL_FUNCTION pixelFunc
#include "../small3dlib.h"

#define TEST_BUFFER_W 16
#define TEST_BUFFER_H 16

uint8_t testRaster[TEST_BUFFER_W * TEST_BUFFER_H];

void pixelFunc(S3L_PixelInfo *p)
{
  testRaster[p->y * TEST_BUFFER_W + p->x] += 1;
}

uint16_t testTriangleRasterization(
  S3L_ScreenCoord x0,
  S3L_ScreenCoord y0,
  S3L_ScreenCoord x1,
  S3L_ScreenCoord y1,
  S3L_ScreenCoord x2,
  S3L_ScreenCoord y2,
  uint8_t *expectedPixels
  )
{
  printf("  --- testing tringle rasterization [%d,%d] [%d,%d] [%d,%d] (|: expected, -: rasterized) ----\n",x0,y0,x1,y1,x2,y2);

  S3L_DrawConfig conf;

  conf.backfaceCulling = 0;

  memset(testRaster,0,TEST_BUFFER_W * TEST_BUFFER_H);

  S3L_Vec4 p0, p1, p2;

  S3L_setVec4(&p0,x0,y0,1000,0);
  S3L_setVec4(&p1,x1,y1,1000,0);
  S3L_setVec4(&p2,x2,y2,1000,0);

  S3L_drawTriangle(p0,p1,p2,0,0);
  
  printf("     0123456789ABCDEF\n");

  uint16_t numErrors = 0;

  for (uint8_t y = 0; y < TEST_BUFFER_H; ++y)
  {
    printf("  %d",y);
    
    if (y < 10)
      printf(" ");

    for (uint8_t x = 0; x < TEST_BUFFER_W; ++x)
    {
      uint8_t expected = expectedPixels[y * TEST_BUFFER_W + x];
      uint8_t rasterized = testRaster[y * TEST_BUFFER_W + x];

      char c =
        expected ?
          (rasterized ? '+' : '|')
          :
          (rasterized ? '-' : ' ');

      if (c == '-' || c == '|')
        numErrors++;

      printf("%c",c);
    }
  
    printf("\n");
  }
    
  printf("  errors: %d\n\n",numErrors);

  return numErrors;
}

uint16_t testRasterization()
{
  printf("\n=== TESTING RASTERIZATION ===\n");
  
  uint16_t numErrors = 0;
  
  uint8_t pixelsEmpty[TEST_BUFFER_W * TEST_BUFFER_H];
  memset(pixelsEmpty,0,TEST_BUFFER_W * TEST_BUFFER_H);

  numErrors += testTriangleRasterization(5,3, 3,3, 9,3, pixelsEmpty);
  numErrors += testTriangleRasterization(9,4, 9,0, 9,9, pixelsEmpty);
  numErrors += testTriangleRasterization(9,9, 6,6, 3,3, pixelsEmpty);
  numErrors += testTriangleRasterization(0,6, 3,3, 6,0, pixelsEmpty);
  numErrors += testTriangleRasterization(7,7, 7,7, 7,7, pixelsEmpty);

  uint8_t pixels1[TEST_BUFFER_W * TEST_BUFFER_H] =
  {
  // 0 1 2 3 4 5 6 7 8 9 A B C D E F
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 1
     0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0, // 2
     0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0, // 3
     0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0, // 4
     0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0, // 5
     0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0, // 6
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 7
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 8
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 9
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // A
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // B
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // C
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // D
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // E
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0  // F
  }; 

  numErrors += testTriangleRasterization(4,1, 1,6, 9,7, pixels1);

  uint8_t pixels2[TEST_BUFFER_W * TEST_BUFFER_H] =
  {
  // 0 1 2 3 4 5 6 7 8 9 A B C D E F
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 1
     0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0, // 2
     0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0, // 3
     0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0, // 4
     0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0, // 5
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 6
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 7
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 8
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 9
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // A
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // B
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // C
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // D
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // E
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0  // F
  }; 

  numErrors += testTriangleRasterization(7,1, 1,2, 4,6, pixels2);

  uint8_t pixels3[TEST_BUFFER_W * TEST_BUFFER_H] =
  {
  // 0 1 2 3 4 5 6 7 8 9 A B C D E F
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 1
     0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0, // 2
     0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0, // 3
     0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0, // 4
     0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0, // 5
     0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0, // 6
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 7
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 8
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 9
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // A
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // B
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // C
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // D
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // E
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0  // F
  }; 

  numErrors += testTriangleRasterization(2,1, 1,3, 6,9, pixels3);

  uint8_t pixels4[TEST_BUFFER_W * TEST_BUFFER_H] =
  {
  // 0 1 2 3 4 5 6 7 8 9 A B C D E F
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0
     0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 1
     0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0, // 2
     0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0, // 3
     0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0, // 4
     0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0, // 5
     0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0, // 6
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 7
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 8
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 9
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // A
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // B
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // C
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // D
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // E
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0  // F
  }; 

  numErrors += testTriangleRasterization(4,8, 4,2, 0,0, pixels4);

  uint8_t pixels5[TEST_BUFFER_W * TEST_BUFFER_H] =
  {
  // 0 1 2 3 4 5 6 7 8 9 A B C D E F
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0
     0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0, // 1
     0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0, // 2
     0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0, // 3
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 4
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 5
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 6
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 7
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 8
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 9
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // A
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // B
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // C
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // D
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // E
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0  // F
  }; 

  numErrors += testTriangleRasterization(6,2, 2,4, 0,0, pixels5);

  uint8_t pixels6[TEST_BUFFER_W * TEST_BUFFER_H] =
  {
  // 0 1 2 3 4 5 6 7 8 9 A B C D E F
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0
     0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0, // 1
     1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0, // 2
     0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0, // 3
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 4
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 5
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 6
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 7
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 8
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 9
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // A
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // B
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // C
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // D
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // E
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0  // F
  }; 

  numErrors += testTriangleRasterization(0,2, 6,0, 4,4, pixels6);

  printf("cover test (each pixel should be covered exactly once):\n\n");

  S3L_ScreenCoord coords[] =
  {
    0,0,
    6,0,       
    13,0,     
    15,0,
    14,1,
    11,2,
    3,3,
    11,4,
    14,5,
    0,6,
    6,6,
    13,8,
    8,9,
    3,12,       
    9,12,
    11,13,
    9,14,
    0,15,                          
    15,15
  };

  memset(testRaster,0,TEST_BUFFER_W * TEST_BUFFER_H);

  #define dt(i1,i2,i3)\
  {\
    S3L_Vec4 p0, p1, p2;\
    S3L_setVec4(&p0,coords[2*i1],coords[2*i1 + 1],1000,0);\
    S3L_setVec4(&p1,coords[2*i2],coords[2*i2+1],1000,0);\
    S3L_setVec4(&p2,coords[2*i3],coords[2*i3+1],1000,0);\
    S3L_drawTriangle(p0,p1,p2,0,0);\
  }

  dt(0,1,6)    // 0
  dt(1,2,5)    // 1
  dt(2,4,5)    // 2
  dt(2,3,4)    // 3
  dt(0,6,9)    // 4
  dt(1,10,6)   // 5
  dt(1,5,10)   // 6
  dt(5,4,8)    // 7
  dt(4,3,8)    // 8
  dt(9,6,10)   // 9
  dt(10,5,12)  // 10
  dt(5,7,12)   // 11
  dt(5,7,11)   // 12
  dt(5,8,11)   // 13
  dt(8,3,18)   // 14
  dt(9,10,13)  // 15
  dt(10,12,13) // 16
  dt(12,7,11)  // 17
  dt(11,8,18)  // 18
  dt(9,13,17)  // 19
  dt(13,12,14) // 20
  dt(12,11,14) // 21
  dt(11,14,15) // 22
  dt(15,11,18) // 23
  dt(13,14,16) // 24
  dt(14,15,16) // 25
  dt(17,13,16) // 26
  dt(16,15,18) // 27
  dt(16,17,18) // 28

  // extra empty triangles
  dt(12,12,12);
  dt(9,10,10);
  dt(1,10,10);
  dt(9,6,1);
  dt(0,6,10);

  #undef dt

  uint16_t numErrors2 = 0;
 
  for (uint8_t y = 0; y < TEST_BUFFER_H - 1; ++y)
  {                                    // ^ complete left and bottom aren't 
    printf("  ");                      // supposed to be rasterized

    for (uint8_t x = 0; x < TEST_BUFFER_W - 1; ++x)
    {
      uint8_t count = testRaster[y * TEST_BUFFER_W + x];
     
      printf("%d",count);
      
      if (count != 1)
        numErrors2++;
    }
 
    printf("\n");
  }

  printf("  errors: %d\n",numErrors2);

  numErrors += numErrors2;


  printf("total rasterization errors: %d\n",numErrors);
}

int main()
{
  S3L_Mat4 m, m2;
  S3L_Vec4 v;

  S3L_initMat4(&m);
  S3L_logMat4(m);

  S3L_initVec4(&v);

  S3L_logVec4(v);
 
  S3L_vec4Xmat4(&v,&m); 
  S3L_logVec4(v);

  S3L_makeTranslationMat(100,200,300,&m2);
  S3L_logMat4(m2);

  S3L_mat4Xmat4(&m,&m2);
  S3L_logMat4(m);

  testRasterization();

  return 0;
}
