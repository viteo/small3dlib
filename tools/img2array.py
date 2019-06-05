# Python tool to convert an image to C array for small3dlib.
#
# by drummyfish
# released under CC0 1.0.

import sys
from PIL import Image

def printHelp():
  print("Convert image to C array for small3dlib.")
  print("usage:\n")
  print("  python img2array.py [-xW -yH -h -nS -pT -5] file\n")
  print("  -xW     set width of the output to W pixels")
  print("  -yH     set height of the output to H pixels")
  print("  -h      include header guards (for texture per file)")
  print("  -nS     use the name S for the texture (defaut: \"texture\")")
  print("  -pT     use palette from file T and indexed colors (otherwise direct colors)")
  print("  -5      use 565 format instead of RGB8")
  print("");
  print("by Miloslav \"drummyfish\" Ciz")
  print("released under CC0 1.0")

def rgbTo565(rgb):
  return ((rgb[0] >> 3) << 11) | ((rgb[1] >> 2) << 5) | ((rgb[2] >> 3))

if len(sys.argv) < 2:
  printHelp()
  quit()

FILENAME = ""
PALETTE = ""
USE_PALETTE = False
NAME = "texture"
GUARDS = False
OUT_WIDTH = 64
OUT_HEIGHT = 64
USE_565 = False

for s in sys.argv:
  if s [:2] == "-x":
    OUT_WIDTH = int(s[2:])
  elif s [:2] == "-y":
    OUT_HEIGHT = int(s[2:])
  elif s == "-h":
    GUARDS = True
  elif s[:2] == "-n":
    NAME = s[2:]
  elif s[:2] == "-5":
    USE_565 = True
  elif s[:2] == "-p":
    PALETTE = s[2:]
    USE_PALETTE = True
  else:
    FILENAME = s

imageArray = []
paletteColors = []
paletteArray = []

image = Image.open(FILENAME).convert("RGB")
pixels = image.load()

if USE_PALETTE > 0:
  palette = Image.open(PALETTE).convert("RGB")
  pixelsPal = palette.load()

  for y in range(palette.size[1]):
    for x in range(palette.size[0]):
      c = pixelsPal[x,y]
      paletteColors.append(c)

      if USE_565:
        paletteArray.append(rgbTo565(c))
      else:
        paletteArray.append(c[0])
        paletteArray.append(c[1])
        paletteArray.append(c[2])

image2 = Image.new("RGB",(OUT_WIDTH,OUT_HEIGHT),color="white")
pixels2 = image2.load()

for y in range(OUT_HEIGHT):
  for x in range(OUT_WIDTH):
    coord = (
      int(x / float(OUT_WIDTH) * image.size[0]),
      int(y / float(OUT_HEIGHT) * image.size[1]))

    pixel = pixels[coord]

    if USE_PALETTE:
      closestIndex = 0     
      closestDiff = 1024

      # find the index of the closest color:

      for i in range(len(paletteColors)):
        c = paletteColors[i]
        diff = abs(pixel[0] - c[0]) + abs(pixel[1] - c[1]) + abs(pixel[2] - c[2])

        if diff < closestDiff:
          closestIndex = i
          closestDiff = diff

      imageArray.append(closestIndex)
      pixels2[x,y] = paletteColors[closestIndex]
    else:
      if USE_565:
        imageArray.append(rgbTo565(pixel))
      else:
        imageArray.append(pixel[0])
        imageArray.append(pixel[1])
        imageArray.append(pixel[2])

      pixels2[x,y] = pixel

#-----------------------

def printArray(array, name, sizeString):
  print("uint8_t " + name + "[" + sizeString + "] = {")
  arrayString = ""

  lineLen = 0

  for v in array:
    item = str(v) + ","

    lineLen += len(item)

    if lineLen > 80:
      arrayString += "\n"
      lineLen = 0

    arrayString += item

  print(arrayString[:-1])
  print("}; // " + name)

if GUARDS:
  print("#ifndef " + NAME.upper() + "_TEXTURE_H")
  print("#define " + NAME.upper() + "_TEXTURE_H\n")

if USE_PALETTE:
  printArray(paletteArray,NAME + "Palette",str(len(paletteArray)))
  print("")

print("#define " + NAME.upper() + "_TEXTURE_WIDTH " + str(OUT_WIDTH))
print("#define " + NAME.upper() + "_TEXTURE_HEIGHT " + str(OUT_HEIGHT))
print("")

printArray(imageArray,NAME + "Texture",str(len(imageArray)))

if GUARDS:
  print("\n#endif // guard")

image2.save(NAME + "_preview.png")
