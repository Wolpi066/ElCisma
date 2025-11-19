#include "IndicadorPuerta.h"

IndicadorPuerta::IndicadorPuerta(Vector2 pos) : Consumible(pos) {
    nombreItem = "PUERTA";
}

void IndicadorPuerta::dibujar() {
    if (!consumido)
    {
        Rectangle rect = getRect();
        rect.width = 10;
        rect.height = 10;
        rect.x -= 2;
        rect.y -= 2;
        DrawRectangleRec(rect, GOLD);
    }
}

int IndicadorPuerta::usar(Protagonista& jugador) {
    return 0;
}

bool IndicadorPuerta::esInteraccionPorTecla() const {
    return true;
}

void IndicadorPuerta::consumir()
{
    this->consumido = true;
}

bool IndicadorPuerta::estaConsumido() const {
    return consumido;
}

Texture2D IndicadorPuerta::getTextura() {
    return { 0 }; // Retorna textura vacia
}
