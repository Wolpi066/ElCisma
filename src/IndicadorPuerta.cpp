#include "IndicadorPuerta.h"

// Constructor: Llama al constructor base
IndicadorPuerta::IndicadorPuerta(Vector2 pos) : Consumible(pos) {
}

void IndicadorPuerta::dibujar() {
    if (!consumido)
    {
        Rectangle rect = getRect();
        rect.width = 10;
        rect.height = 10;
        rect.x -= 2; // Ajustar el centro
        rect.y -= 2; // Ajustar el centro
        DrawRectangleRec(rect, GOLD);
    }
}

// --- ¡¡FIRMA ACTUALIZADA!! ---
int IndicadorPuerta::usar(Protagonista& jugador) {
    // No hacer nada. Es solo visual.
    return 0; // No suelta loot
}

bool IndicadorPuerta::esInteraccionPorTecla() const {
    return true;
}

void IndicadorPuerta::consumir()
{
    this->consumido = true;
}

bool IndicadorPuerta::estaConsumido() const {
    // Sobrescribimos la funcion base
    return consumido;
}
