#include "Zombie.h"
#include "Constantes.h"
#include "raymath.h"
#include "Protagonista.h" // ¡¡Importante!!
#include "Mapa.h"         // ¡¡AÑADIDO!!

// --- CORREGIDO: Constructor llama a 8 args ---
Zombie::Zombie(Vector2 pos)
    : Enemigo(pos,
              Constantes::VIDA_ZOMBIE,
              Constantes::DANIO_ZOMBIE,
              Constantes::VELOCIDAD_ZOMBIE,
              Constantes::RADIO_ZOMBIE,
              Constantes::RANGO_VISUAL_ZOMBIE,
              Constantes::ANGULO_CONO_ZOMBIE,
              Constantes::RANGO_AUDIO_ZOMBIE)
{
}

// --- CORREGIDO: Nombre de la funcion ---
// --- ¡¡LÓGICA DE IA COMPLETAMENTE REEMPLAZADA POR FSM!! ---
void Zombie::actualizarIA(Vector2 posJugador, const Mapa& mapa) {

    // --- 1. TRANSICIONES DE ESTADO ---
    // Comprobamos si vemos u oímos al jugador
    bool jugadorDetectado = puedeVearAlJugador(posJugador) || puedeEscucharAlJugador(posJugador);

    if (estadoActual == EstadoIA::PATRULLANDO) {
        if (jugadorDetectado) {
            estadoActual = EstadoIA::PERSIGUIENDO;
        }
    }
    else if (estadoActual == EstadoIA::PERSIGUIENDO) {
        if (!jugadorDetectado) {
            // Si no lo vemos/oímos, comprobamos si está lo suficientemente lejos
            float distancia = Vector2Distance(posicion, posJugador);
            // Si está más lejos que el rango de visión (ej. 1.5x) lo perdemos
            if (distancia > rangoVision * 1.5f) {
                estadoActual = EstadoIA::PATRULLANDO;
                temporizadorPatrulla = 0.0f; // Empezar a patrullar ya
            }
        }
    }
    // (El Zombie no usa el estado ATACANDO, ataca por colisión)

    // --- 2. ACCIONES DE ESTADO ---
    // Basado en el estado, decidimos nuestro objetivo

    Vector2 objetivo; // Hacia dónde queremos ir

    switch (estadoActual)
    {
        case EstadoIA::PATRULLANDO:
        {
            temporizadorPatrulla -= GetFrameTime();
            // Si se acabó el tiempo o llegamos cerca del destino
            if (temporizadorPatrulla <= 0.0f || Vector2Distance(posicion, destinoPatrulla) < radio * 2.0f) {
                elegirNuevoDestinoPatrulla(mapa);
            }
            objetivo = destinoPatrulla;
            break;
        }

        case EstadoIA::PERSIGUIENDO:
        {
            objetivo = posJugador;
            break;
        }

        case EstadoIA::ATACANDO:
            // No usado por el Zombie
            objetivo = posicion;
            break;
    }

    // --- 3. ACTUALIZAR DIRECCIÓN (para MotorFisica) ---
    // Finalmente, actualizamos 'this->direccion'
    Vector2 vectorHaciaObjetivo = Vector2Subtract(objetivo, this->posicion);

    // Solo actualizamos la dirección si el objetivo no está ya debajo de nosotros
    if (Vector2LengthSqr(vectorHaciaObjetivo) > 10.0f) { // Umbral pequeño
        this->direccion = Vector2Normalize(vectorHaciaObjetivo);
    }
    // Si estamos "en el objetivo", this->direccion mantiene su valor anterior,
    // y MotorFisica (al multiplicarlo por velocidad) nos dejará quietos.
}

void Zombie::dibujar() {
    DrawCircleV(this->posicion, this->radio, GREEN);

    Vector2 posCara = Vector2Add(this->posicion, Vector2Scale(this->direccion, this->radio));
    DrawRectangle(posCara.x - 3, posCara.y - 3, 6, 6, DARKGREEN);
}

// --- ¡¡LOGICA DE DAÑO AÑADIDA!! ---
void Zombie::atacar(Protagonista& jugador) {
    // Ahora el Zombie es responsable de su propio daño
    jugador.recibirDanio(this->danio);
}
