#include "Nota.h"
#include "raymath.h"
#include "Constantes.h"

Texture2D Nota::texNota = { 0 };
bool Nota::texturaCargada = false;

void Nota::CargarTextura() {
    if (!texturaCargada) {
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

    float tiempo = GetTime();
    float flotacion = sin(tiempo * 3.0f) * 4.0f;

    float alturaCaja = 40.0f;

    Vector2 posDibujo = { posicion.x, posicion.y - alturaCaja + flotacion };

    if (texNota.id != 0) {
        float targetWidth = 36.0f;
        float escala = targetWidth / (float)texNota.width;

        posDibujo.x -= (texNota.width * escala) / 2.0f;
        posDibujo.y -= (texNota.height * escala) / 2.0f;

        float alphaGlow = 0.8f + (sin(tiempo * 5.0f) * 0.2f);
        Color colorTech = ColorAlpha(SKYBLUE, alphaGlow);

        DrawCircleGradient((int)posicion.x, (int)(posicion.y - alturaCaja), 25.0f, Fade(SKYBLUE, 0.4f), Fade(SKYBLUE, 0.0f));

        DrawTextureEx(texNota, posDibujo, 0.0f, escala, colorTech);

    }
    else {
        DrawRectangle(posicion.x - 8, posicion.y - alturaCaja - 8, 16, 16, SKYBLUE);
    }
}

int Nota::usar(Protagonista& jugador) {
    this->leida = true;
    return notaID;
}

bool Nota::esInteraccionPorTecla() const { return true; }

bool Nota::estaConsumido() const {
    if (notaID == 0) return leida;
    return false;
}

Texture2D Nota::getTextura() { return texNota; }
