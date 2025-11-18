#pragma once
#include "Bala.h"
#include "raylib.h"

class BalaDeRifle : public Bala
{
private:
    static Texture2D texBalaRifle;
    static bool texturaCargada;

public:
    BalaDeRifle(Vector2 pos, Vector2 dir, bool esCheat = false);
    ~BalaDeRifle();

    // --- CORRECCIÓN: La firma debe coincidir con la clase base 'Bala' ---
    void actualizar(Protagonista& jugador, const Mapa& mapa) override;
    // --------------------------------------------------------------------

    void dibujar() override;

    static void CargarTextura();
    static void DescargarTextura();
};
