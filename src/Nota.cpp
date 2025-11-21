#include "Nota.h"
#include "raymath.h"
#include "Constantes.h"

// Inicialización estática
Texture2D Nota::texNota = { 0 };
bool Nota::texturaCargada = false;

void Nota::CargarTextura() {
    if (!texturaCargada) {
        // Asegúrate de que la imagen esté en esta ruta
        texNota = LoadTexture("assets/Nota.png");
        texturaCargada = true;
    }
}

void Nota::DescargarTextura() {
    if (texturaCargada) {
        UnloadTexture(texNota);
        texturaCargada = false;
    }
}

Nota::Nota(Vector2 pos, int id)
    : Consumible(pos), notaID(id), leida(false)
{
    if (!texturaCargada) CargarTextura();
    nombreItem = "REGISTRO DE DATOS";
    descripcionItem = "Informacion encriptada del personal.";
}

Nota::~Nota() {}

void Nota::dibujar() {
    if (estaConsumido()) return;

    // --- LÓGICA VISUAL MEJORADA ---

    float tiempo = GetTime();
    // Flotación un poco más amplia (4px)
    float flotacion = sin(tiempo * 3.0f) * 4.0f;

    // Altura sobre la caja
    float alturaCaja = 40.0f;

    Vector2 posDibujo = { posicion.x, posicion.y - alturaCaja + flotacion };

    if (texNota.id != 0) {
        // --- CAMBIO 1: AUMENTAR TAMAÑO ---
        // Antes era 24.0f, ahora 36.0f para que sea más grande y visible.
        float targetWidth = 36.0f;
        float escala = targetWidth / (float)texNota.width;

        // Centramos el dibujo
        posDibujo.x -= (texNota.width * escala) / 2.0f;
        posDibujo.y -= (texNota.height * escala) / 2.0f;

        // --- CAMBIO 2: POTENCIAR EL BRILLO ---
        // Pulsación más intensa (entre 0.6 y 1.0 de alpha)
        float alphaGlow = 0.8f + (sin(tiempo * 5.0f) * 0.2f);
        // Usamos SKYBLUE en lugar de WHITE para un tinte tecnológico más visible
        Color colorTech = ColorAlpha(SKYBLUE, alphaGlow);

        // --- CAMBIO 3: AUMENTAR EL HALO ---
        // Halo más grande (radio 25) y más brillante en el centro
        DrawCircleGradient((int)posicion.x, (int)(posicion.y - alturaCaja), 25.0f, Fade(SKYBLUE, 0.4f), Fade(SKYBLUE, 0.0f));

        DrawTextureEx(texNota, posDibujo, 0.0f, escala, colorTech);

    }
    else {
        // Fallback más visible por si falla la textura
        DrawRectangle(posicion.x - 8, posicion.y - alturaCaja - 8, 16, 16, SKYBLUE);
    }
}

int Nota::usar(Protagonista& jugador) {
    this->leida = true;
    return notaID;
}

bool Nota::esInteraccionPorTecla() const { return true; }

bool Nota::estaConsumido() const {
    // Si quieres que desaparezcan al leerlas, descomenta la siguiente línea:
    // return leida;

    // Por ahora, solo la del tutorial (ID 0) desaparece.
    if (notaID == 0) return leida;
    return false;
}

Texture2D Nota::getTextura() { return texNota; }
