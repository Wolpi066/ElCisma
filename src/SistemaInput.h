#pragma once
#include "raylib.h"

// Clase estatica para gestionar todas las entradas
class SistemaInput
{
public:
    // Devuelve el vector de movimiento (ej. {1, 0} para derecha)
    static Vector2 getDireccionMovimiento();
    // Devuelve true si el jugador quiere disparar
    static bool quiereDisparar();

    // --- ¡¡NUEVO!! ---
    // Devuelve true si el jugador presiona la tecla de interaccion
    static bool quiereInteractuar();
};
