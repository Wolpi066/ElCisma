#include "Protagonista.h"
#include "raymath.h"
#include "Constantes.h"
#include <vector>
#include <cmath>

static const float ALCANCE_LINTERNA_MIN = 80.0f;
static const float ANGULO_CONO_MIN = 0.1f;
static const float DURACION_FRAME_DISPARO = 0.15f;
static const float DURACION_ANIMACION_MUERTE = 2.0f;

Protagonista::Protagonista(Vector2 pos) :
    posicion(pos),
    vida(Constantes::VIDA_MAX_JUGADOR),
    municion(Constantes::MUNICION_MAX),
    bateria(Constantes::BATERIA_MAX),
    tieneArmadura(false),
    direccionVista({ 0.0f, 1.0f }),
    anguloVista(0.0f),
    temporizadorDisparo(0.0f),
    tiempoInmune(0.0f),
    temporizadorBateria(0.0f),
    tieneLlave(false),
    anguloCono(Constantes::ANCHO_CONO_LINTERNA * 0.8f),
    alcanceLinterna(Constantes::ALCANCE_LINTERNA * 0.8f),
    radio(16.0f),
    linternaEncendida(true),
    temporizadorFlicker(0.0f),
    knockbackVelocidad({0.0f, 0.0f}),
    knockbackTimer(0.0f),
    proximoDisparoEsCheat(false),
    bateriaCongelada(false),
    timerVisualDisparo(0.0f),
    timerAnimacionMuerte(0.0f)
{
    texCaminando = LoadTexture("assets/Protagonista/Caminando.png");
    texDisparando = LoadTexture("assets/Protagonista/Disparando.png");
    texMuerto = LoadTexture("assets/Protagonista/Muerto.png");
}

Protagonista::~Protagonista()
{
    UnloadTexture(texCaminando);
    UnloadTexture(texDisparando);
    UnloadTexture(texMuerto);
}

void Protagonista::reset()
{
    vida = Constantes::VIDA_MAX_JUGADOR;
    municion = Constantes::MUNICION_MAX;
    bateria = Constantes::BATERIA_MAX;
    tieneArmadura = false;
    tieneLlave = false;

    linternaEncendida = true;
    temporizadorDisparo = 0.0f;
    tiempoInmune = 0.0f;
    temporizadorBateria = 0.0f;

    knockbackTimer = 0.0f;
    knockbackVelocidad = {0.0f, 0.0f};

    timerAnimacionMuerte = 0.0f;
    timerVisualDisparo = 0.0f;

    bateriaCongelada = false;
    proximoDisparoEsCheat = false;
}

void Protagonista::actualizarInterno(Camera2D camera, Vector2 direccionMovimiento)
{
    if (vida <= 0) {
        if (timerAnimacionMuerte > 0) {
            timerAnimacionMuerte -= GetFrameTime();
        }
        linternaEncendida = false;
        return;
    }

    if (temporizadorDisparo > 0) temporizadorDisparo -= GetFrameTime();
    if (tiempoInmune > 0) tiempoInmune -= GetFrameTime();
    if (knockbackTimer > 0) knockbackTimer -= GetFrameTime();
    if (timerVisualDisparo > 0) timerVisualDisparo -= GetFrameTime();

    Vector2 posMouse = GetScreenToWorld2D(GetMousePosition(), camera);
    Vector2 dirDeseada = Vector2Normalize(Vector2Subtract(posMouse, posicion));

    direccionVista = Vector2Lerp(direccionVista, dirDeseada, Constantes::VELOCIDAD_LINTERNA * GetFrameTime());
    direccionVista = Vector2Normalize(direccionVista);
    anguloVista = atan2f(direccionVista.y, direccionVista.x) * RAD2DEG;

    if (!bateriaCongelada && IsKeyPressed(KEY_F)) {
        linternaEncendida = !linternaEncendida;
    }

    if (!bateriaCongelada && linternaEncendida && bateria > 0) {
        temporizadorBateria += GetFrameTime();
        if (temporizadorBateria >= (1.0f / Constantes::BATERIA_CONSUMO_SEGUNDO)) {
            bateria--;
            temporizadorBateria = 0.0f;
        }
    }
    if (bateria <= 0) linternaEncendida = false;

    if (linternaEncendida && bateria < Constantes::BATERIA_FLICKER_THRESHOLD) {
        temporizadorFlicker -= GetFrameTime();
        if (temporizadorFlicker < 0.0f) {
            float ratio = (float)bateria / Constantes::BATERIA_FLICKER_THRESHOLD;
            temporizadorFlicker = (float)GetRandomValue(0, 100) / 100.0f * (0.1f + ratio * 0.4f);
        }
    }
}


int Protagonista::intentarDisparar(bool quiereDisparar)
{
    if (vida <= 0) return 0;

    if (quiereDisparar && temporizadorDisparo <= 0 && municion > 0) {
        municion--;
        temporizadorDisparo = Constantes::TIEMPO_RECARGA_DISPARO;
        timerVisualDisparo = DURACION_FRAME_DISPARO;

        if (proximoDisparoEsCheat) {
            proximoDisparoEsCheat = false;
            return 2;
        }
        return 1;
    }
    return 0;
}

void Protagonista::setPosicion(Vector2 nuevaPos) {
    this->posicion = nuevaPos;
}

