#include "Llave.h"
#include "raymath.h"

Texture2D Llave::texLlave = { 0 };
bool Llave::texturaCargada = false;

void Llave::CargarTextura() {
    if (!texturaCargada) {
        texLlave = LoadTexture("assets/Consumibles/TarjetaDeAcceso.png");
        texturaCargada = true;
    }
}

void Llave::DescargarTextura() {
    if (texturaCargada) {
        UnloadTexture(texLlave);
        texturaCargada = false;
    }
}

Llave::Llave(Vector2 pos) : Consumible(pos) {
    if (!texturaCargada) CargarTextura();
    nombreItem = "TARJETA DE ACCESO";
    descripcionItem = "Credencial de Nivel 5. Desbloquea la sala central.";
}

Llave::~Llave() {}

int Llave::usar(Protagonista& jugador) {
    jugador.recibirLlave();
    consumido = true;
    return 0;
}

void Llave::dibujar() {
    if (consumido) return;

    if (texLlave.id != 0) {
        // --- ESCALADO INTELIGENTE ---
        float tamanoDeseado = 30.0f; // Tamaño en pixeles en el mundo
        float escala = tamanoDeseado / (float)texLlave.width; // Calculamos escala para que mida 30px

        Vector2 posDibujo = { posicion.x - (texLlave.width * escala) / 2, posicion.y - (texLlave.height * escala) / 2 };

        float flotacion = sin(GetTime() * 3.0f) * 3.0f;
        posDibujo.y += flotacion;

        DrawTextureEx(texLlave, posDibujo, 0.0f, escala, WHITE);
        DrawCircleV(posicion, 5.0f + sin(GetTime()*5)*2.0f, Fade(SKYBLUE, 0.4f));
    } else {
        DrawRectangle(posicion.x-10, posicion.y-10, 20, 20, YELLOW);
    }
}

Texture2D Llave::getTextura() { return texLlave; }
