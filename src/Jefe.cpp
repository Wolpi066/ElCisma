#include "Jefe.h"
#include "Protagonista.h"
#include "Mapa.h"
#include "Constantes.h"
#include <cstdlib>
#include <cmath>
#include "BalaMonstruosa.h"
#include "MinaEnemiga.h"
#include "TrozoDeCarne.h"

// --- TAREA 2: Añadida la nueva bala ---
#include "BalaInfernal.h"
// ----------------------------------

// --- TAREA 1: Reducción de Vida (de 350 a 100) ---
static const int VIDA_JEFE_NUEVA = 100; // (Modificado)
// --------------------------------------------------

static const int DANIO_CONTACTO_NUEVO = 5;
static const int DANIO_BRAZO_NUEVO = 5;
static const float VELOCIDAD_LENTA_JEFE = 1.0f; // px/frame
static const float VELOCIDAD_LENTA_JEFE_F2 = 1.5f; // px/frame
static const float VELOCIDAD_EMBESTIDA_JEFE_REDUCIDA = 7.5f; // px/frame
static const float VELOCIDAD_TAMBALEO_MUERTE = 0.5f; // px/frame
// --------------------------------------------------

static const float TIEMPO_SALTO_JEFE = 2.0f;
static const float TIEMPO_BRAZO_CARGANDO_AVISO = 0.5f;
static const float TIEMPO_BRAZO_EXTENDIENDO = 1.0f;
static const float LARGO_BRAZO_MAXIMO = 300.0f;
static const float EXTENSION_CARNE_RELATIVA = 0.9f;
static const float EXTENSION_HUESO_RELATIVA = 0.1f;
static const float TIEMPO_TIRAR_CARNE = 0.5f;
static const float TIEMPO_TRANSFORMACION_FINAL = 3.0f;

// --- TAREA 2: Duración x2 (100s) ---
static const float TIEMPO_BULLET_HELL = 100.0f; // (Modificado a 100.0)
// -----------------------------------

static const float COOLDOWN_MINAS = 2.0f;
static const float COOLDOWN_ESCOPETA_F2 = 1.0f;
static const float COOLDOWN_PULSO_F2 = 1.5f;


Jefe::Jefe(Vector2 pos) :
    posicion(pos),
    velocidadActual({0, 0}),
    direccionVista({0, 1}),
    vidaMaxima(VIDA_JEFE_NUEVA),
    vida(VIDA_JEFE_NUEVA),
    danioContacto(DANIO_CONTACTO_NUEVO),
    radioHitbox(ConstantesJefe::RADIO_JEFE),
    velocidadLenta(VELOCIDAD_LENTA_JEFE),
    faseActual(FaseJefe::FASE_UNO),
    enTransicion(false),
    esInvulnerable(false),
    estadoF1(EstadoFaseUno::PAUSANDO),
    estadoF2(EstadoFaseDos::PAUSANDO),
    temporizadorEstado(2.5f),
    objetivoEmbestida({0, 0}),
    temporizadorEmbestida(0.0f),
    objetivoSalto({0,0}),
    inicioSalto({0,0}),
    progresoSalto(0.0f),
    objetivoBrazo({0,0}),
    extensionBrazo(0.0f),
    estadoBrazo(EstadoEstirarBrazo::CARGANDO_AVISO),
    haSoltadoLootStun(false),
    progresoMuerte(0.0f),
    bulletHellBaseDirection(0.0f),
    bulletHellSpawnTimer(0), // (int 0)
    bulletHellAngleEspirales(0.0f) // (float 0.0)
{
    this->vidaMaxima = VIDA_JEFE_NUEVA; // (Modificado Tarea 1)
    this->vida = this->vidaMaxima;
    balasGeneradas.clear();
    dropsGenerados.clear();
}

void Jefe::actualizar(Protagonista& jugador, const Mapa& mapa) {

    if (faseActual >= FaseJefe::MURIENDO)
    {
        actualizarFaseMuerte(jugador, mapa);
        return;
    }

    velocidadActual = {0, 0};

    if ((faseActual == FaseJefe::FASE_UNO && estadoF1 == EstadoFaseUno::EMBISTIENDO) ||
        (faseActual == FaseJefe::FASE_DOS && estadoF2 == EstadoFaseDos::EMBISTIENDO) ||
        (faseActual == FaseJefe::FASE_DOS && estadoF2 == EstadoFaseDos::TELETRANSPORTANDO))
    {
         velocidadActual = getVelocidadActual();
    }

    limpiarBalasGeneradas();
    limpiarDropsGenerados();

    esInvulnerable = false;

    temporizadorEstado -= GetFrameTime();

    if (vida <= (vidaMaxima * ConstantesJefe::VIDA_FASE_DOS_TRIGGER) &&
        vida > 0 &&
        faseActual == FaseJefe::FASE_UNO)
    {
        if (!enTransicion) {
            transicionAFaseDos();
        }
        if (enTransicion) {
            esInvulnerable = true;
            if (temporizadorEstado <= 0) {
                enTransicion = false;
                faseActual = FaseJefe::FASE_DOS;
                estadoF2 = EstadoFaseDos::PAUSANDO;
                temporizadorEstado = 1.0f;
            }
            return;
        }
    }

    if (!enTransicion)
    {
        if (estadoF1 == EstadoFaseUno::PAUSANDO ||
            estadoF1 == EstadoFaseUno::APUNTANDO_EMBESTIDA ||
            estadoF2 == EstadoFaseDos::PAUSANDO ||
            estadoF2 == EstadoFaseDos::APUNTANDO_EMBESTIDA ||
            (estadoF1 == EstadoFaseUno::ESTIRANDO_BRAZO && estadoBrazo == EstadoEstirarBrazo::CARGANDO_AVISO) ||
            (estadoF1 == EstadoFaseUno::TIRANDO_CARNE))
        {
            if (Vector2DistanceSqr(jugador.getPosicion(), posicion) > 1.0f) {
                direccionVista = Vector2Normalize(Vector2Subtract(jugador.getPosicion(), posicion));
            }
        }

        // --- ¡¡VACA FIX 4.0!! (Velocidad en px/frame) ---
        if (faseActual == FaseJefe::FASE_UNO && estadoF1 == EstadoFaseUno::PAUSANDO)
        {
            velocidadActual = Vector2Scale(direccionVista, velocidadLenta);
        }
        else if (faseActual == FaseJefe::FASE_DOS && estadoF2 == EstadoFaseDos::PAUSANDO)
        {
            velocidadActual = Vector2Scale(direccionVista, VELOCIDAD_LENTA_JEFE_F2);
        }
        // ---------------------------------------------
    }


    switch (faseActual) {
        case FaseJefe::FASE_UNO:
            actualizarFaseUno(jugador, mapa);
            break;
        case FaseJefe::FASE_DOS:
            actualizarFaseDos(jugador, mapa);
            break;
        // --- FIX WARNING (Línea 148) ---
        // (Los otros casos se manejan en el 'if' al inicio de la función)
        default:
            break;
        // -----------------------------
    }
}

