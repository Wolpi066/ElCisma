#pragma once

#include <vector>
#include <list>
#include "Protagonista.h"
#include "Enemigo.h"
#include "Jefe.h"
#include "Bala.h"
#include "Consumible.h"

#include "Spawner.h"
#include "Botiquin.h"
#include "CajaDeMuniciones.h"
#include "Armadura.h"

class Mapa;

class GestorEntidades
{
private:
    std::vector<Enemigo*> enemigos;
    std::vector<Bala*> balas;
    std::list<Consumible*> consumibles;
    std::vector<Jefe*> jefes;

    template <typename T>
    void limpiarLista(std::vector<T*>& lista) {
        auto it = lista.begin();
        while (it != lista.end()) {
            if ((*it)->estaMuerto() || (*it)->estaConsumido()) {
                delete *it;
                it = lista.erase(it);
            } else {
                ++it;
            }
        }
    }

    template <typename T>
    void limpiarLista(std::list<T*>& lista) {
        auto it = lista.begin();
        while (it != lista.end()) {
            if ((*it)->estaMuerto() || (*it)->estaConsumido()) {
                delete *it;
                it = lista.erase(it);
            } else {
                ++it;
            }
        }
    }

public:
    GestorEntidades();
    ~GestorEntidades();

    void actualizarIAEntidades(Protagonista& jugador, const Mapa& mapa);
    void dibujarEntidades();
    void recolectarBasura();
    void limpiarTodo();

    void registrarEnemigo(Enemigo* enemigo);
    void registrarBala(Bala* bala);
    void registrarConsumible(Consumible* consumible);
    void registrarJefe(Jefe* jefe);

    std::vector<Enemigo*>& getEnemigos();
    std::vector<Bala*>& getBalas();
    std::list<Consumible*>& getConsumibles();
    std::vector<Jefe*>& getJefes();
};
