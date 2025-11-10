#include "MinaEnemiga.h"
#include "raymath.h"
#include "Protagonista.h"
#include "Mapa.h"

// --- ¡¡VACA FIX 4.0!! (Velocidad en px/sec) ---
static const float VELOCIDAD_MINA_INICIAL = 300.0f;
static const float RADIO_MINA_OBJETO = 10.0f;
static const float RADIO_MINA_AOE = 75.0f;
static const int   DANIO_MINA = 5;
static const float TIEMPO_MOVIMIENTO = 2.0f;
static const float TIEMPO_VIDA_MINA = 5.0f;
static const float TIEMPO_EXPLOSION = 0.3f;

MinaEnemiga::MinaEnemiga(Vector2 pos, Vector2 dir)
    : Bala(
        pos,
        dir,
        VELOCIDAD_MINA_INICIAL,
        DANIO_MINA,
        OrigenBala::ENEMIGO,
        RADIO_MINA_OBJETO
    ),
    temporizadorActivacion(TIEMPO_MOVIMIENTO),
    temporizadorVida(TIEMPO_VIDA_MINA),
    estaArmada(false),
    estaMuriendo(false),
    temporizadorExplosion(TIEMPO_EXPLOSION),
    explosionLastimaJefe(false)
{
    // --- ¡¡VACA FIX 4.0!! ---
    this->velocidadInicial = VELOCIDAD_MINA_INICIAL;
    this->direccionNormalizada = Vector2Normalize(dir);
    // (setVelocidad ya lo hace el constructor base)
    // ----------------------
}

void MinaEnemiga::actualizar(Protagonista& jugador, const Mapa& mapa)
{
    if (!activa) return;

    if (estaMuriendo)
    {
        temporizadorExplosion -= GetFrameTime();
        if (temporizadorExplosion <= 0)
        {
            desactivar();
        }
        return;
    }

    if (!estaArmada)
    {
        temporizadorActivacion -= GetFrameTime();

        // --- ¡¡VACA FIX 4.0!! (Deceleración) ---
        float ratio = temporizadorActivacion / TIEMPO_MOVIMIENTO;
        ratio = Clamp(ratio, 0.0f, 1.0f);
        // Setea la velocidad (px/sec)
        setVelocidad(Vector2Scale(this->direccionNormalizada, this->velocidadInicial * ratio));
        // ---------------------------------

        if (temporizadorActivacion <= 0)
        {
            estaArmada = true;
            setVelocidad({0, 0});
        }
    }
    else
    {
        temporizadorVida -= GetFrameTime();
        if (temporizadorVida <= 0)
        {
            explotar(false);
        }
    }
}

void MinaEnemiga::recibirDanio(int cantidad, OrigenBala origenDanio)
{
    if (origenDanio == OrigenBala::JUGADOR && estaArmada && !estaMuriendo)
    {
        explotar(true);
    }
}

void MinaEnemiga::explotar(bool porJugador)
{
    if (estaMuriendo) return;

    estaMuriendo = true;
    temporizadorExplosion = TIEMPO_EXPLOSION;
    explosionLastimaJefe = porJugador;

    setVelocidad({0, 0});
    estaArmada = false;
    danio = DANIO_MINA;
    radio = RADIO_MINA_AOE;
}

bool MinaEnemiga::estaExplotando() const
{
    return estaMuriendo;
}

bool MinaEnemiga::explosionPuedeHerirJefe() const
{
    return explosionLastimaJefe;
}


void MinaEnemiga::dibujar()
{
    if (!activa) return;

    if (estaMuriendo)
    {
        float progreso = 1.0f - (temporizadorExplosion / TIEMPO_EXPLOSION);
        float radioActual = Lerp(RADIO_MINA_OBJETO, RADIO_MINA_AOE, progreso);
        float alpha = 1.0f - progreso;
        DrawCircleV(posicion, radioActual, Fade(ORANGE, alpha));
    }
    else if (!estaArmada)
    {
        DrawCircleV(posicion, radio, RED);
    }
    else
    {
        float alpha = (sin(GetTime() * 10.0f) + 1.0f) / 2.0f;
        DrawCircleV(posicion, radio, Fade(ORANGE, alpha));
    }
}