void Jefe::transicionAFaseDos() {
    enTransicion = true;
    temporizadorEstado = 3.0f;
    velocidadActual = {0, 0};
    estadoF1 = EstadoFaseUno::PAUSANDO;
}


// -------------------------------------------------------------------
// --- FASE 1
// -------------------------------------------------------------------
void Jefe::actualizarFaseUno(Protagonista& jugador, const Mapa& mapa) {
    switch (estadoF1) {

        case EstadoFaseUno::PAUSANDO:
            if (temporizadorEstado <= 0) {
                ejecutarPausaF1(jugador);
            }
            break;

        case EstadoFaseUno::APUNTANDO_EMBESTIDA:
            if (temporizadorEstado <= 0) {
                estadoF1 = EstadoFaseUno::EMBISTIENDO;
                direccionVista = objetivoEmbestida;
                temporizadorEmbestida = ConstantesJefe::TIEMPO_MAX_EMBESTIDA;
            }
            break;

        case EstadoFaseUno::EMBISTIENDO:
        {
            // --- ¡¡VACA FIX 4.0!! (Velocidad en px/frame) ---
            velocidadActual = Vector2Scale(objetivoEmbestida, VELOCIDAD_EMBESTIDA_JEFE_REDUCIDA);

            temporizadorEmbestida -= GetFrameTime();
            if (temporizadorEmbestida <= 0) {
                estadoF1 = EstadoFaseUno::PAUSANDO;
                temporizadorEstado = 1.0f;
            }
            break;
        }

        case EstadoFaseUno::ATURDIDO_EMBESTIDA:
            if (!haSoltadoLootStun)
            {
                haSoltadoLootStun = true;
                Vector2 posDrop = {posicion.x + GetRandomValue(-80, 80), posicion.y + GetRandomValue(-80, 80)};
                dropsGenerados.push_back({posDrop, 3});
                int dropsExtra = GetRandomValue(1, 2);
                for(int i=0; i < dropsExtra; i++)
                {
                    posDrop = {posicion.x + GetRandomValue(-150, 150), posicion.y + GetRandomValue(-150, 150)};
                    int tipoExtra = GetRandomValue(0, 1) == 0 ? 1 : 4;
                    dropsGenerados.push_back({posDrop, tipoExtra});
                }
            }
            if (temporizadorEstado <= 0) {
                estadoF1 = EstadoFaseUno::PAUSANDO;
                temporizadorEstado = ConstantesJefe::COOLDOWN_ATURDIMIENTO;
            }
            break;

        case EstadoFaseUno::SALTANDO:
        {
            float tiempoMax = TIEMPO_SALTO_JEFE;
            float tiempoPasado = tiempoMax - temporizadorEstado;
            progresoSalto = Clamp(tiempoPasado / tiempoMax, 0.0f, 1.0f);

            if (temporizadorEstado <= 0) {
                progresoSalto = 1.0f;
                posicion = objetivoSalto;
                ejecutarPulsoRadial();
                estadoF1 = EstadoFaseUno::PAUSANDO;
                temporizadorEstado = 1.5f;
            }
            break;
        }

        case EstadoFaseUno::ESTIRANDO_BRAZO:
        {
            if (estadoBrazo == EstadoEstirarBrazo::CARGANDO_AVISO) {
                if (temporizadorEstado <= 0) {
                    estadoBrazo = EstadoEstirarBrazo::EXTENDIENDO;
                    temporizadorEstado = TIEMPO_BRAZO_EXTENDIENDO;
                    objetivoBrazo = direccionVista;
                    extensionBrazo = 0.0f;
                }
                extensionBrazo = 0.0f;
            } else {
                float duracionEstirar = temporizadorEstado;
                float tiempoMax = TIEMPO_BRAZO_EXTENDIENDO;
                float tiempoPasado = tiempoMax - duracionEstirar;
                extensionBrazo = Lerp(0.0f, 1.0f, tiempoPasado / tiempoMax);

                if (temporizadorEstado <= 0) {
                    extensionBrazo = 0.0f;
                    estadoF1 = EstadoFaseUno::PAUSANDO;
                    temporizadorEstado = 1.0f;
                    estadoBrazo = EstadoEstirarBrazo::CARGANDO_AVISO;
                }
            }
            break;
        }

        case EstadoFaseUno::TIRANDO_CARNE:
            if (temporizadorEstado <= 0) {
                ejecutarTirarCarne(jugador);
                estadoF1 = EstadoFaseUno::PAUSANDO;
                temporizadorEstado = 1.5f;
            }
            break;
    }
}

