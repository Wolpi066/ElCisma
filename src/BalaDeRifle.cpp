#include "BalaDeRifle.h"
#include "Constantes.h"

// --- ¡¡FIX DE CONSTANTES!! ---
// Definimos los valores localmente
static const float VELOCIDAD_BALA_RIFLE = 1200.0f;
static const int   DANIO_BALA_RIFLE     = 1;
static const float RADIO_BALA_RIFLE     = 4.0f;
// -----------------------------

BalaDeRifle::BalaDeRifle(Vector2 pos, Vector2 dir, bool esCheat)
    : Bala(
        pos,
        dir,
        VELOCIDAD_BALA_RIFLE,   // <-- Arreglado
        DANIO_BALA_RIFLE,       // <-- Arreglado
        OrigenBala::JUGADOR,
        RADIO_BALA_RIFLE,       // <-- Arreglado
        esCheat
    )
{
}

void BalaDeRifle::dibujar()
{
    if (!activa) return;

    Color colorBala = esCheat ? (Color){255, 0, 0, 255} : (Color){255, 255, 0, 255};

    DrawCircleV(posicion, radio, colorBala);
}
