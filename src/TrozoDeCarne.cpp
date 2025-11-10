#include "TrozoDeCarne.h"
#include "raylib.h"
#include "raymath.h"
#include "Protagonista.h"
#include "Mapa.h"

// --- Constantes del Ataque ---
const float TrozoDeCarne::TIEMPO_VUELO = 0.7f;
const float TrozoDeCarne::TIEMPO_EN_SUELO = 8.0f;
const float TrozoDeCarne::RADIO_CHARCO = 30.0f;

TrozoDeCarne::TrozoDeCarne(Vector2 pos, Vector2 objetivo)
    : Bala(
        pos,
        Vector2Subtract(objetivo, pos),
        0.0f, // No se mueve por MotorFisica
        5,    // Daño al impacto
        OrigenBala::ENEMIGO,
        RADIO_CHARCO,
        false
    ),
    inicioSalto(pos),
    objetivoSuelo(objetivo),
    progresoSalto(0.0f),
    temporizadorSuelo(TIEMPO_EN_SUELO),
    estaEnSuelo(false)
{
    setVelocidad({0, 0});
}

void TrozoDeCarne::actualizar(Protagonista& jugador, const Mapa& mapa)
{
    if (!activa) return;

    if (!estaEnSuelo)
    {
        // --- Fase de Vuelo ---
        progresoSalto += GetFrameTime() / TIEMPO_VUELO;
        progresoSalto = Clamp(progresoSalto, 0.0f, 1.0f);

        posicion = Vector2Lerp(inicioSalto, objetivoSuelo, progresoSalto);

        if (progresoSalto >= 1.0f)
        {
            estaEnSuelo = true;
            posicion = objetivoSuelo;
            danio = 0;
            radio = RADIO_CHARCO;
        }
    }
    else
    {
        // --- Fase de Charco ---
        temporizadorSuelo -= GetFrameTime();
        if (temporizadorSuelo <= 0)
        {
            desactivar();
        }
    }
}

void TrozoDeCarne::dibujar()
{
    if (!activa) return;

    if (!estaEnSuelo)
    {
        float t = progresoSalto;
        float escalaSalto;

        if (t < 0.5f) {
            escalaSalto = Lerp(0.5f, 1.5f, t / 0.5f);
        } else {
            escalaSalto = Lerp(1.5f, 1.0f, (t - 0.5f) / 0.5f);
        }

        float radioDibujo = radio * escalaSalto;

        DrawCircleV(objetivoSuelo, radioDibujo, Fade(BLACK, 0.4f));
        DrawCircleV(posicion, radioDibujo, DARKBROWN);
    }
    else
    {
        float alpha = 1.0f;
        if (temporizadorSuelo < 2.0f) {
            alpha = (sin(temporizadorSuelo * 10.0f) + 1.0f) / 2.0f;
        }
        DrawCircleV(posicion, radio, Fade(BROWN, 0.7f * alpha));
    }
}

bool TrozoDeCarne::esCharco() const
{
    return estaEnSuelo;
}
