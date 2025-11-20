#include "IndicadorPuerta.h"

IndicadorPuerta::IndicadorPuerta(Vector2 pos) : Consumible(pos) {
    nombreItem = "PUERTA";
}

void IndicadorPuerta::dibujar() {
    // --- CORRECCIÓN: INVISIBLE ---
    // Ya no dibujamos el cuadrado dorado.
    // El objeto existe fisicamente para la interacción, pero no se ve.

    // (Si necesitas debuggear, descomenta la linea de abajo)
    // DrawRectangleRec(getRect(), Fade(GOLD, 0.5f));
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
    return { 0 };
}
