#pragma once
#include "Bala.h"
#include "raymath.h"

// FWD Declarations
class Protagonista;
class Mapa;

class TrozoDeCarne : public Bala {
private:
    Vector2 inicioSalto;
    Vector2 objetivoSuelo;
    float progresoSalto; // 0.0 a 1.0
    float temporizadorSuelo; // Cuánto dura el charco
    bool estaEnSuelo;

    static const float TIEMPO_VUELO;
    static const float TIEMPO_EN_SUELO;
    static const float RADIO_CHARCO;

public:
    TrozoDeCarne(Vector2 pos, Vector2 objetivo);

    // Sobrescribe la actualización de Bala
    void actualizar(Protagonista& jugador, const Mapa& mapa) override;
    void dibujar() override;

    bool esCharco() const;
};