void Jefe::ejecutarPausaF1(Protagonista& jugador) {
    haSoltadoLootStun = false;

    // Esta es la lógica que faltaba en FASE 2
    if (jugador.getMunicion() <= 0)
    {
        estadoF1 = EstadoFaseUno::APUNTANDO_EMBESTIDA;
        temporizadorEstado = ConstantesJefe::TIEMPO_APUNTAR_EMBESTIDA;
        objetivoEmbestida = direccionVista;
        return;
    }

    int ataque = GetRandomValue(0, 9);
    if (ataque == 0) { // 10% chance
        estadoF1 = EstadoFaseUno::APUNTANDO_EMBESTIDA;
        temporizadorEstado = ConstantesJefe::TIEMPO_APUNTAR_EMBESTIDA;
        objetivoEmbestida = direccionVista;
    } else if (ataque <= 3) { // 30% chance
        ejecutarSalto(jugador);
    } else if (ataque <= 6) { // 30% chance
        ejecutarEstirarBrazo(jugador);
    } else { // 30% chance
        estadoF1 = EstadoFaseUno::TIRANDO_CARNE;
        temporizadorEstado = TIEMPO_TIRAR_CARNE;
    }
}

void Jefe::ejecutarSalto(Protagonista& jugador)
{
    estadoF1 = EstadoFaseUno::SALTANDO;
    temporizadorEstado = TIEMPO_SALTO_JEFE;
    objetivoSalto = jugador.getPosicion();
    inicioSalto = posicion;
    progresoSalto = 0.0f;
}

void Jefe::ejecutarEstirarBrazo(Protagonista& jugador)
{
    estadoF1 = EstadoFaseUno::ESTIRANDO_BRAZO;
    estadoBrazo = EstadoEstirarBrazo::CARGANDO_AVISO;
    temporizadorEstado = TIEMPO_BRAZO_CARGANDO_AVISO;
    extensionBrazo = 0.0f;
}

void Jefe::ejecutarTirarCarne(Protagonista& jugador)
{
    Vector2 dir = direccionVista;
    balasGeneradas.push_back(new TrozoDeCarne(posicion, jugador.getPosicion()));
    Vector2 posLateral1 = Vector2Add(jugador.getPosicion(), Vector2Scale(Vector2Rotate(dir, PI/2), 100.0f));
    Vector2 posLateral2 = Vector2Add(jugador.getPosicion(), Vector2Scale(Vector2Rotate(dir, -PI/2), 100.0f));
    balasGeneradas.push_back(new TrozoDeCarne(posicion, posLateral1));
    balasGeneradas.push_back(new TrozoDeCarne(posicion, posLateral2));
}

void Jefe::ejecutarDisparoEscopeta() {
    Vector2 dir = direccionVista;
    balasGeneradas.push_back(new BalaMonstruosa(posicion, dir));
    balasGeneradas.push_back(new BalaMonstruosa(posicion, Vector2Rotate(dir, -0.2f)));
    balasGeneradas.push_back(new BalaMonstruosa(posicion, Vector2Rotate(dir, 0.2f)));
    balasGeneradas.push_back(new BalaMonstruosa(posicion, Vector2Rotate(dir, -0.4f)));
    balasGeneradas.push_back(new BalaMonstruosa(posicion, Vector2Rotate(dir, 0.4f)));
}

void Jefe::ejecutarPulsoRadial() {
    balasGeneradas.push_back(new BalaMonstruosa(posicion, {0, 1}));
    balasGeneradas.push_back(new BalaMonstruosa(posicion, {0, -1}));
    balasGeneradas.push_back(new BalaMonstruosa(posicion, {1, 0}));
    balasGeneradas.push_back(new BalaMonstruosa(posicion, {-1, 0}));
    balasGeneradas.push_back(new BalaMonstruosa(posicion, Vector2Normalize({1, 1})));
    balasGeneradas.push_back(new BalaMonstruosa(posicion, Vector2Normalize({1, -1})));
    balasGeneradas.push_back(new BalaMonstruosa(posicion, Vector2Normalize({-1, 1})));
    balasGeneradas.push_back(new BalaMonstruosa(posicion, Vector2Normalize({-1, -1})));
}


