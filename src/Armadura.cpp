#include "Armadura.h"
#include "Protagonista.h"

Armadura::Armadura(Vector2 pos) : Consumible(pos) {}

void Armadura::dibujar() {
    if (!consumido) {
        DrawRectangleRec(getRect(), GRAY);
    }
}

// --- ¡¡FIRMA ACTUALIZADA!! ---
int Armadura::usar(Protagonista& jugador) {
    if (consumido) return 0;

    jugador.recibirArmadura();

    this->consumido = true;
    return 0; // No suelta loot
}
