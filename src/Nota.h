#pragma once
#include "Consumible.h"

class Nota : public Consumible {
private:
    int notaID; // 1 = "Hola mundo"

public:
    Nota(Vector2 pos, int id);

    virtual int usar(Protagonista& jugador) override;
    virtual void dibujar() override;

    virtual bool esInteraccionPorTecla() const override;
    virtual bool estaConsumido() const override; // Nunca desaparece
};
