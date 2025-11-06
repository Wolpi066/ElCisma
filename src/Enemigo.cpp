#include "Enemigo.h"
#include "Protagonista.h"
#include "raymath.h"
#include "Mapa.h" // <-- ¡AÑADIDO!

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
    estadoActual(EstadoIA::PATRULLANDO),
    destinoPatrulla(pos),
    temporizadorPatrulla(0.0f),
    rangoAtaque(50.0f),
    rangoDmg(60.0f),
    temporizadorAtaque(0.0f),
    temporizadorPausaAtaque(0.0f),
    temporizadorDanio(0.0f) // <-- ¡¡NUEVO!!
{
}

// --- ¡¡NUEVA FUNCION BASE!! ---
// Se llama siempre, antes que actualizarIA
void Enemigo::actualizarBase()
{
    // Actualizar timers que deben correr siempre
    if (temporizadorDanio > 0.0f) {
        temporizadorDanio -= GetFrameTime();
    }
}
// -----------------------------

// --- Métodos de IA ---

bool Enemigo::puedeVearAlJugador(Vector2 posJugador) {
    float distancia = Vector2Distance(this->posicion, posJugador);
    if (distancia > this->rangoVision) {
        return false;
    }
    Vector2 vectorAlJugador = Vector2Normalize(Vector2Subtract(posJugador, this->posicion));
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

void Enemigo::elegirNuevoDestinoPatrulla(const Mapa& mapa)
{
    float radioPatrulla = (float)GetRandomValue(150, 300);
    float anguloPatrulla = (float)GetRandomValue(0, 360) * DEG2RAD;
    destinoPatrulla.x = posicion.x + cos(anguloPatrulla) * radioPatrulla;
    destinoPatrulla.y = posicion.y + sin(anguloPatrulla) * radioPatrulla;
    Rectangle rectDestino = { destinoPatrulla.x - radio, destinoPatrulla.y - radio, radio * 2, radio * 2 };
    bool colision = false;
    for (const auto& muro : mapa.getMuros()) {
        if (CheckCollisionRecs(rectDestino, muro)) {
            colision = true;
            break;
        }
    }
    if (!colision) {
        for (const auto& caja : mapa.getCajas()) {
            if (CheckCollisionRecs(rectDestino, caja)) {
                colision = true;
                break;
            }
        }
    }
    if (colision) {
        destinoPatrulla = posicion;
        temporizadorPatrulla = 2.0f;
    } else {
        temporizadorPatrulla = (float)GetRandomValue(5, 10);
    }
}


// --- Métodos Comunes ---
void Enemigo::recibirDanio(int cantidad) {
    this->vida -= cantidad;
    if (this->vida < 0) {
        this->vida = 0;
    }
    this->estadoActual = EstadoIA::PERSIGUIENDO;
    this->temporizadorDanio = 0.2f; // <-- ¡¡NUEVO!! Activa el flash por 0.2s
}

void Enemigo::setPosicion(Vector2 nuevaPos) {
    this->posicion = nuevaPos;
}

void Enemigo::setDireccion(Vector2 nuevaDir) {
    if (Vector2LengthSqr(nuevaDir) > 0.001f) {
        this->direccion = Vector2Normalize(nuevaDir);
    }
}

// --- ¡¡NUEVAS FUNCIONES!! ---
bool Enemigo::estaListoParaAtacar() const {
    return (estadoActual == EstadoIA::ATACANDO && temporizadorPausaAtaque <= 0.0f);
}

EstadoIA Enemigo::getEstadoIA() const {
    return estadoActual;
}
// ----------------------------


// Getters
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
float Enemigo::getRadio() const {
    return this->radio;
}
bool Enemigo::estaMuerto() const {
    return !estaVivo();
}
bool Enemigo::estaConsumido() const {
    return false;
}
