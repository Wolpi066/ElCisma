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

// --- ¡¡MODIFICADO!! ---
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

    // --- ¡¡NUEVA LOGICA DE KNOCKBACK!! ---
    if (jugador.getKnockbackTimer() > 0.0f)
    {
        // Si esta en knockback, ignora el input y usa la velocidad del golpe
        velocidad = jugador.getVelocidadKnockback();
    }
    else if (Vector2Length(dirMovimiento) > 0.0f)
    {
        // Movimiento normal (controlado por el jugador)
        velocidad = Vector2Scale(Vector2Normalize(dirMovimiento), Constantes::VELOCIDAD_JUGADOR);
    }
    // ------------------------------------

    Rectangle rectJugador = jugador.getRect();

    // --- LLAMADA ACTUALIZADA ---
    Vector2 nuevaPos = calcularMovimientoValido(
        jugador.getPosicion(),
        velocidad, // Usa la velocidad (de knockback o de input)
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
        Rectangle rectEnemigo = enemigo->getRect();
        float distSensor = rectEnemigo.width;

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
                    Vector2 centroObstaculo = { obs.x + obs.width / 2, obs.y + obs.height / 2 };
                    Vector2 vectorAlejamiento = Vector2Normalize(Vector2Subtract(posSensor, centroObstaculo));
                    vectorEvitacion = Vector2Add(vectorEvitacion, vectorAlejamiento);
                    pesoEvitacion += 1.0f;
                }
            }
        }

        if (!puertaEstaAbierta && CheckCollisionRecs(rectSensor, puerta)) {
             Vector2 centroObstaculo = { puerta.x + puerta.width / 2, puerta.y + puerta.height / 2 };
             Vector2 vectorAlejamiento = Vector2Normalize(Vector2Subtract(posSensor, centroObstaculo));
             vectorEvitacion = Vector2Add(vectorEvitacion, vectorAlejamiento);
             pesoEvitacion += 1.0f;
        }

        Vector2 dirFinal;

        // 4. COMBINAR VECTORES (Steering)
        if (pesoEvitacion > 0.0f) {
            vectorEvitacion = Vector2Normalize(vectorEvitacion);
            Vector2 fuerzaDeseo = Vector2Scale(dirDeseada, 1.0f);
            Vector2 fuerzaEvitacion = Vector2Scale(vectorEvitacion, 1.5f);
            dirFinal = Vector2Normalize(Vector2Add(fuerzaDeseo, fuerzaEvitacion));
        } else {
            dirFinal = dirDeseada;
        }

        // 5. CALCULAR VELOCIDAD FINAL Y MOVER
        Vector2 velocidad = Vector2Scale(dirFinal, enemigo->getVelocidad());
        enemigo->setDireccion(dirFinal);

        // 6. DELEGAR AL CÁLCULO DE MOVIMIENTO VÁLIDO
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
        for (const auto& obs : muros) {
            if (CheckCollisionRecs(rectBala, obs)) {
                choca = true;
                break;
            }
        }
        if (!choca) {
            for (const auto& obs : cajas) {
                if (CheckCollisionRecs(rectBala, obs)) {
                    choca = true;
                    break;
                }
            }
        }

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

// --- ¡¡NUEVA FUNCION DE SEPARACION!! ---
void MotorFisica::resolverColisionesDinamicas(Protagonista& jugador, std::vector<Enemigo*>& enemigos)
{
    if (!jugador.estaVivo()) return;

    Rectangle rectJugador = jugador.getRect();
    Vector2 posJugador = jugador.getPosicion();

    for (Enemigo* enemigo : enemigos)
    {
        if (!enemigo->estaVivo()) continue;

        // El Fantasma no colisiona
        if (dynamic_cast<Fantasma*>(enemigo)) {
            continue;
        }

        Rectangle rectEnemigo = enemigo->getRect();

        // 1. Comprobar si hay colision
        if (CheckCollisionRecs(rectJugador, rectEnemigo))
        {
            // 2. Calcular superposicion (usando radios para colision circular)
            Vector2 posEnemigo = enemigo->getPosicion();
            float radioTotal = jugador.getRadio() + enemigo->getRadio();
            float dist = Vector2Distance(posJugador, posEnemigo);

            float overlap = radioTotal - dist;

            // 3. Calcular vector de separacion (de jugador a enemigo)
            Vector2 vectorSep = Vector2Subtract(posEnemigo, posJugador);

            // Fallback por si estan perfectamente en el mismo pixel
            if (Vector2LengthSqr(vectorSep) == 0.0f) {
                vectorSep = {1.0f, 0.0f};
            }
            vectorSep = Vector2Normalize(vectorSep);

            // 4. Mover *solo* al enemigo
            // Lo empujamos hacia atras la distancia total de la superposicion
            Vector2 moverEnemigo = Vector2Scale(vectorSep, overlap);
            enemigo->setPosicion(Vector2Add(posEnemigo, moverEnemigo));
        }
    }
}
