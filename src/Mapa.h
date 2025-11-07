#pragma once
#include "raylib.h"
#include <vector>
#include <list>
#include "Cofre.h" // Para el struct

// Pre-declaraciones
class Enemigo;
class Consumible;
class Jefe;
class GestorEntidades;

// (El struct SpawnCofre sigue aqui)
struct SpawnCofre {
    Vector2 pos;
    CofreOrientacion orient;
};

class Mapa
{
private:
    std::vector<Rectangle> muros;  // Bloquean fisica Y luz
    std::vector<Rectangle> cajas;  // Bloquean fisica pero NO luz

    Texture2D pisoTexture;
    Rectangle mundoRect;

    // --- Logica de la Puerta ---
    Rectangle puertaJefe;
    bool puertaAbierta;

    std::vector<Vector2> spawnsCofres;

    void cargarMapa();

    // --- ¡¡MOVIDO A PUBLIC!! ---
    // void dibujarPiso();

    bool esAreaValida(Vector2 pos);
    bool esAreaValida(Vector2 pos, CofreOrientacion orient);

    SpawnCofre getSpawnCofrePegadoAPared(Rectangle zona);

    Vector2 getPosicionSpawnNota(const Rectangle& caja);

public:
    Mapa();
    ~Mapa();

    // --- ¡¡NUEVO MÉTODO PÚBLICO!! ---
    // (Movido desde private para que SistemaRender pueda usarlo)
    void dibujarPiso();
    // ---------------------------------

    void poblarMundo(GestorEntidades& gestor);

    Vector2 getPosicionSpawnValida(Rectangle zona);
    void dibujar(); // <-- Esta función ahora es redundante, pero la dejamos

    const std::vector<Rectangle>& getMuros() const;
    const std::vector<Rectangle>& getCajas() const;

    void abrirPuerta();
    void cerrarPuerta();
    bool estaPuertaAbierta() const;
    Rectangle getPuertaJefe() const;
};
