#include "MotorColisiones.h"
#include "Spawner.h"
#include "Botiquin.h"
#include "CajaDeMuniciones.h"
#include "Armadura.h"
#include "Fantasma.h"
#include "IndicadorPuerta.h"
#include "raymath.h"
#include "Jefe.h"
#include "TrozoDeCarne.h"
#include "MinaEnemiga.h"

void MotorColisiones::procesar(
    Protagonista& jugador,
    GestorEntidades& gestor)
{
    std::vector<Enemigo*>& enemigos = gestor.getEnemigos();
    std::vector<Bala*>& balas = gestor.getBalas();
    std::vector<Jefe*>& jefes = gestor.getJefes();
    Rectangle rectJugador = jugador.getRect();

    for (Bala* bala : balas) {
        if (!bala->estaActiva()) continue;
        Rectangle rectBala = bala->getRect();

        if (bala->getOrigen() == OrigenBala::JUGADOR) {

            if (dynamic_cast<MinaEnemiga*>(bala) || dynamic_cast<TrozoDeCarne*>(bala))
            {
            }
            else
            {
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
                if (!bala->estaActiva()) continue;

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
                if (!bala->estaActiva()) continue;

                for (Bala* balaEnemiga : balas)
                {
                    if (balaEnemiga->getOrigen() == OrigenBala::ENEMIGO && CheckCollisionRecs(rectBala, balaEnemiga->getRect()))
                    {
                        balaEnemiga->recibirDanio(bala->getDanio(), OrigenBala::JUGADOR);
                        bala->desactivar();
                        break;
                    }
                }
            }
        }
        else if (bala->getOrigen() == OrigenBala::ENEMIGO) {
            if (jugador.estaVivo() && CheckCollisionRecs(rectBala, rectJugador)) {

                if (TrozoDeCarne* charco = dynamic_cast<TrozoDeCarne*>(bala))
                {
                    if (charco->esCharco())
                    {
                        continue;
                    }
                }

                MinaEnemiga* mina = dynamic_cast<MinaEnemiga*>(bala);
                if (mina && !mina->estaExplotando())
                {
                    mina->explotar(false);
                }

                float tiempoInmuneAntes = jugador.getTiempoInmune();
                jugador.recibirDanio(bala->getDanio());

                if (mina)
                {
                    if (!mina->estaExplotando()) mina->desactivar();
                } else {
                    bala->desactivar();
                }

                float tiempoInmuneDespues = jugador.getTiempoInmune();
                if (tiempoInmuneAntes <= 0.0f && tiempoInmuneDespues > 0.0f)
                {
                    Vector2 dirKnockback = Vector2Normalize(bala->getVelocidad());
                    if (Vector2LengthSqr(dirKnockback) == 0.0f)
                    {
                        dirKnockback = Vector2Normalize(Vector2Subtract(jugador.getPosicion(), bala->getPosicion()));
                    }
                    jugador.aplicarKnockback(dirKnockback, 2.0f, 0.15f);
                }
            }

            if (MinaEnemiga* mina = dynamic_cast<MinaEnemiga*>(bala))
            {
                if (mina->estaExplotando() && mina->explosionPuedeHerirJefe())
                {
                    for (Jefe* jefe : jefes)
                    {
                        if (jefe->estaVivo() && !jefe->esInvulnerableActualmente() && CheckCollisionRecs(mina->getRect(), jefe->getRect()))
                        {
                            jefe->recibirDanio(mina->getDanio(), mina->getPosicion());
                        }
                    }
                }
            }
        }
    }

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

    for (Jefe* jefe : jefes) {
        if (!jefe->estaVivo() || jefe->esInvulnerableActualmente()) continue;

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

        if (jefe->getFase() == FaseJefe::FASE_UNO && jefe->getEstadoF1() == EstadoFaseUno::ESTIRANDO_BRAZO)
        {
            Rectangle hitboxBrazo = jefe->getHitboxBrazo();
            if (hitboxBrazo.width > 0 && CheckCollisionRecs(rectJugador, hitboxBrazo))
            {
                 float tiempoInmuneAntes = jugador.getTiempoInmune();
                 jugador.recibirDanio(5);
                 float tiempoInmuneDespues = jugador.getTiempoInmune();
                 if (tiempoInmuneAntes <= 0.0f && tiempoInmuneDespues > 0.0f)
                 {
                    Vector2 dirKnockback = Vector2Normalize(Vector2Subtract(jugador.getPosicion(), jefe->getPosicion()));
                    jugador.aplicarKnockback(dirKnockback, 10.0f, 0.2f);
                 }
            }
        }
    }
}
