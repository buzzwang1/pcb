
#include "main.h"
#include "TypeDef.h"

#define NOMINMAX
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>
#include <cstdint>

// IDs für die UI-Elemente
#define IDC_BTN_SELECT_STL 1001
#define IDC_TXT_Z_PLANE    1002
#define IDC_BTN_START      1003

// Strukturen für 3D-Geometrie
struct Vec3 
{ 
  float x, y, z; 
};

#pragma pack(push, 1)
struct Triangle 
{
  Vec3 normal;
  Vec3 v1, v2, v3;
  uint16_t attr;
};
#pragma pack(pop)

// Struktur für 2D-Segmente (Linien)
struct LineSegment2D
{
  float x1, y1;
  float x2, y2;
};

// Struktur für die globale Ausdehnung des Modells
struct BoundingBox2D
{
  float minX, maxX;
  float minY, maxY;
};

// Globale Variablen für die App-Steuerung
std::string g_stlFilePath = "";
HWND hEditZ, hBtnStart, hStaticStatus;

// 1. STL Loader (Binär)[cite: 2]
std::vector<Triangle> loadSTL(const std::string& filename) 
{
  std::ifstream file(filename, std::ios::binary);
  if (!file) 
  {
    return {};
  }

  char header[80];
  file.read(header, 80);
  
  uint32_t numTriangles;
  file.read(reinterpret_cast<char*>(&numTriangles), 4);

  std::vector<Triangle> mesh(numTriangles);
  file.read(reinterpret_cast<char*>(mesh.data()), numTriangles * sizeof(Triangle));
  
  return mesh;
}

// Funktion zur Berechnung der maximalen 2D-Ausdehnung des gesamten 3D-Körpers
BoundingBox2D calculateMeshBoundingBox(const std::vector<Triangle>& mesh)
{
  BoundingBox2D box;
  if (mesh.empty()) return { 0, 0, 0, 0 };

  box.minX = mesh[0].v1.x; box.maxX = mesh[0].v1.x;
  box.minY = mesh[0].v1.y; box.maxY = mesh[0].v1.y;

  for (const auto& tri : mesh)
  {
    // Untersuche alle 3 Ecken jedes Dreiecks im Modell
    const Vec3* vertices[3] = { &tri.v1, &tri.v2, &tri.v3 };
    for (int i = 0; i < 3; ++i)
    {
      if (vertices[i]->x < box.minX) box.minX = vertices[i]->x;
      if (vertices[i]->x > box.maxX) box.maxX = vertices[i]->x;
      if (vertices[i]->y < box.minY) box.minY = vertices[i]->y;
      if (vertices[i]->y > box.maxY) box.maxY = vertices[i]->y;
    }
  }
  return box;
}

// 2. Hilfsfunktion: Schnittpunkt einer Kante mit der Ebene z = z_plane
bool intersectEdge(Vec3 a, Vec3 b, float z_plane, Vec3& out_point)
{
  // Wir prüfen jetzt auf <= und >=, um Punkte auf der Ebene zu erfassen
  if ((a.z <= z_plane && b.z >= z_plane) || (a.z >= z_plane && b.z <= z_plane))
  {
    float diff = b.z - a.z;
    if (std::abs(diff) < 0.00001f)
    {
      // Kante liegt komplett auf der Ebene
      return false;
    }
    float t = (z_plane - a.z) / diff;
    out_point.x = a.x + t * (b.x - a.x);
    out_point.y = a.y + t * (b.y - a.y);
    out_point.z = z_plane;
    return true;
  }
  return false;
}

// 3. Slicing-Algorithmus: Berechnet alle Schnittsegmente auf der Z-Ebene
std::vector<Vec3> sliceMesh(const std::vector<Triangle>& mesh, float z_plane) 
{
  std::vector<Vec3> slicePoints;
  const float EPSILON = 0.0001f; // Toleranz für Flachheit

  for (const auto& tri : mesh) 
  {
    // 1. Prüfe, ob das Dreieck flach auf der Schnittebene liegt
    bool v1_on = std::abs(tri.v1.z - z_plane) < EPSILON;
    bool v2_on = std::abs(tri.v2.z - z_plane) < EPSILON;
    bool v3_on = std::abs(tri.v3.z - z_plane) < EPSILON;

    if (v1_on && v2_on && v3_on)
    {
      // Das ganze Dreieck liegt auf der Ebene -> Punkte hinzufügen
      slicePoints.push_back(tri.v1);
      slicePoints.push_back(tri.v2);
      slicePoints.push_back(tri.v2);
      slicePoints.push_back(tri.v3);
      slicePoints.push_back(tri.v3);
      slicePoints.push_back(tri.v1);
      continue; // Fertig mit diesem Dreieck
    }

    // 2. Ansonsten: Suche Schnittpunkte durch Kanten (wie bisher)
    std::vector<Vec3> intersects;
    Vec3 p;
    if (intersectEdge(tri.v1, tri.v2, z_plane, p)) intersects.push_back(p);
    if (intersectEdge(tri.v2, tri.v3, z_plane, p)) intersects.push_back(p);
    if (intersectEdge(tri.v3, tri.v1, z_plane, p)) intersects.push_back(p);

    if (intersects.size() >= 2) 
    {
      slicePoints.push_back(intersects[0]);
      slicePoints.push_back(intersects[1]);
    }
  }
  return slicePoints;
}

