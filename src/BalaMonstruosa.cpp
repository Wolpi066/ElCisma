#include "BalaMonstruosa.h"

// Inicialización de estáticos
Texture2D BalaMonstruosa::texBala = { 0 };

// Configuración
static const float VELOCIDAD_BALA_JEFE = 420.0f;
static const int DANIO_BALA_JEFE = 5;

void BalaMonstruosa::CargarRecursos() {
    if (texBala.id == 0) {
        texBala = LoadTexture("assets/Jefe/Proyectiles/BalaMonstruosa.png");
    }
}

void BalaMonstruosa::DescargarRecursos() {
    if (texBala.id != 0) {
        UnloadTexture(texBala);
        texBala.id = 0;
    }
}

BalaMonstruosa::BalaMonstruosa(Vector2 pos, Vector2 dir)
    : Bala(
        pos,
        dir,
        VELOCIDAD_BALA_JEFE,
        DANIO_BALA_JEFE,
        OrigenBala::ENEMIGO,
        6.0f
    )
{
}

void BalaMonstruosa::dibujar() {
    if (estaActiva()) {
        if (texBala.id != 0) {
            // Rotación continua para efecto visual
            float rotacion = GetTime() * 360.0f;

            // Escala: Ajustamos la imagen para que cubra un poco más que la hitbox (6.0f * 2 = 12px diametro)
            // Asumimos que el sprite es aprox 32x32 o similar, lo escalamos dinámicamente
            float escala = (radio * 2.5f) / (float)texBala.width;

            Rectangle source = { 0, 0, (float)texBala.width, (float)texBala.height };
            Rectangle dest = { posicion.x, posicion.y, (float)texBala.width * escala, (float)texBala.height * escala };
            Vector2 origin = { dest.width / 2, dest.height / 2 };

            DrawTexturePro(texBala, source, dest, origin, rotacion, WHITE);
        } else {
            // Fallback
            DrawCircleV(getPosicion(), 6.0f, RED);
        }
    }
}
