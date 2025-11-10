#include "BalaMonstruosa.h"

// --- ¡¡VACA FIX 4.0!! (Velocidad en px/sec) ---
static const float VELOCIDAD_BALA_JEFE = 420.0f;
static const int DANIO_BALA_JEFE = 5;

BalaMonstruosa::BalaMonstruosa(Vector2 pos, Vector2 dir)
    : Bala(
        pos,
        dir,
        VELOCIDAD_BALA_JEFE, // <-- 420.0f
        DANIO_BALA_JEFE,
        OrigenBala::ENEMIGO,
        6.0f
    )
{
}

void BalaMonstruosa::dibujar() {
    if (estaActiva()) {
        DrawCircleV(getPosicion(), 6.0f, RED);
    }
}