// -------------------------------------------------------------------
// --- FASE 2
// -------------------------------------------------------------------
void Jefe::actualizarFaseDos(Protagonista& jugador, const Mapa& mapa) {
    switch (estadoF2) {
        case EstadoFaseDos::PAUSANDO:
            if (temporizadorEstado <= 0) {
                ejecutarPausaF2(jugador);
            }
            break;

        case EstadoFaseDos::TELETRANSPORTANDO:
            esInvulnerable = true;
            if (temporizadorEstado <= 0) {
                posicion.x = (float)GetRandomValue(-300, 300);
                posicion.y = (float)GetRandomValue(-300, 300);
                esInvulnerable = false;
                estadoF2 = EstadoFaseDos::PAUSANDO;
                temporizadorEstado = 0.5f;
            }
            break;

        case EstadoFaseDos::PLANTANDO_MINAS:
            if (temporizadorEstado <= 0) {
                ejecutarPlantandoMinas();
                estadoF2 = EstadoFaseDos::PAUSANDO;
                temporizadorEstado = COOLDOWN_MINAS;
            }
            break;

        case EstadoFaseDos::DISPARO_ESCOPETA:
            if (temporizadorEstado <= 0) {
                ejecutarDisparoEscopeta();
                estadoF2 = EstadoFaseDos::PAUSANDO;
                temporizadorEstado = COOLDOWN_ESCOPETA_F2;
            }
            break;

        case EstadoFaseDos::PULSO_RADIAL:
            if (temporizadorEstado <= 0) {
                ejecutarPulsoRadial();
                estadoF2 = EstadoFaseDos::PAUSANDO;
                temporizadorEstado = COOLDOWN_PULSO_F2;
            }
            break;

        case EstadoFaseDos::APUNTANDO_EMBESTIDA:
            if (temporizadorEstado > 0.0f) {
                 objetivoEmbestida = direccionVista;
            }
            if (temporizadorEstado <= 0) {
                estadoF2 = EstadoFaseDos::EMBISTIENDO;
                direccionVista = objetivoEmbestida;
                temporizadorEmbestida = ConstantesJefe::TIEMPO_MAX_EMBESTIDA;
            }
            break;

        case EstadoFaseDos::EMBISTIENDO:
        {
            velocidadActual = Vector2Scale(objetivoEmbestida, VELOCIDAD_EMBESTIDA_JEFE_REDUCIDA);

            temporizadorEmbestida -= GetFrameTime();
            if (temporizadorEmbestida <= 0) {
                estadoF2 = EstadoFaseDos::PAUSANDO;
                temporizadorEstado = 1.0f;
            }
            break;
        }

        case EstadoFaseDos::ATURDIDO_EMBESTIDA:
            if (!haSoltadoLootStun)
            {
                haSoltadoLootStun = true;
                Vector2 posDrop = {posicion.x + GetRandomValue(-80, 80), posicion.y + GetRandomValue(-80, 80)};
                dropsGenerados.push_back({posDrop, 3});
                int dropsExtra = GetRandomValue(1, 2);
                for(int i=0; i < dropsExtra; i++)
                {
                    posDrop = {posicion.x + GetRandomValue(-150, 150), posicion.y + GetRandomValue(-150, 150)};
                    int tipoExtra = GetRandomValue(0, 1) == 0 ? 1 : 4;
                    dropsGenerados.push_back({posDrop, tipoExtra});
                }
            }
            if (temporizadorEstado <= 0) {
                estadoF2 = EstadoFaseDos::PAUSANDO;
                temporizadorEstado = ConstantesJefe::COOLDOWN_ATURDIMIENTO;
            }
            break;
    }
}

void Jefe::ejecutarPausaF2(Protagonista& jugador) {
    haSoltadoLootStun = false;

    // --- TAREA 2: Añadir lógica de munición FALTANTE ---
    // (Añadido el check que estaba en F1 y faltaba aquí)
    if (jugador.getMunicion() <= 0)
    {
        estadoF2 = EstadoFaseDos::APUNTANDO_EMBESTIDA;
        temporizadorEstado = ConstantesJefe::TIEMPO_APUNTAR_EMBESTIDA;
        objetivoEmbestida = direccionVista;
        return;
    }
    // --- FIN TAREA 2 ---

    int ataque = GetRandomValue(0, 3);
    switch(ataque) {
        case 0:
            ejecutarTeletransporte();
            break;
        case 1:
            estadoF2 = EstadoFaseDos::PLANTANDO_MINAS;
            temporizadorEstado = 0.3f;
            break;
        case 2:
            estadoF2 = EstadoFaseDos::DISPARO_ESCOPETA;
            temporizadorEstado = ConstantesJefe::TIEMPO_PREPARAR_ESCOPETA;
            break;
        case 3:
            estadoF2 = EstadoFaseDos::PULSO_RADIAL;
            temporizadorEstado = ConstantesJefe::TIEMPO_PREPARAR_PULSO;
            break;
    }
}

void Jefe::ejecutarTeletransporte()
{
    estadoF2 = EstadoFaseDos::TELETRANSPORTANDO;
    temporizadorEstado = 0.3f;
}

void Jefe::ejecutarPlantandoMinas()
{
    Vector2 dir = direccionVista;
    balasGeneradas.push_back(new MinaEnemiga(posicion, dir));
    balasGeneradas.push_back(new MinaEnemiga(posicion, Vector2Rotate(dir, -0.4f)));
    balasGeneradas.push_back(new MinaEnemiga(posicion, Vector2Rotate(dir, 0.4f)));
    balasGeneradas.push_back(new MinaEnemiga(posicion, Vector2Rotate(dir, -0.8f)));
    balasGeneradas.push_back(new MinaEnemiga(posicion, Vector2Rotate(dir, 0.8f)));
}

void Jefe::ejecutarAtaqueSombra(Protagonista& jugador) {}
void Jefe::ejecutarDisparoInteligente(Protagonista& jugador) {}


