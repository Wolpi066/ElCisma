#include "Fantasma.h"
#include "Constantes.h"
#include "raymath.h"
#include "Protagonista.h" // Necesario para 'atacar'
#include "Mapa.h"         // <-- ¡AÑADIDO!

// Inicializamos las variables estaticas
bool Fantasma::despertado = false;
bool Fantasma::modoFuria = false;
bool Fantasma::modoDialogo = false;

// --- Inicializar estado de "Susto" ---
bool Fantasma::estaAsustando = false;
float Fantasma::temporizadorSusto = 0.0f;
Vector2 Fantasma::posSustoInicio = {0, 0};
Vector2 Fantasma::posSustoFin = {0, 0};

// --- ¡¡NUEVO!!: Inicializar estado "Despertando" ---
bool Fantasma::estaDespertando = false;
float Fantasma::temporizadorDespertar = 0.0f;
// ---------------------------------------------

// --- CORREGIDO: Constructor llama a 8 args ---
Fantasma::Fantasma(Vector2 pos)
    : Enemigo(pos,
              Constantes::VIDA_FANTASMA,
              Constantes::DANIO_FANTASMA,
              // --- CAMBIO: La velocidad base ahora es la del jugador x2 ---
              Constantes::VELOCIDAD_JUGADOR * 2.0f,
              Constantes::RADIO_FANTASMA,
              0.0f, 0.0f, 0.0f) // No usa IA de vision/escucha
{
    // La velocidad base (this->velocidad) es 2x la del jugador
}

// --- ACTUALIZADO: Logica de IA con 4 estados ---
// ¡¡MODIFICADO!! Añadido 'const Mapa& mapa'
void Fantasma::actualizarIA(Vector2 posJugador, const Mapa& mapa) {
    // El parámetro 'mapa' no se usa aquí, pero es necesario
    // para sobreescribir la función de la clase base Enemigo.

    if (despertado) {
        // --- 1. ESTADO: DESPERTADO (Logica normal de persecucion) ---
        Vector2 dirBase = Vector2Normalize(Vector2Subtract(posJugador, this->posicion));
        Vector2 temblor = movimientoRecursivo(3);
        this->direccion = Vector2Normalize(Vector2Add(dirBase, temblor));

        // La velocidad base (this->velocidad) ya es 2x jugador.
        float velocidadActual = this->velocidad;

        if (modoDialogo) {
            float distancia = Vector2Distance(this->posicion, posJugador);
            if (distancia < 100.0f) {
                velocidadActual = 0.0f;
            }
        }
        this->posicion = Vector2Add(this->posicion, Vector2Scale(this->direccion, velocidadActual));

    } else if (estaDespertando) {
        // --- 2. ¡¡NUEVO ESTADO!!: DESPERTANDO (Delay de 3 seg) ---
        temporizadorDespertar -= GetFrameTime();
        if (temporizadorDespertar <= 0) {
            estaDespertando = false;
            despertado = true; // ¡Ahora si empieza la persecucion!
        }
        // No se mueve, solo espera

    } else if (estaAsustando) {
        // --- 3. ESTADO: ASUSTANDO (Moverse de A a B) ---
        temporizadorSusto -= GetFrameTime();
        if (temporizadorSusto <= 0) {
            estaAsustando = false;
            this->posicion = {-9999, -9999}; // Ocultar
        } else {
            // Moverse de inicio a fin usando Lerp
            float duracionTotalSusto = 0.75f; // Duracion del susto (la que te gustaba)
            float progreso = 1.0f - (temporizadorSusto / duracionTotalSusto);
            if (progreso > 1.0f) progreso = 1.0f;

            this->posicion = Vector2Lerp(posSustoInicio, posSustoFin, progreso);
        }
    } else {
        // --- 4. ESTADO: OCULTO ---
        this->posicion = {-9999, -9999}; // Mantener fuera de pantalla
    }
}

// --- IMPLEMENTACION DE RECURSIVIDAD ---
Vector2 Fantasma::movimientoRecursivo(int profundidad) {
    if (profundidad <= 0) {
        return {0, 0};
    }

    Vector2 dirAleatoria = { (float)GetRandomValue(-10, 10) / 10.0f, (float)GetRandomValue(-10, 10) / 10.0f };
    Vector2 subTemblor = movimientoRecursivo(profundidad - 1);

    return Vector2Add(dirAleatoria, subTemblor);
}
// ------------------------------------

// --- ACTUALIZADO: Logica de Dibujado ---
void Fantasma::dibujar() {
    if (despertado) {
        // --- DIBUJO NORMAL (DESPERTADO) ---
        if (modoFuria) {
            DrawCircleV(this->posicion, this->radio * 1.5f, Fade(RED, 0.3f));
        }
        DrawCircleV(this->posicion, this->radio, Fade(WHITE, 0.6f));

    } else if (estaDespertando) {
        // --- ¡¡NUEVO DIBUJO!!: "CARGA" ---
        // Parpadeo rojo mientras se activa
        float alpha = (sin(GetTime() * 20.0f) + 1.0f) / 2.0f; // 0.0 a 1.0
        DrawCircleV(this->posicion, this->radio * 1.5f, Fade(RED, alpha * 0.5f));
        DrawCircleV(this->posicion, this->radio, Fade(WHITE, 0.4f));

    } else if (estaAsustando) {
        // --- DIBUJO DE SUSTO (TENUE) ---
        DrawCircleV(this->posicion, this->radio, Fade(WHITE, 0.05f));
    }
    // Si no esta ni despertado ni asustando, no se dibuja nada.
}

// --- ACTUALIZADO: Logica de Ataque ---
void Fantasma::atacar(Protagonista& jugador) {
    // Si esta en modo dialogo, NO ataca
    if (modoDialogo) return;

    // Si esta en modo susto, NO ataca
    if (estaAsustando && !despertado) return;

    // --- ¡¡NUEVO!!: Si se esta despertando, NO ataca
    if (estaDespertando) return;

    // --- ¡¡LOGICA DE INSTAKILL!! ---
    // Si esta despertado, mata al jugador al tocarlo
    if (Fantasma::despertado && CheckCollisionRecs(this->getRect(), jugador.getRect())) {
        jugador.matar();
    }
}
