#include "MotorFisica.h"
#include "raymath.h"
#include "Constantes.h" // Para velocidades

// --- LOGICA DE MOVIMIENTO Y COLISION AABB ---

// --- ¢B¢BFIRMA ACTUALIZADA!! ---
Vector2 MotorFisica::calcularMovimientoValido(
    Vector2 posActual,
    Vector2 velActual,
    Rectangle rectColision,
    const std::vector<Rectangle>& muros,
    const std::vector<Rectangle>& cajas,
    const Rectangle& puerta, // ¢B¢BNUEVO!!
    bool puertaEstaAbierta   // ¢B¢BNUEVO!!
)
{
    Vector2 nuevaPos = posActual;

    // --- Comprobar Eje X ---
    nuevaPos.x += velActual.x;
    rectColision.x = nuevaPos.x - (rectColision.width / 2);

    // Chequeo contra muros
    for (const auto& obs : muros) {
        if (CheckCollisionRecs(rectColision, obs)) {
            nuevaPos.x = posActual.x;
            rectColision.x = posActual.x - (rectColision.width / 2);
            goto ChequeoEjeY_X; // Optimizar: si choca, no seguir
        }
    }
    // Chequeo contra cajas
    for (const auto& obs : cajas) {
        if (CheckCollisionRecs(rectColision, obs)) {
            nuevaPos.x = posActual.x;
            rectColision.x = posActual.x - (rectColision.width / 2);
            goto ChequeoEjeY_X;
        }
    }
    // Chequeo contra puerta
    if (!puertaEstaAbierta) {
        if (CheckCollisionRecs(rectColision, puerta)) {
             nuevaPos.x = posActual.x;
             rectColision.x = posActual.x - (rectColision.width / 2);
        }
    }

ChequeoEjeY_X:
    // --- Comprobar Eje Y ---
    nuevaPos.y += velActual.y;
    rectColision.y = nuevaPos.y - (rectColision.height / 2);

    // Chequeo contra muros
    for (const auto& obs : muros) {
        if (CheckCollisionRecs(rectColision, obs)) {
            nuevaPos.y = posActual.y;
            rectColision.y = posActual.y - (rectColision.height / 2);
            goto FinChequeo_Y;
        }
    }
    // Chequeo contra cajas
    for (const auto& obs : cajas) {
        if (CheckCollisionRecs(rectColision, obs)) {
            nuevaPos.y = posActual.y;
            rectColision.y = posActual.y - (rectColision.height / 2);
            goto FinChequeo_Y;
        }
    }
    // Chequeo contra puerta
    if (!puertaEstaAbierta) {
        if (CheckCollisionRecs(rectColision, puerta)) {
             nuevaPos.y = posActual.y;
             rectColision.y = posActual.y - (rectColision.height / 2);
        }
    }

FinChequeo_Y:
    return nuevaPos;
}

// --- METODOS PUBLICOS ---

// --- ¢B¢BFIRMA ACTUALIZADA!! ---
void MotorFisica::moverJugador(
    Protagonista& jugador,
    Vector2 dirMovimiento,
    const std::vector<Rectangle>& muros,
    const std::vector<Rectangle>& cajas,
    const Rectangle& puerta,
    bool puertaEstaAbierta
)
{
    if (!jugador.estaVivo()) return;

    Vector2 velocidad = {0, 0};
    if (Vector2Length(dirMovimiento) > 0.0f) {
        velocidad = Vector2Scale(Vector2Normalize(dirMovimiento), Constantes::VELOCIDAD_JUGADOR);
    }

    Rectangle rectJugador = jugador.getRect();

    // --- LLAMADA ACTUALIZADA ---
    Vector2 nuevaPos = calcularMovimientoValido(
        jugador.getPosicion(),
        velocidad,
        rectJugador,
        muros,
        cajas,
        puerta,
        puertaEstaAbierta
    );

    jugador.setPosicion(nuevaPos);
}

// --- ¢B¢BFIRMA ACTUALIZADA!! ---
void MotorFisica::moverEnemigos(
    std::vector<Enemigo*>& enemigos,
    const std::vector<Rectangle>& muros,
    const std::vector<Rectangle>& cajas,
    const Rectangle& puerta,
    bool puertaEstaAbierta
)
{
    for (Enemigo* enemigo : enemigos) {
        if (!enemigo->estaVivo()) continue;

        if (dynamic_cast<Fantasma*>(enemigo)) {
            continue;
        }

        Vector2 velocidad = Vector2Scale(enemigo->getDireccion(), enemigo->getVelocidad());
        Rectangle rectEnemigo = enemigo->getRect();

        // --- LLAMADA ACTUALIZADA ---
        Vector2 nuevaPos = calcularMovimientoValido(
            enemigo->getPosicion(),
            velocidad,
            rectEnemigo,
            muros,
            cajas,
            puerta,
            puertaEstaAbierta
        );

        enemigo->setPosicion(nuevaPos);
    }
}

// --- ¢B¢BFIRMA ACTUALIZADA!! ---
void MotorFisica::moverJefes(
    std::vector<Jefe*>& jefes,
    const std::vector<Rectangle>& muros,
    const std::vector<Rectangle>& cajas,
    const Rectangle& puerta,
    bool puertaEstaAbierta
)
{
    for (Jefe* jefe : jefes) {
        if (!jefe->estaVivo()) continue;

        Vector2 velocidad = jefe->getVelocidadActual();
        Rectangle rectJefe = jefe->getRect();

        // --- LLAMADA ACTUALIZADA ---
        Vector2 nuevaPos = calcularMovimientoValido(
            jefe->getPosicion(),
            velocidad,
            rectJefe,
            muros,
            cajas,
            puerta,
            puertaEstaAbierta
        );

        jefe->setPosicion(nuevaPos);
    }
}

// --- ¢B¢BFIRMA ACTUALIZADA!! ---
void MotorFisica::moverBalas(
    std::vector<Bala*>& balas,
    const std::vector<Rectangle>& muros,
    const std::vector<Rectangle>& cajas,
    const Rectangle& puerta,
    bool puertaEstaAbierta
)
{
    for (Bala* bala : balas) {
        if (!bala->estaActiva()) continue;

        Vector2 velocidad = bala->getVelocidad();
        Vector2 nuevaPos = Vector2Add(bala->getPosicion(), velocidad);

        Rectangle rectBala = bala->getRect();
        rectBala.x = nuevaPos.x - rectBala.width / 2;
        rectBala.y = nuevaPos.y - rectBala.height / 2;

        bool choca = false;
        // Chequeo contra muros
        for (const auto& obs : muros) {
            if (CheckCollisionRecs(rectBala, obs)) {
                choca = true;
                break;
            }
        }
        // Chequeo contra cajas
        if (!choca) {
            for (const auto& obs : cajas) {
                if (CheckCollisionRecs(rectBala, obs)) {
                    choca = true;
                    break;
                }
            }
        }

        // Chequeo contra puerta
        if (!puertaEstaAbierta && !choca) {
            if (CheckCollisionRecs(rectBala, puerta)) {
                choca = true;
            }
        }

        if (choca) {
            bala->desactivar();
        } else {
            bala->setPosicion(nuevaPos);
        }
    }
}
