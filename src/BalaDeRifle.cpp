#include "BalaDeRifle.h"

// El constructor de BalaDeRifle llama al constructor base (Bala)
BalaDeRifle::BalaDeRifle(Vector2 pos, Vector2 dir)
    : Bala(pos, dir, 10.0f, 1, OrigenBala::JUGADOR, 3.0f)
{
}

void BalaDeRifle::dibujar() {
    if (estaActiva()) {
        DrawCircleV(getPosicion(), 3.0f, YELLOW);
    }
}
