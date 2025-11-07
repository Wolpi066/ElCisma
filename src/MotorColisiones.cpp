#include "MotorColisiones.h"
#include "Spawner.h"
#include "Botiquin.h"
#include "CajaDeMuniciones.h"
#include "Armadura.h"
#include "Fantasma.h"
#include "IndicadorPuerta.h"
#include "raymath.h"
#include "Jefe.h"

void MotorColisiones::procesar(
    Protagonista& jugador,
    GestorEntidades& gestor)
{
    std::vector<Enemigo*>& enemigos = gestor.getEnemigos();
    std::vector<Bala*>& balas = gestor.getBalas();
    std::vector<Jefe*>& jefes = gestor.getJefes();

    // 1. Balas vs Entidades
    for (Bala* bala : balas) {
        if (!bala->estaActiva()) continue;

        if (bala->getOrigen() == OrigenBala::JUGADOR) {
            // vs Enemigos
            for (Enemigo* enemigo : enemigos) {
                if (enemigo->estaVivo() && CheckCollisionRecs(bala->getRect(), enemigo->getRect())) {

                    int danio = bala->esDisparoCheat() ? 1000 : bala->getDanio();

                    bool enemigoMurio = (enemigo->getVida() <= danio);
                    enemigo->recibirDanio(danio);
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
                if (jefe->estaVivo() && !jefe->esInvulnerableActualmente() && CheckCollisionRecs(bala->getRect(), jefe->getRect())) {

                    // --- ¡¡LÓGICA CHEAT MODIFICADA!! ---
                    if (bala->esDisparoCheat()) {
                        jefe->forzarFaseDos(); // ¡¡NUEVO!! No hace daño, fuerza la fase
                    } else {
                        jefe->recibirDanio(bala->getDanio(), jugador.getPosicion());
                    }
                    // -----------------------------

                    bala->desactivar();
                    break;
                }
            }
        }
        // Balas Enemigas
        else if (bala->getOrigen() == OrigenBala::ENEMIGO) {
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

    // 2. Comprobacion de Ataques Enemigos
    for (Enemigo* enemigo : enemigos) {
        if (!enemigo->estaVivo()) continue;

        if (Fantasma* f = dynamic_cast<Fantasma*>(enemigo))
        {
            if (CheckCollisionRecs(jugador.getRect(), f->getRect())) {
                f->atacar(jugador);
            }
        }
        else if (enemigo->estaListoParaAtacar())
        {
            enemigo->atacar(jugador);
        }
    }

    // 3. Comprobación de Daño por Contacto (Jefe vs Jugador)
    for (Jefe* jefe : jefes) {
        if (!jefe->estaVivo() || jefe->esInvulnerableActualmente()) continue;

        if (CheckCollisionRecs(jugador.getRect(), jefe->getRect())) {

            float tiempoInmuneAntes = jugador.getTiempoInmune();
            jugador.recibirDanio(jefe->getDanioContacto());
            float tiempoInmuneDespues = jugador.getTiempoInmune();

            if (tiempoInmuneAntes <= 0.0f && tiempoInmuneDespues > 0.0f)
            {
                Vector2 dirKnockback = Vector2Normalize(Vector2Subtract(jugador.getPosicion(), jefe->getPosicion()));
                float fuerza = 8.0f;
                float duracion = 0.3f;
                jugador.aplicarKnockback(dirKnockback, fuerza, duracion);
            }
        }
    }
}
