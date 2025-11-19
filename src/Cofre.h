#pragma once
#include "Consumible.h"

enum class CofreOrientacion {
    HORIZONTAL,
    VERTICAL
};

class Cofre : public Consumible {
private:
    int tipoDeLoot;
    Rectangle rect;
    CofreOrientacion orientacion;

public:
    Cofre(Vector2 pos, int lootID, CofreOrientacion orient);

    int usar(Protagonista& jugador) override;
    void dibujar() override;

    // Implementacion obligatoria (aunque retorne vacio)
    Texture2D getTextura() override;

    bool esInteraccionPorTecla() const override;
    Rectangle getRect() const override;
};
