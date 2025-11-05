#pragma once
#include "Bala.h" // Herencia

class BalaMonstruosa : public Bala {
public:
    // Constructor especifico para la bala del jefe
    BalaMonstruosa(Vector2 pos, Vector2 dir);

    virtual void dibujar() override;
};
