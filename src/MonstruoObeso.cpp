#include "MonstruoObeso.h"
#include "Constantes.h"
#include "raymath.h"
#include "Protagonista.h" // ¡¡Importante!!

// --- CORREGIDO: Constructor llama a 8 args ---
MonstruoObeso::MonstruoObeso(Vector2 pos)
    : Enemigo(pos,
              Constantes::VIDA_OBESO,
              Constantes::DANIO_OBESO,
              Constantes::VELOCIDAD_OBESO,
              Constantes::RADIO_OBESO,
              Constantes::RANGO_VISUAL_ZOMBIE * 0.8f, // Ve un poco menos
              Constantes::ANGULO_CONO_ZOMBIE * 0.8f,  // Cono mas estrecho
              Constantes::RANGO_AUDIO_ZOMBIE * 1.2f)  // Escucha un poco mas
{
}

// --- CORREGIDO: Nombre de la funcion ---
void MonstruoObeso::actualizarIA(Vector2 posJugador) {

    if (!this->haDetectadoAlJugador) {
        if (puedeVearAlJugador(posJugador) || puedeEscucharAlJugador(posJugador)) {
            this->haDetectadoAlJugador = true;
        }
    }

    if (this->haDetectadoAlJugador) {
        this->direccion = Vector2Normalize(Vector2Subtract(posJugador, this->posicion));
    }
}

void MonstruoObeso::dibujar() {
    DrawCircleV(this->posicion, this->radio, DARKGREEN);

    Vector2 posCara = Vector2Add(this->posicion, Vector2Scale(this->direccion, this->radio));
    DrawRectangle(posCara.x - 5, posCara.y - 5, 10, 10, GREEN);
}

// --- ¡¡LOGICA DE DAÑO AÑADIDA!! ---
void MonstruoObeso::atacar(Protagonista& jugador) {
    // Ahora el MonstruoObeso es responsable de su propio daño
    jugador.recibirDanio(this->danio);
}
