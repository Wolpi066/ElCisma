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
    // --- ¡¡VALORES MODIFICADOS!! ---
    this->rangoAtaque = 40.0f;
    this->rangoDmg = 60.0f;  // Alcance aumentado (antes 50.0f)
}

// --- ¡¡LÓGICA DE IA CON FSM DE 3 ESTADOS!! ---
void Zombie::actualizarIA(Vector2 posJugador, const Mapa& mapa) {

    // --- 0. ACTUALIZAR TIMERS ---
    if (temporizadorAtaque > 0.0f) {
        temporizadorAtaque -= GetFrameTime();
    }
    if (temporizadorPausaAtaque > 0.0f) {
        temporizadorPausaAtaque -= GetFrameTime();
    }

    // --- 1. TRANSICIONES DE ESTADO ---
    bool jugadorDetectado = puedeVearAlJugador(posJugador) || puedeEscucharAlJugador(posJugador);
    float distancia = Vector2Distance(posicion, posJugador);

    switch (estadoActual)
    {
        case EstadoIA::PATRULLANDO:
        {
            if (jugadorDetectado) {
                estadoActual = EstadoIA::PERSIGUIENDO;
            }
            break;
        }
        case EstadoIA::PERSIGUIENDO:
        {
            // Transicion a ATACAR
            if (jugadorDetectado && distancia <= this->rangoAtaque && temporizadorAtaque <= 0.0f) {
                estadoActual = EstadoIA::ATACANDO;
                // --- ¡¡VALOR MODIFICADO!! ---
                temporizadorPausaAtaque = 0.2f; // Pausa unificada y rapida (Antes 0.3f)
                this->direccion = {0, 0}; // ¡Se frena!
            }
            // Transicion a PATRULLAR
            else if (!jugadorDetectado && distancia > rangoVision * 1.5f) {
                estadoActual = EstadoIA::PATRULLANDO;
                temporizadorPatrulla = 0.0f;
            }
            break;
        }
        case EstadoIA::ATACANDO:
        {
            // La transicion de salida (ATACANDO -> PERSIGUIENDO)
            // ocurre en el metodo atacar()
            break;
        }
    }

    // --- 2. ACCIONES DE ESTADO ---
    Vector2 objetivo = posicion; // Por defecto, quedarse quieto

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
            // Se queda quieto
            break;
        }
    }

    // --- 3. ACTUALIZAR DIRECCIÓN (para MotorFisica) ---
    Vector2 vectorHaciaObjetivo = Vector2Subtract(objetivo, this->posicion);

    if (estadoActual == EstadoIA::ATACANDO) {
        this->direccion = {0, 0}; // Forzar freno
    } else if (Vector2LengthSqr(vectorHaciaObjetivo) > 10.0f) {
        this->direccion = Vector2Normalize(vectorHaciaObjetivo);
    }
}

void Zombie::dibujar() {
    Color color = GREEN;
    if (getEstadoIA() == EstadoIA::ATACANDO) {
        // Parpadeo rojo brillante durante la pausa
        color = (Color){ 255, 0, 0, (unsigned char)(fabs(sin(GetTime() * 20.0f)) * 255) };
    }
    DrawCircleV(this->posicion, this->radio, color);

    Vector2 posCara = Vector2Add(this->posicion, Vector2Scale(this->direccion, this->radio));
    DrawRectangle(posCara.x - 3, posCara.y - 3, 6, 6, DARKGREEN);
}

// --- ¡¡LÓGICA DE DAÑO "LUNGE"!! ---
void Zombie::atacar(Protagonista& jugador) {
    // Esta funcion es llamada por MotorColisiones cuando
    // estaListoParaAtacar() devuelve true.

    float distancia = Vector2Distance(this->posicion, jugador.getPosicion());

    // Si el jugador esta dentro del rango de golpeo
    if (distancia <= this->rangoDmg)
    {
        float tiempoInmuneAntes = jugador.getTiempoInmune();

        jugador.recibirDanio(this->danio); // ¡Golpe!

        float tiempoInmuneDespues = jugador.getTiempoInmune();

        // Si el golpe fue exitoso (no estaba inmune), aplicar knockback
        if (tiempoInmuneAntes <= 0.0f && tiempoInmuneDespues > 0.0f)
        {
            Vector2 dirKnockback = Vector2Normalize(Vector2Subtract(jugador.getPosicion(), this->posicion));
            if (Vector2LengthSqr(dirKnockback) == 0.0f) {
                dirKnockback = {1.0f, 0.0f};
            }
            // Valores ajustados (lento y fluido)
            float fuerza = 3.0f;
            float duracion = 0.2f;
            jugador.aplicarKnockback(dirKnockback, fuerza, duracion);
        }
    }

    // --- IMPORTANTE: Resetear la IA ---
    this->temporizadorAtaque = 2.0f; // Cooldown de 2s
    this->estadoActual = EstadoIA::PERSIGUIENDO;
}
