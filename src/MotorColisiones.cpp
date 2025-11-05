#include "MotorColisiones.h"
#include "Spawner.h" // Necesario para el drop de enemigos
#include "Botiquin.h"
#include "CajaDeMuniciones.h"
#include "Armadura.h"
#include "Fantasma.h"
#include "IndicadorPuerta.h" // ¡¡Añadido!!

// CAMBIO: Firma del metodo actualizada
void MotorColisiones::procesar(
    Protagonista& jugador,
    GestorEntidades& gestor)
{
    // Obtenemos las listas del gestor
    std::vector<Enemigo*>& enemigos = gestor.getEnemigos();
    std::vector<Bala*>& balas = gestor.getBalas();
    std::list<Consumible*>& consumibles = gestor.getConsumibles();
    std::vector<Jefe*>& jefes = gestor.getJefes();

    // 1. Jugador vs Consumibles (POR CONTACTO)
    for (Consumible* item : consumibles) {

        // --- ¡¡LOGICA ACTUALIZADA!! ---
        // Solo procesamos los items que NO son por tecla ('E')
        if (!item->esInteraccionPorTecla())
        {
            if (!item->estaConsumido() && CheckCollisionRecs(jugador.getRect(), item->getRect())) {
                item->usar(jugador);
            }
        }
        // Los items por tecla (Llave, etc.) se manejan en Juego.cpp
        // ---------------------------------
    }

    // 2. Balas vs Entidades
    for (Bala* bala : balas) {
        if (!bala->estaActiva()) continue;

        // Balas del Jugador
        if (bala->getOrigen() == OrigenBala::JUGADOR) {
            // vs Enemigos
            for (Enemigo* enemigo : enemigos) {
                if (enemigo->estaVivo() && CheckCollisionRecs(bala->getRect(), enemigo->getRect())) {

                    bool enemigoMurio = (enemigo->getVida() <= bala->getDanio());
                    enemigo->recibirDanio(bala->getDanio());
                    bala->desactivar();

                    if (enemigoMurio) {
                        // El fantasma no deberia dropear items
                        if (!dynamic_cast<Fantasma*>(enemigo))
                        {
                            if (GetRandomValue(1, 10) <= 9) { // 90%
                                int tipoDrop = GetRandomValue(0, 2);
                                Vector2 posDrop = enemigo->getPosicion();
                                switch(tipoDrop) {
                                    // Usamos el gestor para registrar nuevos consumibles
                                    case 0: gestor.registrarConsumible(Spawner<Botiquin>::Spawn(posDrop)); break;
                                    case 1: gestor.registrarConsumible(Spawner<CajaDeMuniciones>::Spawn(posDrop)); break;
                                    case 2: gestor.registrarConsumible(Spawner<Armadura>::Spawn(posDrop)); break;
                                }
                            }
                        }
                    }
                    break;
                }
            }
            // vs Jefe
            for (Jefe* jefe : jefes) {
                if (jefe->estaVivo() && CheckCollisionRecs(bala->getRect(), jefe->getRect())) {
                    jefe->recibirDanio(bala->getDanio());
                    bala->desactivar();
                    break;
                }
            }
        }
        // Balas Enemigas
        else if (bala->getOrigen() == OrigenBala::ENEMIGO) {
            // vs Jugador
            if (CheckCollisionRecs(bala->getRect(), jugador.getRect())) {
                jugador.recibirDanio(bala->getDanio());
                bala->desactivar();
            }
        }
    }

    // 3. Enemigos vs Jugador (Contacto)
    for (Enemigo* enemigo : enemigos) {
        if (enemigo->estaVivo() && CheckCollisionRecs(jugador.getRect(), enemigo->getRect())) {
            enemigo->atacar(jugador);
        }
    }
}
