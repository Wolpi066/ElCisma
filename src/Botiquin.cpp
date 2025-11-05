#include "Botiquin.h"
#include "Protagonista.h"
#include "Constantes.h"

Botiquin::Botiquin(Vector2 pos) : Consumible(pos) {}

void Botiquin::dibujar() {
    if (!consumido) {
        DrawRectangleRec(getRect(), RED);
    }
}

// --- ¡¡FIRMA ACTUALIZADA!! ---
int Botiquin::usar(Protagonista& jugador) {
    if (consumido) return 0;

    int cura = Constantes::VIDA_MAX_JUGADOR;
    jugador.curarVida(cura);

    this->consumido = true;
    return 0; // No suelta loot
}
