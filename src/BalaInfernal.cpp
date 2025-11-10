#include "BalaInfernal.h"

// Estadísticas base (igual que BalaMonstruosa)
static const float VELOCIDAD_BALA_JEFE = 420.0f;
static const int DANIO_BALA_JEFE = 5;

// --- TAREA: Balas más grandes ---
static const float RADIO_BALA_INFERNAL = 9.0f; // (Original era 6.0f)

BalaInfernal::BalaInfernal(Vector2 pos, Vector2 dir)
    : Bala(
        pos,
        dir,
        VELOCIDAD_BALA_JEFE,
        DANIO_BALA_JEFE,
        OrigenBala::ENEMIGO,
        RADIO_BALA_INFERNAL // <-- Radio aumentado
    )
{
}

void BalaInfernal::dibujar() {
    if (estaActiva()) {
        // Color diferente y radio aumentado
        DrawCircleV(getPosicion(), RADIO_BALA_INFERNAL, ORANGE);
    }
}
