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
    RenderTexture2D minimapaTextura; // El mapa estatico (muros, cajas)
    RenderTexture2D nieblaMinimapa;  // La mascara de "niebla de guerra"
    float minimapaZoom;
    Vector2 minimapaOffset;
    // -------------------------

    void dibujarMundo(Mapa& mapa, GestorEntidades& gestor, Protagonista& jugador);
    void dibujarHUD(Protagonista& jugador);

public:
    SistemaRender();
    ~SistemaRender();

    // --- ¡¡MODIFICADO!! ---
    // Se llama una vez desde Juego::Juego() para crear el mapa
    void inicializarMinimapa(Mapa& mapa);

    // Se llama CADA FRAME desde Juego::actualizarJugando()
    void actualizarNieblaMinimapa(const Protagonista& jugador);
    // -----------------

    void dibujarTodo(Protagonista& jugador, Mapa& mapa, GestorEntidades& gestor);

    Camera2D getCamera() const;
};
