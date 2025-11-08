#include "GestorEntidades.h"
#include "Mapa.h"
#include "BalaMonstruosa.h"
#include "MinaEnemiga.h" // <-- ¡¡NUEVO!!
#include "Constantes.h"

GestorEntidades::GestorEntidades()
{
}

GestorEntidades::~GestorEntidades()
{
}

void GestorEntidades::actualizarIAEntidades(Protagonista& jugador, const Mapa& mapa)
{
    Vector2 posJugador = jugador.getPosicion();

    // Actualiza Enemigos
    for (Enemigo* enemigo : enemigos) {
        if (enemigo->estaVivo()) {
            enemigo->actualizarBase();
            enemigo->actualizarIA(posJugador, mapa);
        }
    }

    // Actualiza Jefes
    for (Jefe* jefe : jefes) {
        if (jefe->estaVivo()) {

            jefe->actualizar(jugador, mapa);

            // --- ¡¡LÓGICA DE DISPARO MODIFICADA!! ---
            // 2. Tomamos las balas que el jefe generó en su 'actualizar()'
            std::vector<Bala*>& balasNuevas = jefe->getBalasGeneradas();
            if (!balasNuevas.empty())
            {
                for (Bala* bala : balasNuevas)
                {
                    // Las registramos en el gestor
                    registrarBala(bala);
                }
                jefe->limpiarBalasGeneradas(); // ¡Importante!
            }
            // --- FIN LÓGICA MODIFICADA ---
        }
    }

    // --- ¡¡NUEVA LÓGICA DE MINAS!! ---
    // 3. Actualizamos las balas (para las minas)
    for (Bala* bala : balas) {
        if (bala->estaActiva()) {
            // Llama al 'actualizar' de la bala (solo las minas lo usan)
            bala->actualizar(jugador, mapa);
        }
    }
    // -------------------------------
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


// --- Registradores ---
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
