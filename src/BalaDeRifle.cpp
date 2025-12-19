#include "BalaDeRifle.h"
#include "Constantes.h"
#include "raymath.h"

Texture2D BalaDeRifle::texBalaRifle = { 0 };
bool BalaDeRifle::texturaCargada = false;

static const float VELOCIDAD_BALA_RIFLE = 1200.0f;
static const int   DANIO_BALA_RIFLE     = 1;
static const float RADIO_BALA_RIFLE     = 4.0f;

BalaDeRifle::BalaDeRifle(Vector2 pos, Vector2 dir, bool esCheat)
    : Bala(
        pos,
        dir,
        VELOCIDAD_BALA_RIFLE,
        DANIO_BALA_RIFLE,
        OrigenBala::JUGADOR,
        RADIO_BALA_RIFLE,
        esCheat
    )
{
    if (!texturaCargada) {
        CargarTextura();
    }
}

BalaDeRifle::~BalaDeRifle() {
}

void BalaDeRifle::CargarTextura() {
    if (!texturaCargada) {
        texBalaRifle = LoadTexture("assets/Protagonista/Bala.png");
        texturaCargada = true;
    }
}

void BalaDeRifle::DescargarTextura() {
    if (texturaCargada) {
        UnloadTexture(texBalaRifle);
        texturaCargada = false;
    }
}

void BalaDeRifle::actualizar(Protagonista& jugador, const Mapa& mapa)
{
    if (activa) {
        float dt = GetFrameTime();
        posicion.x += velocidad.x * dt;
        posicion.y += velocidad.y * dt;
    }
}

void BalaDeRifle::dibujar()
{
    if (!activa) return;

    if (texturaCargada) {
        float rotacion = atan2f(velocidad.y, velocidad.x) * RAD2DEG;

        Rectangle sourceRec = { 0.0f, 0.0f, (float)texBalaRifle.width, (float)texBalaRifle.height };

        float ancho = 55.0f;
        float alto = 7.0f;
        // ------------------------

        Rectangle destRec = { posicion.x, posicion.y, ancho, alto };
        Vector2 origen = { ancho / 2.0f, alto / 2.0f };

        DrawTexturePro(texBalaRifle, sourceRec, destRec, origen, rotacion, WHITE);
    } else {
        Color c = esCheat ? RED : YELLOW;
        DrawCircleV(posicion, radio, c);
    }
}
