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

    bool abierto;

    // Visuales
    static Texture2D texCerrado;
    static Texture2D texAbierto;
    static bool texturasCargadas;

public:
    Cofre(Vector2 pos, int lootID, CofreOrientacion orient);
    ~Cofre();

    int usar(Protagonista& jugador) override;
    void dibujar() override;

    Texture2D getTextura() override;

    bool esInteraccionPorTecla() const override;
    Rectangle getRect() const override;

    bool estaConsumido() const override;

    // Para que Juego.cpp sepa ignorarlo
    bool estaAbierto() const;

    static void CargarTexturas();
    static void DescargarTexturas();
};
