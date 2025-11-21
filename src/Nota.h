#pragma once
#include "Consumible.h"

class Nota : public Consumible {
private:
    int notaID;
    bool leida;

    // --- VISUALES ---
    static Texture2D texNota;
    static bool texturaCargada;

public:
    Nota(Vector2 pos, int id);
    ~Nota(); // Añadimos destructor para limpieza si fuera necesario

    int usar(Protagonista& jugador) override;
    void dibujar() override;

    Texture2D getTextura() override;

    bool esInteraccionPorTecla() const override;
    bool estaConsumido() const override;

    // Gestión de recursos
    static void CargarTextura();
    static void DescargarTextura();
};
