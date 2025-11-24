#include "MonstruoObeso.h"
#include "Constantes.h"
#include "raymath.h"
#include <string>
#include "Protagonista.h"

// Inicialización Estáticos
std::vector<Texture2D> MonstruoObeso::animCaminando;
std::vector<Texture2D> MonstruoObeso::animAtaque;
std::vector<Texture2D> MonstruoObeso::animMuerte;
bool MonstruoObeso::texturasCargadas = false;

Sound MonstruoObeso::fxGrito = { 0 };
bool MonstruoObeso::recursosSonidoCargados = false;

// --- CONSTANTES DE ANIMACIÓN (Nombres Unificados) ---
const float VEL_ANIM_WALK = 8.0f;
const float VEL_ANIM_ATK = 7.0f;
const float VEL_ANIM_DEATH = 6.0f;
const float TIEMPO_CADAVER_OBESO = 10.0f;

// Audio
const float DISTANCIA_AUDIO_MAX = 400.0f;
const float VOLUMEN_GRITO_BASE = 0.9f;

void MonstruoObeso::CargarTexturas() {
    if (!texturasCargadas) {
        for (int i = 1; i <= 7; i++) {
            std::string path = "assets/Obeso/ObesoCaminando" + std::to_string(i) + ".png";
            animCaminando.push_back(LoadTexture(path.c_str()));
        }
        for (int i = 1; i <= 6; i++) {
            std::string path = "assets/Obeso/ObesoAtacando" + std::to_string(i) + ".png";
            animAtaque.push_back(LoadTexture(path.c_str()));
        }
        for (int i = 1; i <= 3; i++) {
            std::string path = "assets/Obeso/ObesoMuriendo" + std::to_string(i) + ".png";
            animMuerte.push_back(LoadTexture(path.c_str()));
        }
        texturasCargadas = true;
    }
}

void MonstruoObeso::DescargarTexturas() {
    if (texturasCargadas) {
        for (auto& t : animCaminando) UnloadTexture(t);
        for (auto& t : animAtaque) UnloadTexture(t);
        for (auto& t : animMuerte) UnloadTexture(t);
        animCaminando.clear();
        animAtaque.clear();
        animMuerte.clear();
        texturasCargadas = false;
    }
}

void MonstruoObeso::CargarSonidos() {
    if (!recursosSonidoCargados) {
        if (FileExists("assets/Audio/Sonidos/Enemigos/MonstruoObeso.mp3")) {
            fxGrito = LoadSound("assets/Audio/Sonidos/Enemigos/MonstruoObeso.mp3");
        }
        recursosSonidoCargados = true;
    }
}

void MonstruoObeso::DescargarSonidos() {
    if (recursosSonidoCargados) {
        if (fxGrito.stream.buffer) UnloadSound(fxGrito);
        recursosSonidoCargados = false;
    }
}

MonstruoObeso::MonstruoObeso(Vector2 pos)
    : Enemigo(pos,
              Constantes::VIDA_OBESO,
              Constantes::DANIO_OBESO,
              Constantes::VELOCIDAD_OBESO,
              Constantes::RADIO_OBESO,
              Constantes::RANGO_VISUAL_ZOMBIE,
              Constantes::ANGULO_CONO_ZOMBIE,
              Constantes::RANGO_AUDIO_ZOMBIE),
      frameActual(0),
      tiempoAnimacion(0.0f),
      estaMuriendo(false),
      animacionMuerteTerminada(false),
      temporizadorCadaver(0.0f),
      haDaniadoEnEsteAtaque(false),
      haRugidoInicial(false),
      animacionActual(&animCaminando)
{
    if (!texturasCargadas) CargarTexturas();
}

MonstruoObeso::~MonstruoObeso() {}

void MonstruoObeso::recibirDanio(int cantidad) {
    if (estaMuriendo) return;

    // Lógica de persecución al recibir daño
    Enemigo::recibirDanio(cantidad);

    if (vida <= 0) {
        vida = 0;
        estaMuriendo = true;
        frameActual = 0;
        tiempoAnimacion = 0.0f;
        animacionActual = &animMuerte;
        temporizadorCadaver = TIEMPO_CADAVER_OBESO;
    }
}

bool MonstruoObeso::estaMuerto() const { return animacionMuerteTerminada; }

