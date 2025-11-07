#pragma once
#include "Bala.h"

class BalaDeRifle : public Bala
{
public:
    // --- ¡¡MODIFICADO!! (Añadido bool esCheat) ---
    BalaDeRifle(Vector2 pos, Vector2 dir, bool esCheat = false);
    void dibujar() override;
};
