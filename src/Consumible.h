#pragma once
#include "raylib.h"

// Pre-declaracion de Protagonista
class Protagonista;

class Consumible {
protected:
    Vector2 posicion;
    bool consumido;

public:
    // Constructor
    Consumible(Vector2 pos);

    // Destructor virtual
    virtual ~Consumible() {}

    // --- ¡¡CAMBIO DE FIRMA!! ---
    // Ahora devuelve un 'int' (codigo de loot).
    // 0 = sin loot.
    virtual int usar(Protagonista& jugador) = 0;
    // -------------------------

    virtual void dibujar() = 0;

    virtual bool esInteraccionPorTecla() const;
    virtual bool estaConsumido() const;
    virtual Rectangle getRect() const; // <-- ¡¡NUEVO VIRTUAL!!
    Vector2 getPosicion() const;

    virtual bool estaMuerto() const {
        return false;
    }
};
