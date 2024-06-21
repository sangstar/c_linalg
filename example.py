import linalg

print("HELLO")

vec1 = linalg.Vector([1.0, 2.0])
vec2 = linalg.Vector([1.0, 3.0])

vec3 = linalg.Vector([2.0, 2.0])
vec4 = linalg.Vector([1.0, 4.0])

print(linalg.dot_product(vec1, vec2))

mat_a = linalg.Matrix([vec1, vec2])

mat_b = linalg.Matrix([vec3, vec4])

linalg.matmul(mat_a, mat_b)
