#include "Cofre.h"
#include "raylib.h"

Cofre::Cofre(Vector2 pos, int lootID, CofreOrientacion orient)
    : Consumible(pos), tipoDeLoot(lootID), orientacion(orient)
{
    nombreItem = "COFRE"; // Nombre dummy

    float anchoLargo = 25;
    float anchoCorto = 15;

    if (orientacion == CofreOrientacion::HORIZONTAL) {
        this->rect = { pos.x - anchoLargo/2, pos.y - anchoCorto/2, anchoLargo, anchoCorto };
    } else {
        this->rect = { pos.x - anchoCorto/2, pos.y - anchoLargo/2, anchoCorto, anchoLargo };
    }
}

void Cofre::dibujar() {
    if (consumido) {
        DrawRectangleRec(rect, DARKGRAY);
        DrawRectangleLinesEx(rect, 2.0f, GRAY);
    } else {
        DrawRectangleRec(rect, BROWN);
        DrawRectangleLinesEx(rect, 2.0f, DARKBROWN);
    }
}

int Cofre::usar(Protagonista& jugador) {
    if (consumido) return 0;
    this->consumido = true;
    return this->tipoDeLoot;
}

bool Cofre::esInteraccionPorTecla() const {
    return true;
}

Rectangle Cofre::getRect() const {
    return rect;
}

// Implementacion dummy para cumplir el contrato de Consumible
Texture2D Cofre::getTextura() {
    return { 0 }; // Retorna textura vacia/nula
}