// 4. BMP Datei-Speicherfunktion (Win32 API konform)
bool saveBitmapFile(const std::string& filename, const std::vector<uint32_t>& buffer, int width, int height) 
{
  std::ofstream file(filename, std::ios::binary);
  if (!file) 
  {
    return false;
  }

  BITMAPFILEHEADER bfh = {};
  bfh.bfType = 0x4D42; // "BM"
  bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (width * height * 4);
  bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

  BITMAPINFOHEADER bih = {};
  bih.biSize = sizeof(BITMAPINFOHEADER);
  bih.biWidth = width;
  bih.biHeight = height; // Positiv für Bottom-Up (BMP-Standard)
  bih.biPlanes = 1;
  bih.biBitCount = 32;
  bih.biCompression = BI_RGB;

  file.write(reinterpret_cast<char*>(&bfh), sizeof(bfh));
  file.write(reinterpret_cast<char*>(&bih), sizeof(bih));
  file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size() * 4);
  return true;
}

// 5. Hauptprozess: Laden, Slicen, Scanline-Füllung & Speichern
void processSlicing(HWND hwnd) 
{
  if (g_stlFilePath.empty()) 
  {
    MessageBoxA(hwnd, "Bitte zuerst eine STL-Datei auswählen!", "Fehler", MB_ICONERROR);
    return;
  }

  char zText[32];
  GetWindowTextA(hEditZ, zText, 32);
  float z_plane = static_cast<float>(std::atof(zText));

  SetWindowTextA(hStaticStatus, "Status: Lade STL...");
  std::vector<Triangle> mesh = loadSTL(g_stlFilePath);
  if (mesh.empty()) 
  {
    MessageBoxA(hwnd, "Fehler beim Laden oder leere STL-Datei.", "Fehler", MB_ICONERROR);
    return;
  }

  SetWindowTextA(hStaticStatus, "Status: Berechne maximale Grundfläche...");
  // HIER NEU: Nutze die Bounding Box des GESAMTEN Körpers, nicht nur des aktuellen Schnitts!
  BoundingBox2D globalBox = calculateMeshBoundingBox(mesh);

  // Skalierung: 1 cm STL-Einheit = 100 Pixel
  const float SCALE_FACTOR = 10.0f; 

  // Berechne feste Gesamt-Bitmap-Größe basierend auf dem kompletten 3D-Körper (+ 20px Sicherheitsrand)
  int bitmapWidth = static_cast<int>((globalBox.maxX - globalBox.minX) * SCALE_FACTOR) + 20;
  int bitmapHeight = static_cast<int>((globalBox.maxY - globalBox.minY) * SCALE_FACTOR) + 20;

  if (bitmapWidth <= 0 || bitmapHeight <= 0 || bitmapWidth > 8000 || bitmapHeight > 8000) 
  {
    MessageBoxA(hwnd, "Berechnete globale Bitmap-Dimensionen sind ungültig.", "Fehler", MB_ICONERROR);
    return;
  }

  // Weißer Hintergrund-Buffer (Alles außerhalb bleibt weiß)[cite: 2]
  std::vector<uint32_t> buffer(bitmapWidth * bitmapHeight, 0xFFFFFFFF);

  SetWindowTextA(hStaticStatus, "Status: Berechne Schnittfläche...");
  std::vector<Vec3> slicePoints = sliceMesh(mesh, z_plane);
  
  // Wenn an dieser Z-Höhe nichts geschnitten wird, speichern wir einfach ein komplett weißes Bild der exakt gleichen Größe!
  if (!slicePoints.empty()) 
  {
    SetWindowTextA(hStaticStatus, "Status: Transformiere Segmente...");
    // 2D Segmente basierend auf der GLOBALEN Box ausrichten
    std::vector<LineSegment2D> segments;
    for (size_t i = 0; i < slicePoints.size(); i += 2) 
    {
      if (i + 1 >= slicePoints.size()) 
      {
        break;
      }

      LineSegment2D seg;
      // Wichtig: pt - globalBox.minX stellt sicher, dass der Ursprung für jeden Schnitt fest verankert bleibt
      seg.x1 = (slicePoints[i].x - globalBox.minX) * SCALE_FACTOR + 10.0f;
      seg.y1 = (slicePoints[i].y - globalBox.minY) * SCALE_FACTOR + 10.0f;
      seg.x2 = (slicePoints[i + 1].x - globalBox.minX) * SCALE_FACTOR + 10.0f;
      seg.y2 = (slicePoints[i + 1].y - globalBox.minY) * SCALE_FACTOR + 10.0f;
      
      segments.push_back(seg);
    }

    SetWindowTextA(hStaticStatus, "Status: Fülle innenliegende Bereiche...");
    // Scanline-Füllalgorithmus (Even-Odd)
    for (int y = 0; y < bitmapHeight; ++y)
    {
      std::vector<float> intersectionX;

      for (const auto& seg : segments)
      {
        if ((seg.y1 <= y && seg.y2 > y) || (seg.y2 <= y && seg.y1 > y))
        {
          if (std::abs(seg.y2 - seg.y1) > 0.00001f)
          {
            float x = seg.x1 + (y - seg.y1) * (seg.x2 - seg.x1) / (seg.y2 - seg.y1);
            intersectionX.push_back(x);
          }
        }
      }

      std::sort(intersectionX.begin(), intersectionX.end());

      for (size_t i = 0; i < intersectionX.size(); i += 2)
      {
        if (i + 1 >= intersectionX.size())
        {
          break;
        }

        int startX = static_cast<int>(std::ceil(intersectionX[i]));
        int endX = static_cast<int>(std::floor(intersectionX[i + 1]));

        startX = std::max(0, startX);
        endX = std::min(bitmapWidth - 1, endX);

        for (int x = startX; x <= endX; ++x)
        {
          buffer[y * bitmapWidth + x] = 0x00000000; // Innenbereich schwarz ausmalen
        }
      }
    }
  }

  // Speicher-Dialog für BMP-Datei aufrufen
  OPENFILENAMEA ofn = {};
  char szFile[260] = "schnittflaeche_solid.bmp";
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFile = szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFilter = "Bitmap-Datei (*.bmp)\0*.bmp\0";
  ofn.nFilterIndex = 1;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

  if (GetSaveFileNameA(&ofn) == TRUE) 
  {
    if (saveBitmapFile(ofn.lpstrFile, buffer, bitmapWidth, bitmapHeight)) 
    {
      MessageBoxA(hwnd, "Schnittfläche erfolgreich mit einheitlicher Grundfläche gespeichert!", "Erfolg", MB_ICONINFORMATION);
    } 
    else 
    {
      MessageBoxA(hwnd, "Fehler beim Schreiben der Datei.", "Fehler", MB_ICONERROR);
    }
  }
   SetWindowTextA(hStaticStatus, "Status: Bereit.");
}

