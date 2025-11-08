#include "Bala.h"
#include "Constantes.h"
#include "raymath.h"

// --- ¡¡MODIFICADO!! (Almacena velocidad completa) ---
Bala::Bala(Vector2 pos, Vector2 dir, float rapidez, int dmg, OrigenBala org, float rad, bool esCheat) :
    posicion(pos),
    danio(dmg),
    origen(org),
    radio(rad),
    activa(true),
    esCheat(esCheat)
{
    // Almacenamos la velocidad completa (píxeles por segundo)
    this->velocidad = Vector2Scale(Vector2Normalize(dir), rapidez);
}
// ---------------------------------

void Bala::actualizarVidaUtil(Vector2 posJugador) {
    if (!activa) return;
    float distanciaAlJugador = Vector2Distance(posicion, posJugador);
    if (distanciaAlJugador > 2000.0f) {
        activa = false;
    }
}

void Bala::setPosicion(Vector2 nuevaPos) {
    this->posicion = nuevaPos;
}

void Bala::setVelocidad(Vector2 nuevaVel) {
    this->velocidad = nuevaVel;
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

bool Bala::esDisparoCheat() const {
    return esCheat;
}

Vector2 Bala::getPosicion() const {
    return posicion;
}

// --- ¡¡MODIFICADO!! (Devuelve la velocidad completa) ---
Vector2 Bala::getVelocidad() const {
    return this->velocidad;
}
// ---------------------------------------------

bool Bala::estaMuerto() const {
    return !activa;
}

bool Bala::estaConsumido() const {
    return false;
}
