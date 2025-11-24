#include "Zombie.h"
#include "Constantes.h"
#include "raymath.h"
#include <string>
#include "Protagonista.h"

// Inicialización de estáticos
std::vector<Texture2D> Zombie::animCaminando;
std::vector<Texture2D> Zombie::animAtaque;
std::vector<Texture2D> Zombie::animMuerte;
bool Zombie::texturasCargadas = false;

// Audio Estático
Sound Zombie::fxRugido = { 0 };
bool Zombie::recursosSonidoCargados = false;

const float VELOCIDAD_ANIM_ZOMBIE = 10.0f;
const float VELOCIDAD_ANIM_ATAQUE = 9.0f;
const float VELOCIDAD_ANIM_MUERTE = 8.0f;
const float TIEMPO_CADAVER_EN_PISO = 10.0f;

// Config Audio
const float DISTANCIA_AUDIO_MAX = 350.0f;
const float VOLUMEN_RUGIDO_BASE = 0.7f;

void Zombie::CargarTexturas()
{
    if (!texturasCargadas)
    {
        for (int i = 1; i <= 6; i++) {
            std::string path = "assets/Zombie/ZombieCaminando" + std::to_string(i) + ".png";
            animCaminando.push_back(LoadTexture(path.c_str()));
        }
        // Loop extra
        if (animCaminando.size() >= 4) {
            animCaminando.push_back(animCaminando[2]);
            animCaminando.push_back(animCaminando[3]);
        }

        for (int i = 1; i <= 5; i++) {
            std::string path = "assets/Zombie/ZombieAtaque" + std::to_string(i) + ".png";
            animAtaque.push_back(LoadTexture(path.c_str()));
        }

        for (int i = 1; i <= 3; i++) {
            std::string path = "assets/Zombie/ZombieMuriendo" + std::to_string(i) + ".png";
            animMuerte.push_back(LoadTexture(path.c_str()));
        }
        texturasCargadas = true;
    }
}

void Zombie::DescargarTexturas()
{
    if (texturasCargadas)
    {
        for (auto& t : animCaminando) UnloadTexture(t);
        for (auto& t : animAtaque) UnloadTexture(t);
        for (auto& t : animMuerte) UnloadTexture(t);
        animCaminando.clear();
        animAtaque.clear();
        animMuerte.clear();
        texturasCargadas = false;
    }
}

void Zombie::CargarSonidos() {
    if (!recursosSonidoCargados) {
        if (FileExists("assets/Audio/Sonidos/Enemigos/Rugido.wav")) {
            fxRugido = LoadSound("assets/Audio/Sonidos/Enemigos/Rugido.wav");
        }
        recursosSonidoCargados = true;
    }
}

void Zombie::DescargarSonidos() {
    if (recursosSonidoCargados) {
        if (fxRugido.stream.buffer) UnloadSound(fxRugido);
        recursosSonidoCargados = false;
    }
}

