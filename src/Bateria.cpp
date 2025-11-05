#include "Bateria.h"
#include "Protagonista.h"

Bateria::Bateria(Vector2 pos) : Consumible(pos) {}

void Bateria::dibujar() {
    if (!consumido) {
        DrawRectangleRec(getRect(), YELLOW);
    }
}

// --- ¡¡FIRMA ACTUALIZADA!! ---
int Bateria::usar(Protagonista& jugador) {
    if (consumido) return 0;

    int recarga = 30;
    jugador.recargarBateria(recarga);

    this->consumido = true;
    return 0; // No suelta loot
}
