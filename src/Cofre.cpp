#include "Cofre.h"
#include "raylib.h"

// Inicialización estática
Texture2D Cofre::texCerrado = { 0 };
Texture2D Cofre::texAbierto = { 0 };
bool Cofre::texturasCargadas = false;

void Cofre::CargarTexturas() {
    if (!texturasCargadas) {
        texCerrado = LoadTexture("assets/CofreCerrado.png");
        texAbierto = LoadTexture("assets/CofreAbierto.png");
        texturasCargadas = true;
    }
}

void Cofre::DescargarTexturas() {
    if (texturasCargadas) {
        UnloadTexture(texCerrado);
        UnloadTexture(texAbierto);
        texturasCargadas = false;
    }
}

Cofre::Cofre(Vector2 pos, int lootID, CofreOrientacion orient)
    : Consumible(pos), tipoDeLoot(lootID), orientacion(orient), abierto(false)
{
    if (!texturasCargadas) CargarTexturas();

    nombreItem = "CAJA DE SUMINISTROS";

    float anchoLargo = 25;
    float anchoCorto = 15;

    if (orientacion == CofreOrientacion::HORIZONTAL) {
        this->rect = { pos.x - anchoLargo/2, pos.y - anchoCorto/2, anchoLargo, anchoCorto };
    } else {
        this->rect = { pos.x - anchoCorto/2, pos.y - anchoLargo/2, anchoCorto, anchoLargo };
    }

    this->consumido = false;
}

Cofre::~Cofre() {}

void Cofre::dibujar() {
    Texture2D* texActual = abierto ? &texAbierto : &texCerrado;

    if (texActual->id != 0) {
        Rectangle source = { 0, 0, (float)texActual->width, (float)texActual->height };

        float destW, destH;
        float rotacion = 0.0f;

        if (orientacion == CofreOrientacion::VERTICAL) {
            destW = rect.height * 2.0f;
            destH = rect.width * 2.0f;
            rotacion = 90.0f;
        } else {
            destW = rect.width * 2.0f;
            destH = rect.height * 2.0f;
            rotacion = 0.0f;
        }

        Vector2 centro = { rect.x + rect.width/2, rect.y + rect.height/2 };
        if (orientacion == CofreOrientacion::HORIZONTAL) {
            centro.y -= 10.0f;
        }

        Rectangle dest = { centro.x, centro.y, destW, destH };
        Vector2 origin = { destW / 2, destH / 2 };

        DrawTexturePro(*texActual, source, dest, origin, rotacion, WHITE);
    }
    else {
        Color c = abierto ? DARKGRAY : BROWN;
        DrawRectangleRec(rect, c);
        DrawRectangleLinesEx(rect, 1.0f, BLACK);
    }
}

int Cofre::usar(Protagonista& jugador) {
    if (abierto) return 0;

    this->abierto = true;
    // Sonido de apertura iría aquí
    return this->tipoDeLoot;
}

bool Cofre::esInteraccionPorTecla() const {
    return true;
}

bool Cofre::estaConsumido() const {
    return false;
}

// --- NUEVO ---
bool Cofre::estaAbierto() const {
    return abierto;
}

Rectangle Cofre::getRect() const {
    return rect;
}

Texture2D Cofre::getTextura() {
    return abierto ? texAbierto : texCerrado;
}
