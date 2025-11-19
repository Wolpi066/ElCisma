#include "Consumible.h"

Consumible::Consumible(Vector2 pos)
    : posicion(pos), consumido(false), nombreItem("Item"), descripcionItem("")
{
}

Consumible::~Consumible()
{
}

const char* Consumible::getNombre() const {
    return nombreItem;
}

const char* Consumible::getDescripcion() const {
    return descripcionItem;
}

bool Consumible::estaConsumido() const {
    return consumido;
}

Vector2 Consumible::getPosicion() const {
    return posicion;
}

Rectangle Consumible::getRect() const {
    // Area de interaccion un poco mas grande (30x30)
    return { posicion.x - 15, posicion.y - 15, 30, 30 };
}

bool Consumible::esInteraccionPorTecla() const {
    return false;
}

// Esto arregla el error del template en GestorEntidades
bool Consumible::estaMuerto() const {
    return false;
}

void Consumible::consumir() {
    consumido = true;
}