// CONSTRUCTOR RESTAURADO CON CONSTANTES
Zombie::Zombie(Vector2 pos)
    : Enemigo(pos,
              Constantes::VIDA_ZOMBIE,
              Constantes::DANIO_ZOMBIE,
              Constantes::VELOCIDAD_ZOMBIE,
              Constantes::RADIO_ZOMBIE * 1.3f,
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

Zombie::~Zombie() {}

void Zombie::recibirDanio(int cantidad)
{
    if (estaMuriendo) return;

    Enemigo::recibirDanio(cantidad); // Lógica base (Flash rojo + Persiguir)

    if (vida <= 0)
    {
        vida = 0;
        estaMuriendo = true;
        frameActual = 0;
        tiempoAnimacion = 0.0f;
        animacionActual = &animMuerte;
        temporizadorCadaver = TIEMPO_CADAVER_EN_PISO;
    }
}

bool Zombie::estaMuerto() const
{
    return animacionMuerteTerminada;
}

void Zombie::actualizarIA(Vector2 posJugador, const Mapa& mapa)
{
    float dt = GetFrameTime();

    // 1. MUERTE
    if (estaMuriendo)
    {
        if (frameActual < (int)animMuerte.size() - 1)
        {
            tiempoAnimacion += dt;
            if (tiempoAnimacion >= (1.0f / VELOCIDAD_ANIM_MUERTE))
            {
                tiempoAnimacion = 0.0f;
                frameActual++;
            }
        }
        else
        {
            frameActual = (int)animMuerte.size() - 1;
            if (temporizadorCadaver > 0) {
                temporizadorCadaver -= dt;
            } else {
                animacionMuerteTerminada = true;
            }
        }
        return;
    }

    // --- AUDIO (Solo si vivo y no hay jefe) ---
    if (recursosSonidoCargados && !Enemigo::batallaJefeIniciada) {
        float dist = Vector2Distance(posicion, posJugador);
        if (dist < DISTANCIA_AUDIO_MAX) {
            float pan = Remap(posicion.x - posJugador.x, -DISTANCIA_AUDIO_MAX, DISTANCIA_AUDIO_MAX, 0.0f, 1.0f);
            SetSoundPan(fxRugido, Clamp(pan, 0.0f, 1.0f));
            float vol = Remap(dist, 0.0f, DISTANCIA_AUDIO_MAX, VOLUMEN_RUGIDO_BASE, 0.0f);
            SetSoundVolume(fxRugido, Clamp(vol, 0.0f, 1.0f));

            if (!haRugidoInicial) {
                PlaySound(fxRugido);
                haRugidoInicial = true;
            }
        }
    }

    // 2. ATAQUE
    if (estadoActual == EstadoIA::ATACANDO)
    {
        animacionActual = &animAtaque;
        tiempoAnimacion += dt;

        if (tiempoAnimacion >= (1.0f / VELOCIDAD_ANIM_ATAQUE))
        {
            tiempoAnimacion = 0.0f;
            frameActual++;

            if (frameActual >= (int)animAtaque.size()) {
                estadoActual = EstadoIA::PERSIGUIENDO;
                temporizadorPausaAtaque = 1.5f;
                frameActual = 0;
                haDaniadoEnEsteAtaque = false;
            }
        }
        return;
    }

    // 3. MOVIMIENTO
    if (temporizadorPausaAtaque > 0) temporizadorPausaAtaque -= dt;

    Vector2 velocidadMov = {0, 0};

    bool veJugador = puedeVearAlJugador(posJugador);
    bool escuchaJugador = puedeEscucharAlJugador(posJugador);

    if (veJugador || escuchaJugador) {
        estadoActual = EstadoIA::PERSIGUIENDO;
    }

    float distanciaInicioAtaque = getRadio() + 35.0f;

    if (estadoActual == EstadoIA::PERSIGUIENDO)
    {
        Vector2 dirHaciaJugador = Vector2Normalize(Vector2Subtract(posJugador, posicion));

        if (Vector2Distance(posicion, posJugador) <= distanciaInicioAtaque)
        {
             if (temporizadorPausaAtaque <= 0) {
                 estadoActual = EstadoIA::ATACANDO;
                 frameActual = 0;
                 tiempoAnimacion = 0.0f;
                 haDaniadoEnEsteAtaque = false;

                 // Rugido de ataque (cerca)
                 if (recursosSonidoCargados && !Enemigo::batallaJefeIniciada) PlaySound(fxRugido);

                 return;
             }
             else {
                 setDireccion(dirHaciaJugador);
             }
        }
        else
        {
             velocidadMov = Vector2Scale(dirHaciaJugador, velocidad);
             setDireccion(dirHaciaJugador);
        }
    }
    else if (estadoActual == EstadoIA::PATRULLANDO)
    {
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
    if (tiempoAnimacion >= (1.0f / VELOCIDAD_ANIM_ZOMBIE))
    {
        tiempoAnimacion = 0.0f;
        frameActual++;
        if (frameActual >= (int)animacionActual->size()) {
            frameActual = 0;
        }
    }
}

void Zombie::atacar(Protagonista& jugador)
{
    if (frameActual == 2 && !haDaniadoEnEsteAtaque)
    {
        jugador.recibirDanio(this->danio);
        haDaniadoEnEsteAtaque = true;
    }
}

void Zombie::dibujar()
{
    if (!texturasCargadas || !animacionActual || animacionActual->empty())
    {
        DrawCircleV(posicion, radio, DARKGREEN);
        return;
    }

    if (frameActual < 0) frameActual = 0;
    if (frameActual >= (int)animacionActual->size()) frameActual = 0;

    Texture2D tex = (*animacionActual)[frameActual];
    float rotacion = atan2f(direccion.y, direccion.x) * RAD2DEG;

    Texture2D texReferencia = animCaminando[0];
    float escala = (radio * 4.2f) / (float)texReferencia.width;

    Rectangle sourceRec = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
    Rectangle destRec = { posicion.x, posicion.y, (float)tex.width * escala, (float)tex.height * escala };
    Vector2 origen = { destRec.width / 2.0f, destRec.height / 2.0f };

    Color colorFinal = WHITE;
    if (temporizadorDanio > 0) colorFinal = RED;

    if (estaMuriendo && temporizadorCadaver < 2.0f) {
        colorFinal = Fade(WHITE, temporizadorCadaver / 2.0f);
    }

    DrawTexturePro(tex, sourceRec, destRec, origen, rotacion, colorFinal);
}
