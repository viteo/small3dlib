# Python tool to convert an image to C array for small3dlib.
#
# by drummyfish
# released under CC0 1.0.

from PIL import Image




FILENAME = "house.png"
NAME = "house"

OUT_WIDTH = 256
OUT_HEIGHT = 256

image = Image.open(FILENAME).convert("RGB")
pixels = image.load()

image2 = Image.new("RGB",(OUT_WIDTH,OUT_HEIGHT),color="white")
pixels2 = image2.load()

imageArray = []

for y in range(OUT_WIDTH):
  for x in range(OUT_HEIGHT):
    coord = (
      int(x / float(OUT_WIDTH) * image.size[0]),
      int(y / float(OUT_HEIGHT) * image.size[1]))

    pixel = pixels[coord]

    imageArray.append(pixel)

    pixels2[x,y] = pixel

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

image2.save("out.png")
