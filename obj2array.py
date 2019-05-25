# Python tool to convert a 3D model from the text obj format to C arrays to be
# used with small3dlib.
#
#
#
#
# TODO
#
# by drummfish
# released under CC0 1.0.

import sys

def printHelp():
  print("Convert 3D model in OBJ format (text, triangulated) to C array for small3dlib.")
  print("usage:\n")
  print("  python obj2array.py [-c -sX -uY -vZ -n] file\n")
  print("  -c     compact format (off by default)")
  print("  -n     use direct instead of indexed UV coords (off by default)  ")
  print("  -sX    scale the model by X (default: 512)")
  print("  -uY    scale the U texture coord by Y (default: 512)")
  print("  -vZ    scale the V texture coord by Z (default: 512)")
  print("");
  print("by Miloslav \"drummyfish\" Ciz")
  print("released under CC0 1.0")

if len(sys.argv) < 2:
  printHelp()
  quit()

fileName = ""
VERTEX_SCALE = 512
U_SCALE = 512
V_SCALE = 512
NAME = "model"
COMPACT = False
INDEXED_UVS = True

for s in sys.argv:
  if s == "-c":
    COMPACT = True
  elif s == "-n":
    INDEXED_UVS = False
  elif s[:2] == "-s":
    VERTEX_SCALE = int(s[2:])
  elif s[:2] == "-u":
    U_SCALE = int(s[2:])
  elif s[:2] == "-v":
    V_SCALE = int(s[2:])
  else:
    fileName = s

objFile = open(fileName)

vertices = []
uvs = []
triangles = []
triangleUVs = []

# parse the file:

for line in objFile:
  if line[:2] == "v ":
    coords = line[2:].split()
    vertex = [float(coords[i]) for i in range(3)]
    vertices.append(vertex)
  elif line[:3] == "vt ":
    coords = line[3:].split()
    vertex = [float(coords[i]) for i in range(2)]
    uvs.append(vertex)
  elif line[:2] == "f ":
    indices = line[2:].split()

    if len(indices) != 3:
      raise(Exception("The model is not triangulated!"))

    t = []
    u = []
 
    for i in indices:
      components = i.split("/")
      t.append(int(components[0]) - 1)
      u.append(int(components[0]) - 1)

    triangles.append(t)
    triangleUVs.append(t)

# print the result:

def arrayString(name, array, components, scales, align, short):
  result = name + " = [\n"

  if COMPACT:
    lineLen = 0
    first = True
    n = 0

    for v in array:
      for c in v:
        item = ""

        if first:
          first = False
        else:
          result += ","
          lineLen += 1

          if lineLen >= 80:
            result += "\n"
            lineLen = 0

        num = c * scales[n % len(scales)]

        if short:
          item += str(num)
        else:
          item += ("" if num >= 0 else "-") + "0x%x" % abs(num)

        if lineLen + len(item) > 80:
          result += "\n"
          lineLen = 0
       
        result += item
        lineLen += len(item)
        n += 1

    result += "]\n"

  else: # non-compact
    n = 0
    endIndex = len(array) - 1

    for v in array:
      line = "  " + ", ".join([str(int(v[c] * scales[c % len(scales)])).rjust(align) for c in range(components)])

      if n < endIndex:
        line += ","

      line = line.ljust((components + 2) * (align + 1)) + "// " + str(n * components) + "\n"
      result += line
      n += 1

    result += "]; // " + name + "\n"

  return result

result = ""

print(arrayString(NAME + "Vertices",vertices,3,[VERTEX_SCALE],5,False))
print(arrayString(NAME + "TriangleIndices",triangles,3,[1],5,True))

if INDEXED_UVS:
  print(arrayString(NAME + "UVs",vertices,2,[U_SCALE,V_SCALE],5,False))
  print(arrayString(NAME + "UVIndices",triangleUVs,3,[1],5,True))
else:
  uvs2 = []
  for item in triangleUVs:
    uvs2.append([
      uvs[item[0]][0],
      uvs[item[0]][1],
      uvs[item[1]][0],
      uvs[item[1]][1],
      uvs[item[2]][0],
      uvs[item[2]][1]])

  print(arrayString(NAME + "TriangleUVs",uvs2,6,[U_SCALE,V_SCALE],5,False))

