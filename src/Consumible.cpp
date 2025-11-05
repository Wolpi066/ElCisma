#include "Consumible.h"

Consumible::Consumible(Vector2 pos)
    : posicion(pos), consumido(false) {
}

bool Consumible::estaConsumido() const {
    return consumido;
}

bool Consumible::esInteraccionPorTecla() const {
    // Por defecto, se recogen al tocarlos.
    return false;
}

Vector2 Consumible::getPosicion() const {
    return posicion;
}

// ¡¡VIRTUAL!!
Rectangle Consumible::getRect() const {
    // Tamaño por defecto para items de piso
    return { posicion.x - 5, posicion.y - 5, 10, 10 };
}
