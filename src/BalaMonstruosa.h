#pragma once
#include "Bala.h"

class BalaMonstruosa : public Bala {
private:
    static Texture2D texBala;

public:
    BalaMonstruosa(Vector2 pos, Vector2 dir);
    virtual void dibujar() override;

    // Gestión de recursos
    static void CargarRecursos();
    static void DescargarRecursos();
};
