#include <stdio.h>
#define S3L_PIXEL_FUNCTION pixelFunc
#include "s3l.h"

void pixelFunc(S3L_PixelInfo *pixel)
{
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


  return 0;
}