// -------------------------------------------------------------------
// --- FASE MUERTE
// -------------------------------------------------------------------
void Jefe::actualizarFaseMuerte(Protagonista& jugador, const Mapa& mapa)
{
    esInvulnerable = true;

    // --- ¡¡FIX!! (Añadido el decremento del timer) ---
    temporizadorEstado -= GetFrameTime();
    // ---------------------------------------------

    switch (faseActual)
    {
        case FaseJefe::MURIENDO:
        {
            Vector2 dirAlCentro = Vector2Normalize(Vector2Subtract({0, 0}, posicion));
            velocidadActual = Vector2Scale(dirAlCentro, VELOCIDAD_TAMBALEO_MUERTE);

            if (Vector2DistanceSqr(posicion, {0, 0}) < 10.0f) // Más cerca
            {
                posicion = {0, 0};
                velocidadActual = {0, 0};
                faseActual = FaseJefe::TRANSFORMANDO;
                temporizadorEstado = TIEMPO_TRANSFORMACION_FINAL;
            }
            break;
        }

        case FaseJefe::TRANSFORMANDO:
        {
            if (temporizadorEstado <= 0)
            {
                faseActual = FaseJefe::BULLET_HELL;
                temporizadorEstado = TIEMPO_BULLET_HELL; // Ahora son 100s
                progresoMuerte = 0.0f;
                bulletHellBaseDirection = 0.0f;
                bulletHellSpawnTimer = 0; // (int)
                bulletHellAngleEspirales = 0.0f; // (float)
            }
            break;
        }

        case FaseJefe::BULLET_HELL:
        {
            float tiempoPasado = TIEMPO_BULLET_HELL - temporizadorEstado;
            progresoMuerte = Clamp(tiempoPasado / TIEMPO_BULLET_HELL, 0.0f, 1.0f);

            // --- INICIO REDISEÑO BULLET HELL (Tarea 2 - 100 Segundos) ---

            bulletHellSpawnTimer++; // Contador de frames
            bulletHellBaseDirection += 0.025f; // Rotación/offset (más rápido)
            bulletHellAngleEspirales += BH_ESPIRAL_ROTACION; // Ángulo rápido para espirales

            // FASE 1 (100s - 80s): "Doble Pulso" (Más rápido)
            if (temporizadorEstado > 80.0f)
            {
                if (bulletHellSpawnTimer % BH_PULSO_COOLDOWN == 0)
                {
                    float anguloBase = bulletHellBaseDirection;
                    for (int i = 0; i < BH_PULSO_CANTIDAD_BALAS; i++)
                    {
                        float angulo = anguloBase + (i * (2 * PI / BH_PULSO_CANTIDAD_BALAS));
                        Vector2 dir = { cosf(angulo), sinf(angulo) };
                        balasGeneradas.push_back(new BalaInfernal(posicion, dir)); // BALA NUEVA
                    }
                }
                // Segundo pulso, 10 frames después
                if (bulletHellSpawnTimer % BH_PULSO_COOLDOWN == BH_PULSO_DELAY)
                {
                    float anguloBase = bulletHellBaseDirection + (PI / BH_PULSO_CANTIDAD_BALAS); // Offset
                    for (int i = 0; i < BH_PULSO_CANTIDAD_BALAS; i++)
                    {
                        float angulo = anguloBase + (i * (2 * PI / BH_PULSO_CANTIDAD_BALAS));
                        Vector2 dir = { cosf(angulo), sinf(angulo) };
                        balasGeneradas.push_back(new BalaInfernal(posicion, dir)); // BALA NUEVA
                    }
                }
            }
            // FASE 2 (80s - 60s): "Ráfagas Giratorias" (Más rápido)
            else if (temporizadorEstado > 60.0f)
            {
                if (bulletHellSpawnTimer % BH_RAFAGA_COOLDOWN == 0)
                {
                    Vector2 dirAlJugador = Vector2Normalize(Vector2Subtract(jugador.getPosicion(), posicion));
                    // La ráfaga rota lentamente
                    float anguloBase = atan2f(dirAlJugador.y, dirAlJugador.x) + (bulletHellBaseDirection * 3.0f);

                    for (int i = 0; i < BH_RAFAGA_CANTIDAD_BALAS; i++)
                    {
                        float offset = (i - (BH_RAFAGA_CANTIDAD_BALAS / 2.0f)) * BH_RAFAGA_SEPARACION;
                        float angulo = anguloBase + offset;
                        Vector2 dir = { cosf(angulo), sinf(angulo) };
                        balasGeneradas.push_back(new BalaInfernal(posicion, dir)); // BALA NUEVA
                    }
                }
            }
            // FASE 3 (60s - 40s): "Lluvia de Olas" (¡Bug Corregido!)
            else if (temporizadorEstado > 40.0f)
            {
                if (bulletHellSpawnTimer % BH_OLA_COOLDOWN == 0)
                {
                    float sinOffset = sinf(bulletHellBaseDirection * 5.0f);
                    float centroDelHueco = sinOffset * (BH_OLA_AMPLITUD / 2.0f);

                    // FIX: Spawnear relativo al JEFE (posición 0,0), no a la esquina del mundo
                    // Usamos las constantes corregidas para estar DENTRO del trigger de 400x400
                    float spawnY = posicion.y + BH_OLA_Y_ORIGEN_OFFSET; // (0.0 + -190.0)

                    for (int i = 0; i < BH_OLA_CANTIDAD_BALAS; i++)
                    {
                        // Spawnea de -190 a +190
                        float posX = posicion.x - BH_OLA_AMPLITUD + (i * (BH_OLA_AMPLITUD * 2.0f / BH_OLA_CANTIDAD_BALAS));

                        // Chequea el hueco
                        if (fabsf(posX - (posicion.x + centroDelHueco)) < (BH_OLA_HUECO / 2.0f)) continue;

                        Vector2 origenBala = {posX, spawnY};
                        Vector2 dirBala = {0.0f, 1.0f}; // Hacia abajo

                        balasGeneradas.push_back(new BalaInfernal(origenBala, dirBala)); // BALA NUEVA
                    }
                }
            }
            // FASE 4 (40s - 20s): "Caos (Ráfagas + Lluvia)"
            else if (temporizadorEstado > 20.0f)
            {
                // Ráfagas Giratorias (Igual que Fase 2)
                if (bulletHellSpawnTimer % BH_RAFAGA_COOLDOWN == 0)
                {
                    Vector2 dirAlJugador = Vector2Normalize(Vector2Subtract(jugador.getPosicion(), posicion));
                    float anguloBase = atan2f(dirAlJugador.y, dirAlJugador.x) + (bulletHellBaseDirection * 3.0f);
                    for (int i = 0; i < BH_RAFAGA_CANTIDAD_BALAS; i++)
                    {
                        float offset = (i - (BH_RAFAGA_CANTIDAD_BALAS / 2.0f)) * BH_RAFAGA_SEPARACION;
                        float angulo = anguloBase + offset;
                        Vector2 dir = { cosf(angulo), sinf(angulo) };
                        balasGeneradas.push_back(new BalaInfernal(posicion, dir)); // BALA NUEVA
                    }
                }
                // Lluvia de Olas (Igual que Fase 3)
                if (bulletHellSpawnTimer % BH_OLA_COOLDOWN == 0)
                {
                    float sinOffset = sinf(bulletHellBaseDirection * 5.0f);
                    float centroDelHueco = sinOffset * (BH_OLA_AMPLITUD / 2.0f);
                    float spawnY = posicion.y + BH_OLA_Y_ORIGEN_OFFSET;
                    for (int i = 0; i < BH_OLA_CANTIDAD_BALAS; i++)
                    {
                        float posX = posicion.x - BH_OLA_AMPLITUD + (i * (BH_OLA_AMPLITUD * 2.0f / BH_OLA_CANTIDAD_BALAS));
                        if (fabsf(posX - (posicion.x + centroDelHueco)) < (BH_OLA_HUECO / 2.0f)) continue;
                        Vector2 origenBala = {posX, spawnY};
                        Vector2 dirBala = {0.0f, 1.0f};
                        balasGeneradas.push_back(new BalaInfernal(origenBala, dirBala)); // BALA NUEVA
                    }
                }
            }
            // FASE 5 (20s - 0s): "Desesperación (Doble Pulso + Espiral Única)" (Más fácil)
            else
            {
                // Doble Pulso (Igual que Fase 1)
                if (bulletHellSpawnTimer % BH_PULSO_COOLDOWN == 0)
                {
                    float anguloBase = bulletHellBaseDirection;
                    for (int i = 0; i < BH_PULSO_CANTIDAD_BALAS; i++)
                    {
                        float angulo = anguloBase + (i * (2 * PI / BH_PULSO_CANTIDAD_BALAS));
                        Vector2 dir = { cosf(angulo), sinf(angulo) };
                        balasGeneradas.push_back(new BalaInfernal(posicion, dir)); // BALA NUEVA
                    }
                }
                if (bulletHellSpawnTimer % BH_PULSO_COOLDOWN == BH_PULSO_DELAY)
                {
                    float anguloBase = bulletHellBaseDirection + (PI / BH_PULSO_CANTIDAD_BALAS);
                    for (int i = 0; i < BH_PULSO_CANTIDAD_BALAS; i++)
                    {
                        float angulo = anguloBase + (i * (2 * PI / BH_PULSO_CANTIDAD_BALAS));
                        Vector2 dir = { cosf(angulo), sinf(angulo) };
                        balasGeneradas.push_back(new BalaInfernal(posicion, dir)); // BALA NUEVA
                    }
                }

                // Espiral Única (Más fácil)
                if (bulletHellSpawnTimer % BH_ESPIRAL_COOLDOWN == 0)
                {
                    float degreesPerRow = 360.0f / BH_ESPIRAL_FILAS;
                    for (int row = 0; row < BH_ESPIRAL_FILAS; row++)
                    {
                        float bulletDirection = bulletHellAngleEspirales + (degreesPerRow * row);
                        Vector2 dir = { cosf(bulletDirection * DEG2RAD), sinf(bulletDirection * DEG2RAD) };
                        balasGeneradas.push_back(new BalaInfernal(posicion, dir)); // BALA NUEVA
                    }
                }
            }
            // --- FIN REDISEÑO ---


            if (temporizadorEstado <= 0)
            {
                faseActual = FaseJefe::DERROTADO;
                progresoMuerte = 1.0f;
            }
            break;
        }

        case FaseJefe::DERROTADO:
        {
            velocidadActual = {0, 0};
            break;
        }

        // --- FIX WARNING (Línea 502) ---
        // (Los otros casos no deberían ocurrir aquí)
        default:
            break;
        // -----------------------------
    }
}


