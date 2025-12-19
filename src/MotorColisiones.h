#pragma once

#include "Protagonista.h"
#include "GestorEntidades.h"

class MotorColisiones
{
public:
    static void procesar(
        Protagonista& jugador,
        GestorEntidades& gestor
    );
};
