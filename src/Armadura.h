#pragma once
#include "Consumible.h" // Herencia

class Armadura : public Consumible {
public:
    Armadura(Vector2 pos);

    // --- ¡¡FIRMA ACTUALIZADA!! ---
    virtual int usar(Protagonista& jugador) override;
    virtual void dibujar() override;

    // --- ¡¡AÑADIDO!! ---
    // Sobreescribimos para que devuelva true.
    virtual bool esInteraccionPorTecla() const override {
        return true; // Ahora se recoge con 'E'
    }
};
