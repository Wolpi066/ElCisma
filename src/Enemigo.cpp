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
    // haDetectadoAlJugador(false), // <-- ELIMINADO
    // --- ¡NUEVO! Inicialización de IA ---
    estadoActual(EstadoIA::PATRULLANDO),
    destinoPatrulla(pos),
    temporizadorPatrulla(0.0f) // 0.0f para que elija destino en el primer frame
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
        // TODO: Implementar Raycast para línea de visión
        return true;
    }
    return false;
}

bool Enemigo::puedeEscucharAlJugador(Vector2 posJugador) {
    // TODO: Deberíamos recibir si el jugador está corriendo.
    // Por ahora, funciona como antes.
    float distancia = Vector2Distance(this->posicion, posJugador);
    if (distancia < this->rangoEscucha) {
        return true;
    }
    return false;
}

// --- ¡NUEVA FUNCION! ---
void Enemigo::elegirNuevoDestinoPatrulla(const Mapa& mapa)
{
    // Elige un punto aleatorio en un radio de 150-300 pixels
    float radioPatrulla = (float)GetRandomValue(150, 300);
    float anguloPatrulla = (float)GetRandomValue(0, 360) * DEG2RAD;

    destinoPatrulla.x = posicion.x + cos(anguloPatrulla) * radioPatrulla;
    destinoPatrulla.y = posicion.y + sin(anguloPatrulla) * radioPatrulla;

    // Validamos que no esté en un muro o caja
    // (Simplificado: creamos un rect del enemigo en el destino)
    Rectangle rectDestino = { destinoPatrulla.x - radio, destinoPatrulla.y - radio, radio * 2, radio * 2 };
    bool colision = false;

    // getMuros() y getCajas() deben ser const en Mapa.h (¡importante!)
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
        // Si el destino choca, nos quedamos quietos
        destinoPatrulla = posicion;
        temporizadorPatrulla = 2.0f; // Reintentamos en 2 segundos
    } else {
        // Siguiente patrulla en 5 a 10 segundos
        temporizadorPatrulla = (float)GetRandomValue(5, 10);
    }
}


// --- Métodos Comunes ---

// --- CORREGIDO: Nombre y tipo ---
void Enemigo::recibirDanio(int cantidad) {
    this->vida -= cantidad;
    if (this->vida < 0) {
        this->vida = 0;
    }
    // ¡MODIFICADO! Al recibir daño, siempre te detecta
    this->estadoActual = EstadoIA::PERSIGUIENDO;
    // this->haDetectadoAlJugador = true; // <-- ELIMINADO
}

void Enemigo::setPosicion(Vector2 nuevaPos) {
    this->posicion = nuevaPos;
}

// --- ¡¡NUEVA FUNCION!! ---
void Enemigo::setDireccion(Vector2 nuevaDir) {
    // Solo actualizamos si es un vector válido (evita NaN)
    // Usamos un umbral pequeño (en lugar de 0.0f) para estabilidad
    if (Vector2LengthSqr(nuevaDir) > 0.001f) {
        this->direccion = Vector2Normalize(nuevaDir);
    }
}
// -------------------------

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
