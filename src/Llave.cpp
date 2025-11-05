#include "Llave.h"
#include "Protagonista.h"

Llave::Llave(Vector2 pos) : Consumible(pos) {}

void Llave::dibujar() {
    if (!consumido) {
        DrawCircleV(posicion, 8.0f, ORANGE);
        DrawCircleLines(posicion.x, posicion.y, 8.0f, GOLD);
    }
}

// --- ¡¡FIRMA ACTUALIZADA!! ---
int Llave::usar(Protagonista& jugador) {
    if (consumido) return 0;

    jugador.recibirLlave();

    this->consumido = true;
    return 0; // No suelta loot
}

bool Llave::esInteraccionPorTecla() const {
    return true;
}
