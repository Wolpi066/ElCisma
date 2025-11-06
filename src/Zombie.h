#pragma once
#include "Enemigo.h"

class Mapa; // <-- ¡AÑADIDO!

class Zombie : public Enemigo {
public:
    // --- CORREGIDO: Llama al constructor de 8 args de Enemigo ---
    Zombie(Vector2 pos);

    // --- CORREGIDO: Firma de la función ---
    // ¡MODIFICADO!
    virtual void actualizarIA(Vector2 posJugador, const Mapa& mapa) override;
    virtual void dibujar() override;
    virtual void atacar(Protagonista& jugador) override;
};
