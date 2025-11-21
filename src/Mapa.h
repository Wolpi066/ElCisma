#pragma once
#include "raylib.h"
#include <vector>
#include <list>
#include "Cofre.h"

// Pre-declaraciones
class Enemigo;
class Consumible;
class Jefe;
class GestorEntidades;

struct SpawnCofre {
    Vector2 pos;
    CofreOrientacion orient;
};

enum class EstadoPuerta {
    CERRADA,
    ABRIENDO,
    ABIERTA
};

class Mapa
{
private:
    std::vector<Rectangle> muros;

    // --- SISTEMA DE CAJAS (Vectores Simples) ---
    // Vectores de Rectángulos directos para máxima compatibilidad con física
    std::vector<Rectangle> cajas;        // Hitboxes activas (las que usa el juego)
    std::vector<int> tiposVisualesCajas; // IDs visuales de las activas

    // --- BACKUP PARA REINICIO (Soluciona cajas fantasma) ---
    std::vector<Rectangle> cajasEstaticas;
    std::vector<int> tiposVisualesCajasEstaticos;
    // ------------------------------------------------------

    std::vector<Texture2D> texturasCajas; // Las 5 texturas cargadas

    Texture2D pisoTexture;
    Rectangle mundoRect;

    // Visuales Puerta
    Texture2D texPuertaCerrada;
    Texture2D texPuertaAbriendose;
    Texture2D texPuertaAbierta;

    Rectangle puertaJefe;
    EstadoPuerta estadoPuerta;
    float temporizadorAnimacionPuerta;

    std::vector<Vector2> spawnsCofres;

    void cargarMapa();

    // --- Helper VITAL para agregar cajas correctamente ---
    void agregarCajaDecorativa(float x, float y, float w, float h);

    bool esAreaValida(Vector2 pos);
    bool esAreaValida(Vector2 pos, CofreOrientacion orient);
    SpawnCofre getSpawnCofrePegadoAPared(Rectangle zona);
    Vector2 getPosicionSpawnNota(const Rectangle& caja);

public:
    Mapa();
    ~Mapa();

    void actualizar(float dt);
    void dibujarPiso();
    void dibujar(); // Dibuja la capa base
    void dibujarPuerta(float alpha = 1.0f); // Dibuja la puerta (Overlay)

    void poblarMundo(GestorEntidades& gestor);
    Vector2 getPosicionSpawnValida(Rectangle zona);

    // Getters para física y renderizado
    const std::vector<Rectangle>& getMuros() const;
    const std::vector<Rectangle>& getCajas() const;

    void abrirPuerta();
    void cerrarPuerta();
    bool estaPuertaAbierta() const;
    Rectangle getPuertaJefe() const;
};
