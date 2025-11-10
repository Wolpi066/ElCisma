#pragma once
#include "Bala.h"

// NUEVA CLASE PARA EL BULLET HELL
// (Es una BalaMonstruosa pero más grande y de otro color)

class BalaInfernal : public Bala {
public:
    BalaInfernal(Vector2 pos, Vector2 dir);
    virtual void dibujar() override;
};
