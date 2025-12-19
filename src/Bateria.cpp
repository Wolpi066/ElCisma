#include "Bateria.h"
#include "raymath.h"

Texture2D Bateria::texBateria = { 0 };
bool Bateria::texturaCargada = false;

void Bateria::CargarTextura() {
    if (!texturaCargada) {
        texBateria = LoadTexture("assets/Consumibles/Bateria.png");
        texturaCargada = true;
    }
}

void Bateria::DescargarTextura() {
    if (texturaCargada) {
        UnloadTexture(texBateria);
        texturaCargada = false;
    }
}

Bateria::Bateria(Vector2 pos) : Consumible(pos) {
    if (!texturaCargada) CargarTextura();
    nombreItem = "BATERIA LITIO";
    descripcionItem = "Recarga la linterna al 100%.";
}

Bateria::~Bateria() {}

int Bateria::usar(Protagonista& jugador) {
    jugador.recargarBateria(100);
    consumido = true;
    return 0;
}

void Bateria::dibujar() {
    if (consumido) return;

    if (texBateria.id != 0) {
        float tamanoDeseado = 20.0f;
        float maxDim = (float)((texBateria.width > texBateria.height) ? texBateria.width : texBateria.height);
        float escala = tamanoDeseado / maxDim;

        Vector2 posDibujo = { posicion.x - (texBateria.width * escala) / 2, posicion.y - (texBateria.height * escala) / 2 };
        DrawTextureEx(texBateria, posDibujo, 0.0f, escala, WHITE);

        if(GetRandomValue(0,10)>8) DrawCircleV(posicion, 3, YELLOW);
    } else {
        DrawRectangle(posicion.x-5, posicion.y-10, 10, 20, YELLOW);
    }
}

Texture2D Bateria::getTextura() { return texBateria; }
