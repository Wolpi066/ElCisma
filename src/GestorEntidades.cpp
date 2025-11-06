#include "GestorEntidades.h"
#include "Mapa.h" // <-- ¡AÑADIDO!

GestorEntidades::GestorEntidades()
{
}

GestorEntidades::~GestorEntidades()
{
}

// ¡MODIFICADO!
void GestorEntidades::actualizarIAEntidades(Protagonista& jugador, const Mapa& mapa)
{
    // ¡¡MODIFICADO!! Ahora recibe el Mapa
    Vector2 posJugador = jugador.getPosicion();

    for (Enemigo* enemigo : enemigos) {
        if (enemigo->estaVivo()) {
            enemigo->actualizarBase(); // <-- ¡¡NUEVA LLAMADA!! Actualiza timers
            enemigo->actualizarIA(posJugador, mapa);
        }
    }

    for (Jefe* jefe : jefes) {
        if (jefe->estaVivo()) {
            // --- ¡¡ERROR CORREGIDO!! ---
            // La firma de Jefe::actualizar pide Protagonista&
            jefe->actualizar(jugador);
        }
    }

    // --- ¡¡BUCLE ELIMINADO!! ---
    // La clase Bala no tiene metodo actualizar().
    // Se mueve desde MotorFisica::moverBalas()
}


void GestorEntidades::dibujarEntidades()
{
    for (Consumible* consumible : consumibles) {
        if (!consumible->estaConsumido()) {
            consumible->dibujar();
        }
    }

    for (Enemigo* enemigo : enemigos) {
        if (enemigo->estaVivo()) {
            enemigo->dibujar();
        }
    }

    for (Jefe* jefe : jefes) {
        if (jefe->estaVivo()) {
            jefe->dibujar();
        }
    }

    for (Bala* bala : balas) {
        if (bala->estaActiva()) {
            bala->dibujar();
        }
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
    for (Enemigo* e : enemigos) delete e;
    enemigos.clear();

    for (Bala* b : balas) delete b;
    balas.clear();

    for (Consumible* c : consumibles) delete c;
    consumibles.clear();

    for (Jefe* j : jefes) delete j;
    jefes.clear();
}


// --- CORREGIDO: Nombres de métodos ---
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


// Getters
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
