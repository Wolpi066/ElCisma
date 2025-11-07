#pragma once
#include "raylib.h"
#include "raymath.h"

enum class OrigenBala {
    JUGADOR,
    ENEMIGO
};

class Bala {
protected:
    Vector2 posicion;
    // --- ¡¡FIX VACA!! ---
    // Ya no almacenamos la velocidad. Almacenamos la dirección y la rapidez.
    Vector2 direccion;
    float rapidez;
    // -------------------
    int danio;
    OrigenBala origen;
    float radio;
    bool activa;
    bool esCheat;

public:
    Bala(Vector2 pos, Vector2 dir, float rapidez, int dmg, OrigenBala org, float rad, bool esCheat = false);
    virtual ~Bala() {}

    void actualizarVidaUtil(Vector2 posJugador);
    void setPosicion(Vector2 nuevaPos);

    virtual void dibujar() = 0;

    bool estaActiva() const;
    void desactivar();
    Rectangle getRect() const;
    OrigenBala getOrigen() const;
    int getDanio() const;
    bool esDisparoCheat() const;
    Vector2 getPosicion() const;

    // --- ¡¡MODIFICADO!! ---
    // Este método ahora calculará el desplazamiento por frame
    Vector2 getVelocidad() const;
    // ----------------------

    virtual bool estaMuerto() const;
    virtual bool estaConsumido() const;
};
