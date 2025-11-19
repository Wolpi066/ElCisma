#pragma once
#include "Consumible.h"

class Nota : public Consumible {
private:
    int notaID;
    bool leida;

public:
    Nota(Vector2 pos, int id);

    int usar(Protagonista& jugador) override;
    void dibujar() override;

    // Implementacion obligatoria
    Texture2D getTextura() override;

    bool esInteraccionPorTecla() const override;
    bool estaConsumido() const override;
};
