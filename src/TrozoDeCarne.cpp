#include "TrozoDeCarne.h"
#include "raylib.h"
#include "raymath.h"
#include "Protagonista.h"
#include "Mapa.h"

Texture2D TrozoDeCarne::texAire = { 0 };
Texture2D TrozoDeCarne::texSuelo = { 0 };

const float TrozoDeCarne::TIEMPO_VUELO = 0.7f;
const float TrozoDeCarne::TIEMPO_EN_SUELO = 8.0f;
const float TrozoDeCarne::RADIO_CHARCO = 30.0f;

void TrozoDeCarne::CargarRecursos() {
    if (texAire.id == 0) texAire = LoadTexture("assets/Jefe/Proyectiles/TrozoDeCarne1.png");
    if (texSuelo.id == 0) texSuelo = LoadTexture("assets/Jefe/Proyectiles/TrozoDeCarne2.png"); // Corregido espacio
}

void TrozoDeCarne::DescargarRecursos() {
    if (texAire.id != 0) { UnloadTexture(texAire); texAire.id = 0; }
    if (texSuelo.id != 0) { UnloadTexture(texSuelo); texSuelo.id = 0; }
}

TrozoDeCarne::TrozoDeCarne(Vector2 pos, Vector2 objetivo)
    : Bala(pos, Vector2Subtract(objetivo, pos), 0.0f, 5, OrigenBala::ENEMIGO, RADIO_CHARCO, false),
    inicioSalto(pos), objetivoSuelo(objetivo), progresoSalto(0.0f),
    temporizadorSuelo(TIEMPO_EN_SUELO), estaEnSuelo(false)
{
    setVelocidad({0, 0});
}

void TrozoDeCarne::actualizar(Protagonista& jugador, const Mapa& mapa)
{
    if (!activa) return;

    if (!estaEnSuelo)
    {
        progresoSalto += GetFrameTime() / TIEMPO_VUELO;
        progresoSalto = Clamp(progresoSalto, 0.0f, 1.0f);
        posicion = Vector2Lerp(inicioSalto, objetivoSuelo, progresoSalto);

        if (progresoSalto >= 1.0f) {
            estaEnSuelo = true;
            posicion = objetivoSuelo;
            danio = 0; // Charco no daña al impacto directo, es decorativo/ralentizante visual
            radio = RADIO_CHARCO;
        }
    }
    else
    {
        temporizadorSuelo -= GetFrameTime();
        if (temporizadorSuelo <= 0) desactivar();
    }
}

void TrozoDeCarne::dibujar()
{
    if (!activa) return;

    if (!estaEnSuelo)
    {
        // --- EN EL AIRE ---
        float t = progresoSalto;

        // Sombra
        DrawCircleV(objetivoSuelo, 15.0f * t, Fade(BLACK, 0.3f));

        if (texAire.id != 0) {
            // Efecto de parábola visual (subir y bajar)
            float altura = sinf(t * PI) * 100.0f;
            Vector2 posVisual = { posicion.x, posicion.y - altura };

            float rotacion = t * 720.0f; // Girar 2 vueltas
            float escala = (RADIO_CHARCO * 1.5f) / (float)texAire.width;

            Rectangle src = {0, 0, (float)texAire.width, (float)texAire.height};
            Rectangle dest = {posVisual.x, posVisual.y, (float)texAire.width*escala, (float)texAire.height*escala};

            DrawTexturePro(texAire, src, dest, {dest.width/2, dest.height/2}, rotacion, WHITE);
        } else {
            DrawCircleV(posicion, 10.0f, DARKBROWN);
        }
    }
    else
    {
        // --- EN EL SUELO ---
        float alpha = 1.0f;
        if (temporizadorSuelo < 2.0f) alpha = temporizadorSuelo / 2.0f; // Fade out

        if (texSuelo.id != 0) {
            float escala = (RADIO_CHARCO * 2.2f) / (float)texSuelo.width;

            Rectangle src = {0, 0, (float)texSuelo.width, (float)texSuelo.height};
            Rectangle dest = {posicion.x, posicion.y, (float)texSuelo.width*escala, (float)texSuelo.height*escala};

            DrawTexturePro(texSuelo, src, dest, {dest.width/2, dest.height/2}, 0.0f, Fade(WHITE, alpha));
        } else {
            DrawCircleV(posicion, radio, Fade(BROWN, 0.7f * alpha));
        }
    }
}

bool TrozoDeCarne::esCharco() const { return estaEnSuelo; }
