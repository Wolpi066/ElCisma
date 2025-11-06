#include "GestorEntidades.h"
#include "Fantasma.h" // <-- ¡Añadido! Necesario para la comprobacion
#include "Mapa.h"     // <-- ¡AÑADIDO!

GestorEntidades::GestorEntidades()
{
}

GestorEntidades::~GestorEntidades()
{
    // El destructor de Juego llamara a limpiarTodo()
}

// Actualiza la IA (intencion de movimiento) de las entidades
// ¡MODIFICADO!
void GestorEntidades::actualizarIAEntidades(Protagonista& jugador, const Mapa& mapa)
{
    Vector2 posJugador = jugador.getPosicion();

    for (Enemigo* enemigo : enemigos) {
        enemigo->actualizarIA(posJugador, mapa); // <-- CORREGIDO Y MODIFICADO
    }

    for (Jefe* jefe : jefes) {
        jefe->actualizar(jugador);
    }

    for (Bala* bala : balas) {
        bala->actualizarVidaUtil(posJugador); // <-- CORREGIDO
    }
}

void GestorEntidades::dibujarEntidades()
{
    for (Consumible* item : consumibles) {
        item->dibujar();
    }
    for (Enemigo* enemigo : enemigos) {
        // --- ¡MODIFICACION! ---
        // Si el enemigo es el fantasma y esta en modo "susto",
        // no lo dibujamos aqui (se dibujara en SistemaRender)
        if (dynamic_cast<Fantasma*>(enemigo) && Fantasma::estaAsustando && !Fantasma::despertado)
        {
            continue; // Saltar este dibujo
        }
        // ---------------------
        enemigo->dibujar();
    }
    for (Jefe* jefe : jefes) {
        jefe->dibujar();
    }
    for (Bala* bala : balas) {
        bala->dibujar();
    }
}

void GestorEntidades::recolectarBasura()
{
    limpiarLista(enemigos);
    limpiarLista(balas);
    limpiarLista(consumibles);
    limpiarLista(jefes);
}

void GestorEntidades::limpiarTodo()
{
    for (Enemigo* enemigo : enemigos) delete enemigo;
    for (Bala* bala : balas) delete bala;
    for (Consumible* item : consumibles) delete item;
    for (Jefe* jefe : jefes) delete jefe;

    enemigos.clear();
    balas.clear();
    consumibles.clear();
    jefes.clear();
}

// --- CORREGIDO: Nombres de metodos ---
void GestorEntidades::registrarEnemigo(Enemigo* enemigo) {
    enemigos.push_back(enemigo);
}
void GestorEntidades::registrarBala(Bala* bala) {
    balas.push_back(bala);
}
void GestorEntidades::registrarConsumible(Consumible* consumible) {
    consumibles.push_back(consumible);
}
void GestorEntidades::registrarJefe(Jefe* jefe) {
    jefes.push_back(jefe);
}

// --- Getters ---
std::vector<Enemigo*>& GestorEntidades::getEnemigos() {
    return enemigos;
}
std::vector<Bala*>& GestorEntidades::getBalas() {
    return balas;
}
std::list<Consumible*>& GestorEntidades::getConsumibles() {
    return consumibles;
}
std::vector<Jefe*>& GestorEntidades::getJefes() {
    return jefes;
}
