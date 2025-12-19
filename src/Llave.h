#pragma once
#include "Consumible.h"

class Llave : public Consumible
{
private:
    static Texture2D texLlave;
    static bool texturaCargada;

public:
    Llave(Vector2 pos);
    ~Llave();

    int usar(Protagonista& jugador) override;
    void dibujar() override;
    Texture2D getTextura() override;

    static void CargarTextura();
    static void DescargarTextura();
};
