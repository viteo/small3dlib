/*
  Simple example of small3dlib, rendering in terminal.

  license: CC0 1.0
*/

#include <unistd.h>  // for usleep

// we need to define screen resolution before including the library:

#define S3L_RESOLUTION_X 80
#define S3L_RESOLUTION_Y 40

// and a name of the function we'll be using to draw individual pixels:

#define S3L_PIXEL_FUNCTION drawPixel

// now include the library:

#include "../small3dlib.h"

// we'll use a predefined geometry of a cube that's in the library:

S3L_Unit cubeVertices[] = { S3L_CUBE_VERTICES(S3L_FRACTIONS_PER_UNIT) };
S3L_Index cubeTriangles[] = { S3L_CUBE_TRIANGLES };

S3L_Model3D cubeModel; // 3D model, has a geometry, position, rotation etc.
S3L_Scene scene;       // scene we'll be rendring (can have multiple models)

uint8_t screen[S3L_RESOLUTION_X * S3L_RESOLUTION_Y];    // our screen

/* This function will be called by the library to draw individual rasterized
 pixels to the screen. We should try to make this function as fast as possible
 as it tends to be the performance bottle neck. */

void drawPixel(S3L_PixelInfo *p)
{
  uint8_t c;  // ASCII pixel we'll write to the screen

  /* We'll draw different triangles with different ASCII symbols to give the
  illusion of lighting. */

  if (p->triangleIndex == 0 || p->triangleIndex == 1 ||
      p->triangleIndex == 4 || p->triangleIndex == 5)
    c = '#';
  else if (p->triangleIndex == 2 || p->triangleIndex == 3 ||
      p->triangleIndex == 6 || p->triangleIndex == 7)
    c = 'x';
  else
    c = '.';

  screen[p->y * S3L_RESOLUTION_X + p->x] = c; // draw the pixel to the screen
}

void clearScreen()
{
  for (int j = 0; j < S3L_RESOLUTION_X * S3L_RESOLUTION_Y; ++j)
    screen[j] = ' ';
}

// This function prints out the content of the screen to the terminal.

void drawScreen()
{
  for (int i = 0; i < 20; ++i)
    printf("\n");

  int pos = 0;

  for (int y = 0; y < S3L_RESOLUTION_Y; ++y)
  {
    for (int x = 0; x < S3L_RESOLUTION_X; ++x)
    {
      printf("%c",screen[pos]);
      pos++;      
    }

    printf("\n");
  }
}

int main()
{
  S3L_initModel3D(
    cubeVertices,
    S3L_CUBE_VERTEX_COUNT,
    cubeTriangles,
    S3L_CUBE_TRIANGLE_COUNT,
    &cubeModel); 

  S3L_initScene(  // Initialize the scene we'll be rendering.
    &cubeModel,   /* We only have one model (the cube), this is like an array
                     with only one model in it. */
    1,
    &scene);

  // shift the camera a little bit backwards so that it's not inside the cube:

  scene.camera.transform.translation.z = -2 * S3L_FRACTIONS_PER_UNIT;

  for (int i = 0; i < 200; ++i)   // render 200 frames
  {
    clearScreen();

    S3L_drawScene(scene);  /* This starts the scene rendering. The drawPixel
                              function will be called to draw it. */
    drawScreen();

    usleep(100000);        // wait a bit to let the user see the frame

    // now move and rotate the cube a little to see some movement:

    scene.models[0].transform.rotation.y += 10;
    scene.models[0].transform.rotation.x += 4;
    scene.models[0].transform.translation.x = S3L_sin(i * 4);
    scene.models[0].transform.translation.y = S3L_sin(i * 2) / 2;
  }

  return 0;
}
