#include "Nota.h"
#include "raymath.h"

Nota::Nota(Vector2 pos, int id)
    : Consumible(pos), notaID(id), leida(false)
{
    nombreItem = "NOTA";
}

void Nota::dibujar() {
    Rectangle rect = getRect();
    rect.width = 12;
    rect.height = 12;
    rect.x -= 1;
    rect.y -= 1;

    if (notaID == 0) {
        float alpha = (sin(GetTime() * 5.0f) + 1.0f) / 2.0f;
        DrawCircleV(posicion, 10.0f, Fade((Color){0, 150, 255, 255}, alpha * 0.3f));
        DrawRectangleRec(rect, (Color){220, 240, 255, 255});
    } else {
        DrawRectangleRec(rect, WHITE);
    }

    DrawRectangleLinesEx(rect, 1.0f, BLACK);
}

int Nota::usar(Protagonista& jugador) {
    this->leida = true;
    return notaID;
}

bool Nota::esInteraccionPorTecla() const {
    return true;
}

bool Nota::estaConsumido() const {
    return (notaID == 0 && leida);
}

Texture2D Nota::getTextura() {
    return { 0 }; // Retorna textura vacia
}