// 6. Win32 Windows-Procedure (Event-Handler)
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) 
{
  switch (msg) 
  {
  case WM_CREATE:
    CreateWindowA("Static", "STL-Datei:", WS_VISIBLE | WS_CHILD, 20, 20, 300, 20, hwnd, NULL, NULL, NULL);
    CreateWindowA("Button", "STL Datei wählen...", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 20, 45, 150, 30, hwnd, (HMENU)IDC_BTN_SELECT_STL, NULL, NULL);
    
    CreateWindowA("Static", "Z-Schnittebene (z.B. 15.5):", WS_VISIBLE | WS_CHILD, 20, 95, 200, 20, hwnd, NULL, NULL, NULL);
    hEditZ = CreateWindowA("Edit", "0.0", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 20, 120, 150, 25, hwnd, (HMENU)IDC_TXT_Z_PLANE, NULL, NULL);
    
    hBtnStart = CreateWindowA("Button", "Schnitt berechnen & speichern", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 20, 165, 220, 40, hwnd, (HMENU)IDC_BTN_START, NULL, NULL);
    hStaticStatus = CreateWindowA("Static", "Status: Bereit.", WS_VISIBLE | WS_CHILD, 20, 225, 350, 20, hwnd, NULL, NULL, NULL);
    break;

  case WM_COMMAND:
    switch (LOWORD(wp)) 
    {
    case IDC_BTN_SELECT_STL: 
      {
        OPENFILENAMEA ofn = {};
        char szFile[260] = { 0 };
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "3D-Modelle (*.stl)\0*.stl\0";
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileNameA(&ofn) == TRUE) 
        {
          g_stlFilePath = ofn.lpstrFile;
          std::string label = "STL-Datei geladen: " + g_stlFilePath.substr(g_stlFilePath.find_last_of("\\") + 1);
          MessageBoxA(hwnd, label.c_str(), "Datei Geladen", MB_ICONINFORMATION);
        }
        break;
      }
    case IDC_BTN_START:
      processSlicing(hwnd);
      break;
    }
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hwnd, msg, wp, lp);
  }
  return 0;
}

// WinMain Einstiegspunkt
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR args, int nShow) 
{
  WNDCLASSA wc = {};
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInst;
  wc.lpszClassName = "STL_Slicer_Class";
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

  if (!RegisterClassA(&wc)) 
  {
    return -1;
  }

  HWND hwnd = CreateWindowA("STL_Slicer_Class", "3D STL Slicer zu 2D BMP", 
                            WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, 
                            CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, hInst, NULL);
  
  ShowWindow(hwnd, nShow);
  UpdateWindow(hwnd);

  MSG msg = {};
  while (GetMessage(&msg, NULL, 0, 0)) 
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return (int)msg.wParam;
}