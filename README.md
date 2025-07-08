# KD-Tree Accelerated Raytracer

Ein hochperformanter C++ Raytracer mit KD-Tree-Beschleunigung für das Rendern komplexer 3D-Szenen aus OBJ-Dateien.

## 🚀 Features

- **KD-Tree Beschleunigung**: Bis zu 60x schnelleres Rendering durch räumliche Datenstrukturen
- **OBJ-Datei Support**: Lädt und rendert komplexe 3D-Modelle (.obj Format)
- **Hochauflösende Ausgabe**: Bis zu 1920x1920 Pixel für beste Bildqualität
- **Robuste Architektur**: Modularer Aufbau mit separaten Komponenten
- **Automatische Kamera-Positionierung**: Intelligente Szenen-Analyse für optimale Ansichten
- **Performance-Vergleich**: Rendering mit und ohne KD-Tree für Benchmarks

## 📁 Projektstruktur

```
cg-beleg/
├── main.cpp                 # Hauptprogramm
├── CMakeLists.txt           # Build-Konfiguration
├── include/                 # Header-Dateien
│   ├── camera.hpp          # Kamera-System
│   ├── geometry.hpp        # Geometrische Primitiven
│   ├── image.hpp           # Bildverarbeitung
│   ├── kdtree.hpp          # KD-Tree Datenstruktur
│   ├── light.hpp           # Beleuchtungssystem
│   ├── material.hpp        # Material-Eigenschaften
│   ├── obj_loader.hpp      # OBJ-Datei Loader
│   ├── raytracer.hpp       # Raytracing-Algorithmus
│   └── renderer.hpp        # Render-Engine
├── src/                    # Implementierungen
│   ├── kdtree.cpp
│   ├── light.cpp
│   ├── renderer.cpp
│   ├── raytracer.cpp
│   └── stb_image_write.cpp
└── scenes/                 # 3D-Modelle
    ├── heart.obj
    ├── twisted_torus_no_numpy.obj
    ├── cottage_obj.obj
    └── mustang.obj
```

## 🛠 Installation & Kompilierung

### Voraussetzungen
- C++17 kompatible Compiler (GCC, Clang, MSVC)
- CMake (optional)

### Kompilierung

```bash
# Einfache Kompilierung
g++ -std=c++17 -O2 -I. main.cpp src/*.cpp -o raytracer

# Mit CMake
mkdir build && cd build
cmake ..
make
```

## 🎯 Verwendung

### Grundlegendes Rendering

```bash
./raytracer
```

Das Programm rendert automatisch die geladene Szene und speichert zwei Bilder:
- Mit KD-Tree: `output_*.png`
- Ohne KD-Tree: `output_*_normal.png`

### Szenen-Konfiguration

Im `main.cpp` können Sie verschiedene Parameter anpassen:

```cpp
// Auflösung ändern
const int width = 1920;
const int height = 1920;

// Objekt laden
auto scene = load_obj("scenes/heart.obj", {255, 100, 100});

// Kamera positionieren
Point3 cam_pos = {center_x + size * 2.0f, center_y + size * 1.5f, center_z + size * 1.2f};
Vector3 cam_dir = Vector3({-0.9f, -1.0f, -0.8f}).normalize();

// Licht positionieren
Light light = {{center_x, center_y + size * 1.5f, center_z + size * 1.0f}, {255, 255, 255}};
```

## 🎨 Kamera-Steuerung

### Verschiedene Ansichten

```cpp
// Von rechts betrachten
Point3 cam_pos = {center_x + size * 2.0f, center_y + size * 1.5f, center_z + size * 1.2f};
Vector3 cam_dir = Vector3({-0.9f, -1.0f, -0.8f}).normalize();

// Von links betrachten  
Point3 cam_pos = {center_x - size * 2.0f, center_y + size * 1.5f, center_z + size * 1.2f};
Vector3 cam_dir = Vector3({0.9f, -1.0f, -0.8f}).normalize();

// Von vorne betrachten
Point3 cam_pos = {center_x, center_y + size * 1.5f, center_z + size * 2.0f};
Vector3 cam_dir = Vector3({0.0f, -1.0f, -0.9f}).normalize();

// Von oben betrachten
Point3 cam_pos = {center_x, center_y + size * 2.0f, center_z};
Vector3 cam_dir = Vector3({0.0f, -1.0f, 0.0f}).normalize();
```

