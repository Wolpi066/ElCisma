#include "BalaInfernal.h"

Texture2D BalaInfernal::texBalaInfernal = { 0 };

// Configuración Tarea 1
static const float VELOCIDAD_BALA_JEFE = 240.0f;
static const int DANIO_BALA_JEFE = 5;
static const float RADIO_BALA_INFERNAL = 9.0f;

void BalaInfernal::CargarRecursos() {
    if (texBalaInfernal.id == 0) {
        texBalaInfernal = LoadTexture("assets/Jefe/Proyectiles/BalaInfernal.png");
    }
}

void BalaInfernal::DescargarRecursos() {
    if (texBalaInfernal.id != 0) {
        UnloadTexture(texBalaInfernal);
        texBalaInfernal.id = 0;
    }
}

BalaInfernal::BalaInfernal(Vector2 pos, Vector2 dir)
    : Bala(
        pos,
        dir,
        VELOCIDAD_BALA_JEFE,
        DANIO_BALA_JEFE,
        OrigenBala::ENEMIGO,
        RADIO_BALA_INFERNAL
    )
{
}

void BalaInfernal::dibujar() {
    if (estaActiva()) {
        if (texBalaInfernal.id != 0) {
            // Rotación rápida e inversa para distinguir del disparo normal
            float rotacion = -GetTime() * 500.0f;

            // Escala un poco mayor para que se vea peligroso
            float escala = (radio * 2.8f) / (float)texBalaInfernal.width;

            Rectangle source = { 0, 0, (float)texBalaInfernal.width, (float)texBalaInfernal.height };
            Rectangle dest = { posicion.x, posicion.y, (float)texBalaInfernal.width * escala, (float)texBalaInfernal.height * escala };
            Vector2 origin = { dest.width / 2, dest.height / 2 };

            // Usamos PURPLE como tinte base si la imagen es blanca, o WHITE si ya tiene color
            DrawTexturePro(texBalaInfernal, source, dest, origin, rotacion, WHITE);
        } else {
            DrawCircleV(getPosicion(), RADIO_BALA_INFERNAL, PURPLE);
        }
    }
}