void Protagonista::dibujar()
{
    Texture2D* texActual;
    Color tinte = WHITE;

    float rotacion = anguloVista + 90.0f;

    if (vida <= 0) {
        texActual = &texMuerto;
        float progreso = 1.0f - (timerAnimacionMuerte / DURACION_ANIMACION_MUERTE);
        progreso = Clamp(progreso, 0.0f, 1.0f);
        tinte = ColorLerp(WHITE, (Color){80, 80, 80, 255}, progreso);
    } else {
        if (timerVisualDisparo > 0.0f) {
            texActual = &texDisparando;
        } else {
            texActual = &texCaminando;
        }

        if (tiempoInmune > 0) {
            if ((int)(tiempoInmune * 20) % 2 == 0) tinte = Fade(WHITE, 0.0f);
            else tinte = RED;
        }
    }

    float w = (float)texActual->width;
    float h = (float)texActual->height;

    Rectangle sourceRec = { 0.0f, 0.0f, w, h };
    Rectangle destRec = {
        posicion.x,
        posicion.y,
        (float)Constantes::PLAYER_FRAME_WIDTH,
        (float)Constantes::PLAYER_FRAME_HEIGHT
    };
    Vector2 origen = {
        (float)Constantes::PLAYER_FRAME_WIDTH / 2.0f,
        (float)Constantes::PLAYER_FRAME_HEIGHT / 2.0f
    };

    DrawTexturePro(*texActual, sourceRec, destRec, origen, rotacion, tinte);
}

void Protagonista::recibirDanio(int cantidad)
{
    if (tiempoInmune > 0 || vida <= 0) return;

    if (tieneArmadura) {
        tieneArmadura = false;
    } else {
        vida -= cantidad;
    }

    if (vida <= 0) {
        vida = 0;
        matar();
    } else {
        tiempoInmune = Constantes::TIEMPO_INMUNIDAD_DANIO;
    }
}

void Protagonista::matar() {
    if (vida > 0) vida = 0;
    if (timerAnimacionMuerte <= 0.0f) {
         timerAnimacionMuerte = DURACION_ANIMACION_MUERTE;
    }
}

bool Protagonista::haFinalizadoAnimacionMuerte() const {
    return (vida <= 0 && timerAnimacionMuerte <= 0.0f);
}

void Protagonista::aplicarKnockback(Vector2 direccion, float fuerza, float duracion)
{
    if (vida <= 0) return;
    knockbackVelocidad = Vector2Scale(direccion, fuerza);
    knockbackTimer = duracion;
}

void Protagonista::recargarBateria(const int& cantidad) {
    bateria += cantidad;
    if (bateria > Constantes::BATERIA_MAX) bateria = Constantes::BATERIA_MAX;
}
void Protagonista::curarVida(const int& cantidad) {
    vida += cantidad;
    if (vida > Constantes::VIDA_MAX_JUGADOR) vida = Constantes::VIDA_MAX_JUGADOR;
}
void Protagonista::recargarMunicion(const int& cantidad) {
    municion += cantidad;
    if (municion > Constantes::MUNICION_MAX) municion = Constantes::MUNICION_MAX;
}
void Protagonista::recibirArmadura() { tieneArmadura = true; }
void Protagonista::recibirLlave() { tieneLlave = true; }
void Protagonista::quitarLlave() { tieneLlave = false; }
void Protagonista::activarCheatDisparo() { proximoDisparoEsCheat = true; }
void Protagonista::setBateriaCongelada(bool congelada) { bateriaCongelada = congelada; }

bool Protagonista::estaVivo() const { return vida > 0; }
Vector2 Protagonista::getPosicion() const { return posicion; }
Vector2 Protagonista::getDireccionVista() const { return direccionVista; }
Rectangle Protagonista::getRect() const {
    return { posicion.x - radio, posicion.y - radio, radio * 2, radio * 2 };
}
int Protagonista::getVida() const { return vida; }
int Protagonista::getMunicion() const { return municion; }
int Protagonista::getBateria() const { return bateria; }
bool Protagonista::getTieneLlave() const { return tieneLlave; }
float Protagonista::getRadio() const { return radio; }
float Protagonista::getAnguloVista() const { return anguloVista; }
float Protagonista::getTiempoInmune() const { return tiempoInmune; }
float Protagonista::getKnockbackTimer() const { return knockbackTimer; }
Vector2 Protagonista::getVelocidadKnockback() const { return knockbackVelocidad; }

float Protagonista::getAnguloCono() const
{
    if (!linternaEncendida) return 0.0f;
    float bateriaNorm = (float)bateria / (float)Constantes::BATERIA_MAX;
    bateriaNorm = Clamp(bateriaNorm, 0.0f, 1.0f);
    float anguloCalculado = ANGULO_CONO_MIN + (anguloCono - ANGULO_CONO_MIN) * bateriaNorm;
    if (bateria < Constantes::BATERIA_FLICKER_THRESHOLD && temporizadorFlicker < 0.05f) return anguloCalculado * 0.7f;
    return anguloCalculado;
}

float Protagonista::getAlcanceLinterna() const
{
    if (!linternaEncendida) return 0.0f;
    float bateriaNorm = (float)bateria / (float)Constantes::BATERIA_MAX;
    bateriaNorm = Clamp(bateriaNorm, 0.0f, 1.0f);
    float alcanceCalculado = ALCANCE_LINTERNA_MIN + (alcanceLinterna - ALCANCE_LINTERNA_MIN) * bateriaNorm;
    if (bateria < Constantes::BATERIA_FLICKER_THRESHOLD && temporizadorFlicker < 0.05f) return alcanceCalculado * 0.8f;
    return alcanceCalculado;
}
