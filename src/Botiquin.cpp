#include "Botiquin.h"
#include "raymath.h"

Texture2D Botiquin::texBotiquin = { 0 };
bool Botiquin::texturaCargada = false;

void Botiquin::CargarTextura() {
    if (!texturaCargada) {
        texBotiquin = LoadTexture("assets/Consumibles/Botiquin.png");
        texturaCargada = true;
    }
}

void Botiquin::DescargarTextura() {
    if (texturaCargada) {
        UnloadTexture(texBotiquin);
        texturaCargada = false;
    }
}

Botiquin::Botiquin(Vector2 pos) : Consumible(pos) {
    if (!texturaCargada) CargarTextura();
    nombreItem = "BOTIQUIN MEDICO";
    descripcionItem = "Restaura la salud por completo.";
}

Botiquin::~Botiquin() {}

int Botiquin::usar(Protagonista& jugador) {
    if (jugador.getVida() < 10) {
        jugador.curarVida(100);
        consumido = true;
        return 1;
    }
    return 0;
}

void Botiquin::dibujar() {
    if (consumido) return;

    if (texBotiquin.id != 0) {
        // --- ESCALADO INTELIGENTE ---
        float tamanoDeseado = 35.0f; // Un poco mas grande que la llave
        float escala = tamanoDeseado / (float)texBotiquin.width;

        Vector2 posDibujo = { posicion.x - (texBotiquin.width * escala) / 2, posicion.y - (texBotiquin.height * escala) / 2 };
        DrawTextureEx(texBotiquin, posDibujo, 0.0f, escala, WHITE);
    } else {
        DrawRectangle(posicion.x-5, posicion.y-15, 10, 30, RED);
        DrawRectangle(posicion.x-15, posicion.y-5, 30, 10, RED);
    }
}

Texture2D Botiquin::getTextura() { return texBotiquin; }
