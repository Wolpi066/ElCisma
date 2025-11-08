#pragma once
#include "Bala.h"

// --- ¡¡FIX!! Declaraciones anticipadas ---
class Protagonista;
class Mapa;
// ------------------------------------

class MinaEnemiga : public Bala {
private:
    float temporizadorActivacion;
    float temporizadorVida;
    bool estaArmada;

public:
    MinaEnemiga(Vector2 pos, Vector2 dir);

    // --- ¡¡FIX!! Esta es la línea que causaba el error ---
    void actualizar(Protagonista& jugador, const Mapa& mapa) override;
    // --------------------------------------------------

    void dibujar() override;
};
