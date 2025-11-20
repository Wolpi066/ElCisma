#pragma once
#include "raylib.h"
#include <vector>
#include <list>
#include "Cofre.h"

class Enemigo;
class Consumible;
class Jefe;
class GestorEntidades;

struct SpawnCofre {
    Vector2 pos;
    CofreOrientacion orient;
};

// --- ESTADOS PUERTA ---
enum class EstadoPuerta {
    CERRADA,
    ABRIENDO,
    ABIERTA
};

class Mapa
{
private:
    std::vector<Rectangle> muros;
    std::vector<Rectangle> cajas;

    Texture2D pisoTexture;
    Rectangle mundoRect;

    // --- VISUALES PUERTA ---
    Texture2D texPuertaCerrada;
    Texture2D texPuertaAbriendose;
    Texture2D texPuertaAbierta;

    // --- LÓGICA PUERTA ---
    Rectangle puertaJefe;
    EstadoPuerta estadoPuerta;
    float temporizadorAnimacionPuerta;

    std::vector<Vector2> spawnsCofres;

    void cargarMapa();
    bool esAreaValida(Vector2 pos);
    bool esAreaValida(Vector2 pos, CofreOrientacion orient);
    SpawnCofre getSpawnCofrePegadoAPared(Rectangle zona);
    Vector2 getPosicionSpawnNota(const Rectangle& caja);

public:
    Mapa();
    ~Mapa();

    void actualizar(float dt); // Para animaciones
    void dibujarPiso();
    void dibujar(); // Dibuja todo (muros, cajas y puerta)

    void poblarMundo(GestorEntidades& gestor);
    Vector2 getPosicionSpawnValida(Rectangle zona);

    const std::vector<Rectangle>& getMuros() const;
    const std::vector<Rectangle>& getCajas() const;

    void abrirPuerta();
    void cerrarPuerta();
    bool estaPuertaAbierta() const;
    Rectangle getPuertaJefe() const;
};
