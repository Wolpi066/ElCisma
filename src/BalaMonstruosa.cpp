#include "BalaMonstruosa.h"

// --- ¡¡FIX VELOCIDAD "VACA BUG"!! ---
// 7.0f (píxeles/frame) * 60 (frames/segundo) = 420.0f (píxeles/segundo)
static const float VELOCIDAD_BALA_JEFE = 420.0f;
// ------------------------------------

// Llama al constructor base con valores de jefe
BalaMonstruosa::BalaMonstruosa(Vector2 pos, Vector2 dir)
    : Bala(
        pos,
        dir,
        VELOCIDAD_BALA_JEFE, // <-- ¡¡ARREGLADO!! (ya no es 7.0f)
        3,                    // daño
        OrigenBala::ENEMIGO,
        6.0f                 // radio
    )
{
}

void BalaMonstruosa::dibujar() {
    // CORREGIDO: Usar los getters de la clase base
    if (estaActiva()) {
        DrawCircleV(getPosicion(), 6.0f, RED);
    }
}
