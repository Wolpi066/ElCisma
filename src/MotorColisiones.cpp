#include "MotorColisiones.h"
#include "Spawner.h" // Necesario para el drop de enemigos
#include "Botiquin.h"
#include "CajaDeMuniciones.h"
#include "Armadura.h"
#include "Fantasma.h" // ¡¡Importante!!
#include "IndicadorPuerta.h"
#include "raymath.h"

void MotorColisiones::procesar(
    Protagonista& jugador,
    GestorEntidades& gestor)
{
    // Obtenemos las listas del gestor
    std::vector<Enemigo*>& enemigos = gestor.getEnemigos();
    std::vector<Bala*>& balas = gestor.getBalas();
    std::vector<Jefe*>& jefes = gestor.getJefes();

    // 1. Balas vs Entidades
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
                        if (!dynamic_cast<Fantasma*>(enemigo))
                        {
                            if (GetRandomValue(1, 10) <= 9) { // 90%
                                int tipoDrop = GetRandomValue(0, 2);
                                Vector2 posDrop = enemigo->getPosicion();
                                switch(tipoDrop) {
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
            if (jugador.estaVivo() && CheckCollisionRecs(bala->getRect(), jugador.getRect())) {
                float tiempoInmuneAntes = jugador.getTiempoInmune();
                jugador.recibirDanio(bala->getDanio());
                bala->desactivar();
                float tiempoInmuneDespues = jugador.getTiempoInmune();
                if (tiempoInmuneAntes <= 0.0f && tiempoInmuneDespues > 0.0f)
                {
                    Vector2 dirKnockback = Vector2Normalize(bala->getVelocidad());
                    float fuerza = 2.0f;
                    float duracion = 0.15f;
                    jugador.aplicarKnockback(dirKnockback, fuerza, duracion);
                }
            }
        }
    }

    // --- ¡¡BLOQUE REFACTORIZADO!! ---
    // 2. Comprobacion de Ataques Enemigos
    for (Enemigo* enemigo : enemigos) {
        if (!enemigo->estaVivo()) continue;

        // Opcion A: El Fantasma ataca por contacto (como antes)
        if (Fantasma* f = dynamic_cast<Fantasma*>(enemigo))
        {
            if (CheckCollisionRecs(jugador.getRect(), f->getRect())) {
                f->atacar(jugador); // El fantasma se autogestiona
            }
        }
        // Opcion B: El resto de enemigos ataca segun su IA de "lunge"
        else if (enemigo->estaListoParaAtacar())
        {
            // Si la IA dice que esta listo (pausa terminada),
            // llamamos a su funcion de ataque.
            enemigo->atacar(jugador);
        }
    }
}
