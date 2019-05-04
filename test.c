#include <stdio.h>
#define S3L_PIXEL_FUNCTION pixelFunc
#include "s3l.h"

#define TEST_BUFFER_W 16
#define TEST_BUFFER_H 16

uint8_t testRaster[TEST_BUFFER_W * TEST_BUFFER_H];

void pixelFunc(S3L_PixelInfo *p)
{
  testRaster[p->y * TEST_BUFFER_W + p->x] = 1;
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
  printf("  --- testing tringle rasterization (|: expected, -: rasterized) ----\n");

  S3L_DrawConfig conf;

  conf.backfaceCulling = S3L_BACKFACE_CULLING_NONE;
  conf.mode = S3L_MODE_TRIANGLES;

  memset(testRaster,0,TEST_BUFFER_W * TEST_BUFFER_H);

  S3L_drawTriangle(x0,y0,x1,y1,x2,y2,conf,0);
  
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

  uint8_t pixelsEmpty[TEST_BUFFER_W * TEST_BUFFER_H];
  memset(pixelsEmpty,0,TEST_BUFFER_W * TEST_BUFFER_H);

  numErrors += testTriangleRasterization(5,3, 3,3, 9,3, pixelsEmpty);
  numErrors += testTriangleRasterization(9,4, 9,0, 9,9, pixelsEmpty);
  numErrors += testTriangleRasterization(3,3, 6,6, 7,7, pixelsEmpty);
  numErrors += testTriangleRasterization(5,5, 3,7, 9,1, pixelsEmpty);

  printf("total rasterization errors: %d\n",numErrors);
}

int main()
{
  S3L_Mat4 m, m2;
  S3L_Vec4 v;

  S3L_initMat4(&m);
  S3L_writeMat4(m);

  S3L_initVec4(&v);

  S3L_writeVec4(v);
 
  S3L_vec4Xmat4(&v,&m); 
  S3L_writeVec4(v);

  S3L_makeTranslationMat(100,200,300,&m2);
  S3L_writeMat4(m2);

  S3L_mat4Xmat4(&m,&m2);
  S3L_writeMat4(m);

  testRasterization();

  return 0;
}
