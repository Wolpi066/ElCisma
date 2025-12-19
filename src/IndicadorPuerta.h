#pragma once
#include "Consumible.h"

class IndicadorPuerta : public Consumible {
public:
    IndicadorPuerta(Vector2 pos);

    int usar(Protagonista& jugador) override;
    void dibujar() override;

    Texture2D getTextura() override;

    bool esInteraccionPorTecla() const override;
    bool estaConsumido() const override;
    void consumir();
};
