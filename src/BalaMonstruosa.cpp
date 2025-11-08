#include "BalaMonstruosa.h"

static const float VELOCIDAD_BALA_JEFE = 420.0f;

BalaMonstruosa::BalaMonstruosa(Vector2 pos, Vector2 dir)
    : Bala(
        pos,
        dir,
        VELOCIDAD_BALA_JEFE, // 420.0f
        3,                   // daño
        OrigenBala::ENEMIGO,
        6.0f                 // radio
    )
{
}

void BalaMonstruosa::dibujar() {
    if (estaActiva()) {
        DrawCircleV(getPosicion(), 6.0f, RED);
    }
}
