#pragma once
#include "raylib.h"

class SistemaInput
{
public:
    static Vector2 getDireccionMovimiento();
    static bool quiereDisparar();

    static bool quiereInteractuar();
};
