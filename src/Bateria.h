#pragma once
#include "Consumible.h"

class Bateria : public Consumible
{
private:
    static Texture2D texBateria;
    static bool texturaCargada;

public:
    Bateria(Vector2 pos);
    ~Bateria();

    int usar(Protagonista& jugador) override; // Cambio a int
    void dibujar() override;
    Texture2D getTextura() override;

    static void CargarTextura();
    static void DescargarTextura();
};
