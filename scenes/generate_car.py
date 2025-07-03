import math
import os

def generate_car_model():
    vertices = []
    faces = []

    # Grundkörper (Quader für Karosserie)
    base = [
        (-1, 0, -2), (1, 0, -2), (1, 0, 2), (-1, 0, 2),     # 0–3 unten
        (-1, 1, -2), (1, 1, -2), (1, 1, 2), (-1, 1, 2)      # 4–7 oben
    ]
    vertices.extend(base)

    base_faces = [
        (1, 2, 3), (1, 3, 4),  # bottom
        (5, 6, 7), (5, 7, 8),  # top
        (1, 2, 6), (1, 6, 5),  # front
        (2, 3, 7), (2, 7, 6),  # right
        (3, 4, 8), (3, 8, 7),  # back
        (4, 1, 5), (4, 5, 8)   # left
    ]
    faces.extend(base_faces)

    # Fensteraufbau (kleinerer Quader oben)
    offset = len(vertices)
    cabin = [
        (-0.6, 1, -1.2), (0.6, 1, -1.2), (0.6, 1, 1.2), (-0.6, 1, 1.2),
        (-0.6, 1.6, -1.2), (0.6, 1.6, -1.2), (0.6, 1.6, 1.2), (-0.6, 1.6, 1.2)
    ]
    vertices.extend(cabin)

    cabin_faces = [
        (offset+1, offset+2, offset+3), (offset+1, offset+3, offset+4),
        (offset+5, offset+6, offset+7), (offset+5, offset+7, offset+8),
        (offset+1, offset+2, offset+6), (offset+1, offset+6, offset+5),
        (offset+2, offset+3, offset+7), (offset+2, offset+7, offset+6),
        (offset+3, offset+4, offset+8), (offset+3, offset+8, offset+7),
        (offset+4, offset+1, offset+5), (offset+4, offset+5, offset+8)
    ]
    faces.extend(cabin_faces)

    return vertices, faces

def write_obj(filename, vertices, faces):
    with open(filename, 'w') as f:
        f.write("o CarModel\n")
        for v in vertices:
            f.write(f"v {v[0]:.3f} {v[1]:.3f} {v[2]:.3f}\n")
        for face in faces:
            f.write(f"f {face[0]} {face[1]} {face[2]}\n")

output_path = "/mnt/data/car_model.obj"
vertices, faces = generate_car_model()
write_obj(output_path, vertices, faces)

