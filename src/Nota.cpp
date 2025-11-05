#include "Nota.h"

Nota::Nota(Vector2 pos, int id)
    : Consumible(pos), notaID(id)
{
}

void Nota::dibujar() {
    // Dibujamos un papelito blanco
    Rectangle rect = getRect();
    rect.width = 12; // Un poco mas grande que un item normal
    rect.height = 12;
    rect.x -= 1;
    rect.y -= 1;
    DrawRectangleRec(rect, WHITE);
    DrawRectangleLinesEx(rect, 1.0f, BLACK);
}

int Nota::usar(Protagonista& jugador) {
    // Devuelve el ID de la nota al Juego
    return notaID;
}

bool Nota::esInteraccionPorTecla() const {
    return true; // Se usa con 'E'
}

bool Nota::estaConsumido() const {
    return false; // Nunca se consume/desaparece
}
