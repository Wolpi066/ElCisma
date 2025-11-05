#pragma once
#include "Consumible.h"

class IndicadorPuerta : public Consumible {
public:
    IndicadorPuerta(Vector2 pos);

    // --- ¡¡FIRMA ACTUALIZADA!! ---
    virtual int usar(Protagonista& jugador) override;
    virtual void dibujar() override;

    virtual bool esInteraccionPorTecla() const override;
    virtual bool estaConsumido() const override;
    void consumir();
};
