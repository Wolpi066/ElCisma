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
const float VELOCIDAD_ANIM_ATAQUE = 10.0f;
const float VELOCIDAD_ANIM_MUERTE = 6.0f;
const float TIEMPO_CADAVER_EN_PISO = 5.0f; // Tiempo que se queda el cuerpo (5 seg)

void Zombie::CargarTexturas()
{
    if (!texturasCargadas)
    {
        // 1. Caminando
        for (int i = 1; i <= 6; i++) {
            std::string path = "assets/Zombie/ZombieCaminando" + std::to_string(i) + ".png";
            animCaminando.push_back(LoadTexture(path.c_str()));
        }
        if (animCaminando.size() >= 4) {
            animCaminando.push_back(animCaminando[2]);
            animCaminando.push_back(animCaminando[3]);
        }

        // 2. Ataque
        for (int i = 1; i <= 5; i++) {
            std::string path = "assets/Zombie/ZombieAtaque" + std::to_string(i) + ".png";
            animAtaque.push_back(LoadTexture(path.c_str()));
        }

        // 3. Muriendo
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
              Constantes::RADIO_ZOMBIE,
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
    // Solo devolvemos true (para borrarlo) cuando termina TODA la secuencia
    return animacionMuerteTerminada;
}

void Zombie::actualizarIA(Vector2 posJugador, const Mapa& mapa)
{
    float dt = GetFrameTime();

    // --- 1. LÓGICA DE MUERTE ---
    if (estaMuriendo)
    {
        // Si aún no terminamos los frames de caer...
        if (frameActual < (int)animMuerte.size() - 1)
        {
            tiempoAnimacion += dt;
            if (tiempoAnimacion >= (1.0f / VELOCIDAD_ANIM_MUERTE))
            {
                tiempoAnimacion = 0.0f;
                frameActual++;
            }
        }
        // Si ya cayó al suelo...
        else
        {
            frameActual = (int)animMuerte.size() - 1; // Mantener frame final

            if (temporizadorCadaver > 0) {
                temporizadorCadaver -= dt;
            } else {
                animacionMuerteTerminada = true; // Ahora sí, adiós.
            }
        }
        return;
    }


    // --- 2. LÓGICA DE ATAQUE ---
    if (estadoActual == EstadoIA::ATACANDO)
    {
        animacionActual = &animAtaque;
        tiempoAnimacion += dt;

        if (tiempoAnimacion >= (1.0f / VELOCIDAD_ANIM_ATAQUE))
        {
            tiempoAnimacion = 0.0f;
            frameActual++;

            // Nota: El daño ya no se llama aquí para evitar el crash.
            // Se maneja en 'atacar()' que es llamado por MotorColisiones.

            if (frameActual >= (int)animAtaque.size()) {
                // Fin animación ataque
                estadoActual = EstadoIA::PERSIGUIENDO;
                temporizadorPausaAtaque = 1.2f; // Cooldown
                frameActual = 0;
                haDaniadoEnEsteAtaque = false;
            }
        }
        return; // Bloqueamos movimiento mientras ataca
    }


    // --- 3. LÓGICA DE MOVIMIENTO ---

    if (temporizadorPausaAtaque > 0) temporizadorPausaAtaque -= dt;

    Vector2 velocidadMov = {0, 0};

    bool veJugador = puedeVearAlJugador(posJugador);
    bool escuchaJugador = puedeEscucharAlJugador(posJugador);

    if (veJugador || escuchaJugador) {
        estadoActual = EstadoIA::PERSIGUIENDO;
    }

    float distanciaAtaque = 50.0f;

    if (estadoActual == EstadoIA::PERSIGUIENDO)
    {
        Vector2 dirHaciaJugador = Vector2Normalize(Vector2Subtract(posJugador, posicion));

        if (Vector2Distance(posicion, posJugador) <= distanciaAtaque)
        {
             if (temporizadorPausaAtaque <= 0) {
                 // Iniciar ataque
                 estadoActual = EstadoIA::ATACANDO;
                 frameActual = 0;
                 tiempoAnimacion = 0.0f;
                 haDaniadoEnEsteAtaque = false;
                 return;
             }
             else {
                 // Esperando cooldown
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

    // --- 4. ANIMACIÓN MOVIMIENTO ---
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
    // Esta función es llamada por MotorColisiones cada frame si estamos ATACANDO
    // Solo aplicamos daño en el frame 2
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

    // Usamos la textura base para la escala constante (EVITA ENCOGIMIENTO)
    Texture2D texReferencia = animCaminando[0];
    float escala = (radio * 3.8f) / (float)texReferencia.width;

    Rectangle sourceRec = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
    Rectangle destRec = { posicion.x, posicion.y, (float)tex.width * escala, (float)tex.height * escala };
    Vector2 origen = { destRec.width / 2.0f, destRec.height / 2.0f };

    Color colorFinal = WHITE;
    if (temporizadorDanio > 0) colorFinal = RED;

    // Desvanecimiento solo al final del tiempo de cadáver
    if (estaMuriendo && temporizadorCadaver < 1.0f) {
        colorFinal = Fade(WHITE, temporizadorCadaver);
    }

    DrawTexturePro(tex, sourceRec, destRec, origen, rotacion, colorFinal);
}