// -------------------------------------------------------------------
// --- Lógica de Daño y Getters
// -------------------------------------------------------------------

void Jefe::forzarFaseDos()
{
    if (faseActual == FaseJefe::FASE_DOS || faseActual >= FaseJefe::MURIENDO || enTransicion) {
        return;
    }
    int vidaFaseDos = (int)(vidaMaxima * ConstantesJefe::VIDA_FASE_DOS_TRIGGER);
    vida = vidaFaseDos;
    transicionAFaseDos();
}

void Jefe::forzarMuerte()
{
    if (faseActual >= FaseJefe::MURIENDO) return;

    vida = 1;
    faseActual = FaseJefe::MURIENDO;
    estadoF1 = EstadoFaseUno::PAUSANDO;
    estadoF2 = EstadoFaseDos::PAUSANDO;
    enTransicion = false;
    esInvulnerable = true;
    temporizadorEstado = 0.0f;
}


void Jefe::recibirDanio(int cantidad, Vector2 posicionJugador) {
    if (esInvulnerable) return;
    if (vida <= 0 || faseActual >= FaseJefe::MURIENDO) return;

    Vector2 dirHaciaJugador = Vector2Normalize(Vector2Subtract(posicionJugador, posicion));
    float dot = Vector2DotProduct(direccionVista, dirHaciaJugador);

    int danioFinal = 1;
    if (dot <= 0.707f) { // 0.707f es un cono de 90 grados (45 de cada lado)
        danioFinal = 2; // Daño por la espalda
    }
    if (estadoF1 == EstadoFaseUno::ATURDIDO_EMBESTIDA || estadoF2 == EstadoFaseDos::ATURDIDO_EMBESTIDA)
    {
        danioFinal *= 2; // Multiplicador por aturdimiento
    }

    vida -= danioFinal;

    if (vida <= 0)
    {
        vida = 1;
        faseActual = FaseJefe::MURIENDO;
        estadoF1 = EstadoFaseUno::PAUSANDO;
        estadoF2 = EstadoFaseDos::PAUSANDO;
        enTransicion = false;
        esInvulnerable = true;
        temporizadorEstado = 0.0f;
    }
}

