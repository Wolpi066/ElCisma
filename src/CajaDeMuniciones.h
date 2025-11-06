#pragma once
#include "Consumible.h" // Herencia

class CajaDeMuniciones : public Consumible {
public:
    CajaDeMuniciones(Vector2 pos);

    // --- ¡¡FIRMA ACTUALIZADA!! ---
    virtual int usar(Protagonista& jugador) override;
    virtual void dibujar() override;

    // --- ¡¡AÑADIDO!! ---
    // Sobreescribimos para que devuelva true.
    virtual bool esInteraccionPorTecla() const override {
        return true; // Ahora se recoge con 'E'
    }
};
