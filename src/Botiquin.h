#pragma once
#include "Consumible.h"

class Botiquin : public Consumible {
public:
    Botiquin(Vector2 pos);

    // --- ¡¡FIRMA ACTUALIZADA!! ---
    virtual int usar(Protagonista& jugador) override;
    virtual void dibujar() override;
};
