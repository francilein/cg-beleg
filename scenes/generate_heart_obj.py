import math

def heart_curve(t):
    x = 16 * math.sin(t)**3
    y = 13 * math.cos(t) - 5 * math.cos(2*t) - 2 * math.cos(3*t) - math.cos(4*t)
    return x * 0.05, y * 0.05  # Skaliert

segments = 64
depth = 0.4

vertices = []
faces = []

# Vorder- und Rückseite erzeugen
for side in [depth / 2, -depth / 2]:
    for i in range(segments):
        t = 2 * math.pi * i / segments
        x, y = heart_curve(t)
        z = side
        vertices.append((x, y, z))

# Mitte vorne/hinten
vertices.append((0, 0,  depth / 2)) 
vertices.append((0, 0, -depth / 2))  

# Vorderseite
for i in range(segments):
    a = i
    b = (i + 1) % segments
    c = 2 * segments  # Zentrum vorne
    faces.append((a + 1, b + 1, c + 1))

# Rückseite
for i in range(segments):
    a = i + segments
    b = (i + 1) % segments + segments
    c = 2 * segments + 1
    faces.append((b + 1, a + 1, c + 1))

# Seitenflächen
for i in range(segments):
    a1 = i
    a2 = (i + 1) % segments
    b1 = i + segments
    b2 = (i + 1) % segments + segments
    faces.append((a1 + 1, a2 + 1, b2 + 1))
    faces.append((a1 + 1, b2 + 1, b1 + 1))

# Ausgabe
with open("scenes/heart.obj", "w") as f:
    f.write("# color 255 50 50\n")
    for v in vertices:
        f.write(f"v {v[0]:.5f} {v[1]:.5f} {v[2]:.5f}\n")
    for face in faces:
        f.write(f"f {face[0]} {face[1]} {face[2]}\n")
