#pragma once
#include "raylib.h"
#include "Iluminacion.h"
#include "Protagonista.h"
#include "Mapa.h"
#include "GestorEntidades.h"
#include <vector>

class SistemaRender
{
private:
    Camera2D camera;
    LightInfo linterna;

    // --- Minimapa ---
    RenderTexture2D minimapaTextura;
    RenderTexture2D nieblaMinimapa;
    Texture2D texMinimapaFrame;
    float minimapaZoom;
    Vector2 minimapaOffset;

    // --- HUD VISUAL (ICONOS) ---
    Texture2D texBateria[6];
    Texture2D texVida[4];
    Texture2D texIconoMunicion;

    // (Sin texturas de paneles, usamos código)

    void dibujarMundo(const Rectangle& cameraView, Mapa& mapa, GestorEntidades& gestor, Protagonista& jugador);
    void dibujarHUD(Protagonista& jugador);

    Rectangle getCameraViewRect(const Camera2D& cam);

public:
    SistemaRender();
    ~SistemaRender();

    void inicializarMinimapa(Mapa& mapa);
    void actualizarNieblaMinimapa(const Protagonista& jugador);
    void dibujarTodo(Protagonista& jugador, Mapa& mapa, GestorEntidades& gestor);
    Camera2D getCamera() const;
};
