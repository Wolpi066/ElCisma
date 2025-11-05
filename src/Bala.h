#pragma once
#include "raylib.h"
#include "raymath.h"

// --- RESTAURADO: Enum para el origen de la bala ---
enum class OrigenBala {
    JUGADOR,
    ENEMIGO
};

class Bala {
protected:
    Vector2 posicion;
    Vector2 velocidad;
    int danio;
    OrigenBala origen; // <-- RESTAURADO
    float radio;
    bool activa;

public:
    // --- RESTAURADO: Constructor correcto ---
    Bala(Vector2 pos, Vector2 dir, float rapidez, int dmg, OrigenBala org, float rad);
    virtual ~Bala() {}

    // --- RESTAURADO: Métodos correctos ---
    void actualizarVidaUtil(Vector2 posJugador);
    void setPosicion(Vector2 nuevaPos);

    virtual void dibujar() = 0;

    bool estaActiva() const;
    void desactivar();
    Rectangle getRect() const;
    OrigenBala getOrigen() const;
    int getDanio() const;
    Vector2 getPosicion() const;
    Vector2 getVelocidad() const;

    virtual bool estaMuerto() const;
    virtual bool estaConsumido() const;
};
