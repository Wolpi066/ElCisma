#include "MotorColisiones.h"
#include "Spawner.h" // Necesario para el drop de enemigos
#include "Botiquin.h"
#include "CajaDeMuniciones.h"
#include "Armadura.h"
#include "Fantasma.h"
#include "IndicadorPuerta.h" // ¡¡Añadido!!
#include "raymath.h" // ¡¡AÑADIDO!! Necesario para Vector2Subtract y Normalize

// CAMBIO: Firma del metodo actualizada
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
            if (jugador.estaVivo() && CheckCollisionRecs(bala->getRect(), jugador.getRect())) {

                // --- ¡¡LOGICA DE KNOCKBACK!! ---
                float tiempoInmuneAntes = jugador.getTiempoInmune();

                jugador.recibirDanio(bala->getDanio());
                bala->desactivar();

                float tiempoInmuneDespues = jugador.getTiempoInmune();

                // Si el tiempoInmune se acaba de activar, ¡el golpe fue exitoso!
                if (tiempoInmuneAntes <= 0.0f && tiempoInmuneDespues > 0.0f)
                {
                    // Empujado en la direccion de la bala
                    Vector2 dirKnockback = Vector2Normalize(bala->getVelocidad());
                    // --- ¡¡VALORES AJUSTADOS!! ---
                    float fuerza = 2.0f; // Mas lento
                    float duracion = 0.15f; // Mas fluido
                    jugador.aplicarKnockback(dirKnockback, fuerza, duracion);
                }
                // -------------------------------------
            }
        }
    }

    // 2. Enemigos vs Jugador (Contacto)
    for (Enemigo* enemigo : enemigos) {
        if (enemigo->estaVivo() && CheckCollisionRecs(jugador.getRect(), enemigo->getRect())) {

            // --- ¡¡LOGICA DE KNOCKBACK!! ---
            float tiempoInmuneAntes = jugador.getTiempoInmune();

            enemigo->atacar(jugador); // Intenta hacer daño (zombie, obeso) o matar (fantasma)

            // Solo aplica knockback si el jugador SIGUE vivo despues del ataque
            if (jugador.estaVivo())
            {
                float tiempoInmuneDespues = jugador.getTiempoInmune();

                // Si el tiempoInmune se acaba de activar, ¡el golpe fue exitoso!
                if (tiempoInmuneAntes <= 0.0f && tiempoInmuneDespues > 0.0f)
                {
                    // Empujado lejos del enemigo
                    Vector2 dirKnockback = Vector2Normalize(Vector2Subtract(jugador.getPosicion(), enemigo->getPosicion()));

                    // Fallback por si estan perfectamente superpuestos
                    if (Vector2LengthSqr(dirKnockback) == 0.0f) {
                        dirKnockback = {1.0f, 0.0f}; // Empujar a la derecha por defecto
                    }

                    // --- ¡¡VALORES AJUSTADOS!! ---
                    float fuerza = 3.0f; // Mas lento
                    float duracion = 0.2f; // Mas fluido
                    jugador.aplicarKnockback(dirKnockback, fuerza, duracion);
                }
            }
            // -------------------------------------
        }
    }
}
