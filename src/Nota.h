#pragma once
#include "Consumible.h"

class Nota : public Consumible {
private:
    int notaID;
    bool leida; // ¡¡NUEVO!! Para saber si debe desaparecer

public:
    Nota(Vector2 pos, int id);

    virtual int usar(Protagonista& jugador) override;
    virtual void dibujar() override;

    virtual bool esInteraccionPorTecla() const override;
    virtual bool estaConsumido() const override; // ¡¡MODIFICADO!!
};
