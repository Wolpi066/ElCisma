#include "BalaMonstruosa.h"

// Llama al constructor base con valores de jefe
BalaMonstruosa::BalaMonstruosa(Vector2 pos, Vector2 dir)
    : Bala(pos, dir, 7.0f, 3, OrigenBala::ENEMIGO, 6.0f)
{
}

void BalaMonstruosa::dibujar() {
    // CORREGIDO: Usar los getters de la clase base
    if (estaActiva()) {
        DrawCircleV(getPosicion(), 6.0f, RED);
    }
}
