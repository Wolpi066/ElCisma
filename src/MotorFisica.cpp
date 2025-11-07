#include "MotorFisica.h"
#include "raymath.h"
#include "Constantes.h" // Para velocidades

// --- LOGICA DE MOVIMIENTO Y COLISION AABB ---

// --- ¡¡FIRMA ACTUALIZADA!! ---
Vector2 MotorFisica::calcularMovimientoValido(
    Vector2 posActual,
    Vector2 velActual,
    Rectangle rectColision,
    const std::vector<Rectangle>& muros,
    const std::vector<Rectangle>& cajas,
    const Rectangle& puerta, // ¡¡NUEVO!!
    bool puertaEstaAbierta,   // ¡¡NUEVO!!
    bool& choco               // <-- ¡¡AÑADIDO!!
)
{
    Vector2 nuevaPos = posActual;
    choco = false; // <-- ¡¡AÑADIDO!! Inicializamos en false

    // --- Comprobar Eje X ---
    nuevaPos.x += velActual.x;
    rectColision.x = nuevaPos.x - (rectColision.width / 2);

    // Chequeo contra muros
    for (const auto& obs : muros) {
        if (CheckCollisionRecs(rectColision, obs)) {
            nuevaPos.x = posActual.x;
            rectColision.x = posActual.x - (rectColision.width / 2);
            choco = true; // <-- ¡¡AÑADIDO!!
            goto ChequeoEjeY_X;
        }
    }
    // Chequeo contra cajas
    for (const auto& obs : cajas) {
        if (CheckCollisionRecs(rectColision, obs)) {
            nuevaPos.x = posActual.x;
            rectColision.x = posActual.x - (rectColision.width / 2);
            choco = true; // <-- ¡¡AÑADIDO!!
            goto ChequeoEjeY_X;
        }
    }
    // Chequeo contra puerta
    if (!puertaEstaAbierta) {
        if (CheckCollisionRecs(rectColision, puerta)) {
             nuevaPos.x = posActual.x;
             rectColision.x = posActual.x - (rectColision.width / 2);
             choco = true; // <-- ¡¡AÑADIDO!!
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
            choco = true; // <-- ¡¡AÑADIDO!!
            goto FinChequeo_Y;
        }
    }
    // Chequeo contra cajas
    for (const auto& obs : cajas) {
        if (CheckCollisionRecs(rectColision, obs)) {
            nuevaPos.y = posActual.y;
            rectColision.y = posActual.y - (rectColision.height / 2);
            choco = true; // <-- ¡¡AÑADIDO!!
            goto FinChequeo_Y;
        }
    }
    // Chequeo contra puerta
    if (!puertaEstaAbierta) {
        if (CheckCollisionRecs(rectColision, puerta)) {
             nuevaPos.y = posActual.y;
             rectColision.y = posActual.y - (rectColision.height / 2);
             choco = true; // <-- ¡¡AÑADIDO!!
        }
    }

FinChequeo_Y:
    return nuevaPos;
}

