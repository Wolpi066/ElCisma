#pragma once
#include "Enemigo.h"

class Zombie : public Enemigo {
public:
    // --- CORREGIDO: Llama al constructor de 8 args de Enemigo ---
    Zombie(Vector2 pos);

    // --- CORREGIDO: Firma de la función ---
    virtual void actualizarIA(Vector2 posJugador) override;
    virtual void dibujar() override;
    virtual void atacar(Protagonista& jugador) override;
};
