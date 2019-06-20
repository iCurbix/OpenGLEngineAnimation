verticesCount = 36

file = open('Kostka.h', 'w')
file.write('#ifndef Kostka_H\n#define Kostka_H\n\n')
file.write(f'unsigned int myKostkaVertexCount = {verticesCount};\n\n')
file.write('float myKostkaVertices[]={\n')


vertices = []
verticesNormals = []
faces = []
facesTemp = []

with open('kostka.txt', 'r') as f:
	
	for line in f:
		if line[0] == 'v' and line[1] != 'n':
			vertex = line[2:].split(' ')
			vertex[-1] = vertex[-1][:-1]
			vertices.append(vertex)
			continue

		if line[0] == 'v' and line [1] =='n':
			vertexNormal = line[3:].split(' ')
			vertexNormal[-1] = vertexNormal[-1][:-1]
			verticesNormals.append(vertexNormal)
			continue

		if line[0] == 's':
			continue

		if line[0] == 'f':
			face = line[2:].split(' ')
			face[-1] = face[-1][:-1]
			facesTemp.append(face)
			continue


for face in facesTemp:
	for vertex in face:
		vertex = vertex.split('/')
		faces.append(vertex)

faces.pop()
for i in range(len(faces) - 2):
	verts = [vertices[int(faces[i][0]) - 1], vertices[int(faces[i+1][0]) - 1], vertices[int(faces[i+2][0]) - 1]]
	for vert in verts:
		file.write(f'\t\t{vert[0]}f, {vert[1]}f, {vert[2]}f, 1.0f,\n')
	file.write('\n')

file.write('};\nfloat myKostkaVertexNormals[]={\n')

for i in range(len(faces) - 2):
	verts = [vertices[int(faces[i][2]) - 1], vertices[int(faces[i+1][2]) - 1], vertices[int(faces[i+2][2]) - 1]]
	for vert in verts:
		file.write(f'\t\t{vert[0]}f, {vert[1]}f, {vert[2]}f, 0.0f,\n')
	file.write('\n')

file.write('};\n\n#endif')


