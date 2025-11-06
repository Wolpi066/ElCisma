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

// --- ¡¡FUNCIÓN COMPLETAMENTE MODIFICADA CON EVASIÓN!! ---
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

        // El Fantasma tiene su propio movimiento (dentro de su IA)
        // y no colisiona, así que lo saltamos.
        if (dynamic_cast<Fantasma*>(enemigo)) {
            continue;
        }

        // --- INICIO DE LA LÓGICA DE STEERING ---

        // 1. OBTENER DIRECCIÓN DESEADA (Del "cerebro" / FSM)
        Vector2 dirDeseada = enemigo->getDireccion();

        // Si el enemigo no quiere moverse (dir 0,0), no hacemos nada
        if (Vector2LengthSqr(dirDeseada) == 0.0f) {
            continue;
        }

        Vector2 vectorEvitacion = { 0, 0 };
        float pesoEvitacion = 0.0f;

        // 2. DEFINIR EL SENSOR DE EVASIÓN
        // Usaremos un rectángulo (el del enemigo) proyectado hacia adelante
        Rectangle rectEnemigo = enemigo->getRect();
        float distSensor = rectEnemigo.width; // Miramos un "radio" hacia adelante

        // Posicionamos el sensor
        Vector2 posSensor = Vector2Add(enemigo->getPosicion(), Vector2Scale(dirDeseada, distSensor));
        Rectangle rectSensor = rectEnemigo;
        rectSensor.x = posSensor.x - rectSensor.width / 2;
        rectSensor.y = posSensor.y - rectSensor.height / 2;

        const std::vector<Rectangle>* listasObstaculos[] = {&muros, &cajas};

        // 3. COMPROBAR EL SENSOR CONTRA OBSTÁCULOS
        for (const auto& lista : listasObstaculos) {
            for (const auto& obs : *lista) {
                if (CheckCollisionRecs(rectSensor, obs)) {
                    // ¡Obstáculo detectado!
                    // Calculamos un vector que "empuja" desde el centro del obstáculo
                    Vector2 centroObstaculo = { obs.x + obs.width / 2, obs.y + obs.height / 2 };
                    Vector2 vectorAlejamiento = Vector2Normalize(Vector2Subtract(posSensor, centroObstaculo));

                    vectorEvitacion = Vector2Add(vectorEvitacion, vectorAlejamiento);
                    pesoEvitacion += 1.0f;
                }
            }
        }

        // Comprobar la puerta (si está cerrada)
        if (!puertaEstaAbierta && CheckCollisionRecs(rectSensor, puerta)) {
             Vector2 centroObstaculo = { puerta.x + puerta.width / 2, puerta.y + puerta.height / 2 };
             Vector2 vectorAlejamiento = Vector2Normalize(Vector2Subtract(posSensor, centroObstaculo));
             vectorEvitacion = Vector2Add(vectorEvitacion, vectorAlejamiento);
             pesoEvitacion += 1.0f;
        }

        Vector2 dirFinal;

        // 4. COMBINAR VECTORES (Steering)
        if (pesoEvitacion > 0.0f) {
            // Si detectamos obstáculos, combinamos las "fuerzas"
            vectorEvitacion = Vector2Normalize(vectorEvitacion);

            // Ponderamos: Damos más peso a la evasión (1.5) que al deseo (1.0)
            Vector2 fuerzaDeseo = Vector2Scale(dirDeseada, 1.0f);
            Vector2 fuerzaEvitacion = Vector2Scale(vectorEvitacion, 1.5f);

            dirFinal = Vector2Normalize(Vector2Add(fuerzaDeseo, fuerzaEvitacion));
        } else {
            // Si no hay nada que evitar, usamos la dirección deseada
            dirFinal = dirDeseada;
        }

        // 5. CALCULAR VELOCIDAD FINAL Y MOVER
        // Usamos la 'dirFinal' (inteligente) en lugar de la 'dirDeseada' (tonta)
        Vector2 velocidad = Vector2Scale(dirFinal, enemigo->getVelocidad());

        // --- ¡¡NUEVA LÍNEA!! ---
        // Actualizamos la "cara" del enemigo para que coincida con el movimiento real
        enemigo->setDireccion(dirFinal);
        // -------------------------

        // 6. DELEGAR AL CÁLCULO DE MOVIMIENTO VÁLIDO
        // (El motor AABB existente se encarga de la colisión final)
        Vector2 nuevaPos = calcularMovimientoValido(
            enemigo->getPosicion(),
            velocidad, // <-- ¡Velocidad "inteligente"!
            rectEnemigo,
            muros,
            cajas,
            puerta,
            puertaEstaAbierta
        );

        enemigo->setPosicion(nuevaPos);

        // --- FIN DE LA LÓGICA DE STEERING ---
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
        // NOTA: El Jefe aún no usa el steering, se mueve como antes.
        // Podríamos implementarlo para él también si se atasca.
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
