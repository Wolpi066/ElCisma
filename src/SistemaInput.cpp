#include "SistemaInput.h"

Vector2 SistemaInput::getDireccionMovimiento()
{
    Vector2 direccion = { 0.0f, 0.0f };
    if (IsKeyDown(KEY_W)) direccion.y -= 1.0f;
    if (IsKeyDown(KEY_S)) direccion.y += 1.0f;
    if (IsKeyDown(KEY_A)) direccion.x -= 1.0f;
    if (IsKeyDown(KEY_D)) direccion.x += 1.0f;
    return direccion;
}

bool SistemaInput::quiereDisparar()
{
    return IsMouseButtonDown(MOUSE_LEFT_BUTTON);
}

bool SistemaInput::quiereInteractuar()
{
    return IsKeyPressed(KEY_E);
}
