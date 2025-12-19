#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>

class Protagonista;
class Camera2D;

#define MAX_SHADOWS 512

typedef struct ShadowGeometry {
    Vector2 vertices[4];
} ShadowGeometry;

typedef struct LightInfo {
    Vector2 position;
    Color color;
    float radius;
    float intensity;
    RenderTexture2D mask;
    ShadowGeometry shadows[MAX_SHADOWS];
    int shadowCount;
    Rectangle bounds;
    bool valid;
    bool active;
} LightInfo;

class Iluminacion
{
private:
    static void ComputeShadowVolumeForEdge(LightInfo* light, Vector2 sp, Vector2 ep);

    static void DrawLightMask(LightInfo* light, Camera2D& camera, Protagonista& jugador);

public:
    static void SetupLight(LightInfo* light, Vector2 pos, Color color, float radius, float intensity);
    static void MoveLight(LightInfo* light, Vector2 pos);

    static void UpdateLightShadows(
        LightInfo* light,
        const std::vector<Rectangle>& muros,
        const Rectangle& puerta,
        bool puertaEstaAbierta,
        Camera2D& camera,
        Protagonista& jugador
    );
};
