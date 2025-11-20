#include "Zombie.h"
#include "Constantes.h"
#include "raymath.h"
#include <string>
#include "Protagonista.h"

// Inicialización de estáticos
std::vector<Texture2D> Zombie::animIdle;
std::vector<Texture2D> Zombie::animCaminando;
std::vector<Texture2D> Zombie::animAtaque;
std::vector<Texture2D> Zombie::animMuerte;
bool Zombie::texturasCargadas = false;

const float VELOCIDAD_ANIM_ZOMBIE = 10.0f;
const float VELOCIDAD_ANIM_MUERTE = 5.0f;

void Zombie::CargarTexturas()
{
    if (!texturasCargadas)
    {
        // 1. Idle
        animIdle.push_back(LoadTexture("assets/Zombie/ZombieIdle.png"));

        // 2. Caminando (6 frames + FIX FLUIDEZ)
        for (int i = 1; i <= 6; i++) {
            std::string path = "assets/Zombie/ZombieCaminando" + std::to_string(i) + ".png";
            animCaminando.push_back(LoadTexture(path.c_str()));
        }
        // Duplicamos frames intermedios al final para ciclo suave
        if (animCaminando.size() >= 4) {
            animCaminando.push_back(animCaminando[2]);
            animCaminando.push_back(animCaminando[3]);
        }

        // 3. Ataque
        for (int i = 1; i <= 5; i++) {
            std::string path = "assets/Zombie/ZombieAtaque" + std::to_string(i) + ".png";
            animAtaque.push_back(LoadTexture(path.c_str()));
        }

        // 4. Muriendo
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
        for (auto& t : animIdle) UnloadTexture(t);
        for (auto& t : animCaminando) UnloadTexture(t);
        for (auto& t : animAtaque) UnloadTexture(t);
        for (auto& t : animMuerte) UnloadTexture(t);

        animIdle.clear();
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
      animacionActual(&animIdle)
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
    }
}

bool Zombie::estaMuerto() const
{
    return animacionMuerteTerminada;
}

