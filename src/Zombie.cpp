#include "Zombie.h"
#include "Constantes.h"
#include "raymath.h"
#include "Protagonista.h" // ¡¡Importante!!

// --- CORREGIDO: Constructor llama a 8 args ---
Zombie::Zombie(Vector2 pos)
    : Enemigo(pos,
              Constantes::VIDA_ZOMBIE,
              Constantes::DANIO_ZOMBIE,
              Constantes::VELOCIDAD_ZOMBIE,
              Constantes::RADIO_ZOMBIE,
              Constantes::RANGO_VISUAL_ZOMBIE,
              Constantes::ANGULO_CONO_ZOMBIE,
              Constantes::RANGO_AUDIO_ZOMBIE)
{
}

// --- CORREGIDO: Nombre de la funcion ---
void Zombie::actualizarIA(Vector2 posJugador) {

    if (!this->haDetectadoAlJugador) {
        if (puedeVearAlJugador(posJugador) || puedeEscucharAlJugador(posJugador)) {
            this->haDetectadoAlJugador = true;
        }
    }

    if (this->haDetectadoAlJugador) {
        // Solo actualiza la direccion, no la posicion
        this->direccion = Vector2Normalize(Vector2Subtract(posJugador, this->posicion));
    }
}

void Zombie::dibujar() {
    DrawCircleV(this->posicion, this->radio, GREEN);

    Vector2 posCara = Vector2Add(this->posicion, Vector2Scale(this->direccion, this->radio));
    DrawRectangle(posCara.x - 3, posCara.y - 3, 6, 6, DARKGREEN);
}

// --- ¡¡LOGICA DE DAÑO AÑADIDA!! ---
void Zombie::atacar(Protagonista& jugador) {
    // Ahora el Zombie es responsable de su propio daño
    jugador.recibirDanio(this->danio);
}
