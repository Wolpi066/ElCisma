#pragma once
#include "Bala.h"

class BalaInfernal : public Bala {
private:
    static Texture2D texBalaInfernal;

public:
    BalaInfernal(Vector2 pos, Vector2 dir);
    virtual void dibujar() override;

    static void CargarRecursos();
    static void DescargarRecursos();
};
