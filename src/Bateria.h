#pragma once
#include "Consumible.h" // Herencia

class Bateria : public Consumible {
public:
    Bateria(Vector2 pos);

    // --- ¡¡FIRMA ACTUALIZADA!! ---
    virtual int usar(Protagonista& jugador) override;
    virtual void dibujar() override;
};
