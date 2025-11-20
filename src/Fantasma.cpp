#include "Fantasma.h"
#include "Constantes.h"
#include "raymath.h"
#include "Protagonista.h"
#include "Mapa.h"

// Inicializamos variables estaticas de estado
bool Fantasma::despertado = false;
bool Fantasma::modoFuria = false;
bool Fantasma::modoDialogo = false;
bool Fantasma::jefeEnCombate = false;

bool Fantasma::estaAsustando = false;
float Fantasma::temporizadorSusto = 0.0f;
Vector2 Fantasma::posSustoInicio = {0, 0};
Vector2 Fantasma::posSustoFin = {0, 0};

bool Fantasma::estaDespertando = false;
float Fantasma::temporizadorDespertar = 0.0f;

// Inicializamos texturas estaticas
Texture2D Fantasma::texNormalDer = {0};
Texture2D Fantasma::texNormalIzq = {0};
Texture2D Fantasma::texAtaqueDer = {0};
Texture2D Fantasma::texAtaqueIzq = {0};
bool Fantasma::texturasCargadas = false;

// --- CARGA DE RECURSOS ---
void Fantasma::CargarTexturas() {
    if (!texturasCargadas) {
        texNormalDer = LoadTexture("assets/Elana/ElanaDerecha.png");
        texNormalIzq = LoadTexture("assets/Elana/ElanaIzquierda.png");
        texAtaqueDer = LoadTexture("assets/Elana/ElanaAtaqueDerecha.png");
        texAtaqueIzq = LoadTexture("assets/Elana/ElanaAtaqueIzquierda.png");
        texturasCargadas = true;
    }
}

void Fantasma::DescargarTexturas() {
    if (texturasCargadas) {
        UnloadTexture(texNormalDer);
        UnloadTexture(texNormalIzq);
        UnloadTexture(texAtaqueDer);
        UnloadTexture(texAtaqueIzq);
        texturasCargadas = false;
    }
}
// -------------------------

Fantasma::Fantasma(Vector2 pos)
    : Enemigo(pos,
              Constantes::VIDA_FANTASMA,
              Constantes::DANIO_FANTASMA,
              Constantes::VELOCIDAD_JUGADOR * 2.0f,
              Constantes::RADIO_FANTASMA,
              0.0f, 0.0f, 0.0f)
{
    // Aseguramos que las texturas estén listas al crear el fantasma
    CargarTexturas();
}

void Fantasma::actualizarIA(Vector2 posJugador, const Mapa& mapa) {
    if (jefeEnCombate) {
        this->posicion = {-9999, -9999};
        return;
    }

    if (despertado) {
        // ESTADO: PERSECUCION / ATAQUE
        Vector2 dirBase = Vector2Normalize(Vector2Subtract(posJugador, this->posicion));
        Vector2 temblor = movimientoRecursivo(3);

        // Actualizamos la direccion hacia donde mira (para elegir sprite)
        this->direccion = Vector2Normalize(Vector2Add(dirBase, temblor));

        float velocidadActual = this->velocidad;
        if (modoDialogo) {
            if (Vector2Distance(this->posicion, posJugador) < 100.0f) {
                velocidadActual = 0.0f;
            }
        }
        this->posicion = Vector2Add(this->posicion, Vector2Scale(this->direccion, velocidadActual));

    } else if (estaDespertando) {
        // ESTADO: DESPERTANDO (Quieto, parpadea)
        temporizadorDespertar -= GetFrameTime();
        if (temporizadorDespertar <= 0) {
            estaDespertando = false;
            despertado = true;
        }
        // Mantenemos la dirección anterior o miramos al jugador si quisiéramos

    } else if (estaAsustando) {
        // ESTADO: SUSTO (Scripted Movement)
        temporizadorSusto -= GetFrameTime();
        if (temporizadorSusto <= 0) {
            estaAsustando = false;
            this->posicion = {-9999, -9999};
        } else {
            float duracionTotalSusto = 0.75f;
            float progreso = 1.0f - (temporizadorSusto / duracionTotalSusto);
            if (progreso > 1.0f) progreso = 1.0f;

            this->posicion = Vector2Lerp(posSustoInicio, posSustoFin, progreso);

            // Calculamos dirección del susto para elegir sprite correcto
            Vector2 delta = Vector2Subtract(posSustoFin, posSustoInicio);
            if (Vector2Length(delta) > 0) {
                this->direccion = Vector2Normalize(delta);
            }
        }
    } else {
        this->posicion = {-9999, -9999};
    }
}

