#include "CajaDeMuniciones.h"
#include "Protagonista.h"
#include "Constantes.h"

CajaDeMuniciones::CajaDeMuniciones(Vector2 pos) : Consumible(pos) {}

void CajaDeMuniciones::dibujar() {
    if (!consumido) {
        DrawRectangleRec(getRect(), BROWN);
    }
}

// --- ¡¡FIRMA ACTUALIZADA!! ---
int CajaDeMuniciones::usar(Protagonista& jugador) {
    if (consumido) return 0;

    int recarga = Constantes::MUNICION_MAX;
    jugador.recargarMunicion(recarga);

    this->consumido = true;
    return 0; // No suelta loot
}
