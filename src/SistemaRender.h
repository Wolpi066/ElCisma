#pragma once
#include "raylib.h"
#include "Iluminacion.h"
#include "Protagonista.h"
#include "Mapa.h"
#include "GestorEntidades.h"

class SistemaRender
{
private:
    Camera2D camera;
    LightInfo linterna;

    // --- Minimapa ---
    RenderTexture2D minimapaTextura;
    RenderTexture2D nieblaMinimapa;
    float minimapaZoom;
    Vector2 minimapaOffset;

    // Renderizado del mundo
    void dibujarMundo(const Rectangle& cameraView, Mapa& mapa, GestorEntidades& gestor, Protagonista& jugador);
    void dibujarHUD(Protagonista& jugador);

    // Helper
    Rectangle getCameraViewRect(const Camera2D& cam);

public:
    SistemaRender();
    ~SistemaRender();

    void inicializarMinimapa(Mapa& mapa);
    void actualizarNieblaMinimapa(const Protagonista& jugador);

    void dibujarTodo(Protagonista& jugador, Mapa& mapa, GestorEntidades& gestor);

    Camera2D getCamera() const;
};
