#include "Armadura.h"
#include "raymath.h"

Texture2D Armadura::texArmadura = { 0 };
bool Armadura::texturaCargada = false;

void Armadura::CargarTextura() {
    if (!texturaCargada) {
        texArmadura = LoadTexture("assets/Consumibles/Armadura.png");
        texturaCargada = true;
    }
}

void Armadura::DescargarTextura() {
    if (texturaCargada) {
        UnloadTexture(texArmadura);
        texturaCargada = false;
    }
}

Armadura::Armadura(Vector2 pos) : Consumible(pos) {
    if (!texturaCargada) CargarTextura();
    nombreItem = "CHALECO KEVLAR";
    descripcionItem = "Absorbe el daño del proximo golpe.";
}

Armadura::~Armadura() {}

int Armadura::usar(Protagonista& jugador) {
    jugador.recibirArmadura();
    consumido = true;
    return 0;
}

void Armadura::dibujar() {
    if (consumido) return;

    if (texArmadura.id != 0) {
        float tamanoDeseado = 32.0f;
        float escala = tamanoDeseado / (float)texArmadura.width;

        Vector2 posDibujo = { posicion.x - (texArmadura.width * escala) / 2, posicion.y - (texArmadura.height * escala) / 2 };
        DrawTextureEx(texArmadura, posDibujo, 0.0f, escala, WHITE);
    } else {
        DrawRectangle(posicion.x-10, posicion.y-10, 20, 20, BLUE);
    }
}

Texture2D Armadura::getTextura() { return texArmadura; }
