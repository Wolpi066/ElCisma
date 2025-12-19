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
    std::vector<int> tiposVisualesCajas;

    std::vector<Rectangle> cajasEstaticas;
    std::vector<int> tiposVisualesCajasEstaticos;

    std::vector<Texture2D> texturasCajas;

    Texture2D texSueloIndustrial;
    Texture2D texSueloJefe;

    Rectangle mundoRect;

    Texture2D texPuertaCerrada;
    Texture2D texPuertaAbriendose;
    Texture2D texPuertaAbierta;

    Rectangle puertaJefe;
    EstadoPuerta estadoPuerta;
    float temporizadorAnimacionPuerta;

    std::vector<Vector2> spawnsCofres;

    void cargarMapa();
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
    void dibujar();
    void dibujarPuerta(float alpha = 1.0f);

    void poblarMundo(GestorEntidades& gestor);
    Vector2 getPosicionSpawnValida(Rectangle zona);

    const std::vector<Rectangle>& getMuros() const;
    const std::vector<Rectangle>& getCajas() const;

    void abrirPuerta();
    void cerrarPuerta();
    bool estaPuertaAbierta() const;
    Rectangle getPuertaJefe() const;
};
