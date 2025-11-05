#pragma once

#include "Protagonista.h"
#include "GestorEntidades.h" // <-- AHORA USA EL GESTOR

// Clase estática para manejar la lógica de colisiones
class MotorColisiones
{
public:
    // CAMBIO: Firma del método actualizada
    static void procesar(
        Protagonista& jugador,
        GestorEntidades& gestor
    );
};
