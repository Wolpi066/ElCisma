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
    Rectangle rectJugador = jugador.getRect(); // Obtenemos el rect del jugador una vez

    // 1. Balas vs Entidades
    for (Bala* bala : balas) {
        if (!bala->estaActiva()) continue;
        Rectangle rectBala = bala->getRect();

        if (bala->getOrigen() == OrigenBala::JUGADOR) {
            // vs Enemigos
            for (Enemigo* enemigo : enemigos) {
                if (enemigo->estaVivo() && CheckCollisionRecs(rectBala, enemigo->getRect())) {
                    int danio = bala->esDisparoCheat() ? 1000 : bala->getDanio();
                    bool enemigoMurio = (enemigo->getVida() <= danio);
                    enemigo->recibirDanio(danio);
                    bala->desactivar();
                    if (enemigoMurio) {
                        if (!dynamic_cast<Fantasma*>(enemigo))
                        {
                            if (GetRandomValue(1, 10) <= 9) {
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
                if (jefe->estaVivo() && !jefe->esInvulnerableActualmente() && CheckCollisionRecs(rectBala, jefe->getRect())) {
                    if (bala->esDisparoCheat()) {
                        jefe->forzarFaseDos();
                    } else {
                        jefe->recibirDanio(bala->getDanio(), jugador.getPosicion());
                    }
                    bala->desactivar();
                    break;
                }
            }
        }
        // Balas Enemigas
        else if (bala->getOrigen() == OrigenBala::ENEMIGO) {
            // vs Jugador
            if (jugador.estaVivo() && CheckCollisionRecs(rectBala, rectJugador)) {
                float tiempoInmuneAntes = jugador.getTiempoInmune();
                jugador.recibirDanio(bala->getDanio());
                bala->desactivar();
                float tiempoInmuneDespues = jugador.getTiempoInmune();
                if (tiempoInmuneAntes <= 0.0f && tiempoInmuneDespues > 0.0f)
                {
                    Vector2 dirKnockback = Vector2Normalize(bala->getVelocidad());
                    jugador.aplicarKnockback(dirKnockback, 2.0f, 0.15f);
                }
            }
        }
    }

    // 2. Comprobacion de Ataques Enemigos
    for (Enemigo* enemigo : enemigos) {
        if (!enemigo->estaVivo()) continue;
        if (Fantasma* f = dynamic_cast<Fantasma*>(enemigo))
        {
            if (CheckCollisionRecs(rectJugador, f->getRect())) {
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

        // A. Daño por Contacto (Cuerpo a Cuerpo)
        if (CheckCollisionRecs(rectJugador, jefe->getRect())) {
            float tiempoInmuneAntes = jugador.getTiempoInmune();
            jugador.recibirDanio(jefe->getDanioContacto());
            float tiempoInmuneDespues = jugador.getTiempoInmune();
            if (tiempoInmuneAntes <= 0.0f && tiempoInmuneDespues > 0.0f)
            {
                Vector2 dirKnockback = Vector2Normalize(Vector2Subtract(jugador.getPosicion(), jefe->getPosicion()));
                jugador.aplicarKnockback(dirKnockback, 8.0f, 0.3f);
            }
        }

        // --- ¡¡NUEVA COLISIÓN!! ---
        // B. Daño por Ataque de Brazo (Fase 1)
        if (jefe->getFase() == FaseJefe::FASE_UNO && jefe->getEstadoF1() == EstadoFaseUno::ESTIRANDO_BRAZO)
        {
            Rectangle hitboxBrazo = jefe->getHitboxBrazo();
            if (hitboxBrazo.width > 0 && CheckCollisionRecs(rectJugador, hitboxBrazo))
            {
                 float tiempoInmuneAntes = jugador.getTiempoInmune();
                 jugador.recibirDanio(15); // Daño del brazo (hardcodeado 15)
                 float tiempoInmuneDespues = jugador.getTiempoInmune();
                 if (tiempoInmuneAntes <= 0.0f && tiempoInmuneDespues > 0.0f)
                 {
                    Vector2 dirKnockback = Vector2Normalize(Vector2Subtract(jugador.getPosicion(), jefe->getPosicion()));
                    jugador.aplicarKnockback(dirKnockback, 10.0f, 0.2f); // Fuerte knockback
                 }
            }
        }
        // --- FIN NUEVA COLISIÓN ---
    }
}
