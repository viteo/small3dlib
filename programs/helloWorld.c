/**
  Simple hello world for small3dlib. Renders a triangle in terminal as ASCII.

  by drummyfish, released under CC0 1.0, public domain
*/

#include <stdio.h> // for IO

// This tells the library the resolution of our ASCII screen.
#define S3L_RESOLUTION_X 64
#define S3L_RESOLUTION_Y 30

// This tells the library the name of a function we use to write pixels.
#define S3L_PIXEL_FUNCTION drawPixel

#include "../small3dlib.h" // now include the library

#define U S3L_FRACTIONS_PER_UNIT // this is the library unit, like e.g. 1 meter

S3L_Unit triangleVertices[]   = {  // x,   y,   z 
                                      U,   0,   0,     // vertex 1
                                      0,   U,   0,     // vertex 2
                                     -U,   U/2, 0 };   // vertex 3

S3L_Index triangleTriangles[] = { 0, 1, 2 }; // our single triangle

#undef U

#define SCREEN_SIZE (S3L_RESOLUTION_X * S3L_RESOLUTION_Y)

char screen[SCREEN_SIZE]; // our ASCII screen

/* This function will be called by the library to draw individual rasterized
 pixels to our screen. We should try to make this function as fast as possible
 as it tends to be the performance bottle neck. */
void drawPixel(S3L_PixelInfo *p)
{
  screen[p->y * S3L_RESOLUTION_X + p->x] = 'X';
}

int main()
{
  for (int i = 0; i < SCREEN_SIZE; ++i) // init the screen
    screen[i] = '.';

  S3L_Model3D triangleModel; // 3D model representing our triangle
  S3L_initModel3D(triangleVertices,9,triangleTriangles,1,&triangleModel);

  S3L_Scene scene;           // scene of 3D models (we only have 1)
  S3L_initScene(&triangleModel,1,&scene);

  // shift the camera a little bit so that we can see the triangle
  scene.camera.transform.translation.z = -2 * S3L_FRACTIONS_PER_UNIT;
  scene.camera.transform.translation.y = S3L_FRACTIONS_PER_UNIT / 2;

  S3L_newFrame();            // has to be called before each frame
  S3L_drawScene(scene);      /* this starts the scene rendering, the library
                                will now start calling our drawPixel function to
                                render the camera view */
  int index = 0;

  for (int y = 0; y < S3L_RESOLUTION_Y; ++y) // now display the screen
  {
    for (int x = 0; x < S3L_RESOLUTION_X; ++x)
    {
      putchar(screen[index]);
      index++;
    }

    putchar('\n');
  }

  return 0; // done!
}
