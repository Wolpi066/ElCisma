#pragma once
#include "Consumible.h"

class Armadura : public Consumible
{
private:
    static Texture2D texArmadura;
    static bool texturaCargada;

public:
    Armadura(Vector2 pos);
    ~Armadura();

    int usar(Protagonista& jugador) override;
    void dibujar() override;
    Texture2D getTextura() override;

    static void CargarTextura();
    static void DescargarTextura();
};