void MonstruoObeso::actualizarIA(Vector2 posJugador, const Mapa& mapa) {
    float dt = GetFrameTime();

    // 1. MUERTE
    if (estaMuriendo) {
        if (frameActual < (int)animMuerte.size() - 1) {
            tiempoAnimacion += dt;
            // Usamos la constante correcta: VEL_ANIM_DEATH
            if (tiempoAnimacion >= (1.0f / VEL_ANIM_DEATH)) {
                tiempoAnimacion = 0.0f;
                frameActual++;
            }
        } else {
            if (temporizadorCadaver > 0) temporizadorCadaver -= dt;
            else animacionMuerteTerminada = true;
        }
        return;
    }

    // 2. AUDIO DIRECCIONAL
    if (recursosSonidoCargados && !Enemigo::batallaJefeIniciada) {
        float dist = Vector2Distance(posicion, posJugador);
        if (dist < DISTANCIA_AUDIO_MAX) {
            float pan = Remap(posicion.x - posJugador.x, -DISTANCIA_AUDIO_MAX, DISTANCIA_AUDIO_MAX, 0.0f, 1.0f);
            SetSoundPan(fxGrito, Clamp(pan, 0.0f, 1.0f));
            float vol = Remap(dist, 0.0f, DISTANCIA_AUDIO_MAX, VOLUMEN_GRITO_BASE, 0.0f);
            SetSoundVolume(fxGrito, Clamp(vol, 0.0f, 1.0f));

            if (!haRugidoInicial) {
                PlaySound(fxGrito);
                haRugidoInicial = true;
            }
        }
    }

    // 3. ATAQUE
    if (estadoActual == EstadoIA::ATACANDO) {
        animacionActual = &animAtaque;
        tiempoAnimacion += dt;
        // Usamos la constante correcta: VEL_ANIM_ATK
        if (tiempoAnimacion >= (1.0f / VEL_ANIM_ATK)) {
            tiempoAnimacion = 0.0f;
            frameActual++;
            if (frameActual >= (int)animAtaque.size()) {
                estadoActual = EstadoIA::PERSIGUIENDO;
                temporizadorPausaAtaque = 2.0f;
                frameActual = 0;
                haDaniadoEnEsteAtaque = false;
            }
        }
        return;
    }

    // 4. MOVIMIENTO
    if (temporizadorPausaAtaque > 0) temporizadorPausaAtaque -= dt;
    Vector2 velocidadMov = {0,0};

    if (puedeVearAlJugador(posJugador) || puedeEscucharAlJugador(posJugador)) {
        estadoActual = EstadoIA::PERSIGUIENDO;
    }

    float distAtaque = getRadio() + 40.0f;

    if (estadoActual == EstadoIA::PERSIGUIENDO) {
        if (Vector2Distance(posicion, posJugador) <= distAtaque) {
            if (temporizadorPausaAtaque <= 0) {
                estadoActual = EstadoIA::ATACANDO;
                frameActual = 0;
                tiempoAnimacion = 0.0f;
                haDaniadoEnEsteAtaque = false;
                if (recursosSonidoCargados && !Enemigo::batallaJefeIniciada) PlaySound(fxGrito);
                return;
            }
        } else {
            velocidadMov = Vector2Scale(Vector2Normalize(Vector2Subtract(posJugador, posicion)), velocidad);
        }
    } else {
        // Lógica Patrulla (Simplificada del padre si necesario, o personalizada)
        if (temporizadorPatrulla > 0) {
            temporizadorPatrulla -= dt;
        } else {
             Vector2 dirPatrulla = Vector2Normalize(Vector2Subtract(destinoPatrulla, posicion));
             if (Vector2Distance(posicion, destinoPatrulla) < 10.0f) {
                 elegirNuevoDestinoPatrulla(mapa);
             } else {
                 velocidadMov = Vector2Scale(dirPatrulla, velocidad * 0.5f);
                 setDireccion(dirPatrulla);
             }
        }
    }

    if (Vector2Length(velocidadMov) > 0) {
        setDireccion(Vector2Normalize(velocidadMov));
    }

    animacionActual = &animCaminando;
    tiempoAnimacion += dt;
    // Usamos la constante correcta: VEL_ANIM_WALK
    if (tiempoAnimacion >= (1.0f / VEL_ANIM_WALK)) {
        tiempoAnimacion = 0.0f;
        frameActual++;
        if (frameActual >= (int)animacionActual->size()) frameActual = 0;
    }
}

void MonstruoObeso::atacar(Protagonista& jugador) {
    if (frameActual == 3 && !haDaniadoEnEsteAtaque) {
        jugador.recibirDanio(danio);
        haDaniadoEnEsteAtaque = true;
        jugador.aplicarKnockback(direccion, 300.0f, 0.2f);
    }
}

void MonstruoObeso::dibujar() {
    if (!texturasCargadas || !animacionActual || animacionActual->empty()) {
        DrawCircleV(posicion, radio, BROWN);
        return;
    }
    if (frameActual >= (int)animacionActual->size()) frameActual = 0;
    Texture2D tex = (*animacionActual)[frameActual];

    float escala = (radio * 3.5f) / (float)tex.width;
    float rot = atan2f(direccion.y, direccion.x) * RAD2DEG;

    Rectangle src = {0,0,(float)tex.width, (float)tex.height};
    Rectangle dest = {posicion.x, posicion.y, (float)tex.width*escala, (float)tex.height*escala};
    Vector2 origin = {dest.width/2, dest.height/2};

    Color color = WHITE;
    if (temporizadorDanio > 0) color = RED;
    if (estaMuriendo && temporizadorCadaver < 2.0f) color = Fade(WHITE, temporizadorCadaver/2.0f);

    DrawTexturePro(tex, src, dest, origin, rot, color);
}
