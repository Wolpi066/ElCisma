#include "MinaEnemiga.h"
#include "raymath.h"
#include "Protagonista.h"
#include "Mapa.h"

Texture2D MinaEnemiga::texMinaMov = { 0 };
Texture2D MinaEnemiga::texMinaArmada = { 0 };
Sound MinaEnemiga::fxMinaBip = { 0 };

static const float VELOCIDAD_MINA_INICIAL = 300.0f;
static const float RADIO_MINA_OBJETO = 10.0f;
static const float RADIO_MINA_AOE = 75.0f;
static const int   DANIO_MINA = 5;
static const float TIEMPO_MOVIMIENTO = 2.0f;
static const float TIEMPO_VIDA_MINA = 5.0f;
static const float TIEMPO_EXPLOSION = 0.3f;

Sound LoadSoundSafeMina(const char* path) {
    if (FileExists(path)) return LoadSound(path);
    return (Sound){0};
}

void MinaEnemiga::CargarRecursos() {
    if (texMinaMov.id == 0) texMinaMov = LoadTexture("assets/Jefe/Proyectiles/Mina1.png");
    if (texMinaArmada.id == 0) texMinaArmada = LoadTexture("assets/Jefe/Proyectiles/Mina2.png");
    if (fxMinaBip.stream.buffer == 0) fxMinaBip = LoadSoundSafeMina("assets/Audio/Sonidos/Jefe/Fase2/MinaActiva.wav");
}

void MinaEnemiga::DescargarRecursos() {
    if (texMinaMov.id != 0) { UnloadTexture(texMinaMov); texMinaMov.id = 0; }
    if (texMinaArmada.id != 0) { UnloadTexture(texMinaArmada); texMinaArmada.id = 0; }
    if (fxMinaBip.stream.buffer != 0) { UnloadSound(fxMinaBip); fxMinaBip = (Sound){0}; }
}

MinaEnemiga::MinaEnemiga(Vector2 pos, Vector2 dir)
    : Bala(pos, dir, VELOCIDAD_MINA_INICIAL, DANIO_MINA, OrigenBala::ENEMIGO, RADIO_MINA_OBJETO),
    temporizadorActivacion(TIEMPO_MOVIMIENTO), temporizadorVida(TIEMPO_VIDA_MINA),
    estaArmada(false), estaMuriendo(false), temporizadorExplosion(TIEMPO_EXPLOSION),
    explosionLastimaJefe(false), timerBip(0.0f)
{
    this->velocidadInicial = VELOCIDAD_MINA_INICIAL;
    this->direccionNormalizada = Vector2Normalize(dir);
}

void MinaEnemiga::actualizar(Protagonista& jugador, const Mapa& mapa)
{
    if (!activa) return;

    if (estaMuriendo) {
        temporizadorExplosion -= GetFrameTime();
        if (temporizadorExplosion <= 0) desactivar();
        return;
    }

    if (!estaArmada) {
        temporizadorActivacion -= GetFrameTime();
        float ratio = temporizadorActivacion / TIEMPO_MOVIMIENTO;
        ratio = Clamp(ratio, 0.0f, 1.0f);
        setVelocidad(Vector2Scale(this->direccionNormalizada, this->velocidadInicial * ratio));

        if (temporizadorActivacion <= 0) {
            estaArmada = true;
            setVelocidad({0, 0});
        }
    } else {
        temporizadorVida -= GetFrameTime();

        timerBip += GetFrameTime();
        if (timerBip >= 0.5f) {
            PlaySound(fxMinaBip);
            timerBip = 0.0f;
        }

        if (temporizadorVida <= 0) explotar(false);
    }
}

void MinaEnemiga::recibirDanio(int cantidad, OrigenBala origenDanio) {
    if (origenDanio == OrigenBala::JUGADOR && estaArmada && !estaMuriendo) {
        explotar(true);
    }
}

void MinaEnemiga::explotar(bool porJugador) {
    if (estaMuriendo) return;
    estaMuriendo = true;
    temporizadorExplosion = TIEMPO_EXPLOSION;
    explosionLastimaJefe = porJugador;
    setVelocidad({0, 0});
    estaArmada = false;
    danio = DANIO_MINA;
    radio = RADIO_MINA_AOE;
}

bool MinaEnemiga::estaExplotando() const { return estaMuriendo; }
bool MinaEnemiga::explosionPuedeHerirJefe() const { return explosionLastimaJefe; }

void MinaEnemiga::dibujar() {
    if (!activa) return;

    if (estaMuriendo) {
        float progreso = 1.0f - (temporizadorExplosion / TIEMPO_EXPLOSION);
        float radioActual = Lerp(RADIO_MINA_OBJETO, RADIO_MINA_AOE, progreso);
        DrawCircleV(posicion, radioActual, Fade(ORANGE, 1.0f - progreso));
    }
    else if (!estaArmada) {
        if (texMinaMov.id != 0) {
            float rotacion = GetTime() * 200.0f;
            float escala = (RADIO_MINA_OBJETO * 3.0f) / (float)texMinaMov.width;
            Rectangle src = {0,0,(float)texMinaMov.width, (float)texMinaMov.height};
            Rectangle dest = {posicion.x, posicion.y, (float)texMinaMov.width*escala, (float)texMinaMov.height*escala};
            DrawTexturePro(texMinaMov, src, dest, {dest.width/2, dest.height/2}, rotacion, WHITE);
        } else {
            DrawCircleV(posicion, radio, RED);
        }
    }
    else {
        if (texMinaArmada.id != 0) {
            float escala = (RADIO_MINA_OBJETO * 3.0f) / (float)texMinaArmada.width;
            Color tinte = WHITE;
            if ((int)(GetTime() * 10.0f) % 2 == 0) tinte = RED;
            Rectangle src = {0,0,(float)texMinaArmada.width, (float)texMinaArmada.height};
            Rectangle dest = {posicion.x, posicion.y, (float)texMinaArmada.width*escala, (float)texMinaArmada.height*escala};
            DrawTexturePro(texMinaArmada, src, dest, {dest.width/2, dest.height/2}, 0.0f, tinte);
        } else {
            DrawCircleV(posicion, radio, Fade(ORANGE, 0.5f));
        }
    }
}
