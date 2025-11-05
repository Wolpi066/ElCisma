#include "Cofre.h"
#include "raylib.h" // Para DrawRectangleRec

// Constructor: Llama al constructor base y guarda el loot
Cofre::Cofre(Vector2 pos, int lootID, CofreOrientacion orient)
    : Consumible(pos), tipoDeLoot(lootID), orientacion(orient)
{
    // --- ¡NUEVO! Dimensiones Larga/Corta ---
    float anchoLargo = 25;
    float anchoCorto = 15;

    if (orientacion == CofreOrientacion::HORIZONTAL) {
        // Paralelo a pared N/S (mas ancho que alto)
        this->rect = { pos.x - anchoLargo/2, pos.y - anchoCorto/2, anchoLargo, anchoCorto };
    } else {
        // Paralelo a pared E/O (mas alto que ancho)
        this->rect = { pos.x - anchoCorto/2, pos.y - anchoLargo/2, anchoCorto, anchoLargo };
    }
}

void Cofre::dibujar() {
    if (consumido) {
        // Cofre abierto (gris)
        DrawRectangleRec(rect, DARKGRAY);
        DrawRectangleLinesEx(rect, 2.0f, GRAY);
    } else {
        // Cofre cerrado (marron)
        DrawRectangleRec(rect, BROWN);
        DrawRectangleLinesEx(rect, 2.0f, DARKBROWN);
    }
}

int Cofre::usar(Protagonista& jugador) {
    if (consumido) return 0; // Ya se abrio, no devuelve loot

    this->consumido = true; // El cofre se abre
    return this->tipoDeLoot; // Devuelve el codigo de loot al Juego
}

bool Cofre::esInteraccionPorTecla() const {
    // Requiere la tecla 'E'
    return true;
}

Rectangle Cofre::getRect() const {
    // Devuelve el rectangulo mas grande
    return rect;
}
