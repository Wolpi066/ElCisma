#pragma once
#include "Bala.h"
#include "raymath.h"

class Protagonista;
class Mapa;

class TrozoDeCarne : public Bala {
private:
    Vector2 inicioSalto;
    Vector2 objetivoSuelo;
    float progresoSalto;
    float temporizadorSuelo;
    bool estaEnSuelo;
    bool sonidoImpactoJugado; // Nuevo

    static Texture2D texAire;
    static Texture2D texSuelo;
    static Sound fxImpacto; // Nuevo

    static const float TIEMPO_VUELO;
    static const float TIEMPO_EN_SUELO;
    static const float RADIO_CHARCO;

public:
    TrozoDeCarne(Vector2 pos, Vector2 objetivo);
    void actualizar(Protagonista& jugador, const Mapa& mapa) override;
    void dibujar() override;
    bool esCharco() const;

    static void CargarRecursos();
    static void DescargarRecursos();
};
