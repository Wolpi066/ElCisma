#include "Enemigo.h"
#include "Protagonista.h"
#include "Mapa.h"
#include <cmath>
#include <cstdlib>

bool Enemigo::batallaJefeIniciada = false;

Enemigo::Enemigo(Vector2 pos, int v, int d, float vel, float rad,
                 float rangoV, float anguloV, float rangoE)
    : posicion(pos), vida(v), danio(d), velocidad(vel), radio(rad),
      direccion({0.0f, 1.0f}), rangoVision(rangoV), anguloVision(anguloV),
      rangoEscucha(rangoE), estadoActual(EstadoIA::PATRULLANDO),
      destinoPatrulla(pos), temporizadorPatrulla(0.0f),
      rangoAtaque(rad + 10.0f), rangoDmg(rad + 5.0f),
      temporizadorAtaque(0.0f), temporizadorPausaAtaque(0.0f),
      temporizadorDanio(0.0f)
{
}

void Enemigo::actualizarBase() {
    if (temporizadorAtaque > 0.0f) temporizadorAtaque -= GetFrameTime();
    if (temporizadorDanio > 0.0f) temporizadorDanio -= GetFrameTime();
}

void Enemigo::recibirDanio(int cantidad) {
    this->vida -= cantidad;
    if (this->vida < 0) this->vida = 0;

    this->estadoActual = EstadoIA::PERSIGUIENDO;
    this->temporizadorDanio = 0.2f;
}

void Enemigo::setPosicion(Vector2 nuevaPos) { this->posicion = nuevaPos; }

void Enemigo::setDireccion(Vector2 nuevaDir) {
    if (Vector2LengthSqr(nuevaDir) > 0.001f) {
        this->direccion = Vector2Normalize(nuevaDir);
    }
}

bool Enemigo::estaListoParaAtacar() const {
    return (estadoActual == EstadoIA::ATACANDO && temporizadorPausaAtaque <= 0.0f);
}

EstadoIA Enemigo::getEstadoIA() const { return estadoActual; }
bool Enemigo::estaVivo() const { return this->vida > 0; }
Vector2 Enemigo::getPosicion() const { return this->posicion; }
Rectangle Enemigo::getRect() const {
    return { posicion.x - radio, posicion.y - radio, radio * 2, radio * 2 };
}
int Enemigo::getDanio() const { return this->danio; }
int Enemigo::getVida() const { return (int)vida; }
float Enemigo::getVelocidad() const { return this->velocidad; }
Vector2 Enemigo::getDireccion() const { return this->direccion; }
float Enemigo::getRadio() const { return this->radio; }

bool Enemigo::estaMuerto() const { return !estaVivo(); }
bool Enemigo::estaConsumido() const { return false; }

bool Enemigo::puedeVearAlJugador(Vector2 posJugador) {
    float distancia = Vector2Distance(this->posicion, posJugador);
    if (distancia > this->rangoVision) return false;
    Vector2 vectorAlJugador = Vector2Normalize(Vector2Subtract(posJugador, this->posicion));
    float dot = Vector2DotProduct(this->direccion, vectorAlJugador);
    if (dot > 1.0f) dot = 1.0f;
    if (dot < -1.0f) dot = -1.0f;
    float angulo = acos(dot) * RAD2DEG;
    return (angulo <= this->anguloVision / 2.0f);
}

bool Enemigo::puedeEscucharAlJugador(Vector2 posJugador) {
    float distancia = Vector2Distance(this->posicion, posJugador);
    return (distancia < this->rangoEscucha);
}

void Enemigo::elegirNuevoDestinoPatrulla(const Mapa& mapa) {
    float radioPatrulla = (float)GetRandomValue(150, 300);
    float anguloPatrulla = (float)GetRandomValue(0, 360) * DEG2RAD;

    destinoPatrulla.x = posicion.x + cosf(anguloPatrulla) * radioPatrulla;
    destinoPatrulla.y = posicion.y + sinf(anguloPatrulla) * radioPatrulla;

    temporizadorPatrulla = (float)GetRandomValue(2, 5);
}
