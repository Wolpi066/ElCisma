#pragma once
#include "Consumible.h"

class Nota : public Consumible {
private:
    int notaID;
    bool leida;

    static Texture2D texNota;
    static bool texturaCargada;

public:
    Nota(Vector2 pos, int id);
    ~Nota();

    int usar(Protagonista& jugador) override;
    void dibujar() override;

    Texture2D getTextura() override;

    bool esInteraccionPorTecla() const override;
    bool estaConsumido() const override;

    static void CargarTextura();
    static void DescargarTextura();
};
