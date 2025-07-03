import math

def generate_twisted_torus_no_numpy(outer_r=1.0, inner_r=0.3, twist=3, segments=100, rings=30):
    vertices = []
    faces = []

    for i in range(segments):
        theta = 2 * math.pi * i / segments
        for j in range(rings):
            phi = 2 * math.pi * j / rings
            phi_twist = phi + twist * theta
            x = (outer_r + inner_r * math.cos(phi_twist)) * math.cos(theta)
            y = (outer_r + inner_r * math.cos(phi_twist)) * math.sin(theta)
            z = inner_r * math.sin(phi_twist)
            vertices.append((x, y, z))

    for i in range(segments):
        for j in range(rings):
            next_i = (i + 1) % segments
            next_j = (j + 1) % rings
            a = i * rings + j
            b = next_i * rings + j
            c = next_i * rings + next_j
            d = i * rings + next_j
            faces.append((a + 1, b + 1, c + 1))
            faces.append((a + 1, c + 1, d + 1))

    return vertices, faces

def write_obj_no_numpy(filename, vertices, faces):
    with open(filename, 'w') as f:
        f.write("o TwistedTorus\n")
        for v in vertices:
            f.write(f"v {v[0]:.6f} {v[1]:.6f} {v[2]:.6f}\n")
        for face in faces:
            f.write(f"f {face[0]} {face[1]} {face[2]}\n")

# Erzeuge Datei ohne NumPy
obj_path = "/mnt/data/twisted_torus_no_numpy.obj"
vertices, faces = generate_twisted_torus_no_numpy()
write_obj_no_numpy(obj_path, vertices, faces)

obj_path
