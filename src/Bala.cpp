#include "Bala.h"
#include "Constantes.h"
#include "raymath.h"

// --- RESTAURADO: Constructor correcto ---
Bala::Bala(Vector2 pos, Vector2 dir, float rapidez, int dmg, OrigenBala org, float rad) :
    posicion(pos),
    danio(dmg),
    origen(org),
    radio(rad),
    activa(true)
{
    this->velocidad = Vector2Scale(Vector2Normalize(dir), rapidez);
}

// --- RESTAURADO: Implementaciones correctas ---
void Bala::actualizarVidaUtil(Vector2 posJugador) {
    if (!activa) return;

    // Solo comprueba la distancia para desactivarse
    float distanciaAlJugador = Vector2Distance(posicion, posJugador);
    if (distanciaAlJugador > 2000.0f) {
        activa = false;
    }
}

void Bala::setPosicion(Vector2 nuevaPos) {
    this->posicion = nuevaPos;
}

bool Bala::estaActiva() const {
    return activa;
}

void Bala::desactivar() {
    activa = false;
}

Rectangle Bala::getRect() const {
    return { posicion.x - radio, posicion.y - radio, radio * 2, radio * 2 };
}

OrigenBala Bala::getOrigen() const {
    return origen;
}

int Bala::getDanio() const {
    return danio;
}

Vector2 Bala::getPosicion() const {
    return posicion;
}

Vector2 Bala::getVelocidad() const {
    return this->velocidad;
}

bool Bala::estaMuerto() const {
    return !activa;
}

bool Bala::estaConsumido() const {
    return false;
}
