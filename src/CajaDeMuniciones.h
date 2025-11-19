#pragma once
#include "Consumible.h"

class CajaDeMuniciones : public Consumible
{
private:
    static Texture2D texMunicion;
    static bool texturaCargada;

public:
    CajaDeMuniciones(Vector2 pos);
    ~CajaDeMuniciones();

    int usar(Protagonista& jugador) override; // Cambio a int
    void dibujar() override;
    Texture2D getTextura() override;

    static void CargarTextura();
    static void DescargarTextura();
};
