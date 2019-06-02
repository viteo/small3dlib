# Python tool to convert an image to C array for small3dlib.
#
# by drummyfish
# released under CC0 1.0.

import sys
from PIL import Image

def printHelp():
  print("Convert image to C array for small3dlib.")
  print("usage:\n")
  print("  python img2array.py [TODO] file\n")
  print("  TODO\n")
  print("");
  print("by Miloslav \"drummyfish\" Ciz")
  print("released under CC0 1.0")

if len(sys.argv) < 2:
  printHelp()
  quit()

FILENAME = ""
NAME = "texture"
GUARDS = False
OUT_WIDTH = 64
OUT_HEIGHT = 64

for s in sys.argv:
  if s [:2] == "-x":
    OUT_WIDTH = int(s[2:])
  elif s [:2] == "-y":
    OUT_HEIGHT = int(s[2:])
  elif s == "-h":
    GUARDS = True
  elif s[:2] == "-n":
    NAME = s[2:]
  else:
    FILENAME = s

image = Image.open(FILENAME).convert("RGB")
pixels = image.load()

image2 = Image.new("RGB",(OUT_WIDTH,OUT_HEIGHT),color="white")
pixels2 = image2.load()

imageArray = []

for y in range(OUT_HEIGHT):
  for x in range(OUT_WIDTH):
    coord = (
      int(x / float(OUT_WIDTH) * image.size[0]),
      int(y / float(OUT_HEIGHT) * image.size[1]))

    pixel = pixels[coord]

    imageArray.append(pixel)

    pixels2[x,y] = pixel

#-----------------------

if GUARDS:
  print("#ifndef " + NAME.upper() + "_TEXTURE_H")
  print("#define " + NAME.upper() + "_TEXTURE_H\n")

print("#define " + NAME.upper() + "_WIDTH " + str(OUT_WIDTH))
print("#define " + NAME.upper() + "_HEIGHT " + str(OUT_HEIGHT))
print("")
print("uint8_t " + NAME + "Texture[" + NAME.upper() + "_WIDTH * " + NAME.upper() + "_HEIGHT * 3] = {")
arrayString = ""

lineLen = 0

for v in imageArray:
  for c in v:
    item = str(c) + ","

    lineLen += len(item)

    if lineLen > 80:
      arrayString += "\n"
      lineLen -= 80

    arrayString += item

print(arrayString[:-1])
print("}; // " + NAME + "Texture")

if GUARDS:
  print("\n#endif // guard")

image2.save(NAME + "_preview.png")
