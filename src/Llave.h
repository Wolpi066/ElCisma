#pragma once
#include "Consumible.h"

class Llave : public Consumible {
public:
    Llave(Vector2 pos);

    // --- ¡¡FIRMA ACTUALIZADA!! ---
    virtual int usar(Protagonista& jugador) override;
    virtual void dibujar() override;

    virtual bool esInteraccionPorTecla() const override;
};
