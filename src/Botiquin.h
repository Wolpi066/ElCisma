#pragma once
#include "Consumible.h"

class Botiquin : public Consumible
{
private:
    static Texture2D texBotiquin;
    static bool texturaCargada;

public:
    Botiquin(Vector2 pos);
    ~Botiquin();

    int usar(Protagonista& jugador) override;
    void dibujar() override;
    Texture2D getTextura() override;

    // --- CRÍTICO: Habilita agarrarlo con 'E' ---
    bool esInteraccionPorTecla() const override;

    static void CargarTextura();
    static void DescargarTextura();
};
