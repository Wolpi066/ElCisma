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

    // --- ¡¡NUEVO!! Minimapa ---
    RenderTexture2D minimapaTextura;
    float minimapaZoom;
    Vector2 minimapaOffset;
    // -------------------------

    void dibujarMundo(Mapa& mapa, GestorEntidades& gestor, Protagonista& jugador);
    void dibujarHUD(Protagonista& jugador);

public:
    SistemaRender();
    ~SistemaRender();

    // --- ¡¡NUEVO!! ---
    // Se llama una vez desde Juego::Juego() para crear el mapa
    void inicializarMinimapa(Mapa& mapa);
    // -----------------

    void dibujarTodo(Protagonista& jugador, Mapa& mapa, GestorEntidades& gestor);

    Camera2D getCamera() const;
};
