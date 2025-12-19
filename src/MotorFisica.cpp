#include "MotorFisica.h"
#include "raymath.h"
#include "Constantes.h"
#include "TrozoDeCarne.h"
#include "MinaEnemiga.h"

Vector2 MotorFisica::calcularMovimientoValido(
    Vector2 posActual,
    Vector2 velActual,
    Rectangle rectColision,
    const std::vector<Rectangle>& muros,
    const std::vector<Rectangle>& cajas,
    const Rectangle& puerta,
    bool puertaEstaAbierta,
    bool& choco
)
{
    Vector2 nuevaPos = posActual;
    choco = false;

    nuevaPos.x += velActual.x;
    rectColision.x = nuevaPos.x - (rectColision.width / 2);

    for (const auto& obs : muros) {
        if (CheckCollisionRecs(rectColision, obs)) {
            nuevaPos.x = posActual.x;
            rectColision.x = posActual.x - (rectColision.width / 2);
            choco = true;
            goto ChequeoEjeY_X;
        }
    }
    for (const auto& obs : cajas) {
        if (CheckCollisionRecs(rectColision, obs)) {
            nuevaPos.x = posActual.x;
            rectColision.x = posActual.x - (rectColision.width / 2);
            choco = true;
            goto ChequeoEjeY_X;
        }
    }
    if (!puertaEstaAbierta) {
        if (CheckCollisionRecs(rectColision, puerta)) {
             nuevaPos.x = posActual.x;
             rectColision.x = posActual.x - (rectColision.width / 2);
             choco = true;
        }
    }

ChequeoEjeY_X:
    nuevaPos.y += velActual.y;
    rectColision.y = nuevaPos.y - (rectColision.height / 2);

    for (const auto& obs : muros) {
        if (CheckCollisionRecs(rectColision, obs)) {
            nuevaPos.y = posActual.y;
            rectColision.y = posActual.y - (rectColision.height / 2);
            choco = true;
            goto FinChequeo_Y;
        }
    }
    for (const auto& obs : cajas) {
        if (CheckCollisionRecs(rectColision, obs)) {
            nuevaPos.y = posActual.y;
            rectColision.y = posActual.y - (rectColision.height / 2);
            choco = true;
            goto FinChequeo_Y;
        }
    }
    if (!puertaEstaAbierta) {
        if (CheckCollisionRecs(rectColision, puerta)) {
             nuevaPos.y = posActual.y;
             rectColision.y = posActual.y - (rectColision.height / 2);
             choco = true;
        }
    }

FinChequeo_Y:
    return nuevaPos;
}


void MotorFisica::moverJugador(
    Protagonista& jugador,
    Vector2 dirMovimiento,
    const std::vector<Rectangle>& muros,
    const std::vector<Rectangle>& cajas,
    const Rectangle& puerta,
    bool puertaEstaAbierta,
    const std::vector<Bala*>& balas
)
{
    if (!jugador.estaVivo()) return;
    Vector2 velocidad = {0, 0};
    if (jugador.getKnockbackTimer() > 0.0f)
    {
        velocidad = Vector2Scale(jugador.getVelocidadKnockback(), GetFrameTime());
    }
    else if (Vector2Length(dirMovimiento) > 0.0f)
    {
        velocidad = Vector2Scale(Vector2Normalize(dirMovimiento), Constantes::VELOCIDAD_JUGADOR);
    }
    Rectangle rectJugador = jugador.getRect();

    bool estaEnCharco = false;
    for (const Bala* bala : balas)
    {
        if (const TrozoDeCarne* charco = dynamic_cast<const TrozoDeCarne*>(bala))
        {
            if (charco->esCharco() && CheckCollisionRecs(rectJugador, charco->getRect()))
            {
                estaEnCharco = true;
                break;
            }
        }
    }

    if (estaEnCharco)
    {
        velocidad = Vector2Scale(velocidad, 0.5f);
    }

    bool chocoDummy = false;

    Vector2 nuevaPos = calcularMovimientoValido(
        jugador.getPosicion(),
        velocidad,
        rectJugador,
        muros,
        cajas,
        puerta,
        puertaEstaAbierta,
        chocoDummy
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
        bool chocoDummy = false;

        Vector2 nuevaPos = calcularMovimientoValido(
            enemigo->getPosicion(),
            velocidad,
            rectEnemigo,
            muros,
            cajas,
            puerta,
            puertaEstaAbierta,
            chocoDummy
        );
        enemigo->setPosicion(nuevaPos);
    }
}


void MotorFisica::moverJefes(
    std::vector<Jefe*>& jefes,
    const std::vector<Rectangle>& muros,
    const std::vector<Rectangle>& cajas,
    const Rectangle& puerta,
    bool puertaEstaAbierta
)
{
    for (Jefe* jefe : jefes) {
        Vector2 posAntes = jefe->getPosicion();
        Vector2 velocidad = jefe->getVelocidadActual();
        Rectangle rectJefe = jefe->getRect();

        bool haChocado = false;

        Vector2 posNueva = calcularMovimientoValido(
            posAntes,
            velocidad,
            rectJefe,
            muros,
            cajas,
            puerta,
            puertaEstaAbierta,
            haChocado
        );

        if (jefe->getFase() == FaseJefe::FASE_UNO &&
            jefe->getEstadoF1() == EstadoFaseUno::EMBISTIENDO)
        {
            if (haChocado)
            {
                jefe->setVelocidad({0, 0});
                jefe->setEstadoF1(EstadoFaseUno::ATURDIDO_EMBESTIDA);
                jefe->setTemporizadorEstado(2.5f);
            }
        }

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

        Vector2 velocidadCompleta = bala->getVelocidad();
        if (Vector2LengthSqr(velocidadCompleta) == 0.0f) continue;

        Vector2 velPorFrame = Vector2Scale(velocidadCompleta, GetFrameTime());
        Vector2 nuevaPos = Vector2Add(bala->getPosicion(), velPorFrame);

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
            if (dynamic_cast<MinaEnemiga*>(bala) || dynamic_cast<TrozoDeCarne*>(bala))
            {
                 bala->setPosicion(bala->getPosicion());
                 bala->setVelocidad({0,0});
            } else {
                 bala->desactivar();
            }
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

            if (dist == 0.0f)
            {
                dist = 0.1f;
                posEnemigo.x += 0.1f;
            }

            float overlap = radioTotal - dist;
            Vector2 vectorSep = Vector2Subtract(posEnemigo, posJugador);

            vectorSep = Vector2Normalize(vectorSep);
            Vector2 moverEnemigo = Vector2Scale(vectorSep, overlap);
            enemigo->setPosicion(Vector2Add(posEnemigo, moverEnemigo));
        }
    }
}