### Zoom und Größe

```cpp
// Näher heranzoomen (größeres Objekt)
Camera cam(cam_pos, cam_dir, size * 0.3f, size * 0.3f, width, height);

// Weiter wegzoomen (kleineres Objekt)
Camera cam(cam_pos, cam_dir, size * 1.0f, size * 1.0f, width, height);
```

## 📊 Performance

### Benchmark-Ergebnisse (6000 Dreiecke bei 1920x1920):
- **Mit KD-Tree**: ~2.3 Sekunden ⚡
- **Ohne KD-Tree**: ~78 Sekunden 🐌
- **Speedup**: ~34x Beschleunigung

### KD-Tree Statistiken:
- Blattknoten: 1336
- Dreiecke in Blättern: 19073
- Durchschnittlich pro Blatt: 14.3 Dreiecke
- Maximale Tiefe: 15

## 🎭 Verfügbare Szenen

| Datei | Dreiecke | Beschreibung |
|-------|----------|--------------|
| `heart.obj` | 256 | Herzförmiges Objekt |
| `twisted_torus_no_numpy.obj` | 6000 | Komplexer gedrehter Torus |
| `cottage_obj.obj` | 259 | Hausmodell |
| `mustang.obj` | Variable | Fahrzeugmodell |

## 🔧 Technische Details

### KD-Tree Implementation
- Rekursive Raumaufteilung für optimale Ray-Triangle-Intersection
- Überlappungsbehandlung für grenzüberschreitende Dreiecke
- Memory-Limits und Null-Pointer-Checks für Stabilität
- Automatische Tiefenbegrenzung zur Vermeidung von Stack-Overflows

### Raytracing-Pipeline
1. **Szenen-Loading**: OBJ-Dateien mit robustem Parser
2. **Bounding Box Berechnung**: Automatische Szenen-Analyse
3. **KD-Tree Aufbau**: Räumliche Indexierung der Geometrie
4. **Kamera-Setup**: Intelligente Positionierung basierend auf Szenen-Größe
5. **Ray Generation**: Perspektivische Projektion pro Pixel
6. **Intersection Testing**: Optimierte Ray-Triangle-Tests
7. **Shading**: Phong-Beleuchtungsmodell
8. **Image Output**: PNG-Export mit stb_image_write

### Koordinatensystem
- **X-Achse**: Links (-) ↔ Rechts (+)
- **Y-Achse**: Unten (-) ↔ Oben (+)
- **Z-Achse**: Hinten (-) ↔ Vorne (+)

## 🐛 Debugging & Troubleshooting

### Häufige Probleme

**Leeres Bild:**
- Überprüfen Sie Kamera-Position und -Richtung
- Stellen Sie sicher, dass das Objekt im Sichtfeld liegt
- Kontrollieren Sie Licht-Position

**Segmentation Fault:**
- OBJ-Datei könnte korrupt sein
- Memory-Limit des KD-Trees erreicht
- Ungültige Dreieck-Daten

**Langsames Rendering:**
- Verwenden Sie den KD-Tree (standardmäßig aktiviert)
- Reduzieren Sie die Auflösung für Tests
- Überprüfen Sie die Dreiecksanzahl der Szene

### Debug-Ausgaben

Das Programm gibt automatisch folgende Informationen aus:
```
Szenen-Bounding Box: X[-1.3, 1.3] Y[-1.29836, 1.29836] Z[-0.3, 0.3]
Kamera positioniert bei (5.2, 3.9, 3.12) - rechts für Ansicht von rechts
KD-Tree Aufbauzeit: 0.034 Sekunden
KD-Tree Renderzeit: 2.33 Sekunden
```

## 📝 Lizenz

Dieses Projekt wurde als Belegarbeit für Computer Graphics entwickelt.

## 👨‍💻 Autor

Entwickelt als Computer Graphics Belegarbeit mit Fokus auf Performance-Optimierung durch räumliche Datenstrukturen.

---

**Tipp**: Für beste Ergebnisse verwenden Sie komplexe Szenen (>1000 Dreiecke) um die Vorteile der KD-Tree-Beschleunigung voll auszuschöpfen! 🚀