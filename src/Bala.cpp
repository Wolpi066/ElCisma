#include "Bala.h"
#include "Constantes.h"
#include "raymath.h"

// --- ¡¡MODIFICADO!! (Almacena dir y rapidez) ---
Bala::Bala(Vector2 pos, Vector2 dir, float rapidez, int dmg, OrigenBala org, float rad, bool esCheat) :
    posicion(pos),
    danio(dmg),
    origen(org),
    radio(rad),
    activa(true),
    esCheat(esCheat),
    // --- ¡¡FIX VACA!! ---
    direccion(Vector2Normalize(dir)), // Almacenamos la dirección normalizada
    rapidez(rapidez)                // Almacenamos la rapidez (ej: 1200.0f)
{
    // Ya no calculamos 'this->velocidad' aquí
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

// --- ¡¡MODIFICADO!! (Implementa el "Vaca Fix") ---
Vector2 Bala::getVelocidad() const {
    // Calculamos el desplazamiento por frame (lo que MotorFisica espera)
    return Vector2Scale(direccion, rapidez * GetFrameTime());
}
// ---------------------------------------------

bool Bala::estaMuerto() const {
    return !activa;
}

bool Bala::estaConsumido() const {
    return false;
}
