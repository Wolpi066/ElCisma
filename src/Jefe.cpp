#include "Jefe.h"
#include "Protagonista.h"
#include "Constantes.h" // Para VELOCIDAD_OBESO

Jefe::Jefe(Vector2 pos) :
    posicion(pos),
    vida(50),
    danioContacto(10),
    estado(EstadoJefe::ESPERANDO),
    temporizadorEstado(2.0f),
    rafagaRestante(0),
    disparoSolicitado(false),
    velocidadActual({0, 0})
{
}

void Jefe::actualizar(Protagonista& jugador) {
    if (!estaVivo()) return;

    temporizadorEstado -= GetFrameTime();
    disparoSolicitado = false;
    velocidadActual = {0, 0};

    switch (estado) {
        case EstadoJefe::ESPERANDO:
            if (temporizadorEstado <= 0) {
                estado = EstadoJefe::MOVIENDO;
                temporizadorEstado = 3.0f;
            }
            break;

        case EstadoJefe::MOVIENDO:
            {
                Vector2 dir = Vector2Normalize(Vector2Subtract(jugador.getPosicion(), posicion));
                velocidadActual = Vector2Scale(dir, Constantes::VELOCIDAD_OBESO);
            }
            if (temporizadorEstado <= 0) {
                estado = EstadoJefe::ATACANDO_RAFAGA;
                temporizadorEstado = 0.2f;
                rafagaRestante = 5;
            }
            break;

        case EstadoJefe::ATACANDO_RAFAGA:
            if (rafagaRestante > 0 && temporizadorEstado <= 0) {
                disparoSolicitado = true;
                rafagaRestante--;
                temporizadorEstado = 0.2f;
            }
            if (rafagaRestante <= 0) {
                estado = EstadoJefe::ESPERANDO;
                temporizadorEstado = 2.0f;
            }
            break;
    }
}

// --- Nuevos Getters y Setters ---

bool Jefe::quiereDisparar() const {
    return disparoSolicitado;
}

void Jefe::setPosicion(Vector2 nuevaPos) {
    this->posicion = nuevaPos;
}

Vector2 Jefe::getVelocidadActual() const {
    return velocidadActual;
}

// --- Resto de métodos ---

void Jefe::dibujar() {
    if (estaVivo()) {
        DrawRectangleRec(getRect(), MAROON);
    }
}
void Jefe::recibirDanio(int cantidad) {
    vida -= cantidad;
    if (vida < 0) vida = 0;
}
bool Jefe::estaVivo() const {
    return vida > 0;
}
Rectangle Jefe::getRect() const {
    return { posicion.x - 30, posicion.y - 30, 60, 60 };
}
Vector2 Jefe::getPosicion() const {
    return posicion;
}
bool Jefe::estaMuerto() const {
    return !estaVivo();
}
bool Jefe::estaConsumido() const {
    return false;
}