// --- METODOS PUBLICOS ---

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

    if (jugador.getKnockbackTimer() > 0.0f)
    {
        velocidad = jugador.getVelocidadKnockback();
    }
    else if (Vector2Length(dirMovimiento) > 0.0f)
    {
        velocidad = Vector2Scale(Vector2Normalize(dirMovimiento), Constantes::VELOCIDAD_JUGADOR);
    }

    Rectangle rectJugador = jugador.getRect();
    bool chocoDummy = false; // Variable dummy para que compile

    // --- LLAMADA ACTUALIZADA ---
    Vector2 nuevaPos = calcularMovimientoValido(
        jugador.getPosicion(),
        velocidad,
        rectJugador,
        muros,
        cajas,
        puerta,
        puertaEstaAbierta,
        chocoDummy // <-- ¡¡AÑADIDO!!
    );

    jugador.setPosicion(nuevaPos);
}

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

        // (Lógica de Steering omitida por brevedad...)
        // ...
        Vector2 dirDeseada = enemigo->getDireccion();
        if (Vector2LengthSqr(dirDeseada) == 0.0f) {
            continue;
        }
        Vector2 vectorEvitacion = { 0, 0 };
        float pesoEvitacion = 0.0f;
        Rectangle rectEnemigo = enemigo->getRect();
        float distSensor = rectEnemigo.width;
        Vector2 posSensor = Vector2Add(enemigo->getPosicion(), Vector2Scale(dirDeseada, distSensor));
        Rectangle rectSensor = rectEnemigo;
        rectSensor.x = posSensor.x - rectSensor.width / 2;
        rectSensor.y = posSensor.y - rectSensor.height / 2;
        const std::vector<Rectangle>* listasObstaculos[] = {&muros, &cajas};
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
        if (pesoEvitacion > 0.0f) {
            vectorEvitacion = Vector2Normalize(vectorEvitacion);
            Vector2 fuerzaDeseo = Vector2Scale(dirDeseada, 1.0f);
            Vector2 fuerzaEvitacion = Vector2Scale(vectorEvitacion, 1.5f);
            dirFinal = Vector2Normalize(Vector2Add(fuerzaDeseo, fuerzaEvitacion));
        } else {
            dirFinal = dirDeseada;
        }
        Vector2 velocidad = Vector2Scale(dirFinal, enemigo->getVelocidad());
        enemigo->setDireccion(dirFinal);
        // ...
        // (Fin lógica de Steering)


        bool chocoDummy = false; // Variable dummy

        // --- LLAMADA ACTUALIZADA ---
        Vector2 nuevaPos = calcularMovimientoValido(
            enemigo->getPosicion(),
            velocidad,
            rectEnemigo,
            muros,
            cajas,
            puerta,
            puertaEstaAbierta,
            chocoDummy // <-- ¡¡AÑADIDO!!
        );

        enemigo->setPosicion(nuevaPos);
    }
}

// --- ¡¡FUNCIÓN CLAVE MODIFICADA!! ---
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

        Vector2 posAntes = jefe->getPosicion();
        Vector2 velocidad = jefe->getVelocidadActual();
        Rectangle rectJefe = jefe->getRect();

        // --- ¡¡LÓGICA DE ATURDIMIENTO CORREGIDA!! ---

        bool haChocado = false; // <-- 1. Preparamos el bool

        // 2. Calcula la nueva posición válida (y actualiza haChocado)
        Vector2 posNueva = calcularMovimientoValido(
            posAntes,
            velocidad,
            rectJefe,
            muros,
            cajas,
            puerta,
            puertaEstaAbierta,
            haChocado // <-- 3. Pasamos la referencia
        );

        // 4. Comprueba si el jefe estaba embistiendo
        if (jefe->getFase() == FaseJefe::FASE_UNO &&
            jefe->getEstadoF1() == EstadoFaseUno::EMBISTIENDO)
        {
            // 5. Comprueba si el motor de física reportó un choque
            if (haChocado) // <-- 6. ¡¡FIX!! Usamos el bool, no Vector2Equals
            {
                // ¡¡CHOQUE!!
                jefe->setVelocidad({0, 0});
                jefe->setEstadoF1(EstadoFaseUno::ATURDIDO_EMBESTIDA);
                jefe->setTemporizadorEstado(2.5f); // 2.5 seg de aturdimiento (GDD)
            }
        }
        // --- FIN DE LA LÓGICA DE ATURDIMIENTO ---

        // 7. Aplica la posición final
        jefe->setPosicion(posNueva);
    }
}

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

void MotorFisica::resolverColisionesDinamicas(Protagonista& jugador, std::vector<Enemigo*>& enemigos)
{
    if (!jugador.estaVivo()) return;

    Rectangle rectJugador = jugador.getRect();
    Vector2 posJugador = jugador.getPosicion();

    for (Enemigo* enemigo : enemigos)
    {
        if (!enemigo->estaVivo()) continue;
        if (dynamic_cast<Fantasma*>(enemigo)) {
            continue;
        }

        Rectangle rectEnemigo = enemigo->getRect();

        if (CheckCollisionRecs(rectJugador, rectEnemigo))
        {
            Vector2 posEnemigo = enemigo->getPosicion();
            float radioTotal = jugador.getRadio() + enemigo->getRadio();
            float dist = Vector2Distance(posJugador, posEnemigo);
            float overlap = radioTotal - dist;
            Vector2 vectorSep = Vector2Subtract(posEnemigo, posJugador);

            if (Vector2LengthSqr(vectorSep) == 0.0f) {
                vectorSep = {1.0f, 0.0f};
            }
            vectorSep = Vector2Normalize(vectorSep);

            Vector2 moverEnemigo = Vector2Scale(vectorSep, overlap);
            enemigo->setPosicion(Vector2Add(posEnemigo, moverEnemigo));
        }
    }
}
