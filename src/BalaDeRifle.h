#pragma once
#include "Bala.h" // Herencia

class BalaDeRifle : public Bala {
public:
    // Constructor especifico para la bala del jugador
    BalaDeRifle(Vector2 pos, Vector2 dir);

    virtual void dibujar() override;
};
