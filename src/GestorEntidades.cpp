#include "GestorEntidades.h"
#include "Mapa.h"
#include "BalaMonstruosa.h"
#include "MinaEnemiga.h"
#include "TrozoDeCarne.h"
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

    for (Enemigo* enemigo : enemigos) {
        if (enemigo->estaVivo()) {
            enemigo->actualizarBase();
            enemigo->actualizarIA(posJugador, mapa);
        }
    }

    for (Jefe* jefe : jefes) {
        jefe->actualizar(jugador, mapa);

        std::vector<Bala*>& balasNuevas = jefe->getBalasGeneradas();
        if (!balasNuevas.empty())
        {
            for (Bala* bala : balasNuevas)
            {
                registrarBala(bala);
            }
            jefe->limpiarBalasGeneradas();
        }

        std::vector<DropInfo>& dropsNuevos = jefe->getDropsGenerados();
        if (!dropsNuevos.empty())
        {
            for (const auto& drop : dropsNuevos)
            {
                switch (drop.tipo)
                {
                    case 1: registrarConsumible(Spawner<Botiquin>::Spawn(drop.pos)); break;
                    case 3: registrarConsumible(Spawner<CajaDeMuniciones>::Spawn(drop.pos)); break;
                    case 4: registrarConsumible(Spawner<Armadura>::Spawn(drop.pos)); break;
                }
            }
            jefe->limpiarDropsGenerados();
        }
    }

    for (Bala* bala : balas) {
        if (bala->estaActiva()) {
            bala->actualizar(jugador, mapa);
        }
    }
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
        jefe->dibujar();
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
    // ¡¡NO LIMPIAMOS AL JEFE!!
    // limpiarLista(jefes);
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
