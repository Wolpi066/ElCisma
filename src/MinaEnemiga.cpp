#include "MinaEnemiga.h"
#include "raymath.h"
#include "Protagonista.h" // <-- ¡¡FIX!! Incluir .h completo
#include "Mapa.h"         // <-- ¡¡FIX!! Incluir .h completo

// --- Constantes de la Mina ---
static const float VELOCIDAD_MINA = 150.0f;
static const float RADIO_MINA = 8.0f;
static const int   DANIO_MINA = 10;
static const float TIEMPO_MOVIMIENTO = 0.75f;
static const float TIEMPO_VIDA_MINA = 5.0f;

MinaEnemiga::MinaEnemiga(Vector2 pos, Vector2 dir)
    : Bala(
        pos,
        dir,
        VELOCIDAD_MINA,
        DANIO_MINA,
        OrigenBala::ENEMIGO,
        RADIO_MINA
    ),
    temporizadorActivacion(TIEMPO_MOVIMIENTO),
    temporizadorVida(TIEMPO_VIDA_MINA),
    estaArmada(false)
{
}

// --- ¡¡FIX!! Corregida la firma de la función (añadido 'void MinaEnemiga::') ---
void MinaEnemiga::actualizar(Protagonista& jugador, const Mapa& mapa)
{
    if (!activa) return;

    // Fase 1: Moviéndose a su posición
    if (!estaArmada)
    {
        temporizadorActivacion -= GetFrameTime();
        if (temporizadorActivacion <= 0)
        {
            estaArmada = true;
            setVelocidad({0, 0}); // ¡Se detiene!
        }
    }
    // Fase 2: Armada y en el suelo
    else
    {
        temporizadorVida -= GetFrameTime();
        if (temporizadorVida <= 0)
        {
            desactivar(); // "Explota" (desaparece)
        }
    }
}

void MinaEnemiga::dibujar()
{
    if (!activa) return;

    if (!estaArmada)
    {
        DrawCircleV(posicion, radio, RED);
    }
    else
    {
        float alpha = (sin(GetTime() * 10.0f) + 1.0f) / 2.0f;
        DrawCircleV(posicion, radio, Fade(ORANGE, alpha));
    }
}
