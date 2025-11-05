#include "Enemigo.h"
#include "Protagonista.h"
#include "raymath.h"

// --- CONSTRUCTOR RESTAURADO (8 args) ---
Enemigo::Enemigo(Vector2 pos, int v, int d, float vel, float rad,
                 float rangoV, float anguloV, float rangoE) :
    posicion(pos),
    vida(v),
    danio(d),
    velocidad(vel),
    radio(rad),
    direccion({0.0f, 1.0f}),
    rangoVision(rangoV),
    anguloVision(anguloV),
    rangoEscucha(rangoE),
    haDetectadoAlJugador(false)
{
}

// --- Métodos de IA ---

bool Enemigo::puedeVearAlJugador(Vector2 posJugador) {
    float distancia = Vector2Distance(this->posicion, posJugador);
    if (distancia > this->rangoVision) {
        return false;
    }
    Vector2 vectorAlJugador = Vector2Normalize(Vector2Subtract(posJugador, this->posicion));
    // Evita acos(nan) si los vectores son idénticos
    float dot = Vector2DotProduct(this->direccion, vectorAlJugador);
    if (dot > 1.0f) dot = 1.0f;
    if (dot < -1.0f) dot = -1.0f;

    float angulo = acos(dot) * RAD2DEG;
    if (angulo <= this->anguloVision / 2.0f) {
        return true;
    }
    return false;
}

bool Enemigo::puedeEscucharAlJugador(Vector2 posJugador) {
    float distancia = Vector2Distance(this->posicion, posJugador);
    if (distancia < this->rangoEscucha) {
        return true;
    }
    return false;
}

// --- Métodos Comunes ---

// --- CORREGIDO: Nombre y tipo ---
void Enemigo::recibirDanio(int cantidad) {
    this->vida -= cantidad;
    if (this->vida < 0) {
        this->vida = 0;
    }
    this->haDetectadoAlJugador = true;
}

void Enemigo::setPosicion(Vector2 nuevaPos) {
    this->posicion = nuevaPos;
}

bool Enemigo::estaVivo() const {
    return this->vida > 0;
}
Vector2 Enemigo::getPosicion() const {
    return this->posicion;
}
Rectangle Enemigo::getRect() const {
    return { posicion.x - radio, posicion.y - radio, radio * 2, radio * 2 };
}
int Enemigo::getDanio() const {
    return this->danio;
}
int Enemigo::getVida() const {
    return (int)vida;
}
float Enemigo::getVelocidad() const {
    return this->velocidad;
}
Vector2 Enemigo::getDireccion() const {
    return this->direccion;
}
bool Enemigo::estaMuerto() const {
    return !estaVivo();
}
bool Enemigo::estaConsumido() const {
    return false;
}
