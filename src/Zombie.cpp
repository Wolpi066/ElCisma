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

const float VELOCIDAD_ANIM_ZOMBIE = 10.0f;
const float VELOCIDAD_ANIM_ATAQUE = 9.0f;
const float VELOCIDAD_ANIM_MUERTE = 8.0f;
const float TIEMPO_CADAVER_EN_PISO = 10.0f;

void Zombie::CargarTexturas()
{
    if (!texturasCargadas)
    {
        for (int i = 1; i <= 6; i++) {
            std::string path = "assets/Zombie/ZombieCaminando" + std::to_string(i) + ".png";
            animCaminando.push_back(LoadTexture(path.c_str()));
        }
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
      animacionActual(&animCaminando)
{
    if (!texturasCargadas) CargarTexturas();
}

Zombie::~Zombie() {}

void Zombie::recibirDanio(int cantidad)
{
    if (estaMuriendo) return;

    Enemigo::recibirDanio(cantidad);

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

    // --- 1. MUERTE ---
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

    // --- 2. ATAQUE ---
    if (estadoActual == EstadoIA::ATACANDO)
    {
        animacionActual = &animAtaque;
        tiempoAnimacion += dt;

        if (tiempoAnimacion >= (1.0f / VELOCIDAD_ANIM_ATAQUE))
        {
            tiempoAnimacion = 0.0f;
            frameActual++;

            // NOTA: El daño ya no se llama aquí para evitar el crash.
            // Se verifica en el método 'atacar()' llamado por colisiones.

            if (frameActual >= (int)animAtaque.size()) {
                estadoActual = EstadoIA::PERSIGUIENDO;
                temporizadorPausaAtaque = 1.5f;
                frameActual = 0;
                haDaniadoEnEsteAtaque = false;
            }
        }
        return;
    }

    // --- 3. MOVIMIENTO ---
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
    // El MotorColisiones llama a esto cuando chocan.
    // Solo aplicamos daño si la animación está en el frame correcto.
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
