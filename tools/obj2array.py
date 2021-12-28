# Python tool to convert a 3D model from the text obj format to C arrays to be
# used with small3dlib.
#
# by drummyfish
# released under CC0 1.0.

import sys

def printHelp():
  print("Convert 3D model in OBJ format (text, triangulated) to C array for small3dlib.")
  print("usage:\n")
  print("  python obj2array.py [-c -sX -uY -vZ -n] file\n")
  print("  -c     compact format (off by default)")
  print("  -t     use direct instead of indexed UV coords (off by default)")
  print("  -h     include header guards (for model per file)")
  print("  -m     include a material array (per-triangle)")
  print("  -nS    use the name S for the model (defaut: \"model\")")
  print("  -sX    scale the model by X (default: 512)")
  print("  -uY    scale the U texture coord by Y (default: 512)")
  print("  -vZ    scale the V texture coord by Z (default: 512)")
  print("");
  print("by Miloslav \"drummyfish\" Ciz")
  print("released under CC0 1.0")

if len(sys.argv) < 2:
  printHelp()
  quit()

FILENAME = ""
VERTEX_SCALE = 512
U_SCALE = 512
V_SCALE = 512
NAME = "model"
GUARDS = False
COMPACT = False
INDEXED_UVS = True
MATERIALS = False

for s in sys.argv:
  if s == "-c":
    COMPACT = True
  elif s == "-t":
    INDEXED_UVS = False
  elif s == "-h":
    GUARDS = True
  elif s == "-m":
    MATERIALS = True
  elif s[:2] == "-s":
    VERTEX_SCALE = int(s[2:])
  elif s[:2] == "-u":
    U_SCALE = int(s[2:])
  elif s[:2] == "-v":
    V_SCALE = int(s[2:])
  elif s[:2] == "-n":
    NAME = s[2:]
  else:
    FILENAME = s

objFile = open(FILENAME)

vertices = []
uvs = []
triangles = []
triangleUVs = []
materials = []
materialNames = []

currentMatrial = 0

def getMaterialIndex(materialName):
  try:
    return materialNames.index(materialName)
  except Exception:
    materialNames.append(materialName)
    return len(materialNames) - 1

# parse the file:

for line in objFile:
  if line[:2] == "v ":
    coords = line[2:].split()
    vertex = [float(coords[i]) for i in range(3)]
    vertex[2] *= -1
    vertices.append(vertex)
  elif line[:3] == "vt ":
    coords = line[3:].split()
    vertex = [float(coords[i]) for i in range(2)]
    vertex[1] = 1.0 - vertex[1]
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
      try:
        u.append(int(components[1]) - 1)
      except Exception as e:
        u.append(int(components[2]) - 1)

    triangles.append(t)
    triangleUVs.append(u)
    materials.append([currentMatrial])
  elif line[:7] == "usemtl ":
    currentMatrial = getMaterialIndex(line[7:])

# print the result:

def arrayString(name, array, components, scales, align, short, dataType, sizeStr):
  result = "const " + dataType + " " + name + "[" + sizeStr + "] = {\n"

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

        if lineLen + len(item) >= 80:
          result += "\n"
          lineLen = 0
       
        result += item
        lineLen += len(item)
        n += 1

    result += "};\n"

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

    result += "}; // " + name + "\n"

  return result

result = ""

if GUARDS:
  print("#ifndef " + NAME.upper() + "_MODEL_H")
  print("#define " + NAME.upper() + "_MODEL_H\n")

print("#define " + NAME.upper() + "_VERTEX_COUNT " + str(len(vertices)))
print(arrayString(NAME + "Vertices",vertices,3,[VERTEX_SCALE],5,False,"S3L_Unit",NAME.upper() + "_VERTEX_COUNT * 3"))

print("#define " + NAME.upper() + "_TRIANGLE_COUNT " + str(len(triangles)))
print(arrayString(NAME + "TriangleIndices",triangles,3,[1],5,True,"S3L_Index",NAME.upper() + "_TRIANGLE_COUNT * 3"))

if MATERIALS:
  print(arrayString(NAME + "Materials",materials,1,[1],5,True,"uint8_t",NAME.upper() + "_TRIANGLE_COUNT"))

if INDEXED_UVS:
  print("#define " + NAME.upper() + "_UV_COUNT " + str(len(uvs)))
  print(arrayString(NAME + "UVs",uvs,2,[U_SCALE,V_SCALE],5,False,"S3L_Unit",NAME.upper() + "_UV_COUNT * 2"))
  print("#define " + NAME.upper() + "_UV_INDEX_COUNT " + str(len(triangleUVs)))
  print(arrayString(NAME + "UVIndices",triangleUVs,3,[1],5,True,"S3L_Index",NAME.upper() + "_UV_INDEX_COUNT * 3"))
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

  print("#define " + NAME.upper() + "_DIRECT_UV_COUNT " + str(len(uvs2)))
  print(arrayString(NAME + "DirectUVs",uvs2,6,[U_SCALE,V_SCALE],5,False,"S3L_Unit",NAME.upper() + "_DIRECT_UV_COUNT * 6"))

print("S3L_Model3D " + NAME + "Model;\n")

print("void " + NAME + "ModelInit()")
print("{")
print("  S3L_model3DInit(")
print("    " + NAME + "Vertices,")
print("    " + NAME.upper() + "_VERTEX_COUNT,")
print("    " + NAME + "TriangleIndices,")
print("    " + NAME.upper() + "_TRIANGLE_COUNT,")
print("    &" + NAME + "Model);")
print("}")

if GUARDS:
  print("\n#endif // guard")