std::vector<Bala*>& Jefe::getBalasGeneradas() {
    return balasGeneradas;
}
void Jefe::limpiarBalasGeneradas() {
    balasGeneradas.clear();
}

std::vector<DropInfo>& Jefe::getDropsGenerados() {
    return dropsGenerados;
}
void Jefe::limpiarDropsGenerados() {
    dropsGenerados.clear();
}

Rectangle Jefe::getHitboxBrazo() const
{
    if (faseActual != FaseJefe::FASE_UNO ||
        estadoF1 != EstadoFaseUno::ESTIRANDO_BRAZO ||
        extensionBrazo <= 0.0f ||
        estadoBrazo == EstadoEstirarBrazo::CARGANDO_AVISO)
    {
        return {0, 0, 0, 0};
    }

    float largoCarneActual = 0.0f;
    float largoHuesoActual = 0.0f;

    if (extensionBrazo <= EXTENSION_CARNE_RELATIVA) {
        float progresoCarne = extensionBrazo / EXTENSION_CARNE_RELATIVA;
        largoCarneActual = (LARGO_BRAZO_MAXIMO * EXTENSION_CARNE_RELATIVA) * progresoCarne;
    } else {
        largoCarneActual = LARGO_BRAZO_MAXIMO * EXTENSION_CARNE_RELATIVA;
        float progresoHueso = (extensionBrazo - EXTENSION_CARNE_RELATIVA) / EXTENSION_HUESO_RELATIVA;
        largoHuesoActual = (LARGO_BRAZO_MAXIMO * EXTENSION_HUESO_RELATIVA) * progresoHueso;
    }

    Vector2 finalBrazo = Vector2Add(posicion, Vector2Scale(objetivoBrazo, largoCarneActual + largoHuesoActual));
    float anchoMano = 40.0f;
    return {
        finalBrazo.x - anchoMano / 2,
        finalBrazo.y - anchoMano / 2,
        anchoMano,
        anchoMano
    };
}


void Jefe::setPosicion(Vector2 nuevaPos) { this->posicion = nuevaPos; }
void Jefe::setVelocidad(Vector2 vel) { this->velocidadActual = vel; }
Vector2 Jefe::getVelocidadActual() const { return velocidadActual; }
bool Jefe::estaVivo() const {
    return faseActual != FaseJefe::DERROTADO;
}
Rectangle Jefe::getRect() const {
    float radioBase = (faseActual == FaseJefe::FASE_UNO) ? ConstantesJefe::RADIO_JEFE : ConstantesJefe::RADIO_JEFE * 0.8f;
    float radioDibujo = radioBase;
    if (faseActual == FaseJefe::BULLET_HELL) {
        radioDibujo = Lerp(radioBase, radioBase * 0.3f, progresoMuerte);
    } else if (faseActual >= FaseJefe::DERROTADO) {
        radioDibujo = radioBase * 0.3f;
    }
    return { posicion.x - radioDibujo, posicion.y - radioDibujo, radioDibujo * 2, radioDibujo * 2 };
}
Vector2 Jefe::getPosicion() const { return posicion; }
int Jefe::getDanioContacto() const { return danioContacto; }
FaseJefe Jefe::getFase() const { return faseActual; }
bool Jefe::esInvulnerableActualmente() const {
    // --- ¡ERROR CORREGIDO! (FaseJfase -> FaseJefe) ---
    return enTransicion || esInvulnerable || faseActual >= FaseJefe::MURIENDO;
}
bool Jefe::estaEnFaseFinal() const
{
    return faseActual == FaseJefe::DERROTADO;
}
bool Jefe::estaMuerto() const {
    return false;
}
bool Jefe::estaConsumido() const { return false; }
EstadoFaseUno Jefe::getEstadoF1() const { return estadoF1; }
void Jefe::setEstadoF1(EstadoFaseUno nuevoEstado) { this->estadoF1 = nuevoEstado; }
EstadoFaseDos Jefe::getEstadoF2() const { return estadoF2; }
void Jefe::setEstadoF2(EstadoFaseDos nuevoEstado) { this->estadoF2 = nuevoEstado; }
void Jefe::setTemporizadorEstado(float tiempo) { this->temporizadorEstado = tiempo; }


