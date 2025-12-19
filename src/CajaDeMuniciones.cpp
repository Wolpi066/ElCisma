#include "CajaDeMuniciones.h"
#include "raymath.h"

Texture2D CajaDeMuniciones::texMunicion = { 0 };
bool CajaDeMuniciones::texturaCargada = false;

void CajaDeMuniciones::CargarTextura() {
    if (!texturaCargada) {
        texMunicion = LoadTexture("assets/Consumibles/CajaDeMunicion.png");
        texturaCargada = true;
    }
}

void CajaDeMuniciones::DescargarTextura() {
    if (texturaCargada) {
        UnloadTexture(texMunicion);
        texturaCargada = false;
    }
}

CajaDeMuniciones::CajaDeMuniciones(Vector2 pos) : Consumible(pos) {
    if (!texturaCargada) CargarTextura();
    nombreItem = "CAJA DE MUNICION";
    descripcionItem = "Contiene balas de rifle de asalto.";
}

CajaDeMuniciones::~CajaDeMuniciones() {}

int CajaDeMuniciones::usar(Protagonista& jugador) {
    jugador.recargarMunicion(20);
    consumido = true;
    return 0;
}

void CajaDeMuniciones::dibujar() {
    if (consumido) return;

    if (texMunicion.id != 0) {
        float tamanoDeseado = 30.0f;
        float maxDim = (float)((texMunicion.width > texMunicion.height) ? texMunicion.width : texMunicion.height);
        float escala = tamanoDeseado / maxDim;

        Vector2 posDibujo = { posicion.x - (texMunicion.width * escala) / 2, posicion.y - (texMunicion.height * escala) / 2 };
        DrawTextureEx(texMunicion, posDibujo, 0.0f, escala, WHITE);
    } else {
        DrawRectangle(posicion.x-10, posicion.y-10, 20, 20, DARKGREEN);
    }
}

Texture2D CajaDeMuniciones::getTextura() { return texMunicion; }
