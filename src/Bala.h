#pragma once
#include "raylib.h"
#include "raymath.h"

// FWD Declarations
class Protagonista;
class Mapa;

enum class OrigenBala {
    JUGADOR,
    ENEMIGO
};

class Bala {
protected:
    Vector2 posicion;
    Vector2 velocidad; // (Velocidad por SEGUNDO)
    int danio;
    OrigenBala origen;
    float radio;
    bool activa;
    bool esCheat;

public:
    Bala(Vector2 pos, Vector2 dir, float rapidez, int dmg, OrigenBala org, float rad, bool esCheat = false);
    virtual ~Bala() {}

    virtual void actualizar(Protagonista& jugador, const Mapa& mapa);
    virtual void recibirDanio(int cantidad, OrigenBala origenDanio = OrigenBala::JUGADOR);

    void actualizarVidaUtil(Vector2 posJugador);
    void setPosicion(Vector2 nuevaPos);
    void setVelocidad(Vector2 nuevaVel);

    virtual void dibujar() = 0;

    bool estaActiva() const;
    void desactivar();
    Rectangle getRect() const;
    OrigenBala getOrigen() const;
    int getDanio() const;
    bool esDisparoCheat() const;
    Vector2 getPosicion() const;

    Vector2 getVelocidad() const;

    virtual bool estaMuerto() const;
    virtual bool estaConsumido() const;
};