void Zombie::actualizarIA(Vector2 posJugador, const Mapa& mapa)
{
    float dt = GetFrameTime();

    // --- 1. LÓGICA DE MUERTE ---
    if (estaMuriendo)
    {
        tiempoAnimacion += dt;
        if (tiempoAnimacion >= (1.0f / VELOCIDAD_ANIM_MUERTE))
        {
            tiempoAnimacion = 0.0f;
            if (frameActual < (int)animMuerte.size() - 1) {
                frameActual++;
            } else {
                // Mantenemos el cadáver un instante y luego marcamos para borrar
                animacionMuerteTerminada = true;
            }
        }
        return;
    }

    // --- 2. LÓGICA DE COMPORTAMIENTO ---
    Vector2 velocidadMov = {0, 0};

    // Detección
    bool veJugador = puedeVearAlJugador(posJugador);
    bool escuchaJugador = puedeEscucharAlJugador(posJugador);

    if (veJugador || escuchaJugador) {
        estadoActual = EstadoIA::PERSIGUIENDO;
    }

    // Ajuste de Rango de Ataque visual (Radio + margen)
    // Como el zombie es visualmente grande (3.8x), aumentamos el rango efectivo
    float distanciaAtaque = 50.0f; // Aumentado de 25.0f a 50.0f

    if (estadoActual == EstadoIA::PERSIGUIENDO)
    {
        Vector2 dirHaciaJugador = Vector2Normalize(Vector2Subtract(posJugador, posicion));

        // Comprobar rango de ataque corregido
        if (Vector2Distance(posicion, posJugador) <= distanciaAtaque)
        {
             // Solo entramos en ataque si no estamos en cooldown
             if (temporizadorPausaAtaque <= 0) {
                 estadoActual = EstadoIA::ATACANDO;
                 frameActual = 0;
                 tiempoAnimacion = 0.0f;
             }
             // Si estamos en cooldown, nos quedamos quietos mirando al jugador
             else {
                 setDireccion(dirHaciaJugador);
             }
        }
        else
        {
             // Perseguir
             velocidadMov = Vector2Scale(dirHaciaJugador, velocidad);
             setDireccion(dirHaciaJugador);
        }
    }
    else if (estadoActual == EstadoIA::PATRULLANDO)
    {
        if (temporizadorPatrulla > 0) {
            temporizadorPatrulla -= dt;
        } else {
             // Moviéndose en patrulla
             Vector2 dirPatrulla = Vector2Normalize(Vector2Subtract(destinoPatrulla, posicion));

             if (Vector2Distance(posicion, destinoPatrulla) < 10.0f) {
                 elegirNuevoDestinoPatrulla(mapa);
             } else {
                 velocidadMov = Vector2Scale(dirPatrulla, velocidad * 0.5f);
                 setDireccion(dirPatrulla);
             }
        }
    }

    // Cooldown del ataque
    if (temporizadorPausaAtaque > 0) {
        temporizadorPausaAtaque -= dt;
    }

    // Aplicar dirección (si hay movimiento)
    if (estadoActual != EstadoIA::ATACANDO && Vector2Length(velocidadMov) > 0) {
        setDireccion(Vector2Normalize(velocidadMov));
    }

    // --- 3. SELECCIÓN DE ANIMACIÓN ---
    std::vector<Texture2D>* animAnterior = animacionActual;

    if (estadoActual == EstadoIA::ATACANDO)
    {
        animacionActual = &animAtaque;
        tiempoAnimacion += dt;
        // Ataque fluido
        if (tiempoAnimacion >= (1.0f / 12.0f)) {
            tiempoAnimacion = 0.0f;
            frameActual++;

            // Momento del daño (ej: frame 2 o 3)
            if (frameActual == 2) {
                 // Aquí aplicamos el daño justo cuando la animación golpea
                 if (Vector2Distance(posicion, posJugador) <= distanciaAtaque + 10.0f) {
                    atacar(const_cast<Protagonista&>(*reinterpret_cast<Protagonista*>(0)));
                 }
            }

            if (frameActual >= (int)animAtaque.size()) {
                // Fin del ataque
                estadoActual = EstadoIA::PERSIGUIENDO;
                temporizadorPausaAtaque = 1.0f; // Cooldown de 1s
                frameActual = 0;
            }
        }
    }
    else
    {
        // --- FIX "PATRULLA PATINANDO" ---
        // Si hay velocidad real, caminamos. Si no, idle.
        if (Vector2Length(velocidadMov) > 0.1f) {
            animacionActual = &animCaminando;
        } else {
            animacionActual = &animIdle;
        }

        // Loop normal
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

    // Reset de frame al cambiar de tipo de animación (para que no empiece a caminar en el frame 5)
    if (animAnterior != animacionActual && estadoActual != EstadoIA::ATACANDO) {
        frameActual = 0;
        tiempoAnimacion = 0.0f;
    }
}

void Zombie::atacar(Protagonista& jugador)
{
    // El control de cooldown ya se hace arriba, aquí aplicamos el efecto
    jugador.recibirDanio(this->danio);
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

    // --- FIX "ENCOGIMIENTO AL MORIR" ---
    // Usamos siempre la textura IDLE como referencia para calcular la escala.
    // Así, si la textura de muerte es más ancha, no afectará la escala.
    Texture2D texReferencia = animIdle[0];

    // Escala grande (3.8x respecto al radio físico)
    float escala = (radio * 3.8f) / (float)texReferencia.width;

    Rectangle sourceRec = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };

    // El destino usa el tamaño de la textura actual multiplicado por la escala constante
    Rectangle destRec = { posicion.x, posicion.y, (float)tex.width * escala, (float)tex.height * escala };
    Vector2 origen = { destRec.width / 2.0f, destRec.height / 2.0f };

    Color colorFinal = WHITE;
    if (temporizadorDanio > 0) colorFinal = RED;

    DrawTexturePro(tex, sourceRec, destRec, origen, rotacion, colorFinal);
}