Vector2 Fantasma::movimientoRecursivo(int profundidad) {
    if (profundidad <= 0) return {0, 0};
    Vector2 dirAleatoria = { (float)GetRandomValue(-10, 10) / 10.0f, (float)GetRandomValue(-10, 10) / 10.0f };
    Vector2 subTemblor = movimientoRecursivo(profundidad - 1);
    return Vector2Add(dirAleatoria, subTemblor);
}

void Fantasma::dibujar() {
    if (jefeEnCombate) return;

    // 1. Determinar qué textura usar (Normal vs Ataque)
    Texture2D* texActual = nullptr;
    bool mirarDerecha = (this->direccion.x >= 0); // True si X es positivo (derecha)

    if (despertado) {
        // Modo Ataque
        texActual = mirarDerecha ? &texAtaqueDer : &texAtaqueIzq;
    } else {
        // Modos Susto / Despertando / Normal
        texActual = mirarDerecha ? &texNormalDer : &texNormalIzq;
    }

    // 2. Determinar Color y Efectos (Transparencia, Tinte)
    Color colorFinal = WHITE;

    if (despertado) {
        if (modoFuria) {
            colorFinal = RED; // Tinte rojo completo en furia
        } else {
            colorFinal = WHITE; // Normal
        }
        // Leve transparencia fantasmal
        colorFinal = Fade(colorFinal, 0.8f);

    } else if (estaDespertando) {
        // Parpadeo Rojo Intenso
        float alpha = (sin(GetTime() * 25.0f) + 1.0f) / 2.0f;
        colorFinal = ColorLerp(WHITE, RED, alpha); // Interpola entre blanco y rojo
        colorFinal = Fade(colorFinal, 0.7f);

    } else if (estaAsustando) {
        // Muy transparente (Eco)
        colorFinal = Fade(WHITE, 0.15f); // 15% de opacidad
    } else {
        // Si no esta en ningun estado activo, no dibujamos
        return;
    }

    // 3. Dibujar Sprite
    if (texActual && texActual->id != 0) {
        // Escalado inteligente para que mida aprox 60px de alto (tamaño personaje)
        float alturaDeseada = 60.0f;
        float escala = alturaDeseada / (float)texActual->height;

        // Posición centrada
        Vector2 posDibujo = {
            this->posicion.x - (texActual->width * escala) / 2.0f,
            this->posicion.y - (texActual->height * escala) / 2.0f
        };

        DrawTextureEx(*texActual, posDibujo, 0.0f, escala, colorFinal);

        // (Opcional) Mantener el aura roja en modo furia detrás del sprite
        if (modoFuria && despertado) {
            DrawCircleGradient((int)this->posicion.x, (int)this->posicion.y, this->radio * 1.5f, Fade(RED, 0.3f), Fade(RED, 0.0f));
        }

    } else {
        // Fallback: Círculo si fallan las texturas
        DrawCircleV(this->posicion, this->radio, colorFinal);
    }
}

void Fantasma::atacar(Protagonista& jugador) {
    if (jefeEnCombate) return;
    if (modoDialogo) return;
    if (estaAsustando && !despertado) return;
    if (estaDespertando) return;

    if (Fantasma::despertado && CheckCollisionRecs(this->getRect(), jugador.getRect())) {
        jugador.matar();
    }
}