void Jefe::dibujar() {

    float radioBase = (faseActual == FaseJefe::FASE_UNO) ? ConstantesJefe::RADIO_JEFE : ConstantesJefe::RADIO_JEFE * 0.8f;
    Color colorJefe = (faseActual == FaseJefe::FASE_UNO) ? MAROON : RED;
    Vector2 posDibujo = posicion;
    float radioDibujo = radioBase;

    if (faseActual == FaseJefe::DERROTADO)
    {
        DrawCircleV(posicion, radioBase * 0.3f, GRAY);
        return;
    }

    if (faseActual == FaseJefe::BULLET_HELL)
    {
        radioDibujo = Lerp(radioBase, radioBase * 0.3f, progresoMuerte);
        posDibujo.x += (float)GetRandomValue(-8, 8);
        posDibujo.y += (float)GetRandomValue(-8, 8);
        colorJefe = WHITE;
    }
    else if (faseActual == FaseJefe::TRANSFORMANDO)
    {
        posDibujo.x += (float)GetRandomValue(-8, 8);
        posDibujo.y += (float)GetRandomValue(-8, 8);
        colorJefe = WHITE;
    }
    else if (faseActual == FaseJefe::MURIENDO)
    {
        posDibujo.x += (float)GetRandomValue(-3, 3);
        posDibujo.y += (float)GetRandomValue(-3, 3);
        colorJefe = RED;
    }
    else if (faseActual == FaseJefe::FASE_UNO)
    {
        if (estadoF1 == EstadoFaseUno::APUNTANDO_EMBESTIDA) {
            float shake = 5.0f;
            posDibujo.x += (float)GetRandomValue(-shake, shake);
            posDibujo.y += (float)GetRandomValue(-shake, shake);
            colorJefe = YELLOW;
        }
        else if (estadoF1 == EstadoFaseUno::SALTANDO) {
            float t = progresoSalto;
            float escalaT;
            float escalaSalto;
            if (t < 0.5f) {
                escalaT = t / 0.5f;
                escalaSalto = Lerp(1.0f, 2.0f, escalaT);
                posDibujo = Vector2Lerp(inicioSalto, objetivoSalto, escalaT);
            } else {
                escalaT = (t - 0.5f) / 0.5f;
                escalaSalto = Lerp(2.0f, 1.0f, escalaT);
                posDibujo = objetivoSalto;
            }
            radioDibujo = radioBase * escalaSalto;
            float radioSombra = radioBase * escalaSalto;
            DrawCircleV(objetivoSalto, radioSombra, Fade(BLACK, 0.5f));
        }
        else if (estadoF1 == EstadoFaseUno::TIRANDO_CARNE)
        {
            float alpha = (sin(GetTime() * 10.0f) + 1.0f) / 2.0f;
            DrawCircleV(posDibujo, radioDibujo * 1.1f, Fade(DARKBROWN, alpha * 0.7f));
        }
        else if (estadoF1 == EstadoFaseUno::ESTIRANDO_BRAZO)
        {
            if (estadoBrazo == EstadoEstirarBrazo::CARGANDO_AVISO) {
                float alpha = (sin(GetTime() * 10.0f) + 1.0f) / 2.0f;
                DrawCircleV(posDibujo, radioDibujo * 1.1f, Fade(WHITE, alpha * 0.7f));
            } else {
                if (extensionBrazo > 0.0f)
                {
                    float largoCarneActual = 0.0f;
                    float largoHuesoActual = 0.0f;

                    if (extensionBrazo <= EXTENSION_CARNE_RELATIVA) {
                        float progresoCarne = extensionBrazo / EXTENSION_CARNE_RELATIVA;
                        largoCarneActual = (LARGO_BRAZO_MAXIMO * EXTENSION_CARNE_RELATIVA) * progresoCarne;
                    } else {
                        largoCarneActual = LARGO_BRAZO_MAXIMO * EXTENSION_CARNE_RELATIVA;
                        float progresoHueso = (extensionBrazo - EXTENSION_CARNE_RELATIVA) / EXTENSION_HUESO_RELATIVA;
                        largoHuesoActual = (LARGO_BRAZO_MAXIMO * EXTENSION_HUESO_RELATIVA) * progresoHueso;
                    }

                    Vector2 puntoFinalCarne = Vector2Add(posicion, Vector2Scale(objetivoBrazo, largoCarneActual));
                    Vector2 puntoFinalHueso = Vector2Add(puntoFinalCarne, Vector2Scale(objetivoBrazo, largoHuesoActual));

                    DrawLineEx(puntoFinalCarne, puntoFinalHueso, 20.0f, WHITE);
                    DrawLineEx(posicion, puntoFinalCarne, 40.0f, BROWN);
                }
            }
        }
    }
    else if (faseActual == FaseJefe::FASE_DOS)
    {
        if (estadoF2 == EstadoFaseDos::TELETRANSPORTANDO) {
            return;
        }
        if (estadoF2 == EstadoFaseDos::APUNTANDO_EMBESTIDA) {
            float shake = 5.0f;
            posDibujo.x += (float)GetRandomValue(-shake, shake);
            posDibujo.y += (float)GetRandomValue(-shake, shake);
            colorJefe = YELLOW;
        }
    }

    if (enTransicion) colorJefe = WHITE;

    if (estadoF1 == EstadoFaseUno::ATURDIDO_EMBESTIDA || estadoF2 == EstadoFaseDos::ATURDIDO_EMBESTIDA)
    {
        colorJefe = SKYBLUE;
    }

    if (esInvulnerable && !enTransicion) {
        DrawCircleV(posDibujo, radioDibujo * 1.2f, Fade(BLACK, 0.3f));
        colorJefe = Fade(VIOLET, 0.5f);
    }

    if (!(estadoF1 == EstadoFaseUno::ESTIRANDO_BRAZO && estadoBrazo == EstadoEstirarBrazo::CARGANDO_AVISO) &&
        !(estadoF1 == EstadoFaseUno::TIRANDO_CARNE))
    {
        DrawCircleV(posDibujo, radioDibujo, colorJefe);
    }

    if (estadoF1 != EstadoFaseUno::SALTANDO && faseActual < FaseJefe::MURIENDO)
    {
        Vector2 frente = Vector2Add(posDibujo, Vector2Scale(direccionVista, radioDibujo));
        DrawCircleV(frente, radioDibujo / 4, YELLOW);
    }
}
