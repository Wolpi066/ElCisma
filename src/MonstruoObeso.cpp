#include "MonstruoObeso.h"
#include "Constantes.h"
#include "raymath.h"
#include "Protagonista.h" // ¡¡Importante!!
#include "Mapa.h"         // ¡¡AÑADIDO!!

// --- CORREGIDO: Constructor llama a 8 args ---
MonstruoObeso::MonstruoObeso(Vector2 pos)
    : Enemigo(pos,
              Constantes::VIDA_OBESO,
              Constantes::DANIO_OBESO,
              Constantes::VELOCIDAD_OBESO,
              Constantes::RADIO_OBESO,
              Constantes::RANGO_VISUAL_ZOMBIE * 0.8f, // Ve un poco menos
              Constantes::ANGULO_CONO_ZOMBIE * 0.8f,  // Cono mas estrecho
              Constantes::RANGO_AUDIO_ZOMBIE * 1.2f)  // Escucha un poco mas
{
}

// --- CORREGIDO: Nombre de la funcion ---
// --- ¡¡LÓGICA DE IA COMPLETAMENTE REEMPLAZADA POR FSM!! ---
void MonstruoObeso::actualizarIA(Vector2 posJugador, const Mapa& mapa) {

    // --- 1. TRANSICIONES DE ESTADO ---
    bool jugadorDetectado = puedeVearAlJugador(posJugador) || puedeEscucharAlJugador(posJugador);

    if (estadoActual == EstadoIA::PATRULLANDO) {
        if (jugadorDetectado) {
            estadoActual = EstadoIA::PERSIGUIENDO;
        }
    }
    else if (estadoActual == EstadoIA::PERSIGUIENDO) {
        if (!jugadorDetectado) {
            float distancia = Vector2Distance(posicion, posJugador);
            // Lo perdemos si está 1.5x su rango de visión
            if (distancia > rangoVision * 1.5f) {
                estadoActual = EstadoIA::PATRULLANDO;
                temporizadorPatrulla = 0.0f;
            }
        }
        // TODO: Si el Obeso disparara, aquí iría la transición a ATACANDO
        // if (jugadorDetectado && distancia < RANGO_ATAQUE_OBESO) {
        //     estadoActual = EstadoIA::ATACANDO;
        // }
    }
    // else if (estadoActual == EstadoIA::ATACANDO) {
        // ...lógica de ataque...
    // }

    // --- 2. ACCIONES DE ESTADO ---
    Vector2 objetivo;

    switch (estadoActual)
    {
        case EstadoIA::PATRULLANDO:
        {
            temporizadorPatrulla -= GetFrameTime();
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
        {
            // TODO: Si disparase, aquí se quedaría quieto
            // objetivo = posicion;

            // Como ataca por colisión (por ahora), simplemente persigue
            objetivo = posJugador;
            break;
        }
    }

    // --- 3. ACTUALIZAR DIRECCIÓN (para MotorFisica) ---
    Vector2 vectorHaciaObjetivo = Vector2Subtract(objetivo, this->posicion);

    if (Vector2LengthSqr(vectorHaciaObjetivo) > 10.0f) {
        this->direccion = Vector2Normalize(vectorHaciaObjetivo);
    }
}

void MonstruoObeso::dibujar() {
    DrawCircleV(this->posicion, this->radio, DARKGREEN);

    Vector2 posCara = Vector2Add(this->posicion, Vector2Scale(this->direccion, this->radio));
    DrawRectangle(posCara.x - 5, posCara.y - 5, 10, 10, GREEN);
}

// --- ¡¡LOGICA DE DAÑO AÑADIDA!! ---
void MonstruoObeso::atacar(Protagonista& jugador) {
    // Ahora el MonstruoObeso es responsable de su propio daño
    jugador.recibirDanio(this->danio);
}
