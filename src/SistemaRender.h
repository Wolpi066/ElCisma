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
    // -------------------------

    // --- ¡¡FIRMA CORREGIDA!! ---
    // (Ahora recibe el Rectangle de la cámara, no la cámara entera)
    void dibujarMundo(const Rectangle& cameraView, Mapa& mapa, GestorEntidades& gestor, Protagonista& jugador);
    void dibujarHUD(Protagonista& jugador);

    // --- ¡¡NUEVA FUNCIÓN HELPER!! ---
    // (Calcula el rectángulo de la vista de la cámara para el culling)
    Rectangle getCameraViewRect(const Camera2D& cam);
    // ---------------------------------

public:
    SistemaRender();
    ~SistemaRender();

    void inicializarMinimapa(Mapa& mapa);
    void actualizarNieblaMinimapa(const Protagonista& jugador);

    void dibujarTodo(Protagonista& jugador, Mapa& mapa, GestorEntidades& gestor);

    Camera2D getCamera() const;
};
