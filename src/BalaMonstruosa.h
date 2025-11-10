#pragma once
#include "Bala.h"

class BalaMonstruosa : public Bala {
public:
    BalaMonstruosa(Vector2 pos, Vector2 dir);
    virtual void dibujar() override;
};
