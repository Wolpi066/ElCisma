#include "Jefe.h"
#include "Protagonista.h"
#include "Mapa.h"
#include "Constantes.h"
#include <cstdlib>
#include <cmath>
#include "BalaMonstruosa.h"
#include "MinaEnemiga.h"
#include "TrozoDeCarne.h"
#include "BalaInfernal.h"

// --- BALANCE ---
static const int VIDA_JEFE_NUEVA = 100;
static const int DANIO_CONTACTO_NUEVO = 5;
static const int DANIO_BRAZO_NUEVO = 5;

static const float VELOCIDAD_LENTA_JEFE = 0.5f;
static const float VELOCIDAD_LENTA_JEFE_F2 = 1.5f;
static const float VELOCIDAD_EMBESTIDA_JEFE_REDUCIDA = 6.0f;
static const float VELOCIDAD_TAMBALEO_MUERTE = 0.5f;

static const float TIEMPO_SALTO_JEFE = 2.0f;
static const float TIEMPO_BRAZO_CARGANDO_AVISO = 0.5f;
static const float TIEMPO_BRAZO_EXTENDIENDO = 1.0f;
static const float LARGO_BRAZO_MAXIMO = 300.0f;
static const float EXTENSION_CARNE_RELATIVA = 0.9f;
static const float EXTENSION_HUESO_RELATIVA = 0.1f;
static const float TIEMPO_TIRAR_CARNE = 0.5f;
static const float TIEMPO_TRANSFORMACION_FINAL = 3.0f;
static const float TIEMPO_BULLET_HELL = 60.0f;

static const float COOLDOWN_MINAS = 2.0f;
static const float COOLDOWN_ESCOPETA_F2 = 1.0f;
static const float COOLDOWN_PULSO_F2 = 1.5f;

static const float ESCALA_F1 = 0.125f;
static const float ESCALA_F2 = 0.095f;
static const float FPS_ANIMACION = 12.0f;

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
    introFinalizada(false),
    timerAnimacion(0.0f),
    frameActual(0),
    anguloBrazoVisual(0.0f),
    pasoIntro(0),
    timerIntroSequence(0.0f),
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
    bulletHellSpawnTimer(0),
    bulletHellAngleEspirales(0.0f)
{
    this->vidaMaxima = VIDA_JEFE_NUEVA;
    this->vida = this->vidaMaxima;
    balasGeneradas.clear();
    dropsGenerados.clear();
    CargarTexturas();
}

Jefe::~Jefe() {
    DescargarTexturas();
}

void CargarVector(std::vector<Texture2D>& vec, std::string rutaBase, int cantidad, bool baseCero = false) {
    for (int i = (baseCero ? 0 : 1); i <= (baseCero ? cantidad - 1 : cantidad); i++) {
        std::string ruta = rutaBase + std::to_string(i) + ".png";
        vec.push_back(LoadTexture(ruta.c_str()));
    }
}

void Jefe::CargarTexturas() {
    CargarVector(texF1Apareciendo, "assets/Jefe/Fase1/Apareciendo/Apareciendo", 5);
    CargarVector(texF1Caminando, "assets/Jefe/Fase1/Caminando/Caminando", 2);
    CargarVector(texF1Atacando, "assets/Jefe/Fase1/Atacando/Atacando", 4);
    CargarVector(texF1Embestida, "assets/Jefe/Fase1/Embestida/Embestida", 4);
    CargarVector(texF1Brazo, "assets/Jefe/Fase1/EstirandoBrazo/EstirandoBrazo", 13);
    CargarVector(texF1Salto, "assets/Jefe/Fase1/Saltando/Saltando", 6);
    CargarVector(texTransformacion, "assets/Jefe/Fase1/Transformacion/Transformacion", 6, true);

    CargarVector(texF2Caminando, "assets/Jefe/Fase2/Caminando/Caminando", 7);
    CargarVector(texF2Disparando, "assets/Jefe/Fase2/Disparando/Disparando", 4);
    CargarVector(texF2Embestida, "assets/Jefe/Fase2/Embestida/Embestida", 6, true);
    CargarVector(texF2Teleport, "assets/Jefe/Fase2/Teletransportandose/Teletransportandose", 5);
    CargarVector(texF2Enloqueciendo, "assets/Jefe/Fase2/Enloqueciendo/Enloqueciendo", 5);

    texMuerto = LoadTexture("assets/Jefe/Fase2/Muerto/JefeMuerto.png");
}

void Jefe::DescargarTexturas() {
    for (auto& t : texF1Apareciendo) UnloadTexture(t);
    for (auto& t : texF1Caminando) UnloadTexture(t);
    for (auto& t : texF1Atacando) UnloadTexture(t);
    for (auto& t : texF1Embestida) UnloadTexture(t);
    for (auto& t : texF1Brazo) UnloadTexture(t);
    for (auto& t : texF1Salto) UnloadTexture(t);
    for (auto& t : texTransformacion) UnloadTexture(t);
    for (auto& t : texF2Caminando) UnloadTexture(t);
    for (auto& t : texF2Disparando) UnloadTexture(t);
    for (auto& t : texF2Embestida) UnloadTexture(t);
    for (auto& t : texF2Teleport) UnloadTexture(t);
    for (auto& t : texF2Enloqueciendo) UnloadTexture(t);
    UnloadTexture(texMuerto);
}

// --- HELPER: DETECCIÓN DE PAREDES ---
bool Jefe::verificarColisionMuros(Vector2 pos, const Mapa& mapa) {
    Rectangle rectFuturo = getRect();
    rectFuturo.x = pos.x - rectFuturo.width/2;
    rectFuturo.y = pos.y - rectFuturo.height/2;

    for (const auto& muro : mapa.getMuros()) {
        if (CheckCollisionRecs(rectFuturo, muro)) return true;
    }
    return false;
}

void Jefe::actualizar(Protagonista& jugador, const Mapa& mapa) {

    if (!introFinalizada) return;

    if (faseActual >= FaseJefe::MURIENDO) {
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
        if (!enTransicion) transicionAFaseDos();
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

    if (!enTransicion) {
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

        if (faseActual == FaseJefe::FASE_UNO && estadoF1 == EstadoFaseUno::PAUSANDO) {
            velocidadActual = Vector2Scale(direccionVista, velocidadLenta);
        }
        else if (faseActual == FaseJefe::FASE_DOS && estadoF2 == EstadoFaseDos::PAUSANDO) {
            velocidadActual = Vector2Scale(direccionVista, VELOCIDAD_LENTA_JEFE_F2);
        }
    }

    switch (faseActual) {
        case FaseJefe::FASE_UNO: actualizarFaseUno(jugador, mapa); break;
        case FaseJefe::FASE_DOS: actualizarFaseDos(jugador, mapa); break;
        default: break;
    }
}

void Jefe::transicionAFaseDos() {
    enTransicion = true;
    temporizadorEstado = 3.0f;
    velocidadActual = {0, 0};
    estadoF1 = EstadoFaseUno::PAUSANDO;
    frameActual = 0;
    timerAnimacion = 0.0f;
}

void Jefe::actualizarFaseUno(Protagonista& jugador, const Mapa& mapa) {
    switch (estadoF1) {
        case EstadoFaseUno::PAUSANDO:
            if (temporizadorEstado <= 0) ejecutarPausaF1(jugador);
            break;
        case EstadoFaseUno::APUNTANDO_EMBESTIDA:
            if (temporizadorEstado <= 0) {
                estadoF1 = EstadoFaseUno::EMBISTIENDO;
                direccionVista = objetivoEmbestida;
                temporizadorEmbestida = ConstantesJefe::TIEMPO_MAX_EMBESTIDA;
                frameActual = 0;
                timerAnimacion = 0.0f;
            }
            break;
        case EstadoFaseUno::EMBISTIENDO:
            velocidadActual = Vector2Scale(objetivoEmbestida, VELOCIDAD_EMBESTIDA_JEFE_REDUCIDA);
            temporizadorEmbestida -= GetFrameTime();

            // --- CORRECCIÓN COLISIÓN FASE 1 ---
            // ¡QUITADO GetFrameTime()! Ahora detecta 3 frames por delante (aprox 20px)
            if (verificarColisionMuros(Vector2Add(posicion, Vector2Scale(velocidadActual, 3.0f)), mapa)) {
                estadoF1 = EstadoFaseUno::ATURDIDO_EMBESTIDA;
                temporizadorEstado = ConstantesJefe::COOLDOWN_ATURDIMIENTO;
                velocidadActual = {0,0};
            }

            if (temporizadorEmbestida <= 0) {
                estadoF1 = EstadoFaseUno::PAUSANDO;
                temporizadorEstado = 1.0f;
            }
            break;
        case EstadoFaseUno::ATURDIDO_EMBESTIDA:
            if (!haSoltadoLootStun) {
                haSoltadoLootStun = true;
                Vector2 posDrop = {posicion.x + GetRandomValue(-80, 80), posicion.y + GetRandomValue(-80, 80)};
                dropsGenerados.push_back({posDrop, 3});
                int dropsExtra = GetRandomValue(1, 2);
                for(int i=0; i < dropsExtra; i++) {
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

            if (progresoSalto > 0.2f && progresoSalto < 0.8f) {
                float tVuelo = (progresoSalto - 0.2f) / 0.6f;
                posicion = Vector2Lerp(inicioSalto, objetivoSalto, tVuelo);
            }

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
    if (jugador.getMunicion() <= 0) {
        estadoF1 = EstadoFaseUno::APUNTANDO_EMBESTIDA;
        temporizadorEstado = ConstantesJefe::TIEMPO_APUNTAR_EMBESTIDA;
        objetivoEmbestida = direccionVista;
        return;
    }
    int ataque = GetRandomValue(0, 9);
    if (ataque == 0) {
        estadoF1 = EstadoFaseUno::APUNTANDO_EMBESTIDA;
        temporizadorEstado = ConstantesJefe::TIEMPO_APUNTAR_EMBESTIDA;
        objetivoEmbestida = direccionVista;
    } else if (ataque <= 3) {
        ejecutarSalto(jugador);
    } else if (ataque <= 6) {
        ejecutarEstirarBrazo(jugador);
        float angleRad = atan2f(direccionVista.y, direccionVista.x);
        anguloBrazoVisual = (angleRad * RAD2DEG) - 90.0f;
    } else {
        estadoF1 = EstadoFaseUno::TIRANDO_CARNE;
        temporizadorEstado = TIEMPO_TIRAR_CARNE;
        frameActual = 0;
        timerAnimacion = 0.0f;
    }
}

void Jefe::ejecutarSalto(Protagonista& jugador) {
    estadoF1 = EstadoFaseUno::SALTANDO;
    temporizadorEstado = TIEMPO_SALTO_JEFE;
    objetivoSalto = jugador.getPosicion();
    inicioSalto = posicion;
    progresoSalto = 0.0f;
    frameActual = 0;
}

void Jefe::ejecutarEstirarBrazo(Protagonista& jugador) {
    estadoF1 = EstadoFaseUno::ESTIRANDO_BRAZO;
    estadoBrazo = EstadoEstirarBrazo::CARGANDO_AVISO;
    temporizadorEstado = TIEMPO_BRAZO_CARGANDO_AVISO;
    extensionBrazo = 0.0f;
}

void Jefe::ejecutarTirarCarne(Protagonista& jugador) {
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

void Jefe::actualizarFaseDos(Protagonista& jugador, const Mapa& mapa) {
    switch (estadoF2) {
        case EstadoFaseDos::PAUSANDO:
            if (temporizadorEstado <= 0) ejecutarPausaF2(jugador);
            break;
        case EstadoFaseDos::TELETRANSPORTANDO:
            esInvulnerable = true;
            if (temporizadorEstado <= 0) {

                // --- BUCLE SEGURIDAD TP ---
                int intentos = 0;
                Vector2 posTest;
                do {
                    posTest.x = (float)GetRandomValue(-300, 300);
                    posTest.y = (float)GetRandomValue(-300, 300);
                    intentos++;
                } while (verificarColisionMuros(posTest, mapa) && intentos < 50);
                posicion = posTest;

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
            if (temporizadorEstado > 0.0f) objetivoEmbestida = direccionVista;
            if (temporizadorEstado <= 0) {
                estadoF2 = EstadoFaseDos::EMBISTIENDO;
                direccionVista = objetivoEmbestida;
                temporizadorEmbestida = ConstantesJefe::TIEMPO_MAX_EMBESTIDA;
                frameActual = 0;
                timerAnimacion = 0.0f;
            }
            break;
        case EstadoFaseDos::EMBISTIENDO:
            velocidadActual = Vector2Scale(objetivoEmbestida, VELOCIDAD_EMBESTIDA_JEFE_REDUCIDA);
            temporizadorEmbestida -= GetFrameTime();

            // --- CORRECCIÓN COLISIÓN FASE 2 ---
            // ¡Quitado GetFrameTime()! Detecta 3 frames adelante (aprox 20px)
            if (verificarColisionMuros(Vector2Add(posicion, Vector2Scale(velocidadActual, 3.0f)), mapa)) {
                estadoF2 = EstadoFaseDos::ATURDIDO_EMBESTIDA;
                temporizadorEstado = ConstantesJefe::COOLDOWN_ATURDIMIENTO;
                velocidadActual = {0,0};
                haSoltadoLootStun = false; // Reset loot
            }

            if (temporizadorEmbestida <= 0) {
                estadoF2 = EstadoFaseDos::RECUPERANDO_EMBESTIDA;
                temporizadorEstado = 1.5f;
                frameActual = 4;
            }
            break;
        case EstadoFaseDos::RECUPERANDO_EMBESTIDA:
            if (temporizadorEstado <= 0) {
                estadoF2 = EstadoFaseDos::PAUSANDO;
                temporizadorEstado = 0.5f;
            }
            break;
        case EstadoFaseDos::ATURDIDO_EMBESTIDA:
            if (!haSoltadoLootStun) {
                haSoltadoLootStun = true;
                Vector2 posDrop = {posicion.x + GetRandomValue(-80, 80), posicion.y + GetRandomValue(-80, 80)};
                dropsGenerados.push_back({posDrop, 3});
                int dropsExtra = GetRandomValue(1, 2);
                for(int i=0; i < dropsExtra; i++) {
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
    if (jugador.getMunicion() <= 0) {
        estadoF2 = EstadoFaseDos::APUNTANDO_EMBESTIDA;
        temporizadorEstado = ConstantesJefe::TIEMPO_APUNTAR_EMBESTIDA;
        objetivoEmbestida = direccionVista;
        return;
    }
    int ataque = GetRandomValue(0, 3);
    switch(ataque) {
        case 0:
            estadoF2 = EstadoFaseDos::TELETRANSPORTANDO;
            temporizadorEstado = 0.3f;
            frameActual = 0;
            timerAnimacion = 0.0f;
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

void Jefe::ejecutarTeletransporte() {
    // Logica integrada en switch
}

void Jefe::ejecutarPlantandoMinas() {
    Vector2 dir = direccionVista;
    balasGeneradas.push_back(new MinaEnemiga(posicion, dir));
    balasGeneradas.push_back(new MinaEnemiga(posicion, Vector2Rotate(dir, -0.4f)));
    balasGeneradas.push_back(new MinaEnemiga(posicion, Vector2Rotate(dir, 0.4f)));
    balasGeneradas.push_back(new MinaEnemiga(posicion, Vector2Rotate(dir, -0.8f)));
    balasGeneradas.push_back(new MinaEnemiga(posicion, Vector2Rotate(dir, 0.8f)));
}

void Jefe::ejecutarAtaqueSombra(Protagonista& jugador) {}
void Jefe::ejecutarDisparoInteligente(Protagonista& jugador) {}

void Jefe::actualizarFaseMuerte(Protagonista& jugador, const Mapa& mapa) {
    esInvulnerable = true;
    temporizadorEstado -= GetFrameTime();

    switch (faseActual) {
        case FaseJefe::MURIENDO:
        {
            Vector2 dirAlCentro = Vector2Normalize(Vector2Subtract({0, 0}, posicion));
            velocidadActual = Vector2Scale(dirAlCentro, VELOCIDAD_TAMBALEO_MUERTE);
            if (Vector2DistanceSqr(posicion, {0, 0}) < 10.0f) {
                posicion = {0, 0};
                velocidadActual = {0, 0};
                faseActual = FaseJefe::TRANSFORMANDO;
                temporizadorEstado = TIEMPO_TRANSFORMACION_FINAL;
                frameActual = 0;
                timerAnimacion = 0.0f;
            }
            break;
        }
        case FaseJefe::TRANSFORMANDO:
            if (temporizadorEstado <= 0) {
                faseActual = FaseJefe::BULLET_HELL;
                temporizadorEstado = TIEMPO_BULLET_HELL;
                progresoMuerte = 0.0f;
                bulletHellBaseDirection = 0.0f;
                bulletHellSpawnTimer = 0;
                bulletHellAngleEspirales = 0.0f;
            }
            break;
        case FaseJefe::BULLET_HELL:
        {
            float tiempoPasado = TIEMPO_BULLET_HELL - temporizadorEstado;
            progresoMuerte = Clamp(tiempoPasado / TIEMPO_BULLET_HELL, 0.0f, 1.0f);
            bulletHellSpawnTimer++;
            bulletHellBaseDirection += 0.025f;
            bulletHellAngleEspirales += BH_ESPIRAL_ROTACION;

            if (temporizadorEstado > 48.0f) {
                if (bulletHellSpawnTimer % BH_PULSO_COOLDOWN == 0) {
                    float anguloBase = bulletHellBaseDirection;
                    for (int i = 0; i < BH_PULSO_CANTIDAD_BALAS; i++) {
                        float angulo = anguloBase + (i * (2 * PI / BH_PULSO_CANTIDAD_BALAS));
                        Vector2 dir = { cosf(angulo), sinf(angulo) };
                        balasGeneradas.push_back(new BalaInfernal(posicion, dir));
                    }
                }
            } else if (temporizadorEstado > 36.0f) {
                if (bulletHellSpawnTimer % BH_RAFAGA_COOLDOWN == 0) {
                    Vector2 dirAlJugador = Vector2Normalize(Vector2Subtract(jugador.getPosicion(), posicion));
                    float anguloBase = atan2f(dirAlJugador.y, dirAlJugador.x);
                    for (int i = 0; i < BH_RAFAGA_CANTIDAD_BALAS; i++) {
                        float offset = (i - (BH_RAFAGA_CANTIDAD_BALAS / 2.0f)) * BH_RAFAGA_SEPARACION;
                        float angulo = anguloBase + offset;
                        Vector2 dir = { cosf(angulo), sinf(angulo) };
                        balasGeneradas.push_back(new BalaInfernal(posicion, dir));
                    }
                }
            } else if (temporizadorEstado > 24.0f) {
                if (bulletHellSpawnTimer % BH_OLA_COOLDOWN == 0) {
                    float sinOffset = sinf(bulletHellBaseDirection * 5.0f);
                    float centroDelHueco = sinOffset * (BH_OLA_AMPLITUD / 2.0f);
                    float spawnY = posicion.y + BH_OLA_Y_ORIGEN_OFFSET;
                    for (int i = 0; i < BH_OLA_CANTIDAD_BALAS; i++) {
                        float posX = posicion.x - BH_OLA_AMPLITUD + (i * (BH_OLA_AMPLITUD * 2.0f / BH_OLA_CANTIDAD_BALAS));
                        if (fabsf(posX - (posicion.x + centroDelHueco)) < (BH_OLA_HUECO / 2.0f)) continue;
                        Vector2 origenBala = {posX, spawnY};
                        Vector2 dirBala = {0.0f, 1.0f};
                        balasGeneradas.push_back(new BalaInfernal(origenBala, dirBala));
                    }
                }
            } else if (temporizadorEstado > 12.0f) {
                if (bulletHellSpawnTimer % BH_ESPIRAL_COOLDOWN == 0) {
                    float degreesPerRow = 360.0f / BH_ESPIRAL_FILAS;
                    for (int row = 0; row < BH_ESPIRAL_FILAS; row++) {
                        float bulletDirection = bulletHellAngleEspirales + (degreesPerRow * row);
                        Vector2 dir = { cosf(bulletDirection * DEG2RAD), sinf(bulletDirection * DEG2RAD) };
                        balasGeneradas.push_back(new BalaInfernal(posicion, dir));
                    }
                }
            } else {
                if (bulletHellSpawnTimer % BH_PULSO_DOBLE_COOLDOWN == 0) {
                    float anguloBase = bulletHellBaseDirection;
                    for (int i = 0; i < BH_PULSO_CANTIDAD_BALAS; i++) {
                        float angulo = anguloBase + (i * (2 * PI / BH_PULSO_CANTIDAD_BALAS));
                        Vector2 dir = { cosf(angulo), sinf(angulo) };
                        balasGeneradas.push_back(new BalaInfernal(posicion, dir));
                    }
                }
                if (bulletHellSpawnTimer % BH_PULSO_DOBLE_COOLDOWN == BH_PULSO_DOBLE_DELAY) {
                    float anguloBase = bulletHellBaseDirection + (PI / BH_PULSO_CANTIDAD_BALAS);
                    for (int i = 0; i < BH_PULSO_CANTIDAD_BALAS; i++) {
                        float angulo = anguloBase + (i * (2 * PI / BH_PULSO_CANTIDAD_BALAS));
                        Vector2 dir = { cosf(angulo), sinf(angulo) };
                        balasGeneradas.push_back(new BalaInfernal(posicion, dir));
                    }
                }
            }

            if (temporizadorEstado <= 0) {
                faseActual = FaseJefe::DERROTADO;
                progresoMuerte = 1.0f;
            }
            break;
        }
        case FaseJefe::DERROTADO:
            velocidadActual = {0, 0};
            break;
        default: break;
    }
}

void Jefe::forzarFaseDos() {
    if (faseActual == FaseJefe::FASE_DOS || faseActual >= FaseJefe::MURIENDO || enTransicion) return;
    int vidaFaseDos = (int)(vidaMaxima * ConstantesJefe::VIDA_FASE_DOS_TRIGGER);
    vida = vidaFaseDos;
    transicionAFaseDos();
}

void Jefe::forzarMuerte() {
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
    if (dot <= 0.707f) danioFinal = 2;
    if (estadoF1 == EstadoFaseUno::ATURDIDO_EMBESTIDA || estadoF2 == EstadoFaseDos::ATURDIDO_EMBESTIDA) danioFinal *= 2;
    vida -= danioFinal;
    if (vida <= 0) {
        vida = 1;
        faseActual = FaseJefe::MURIENDO;
        estadoF1 = EstadoFaseUno::PAUSANDO;
        estadoF2 = EstadoFaseDos::PAUSANDO;
        enTransicion = false;
        esInvulnerable = true;
        temporizadorEstado = 0.0f;
    }
}

std::vector<Bala*>& Jefe::getBalasGeneradas() { return balasGeneradas; }
void Jefe::limpiarBalasGeneradas() { balasGeneradas.clear(); }
std::vector<DropInfo>& Jefe::getDropsGenerados() { return dropsGenerados; }
void Jefe::limpiarDropsGenerados() { dropsGenerados.clear(); }

Rectangle Jefe::getHitboxBrazo() const {
    if (faseActual != FaseJefe::FASE_UNO ||
        estadoF1 != EstadoFaseUno::ESTIRANDO_BRAZO ||
        extensionBrazo <= 0.0f ||
        estadoBrazo == EstadoEstirarBrazo::CARGANDO_AVISO) return {0, 0, 0, 0};

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
    return { finalBrazo.x - anchoMano / 2, finalBrazo.y - anchoMano / 2, anchoMano, anchoMano };
}

void Jefe::setPosicion(Vector2 nuevaPos) { this->posicion = nuevaPos; }
void Jefe::setVelocidad(Vector2 vel) { this->velocidadActual = vel; }
Vector2 Jefe::getVelocidadActual() const { return velocidadActual; }
bool Jefe::estaVivo() const { return faseActual != FaseJefe::DERROTADO; }
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
bool Jefe::esInvulnerableActualmente() const { return enTransicion || esInvulnerable || faseActual >= FaseJefe::MURIENDO; }
bool Jefe::estaEnFaseFinal() const { return faseActual == FaseJefe::DERROTADO; }
bool Jefe::estaMuerto() const { return false; }
bool Jefe::estaConsumido() const { return false; }
EstadoFaseUno Jefe::getEstadoF1() const { return estadoF1; }
void Jefe::setEstadoF1(EstadoFaseUno nuevoEstado) { this->estadoF1 = nuevoEstado; }
EstadoFaseDos Jefe::getEstadoF2() const { return estadoF2; }
void Jefe::setEstadoF2(EstadoFaseDos nuevoEstado) { this->estadoF2 = nuevoEstado; }
void Jefe::setTemporizadorEstado(float tiempo) { this->temporizadorEstado = tiempo; }


// =========================================================================
// IMPLEMENTACIÓN VISUAL
// =========================================================================

void Jefe::dibujar() {

    std::vector<Texture2D>* animacionActual = nullptr;
    float rotation = 0.0f;
    float escalaActual = (faseActual == FaseJefe::FASE_UNO) ? ESCALA_F1 : ESCALA_F2;
    float velocidadAnimacion = FPS_ANIMACION;
    Vector2 offsetDibujo = {0, 0};
    Color tint = WHITE;

    // 1. INTRO PERSONALIZADA (Secuencia exacta)
    if (!introFinalizada) {
        animacionActual = &texF1Apareciendo;
        timerIntroSequence += GetFrameTime();

        switch (pasoIntro) {
            case 0: // Espera inicial 1s
                frameActual = 0;
                if (timerIntroSequence > 1.0f) { pasoIntro++; timerIntroSequence = 0; }
                break;
            case 1: // Frame 1 (3s)
                frameActual = 0;
                if (timerIntroSequence > 3.0f) { pasoIntro++; timerIntroSequence = 0; }
                break;
            case 2: // Frame 2 (3s)
                frameActual = 1;
                if (timerIntroSequence > 3.0f) { pasoIntro++; timerIntroSequence = 0; }
                break;
            case 3: // Frame 3 (1s)
                frameActual = 2;
                if (timerIntroSequence > 1.0f) { pasoIntro++; timerIntroSequence = 0; }
                break;
            case 4: // Frame 1 (3s)
                frameActual = 0;
                if (timerIntroSequence > 3.0f) { pasoIntro++; timerIntroSequence = 0; }
                break;
            case 5: // Frame 2 (0.5s)
                frameActual = 1;
                if (timerIntroSequence > 0.5f) { pasoIntro++; timerIntroSequence = 0; }
                break;
            case 6: // Frame Final (Fin)
                frameActual = 4;
                introFinalizada = true;
                break;
        }
    }
    // 2. TRANSICIÓN
    else if (enTransicion) {
        animacionActual = &texTransformacion;
        offsetDibujo.x = (float)GetRandomValue(-2, 2);
        offsetDibujo.y = (float)GetRandomValue(-2, 2);
        escalaActual = ESCALA_F1;
    }
    // 3. FASE 1
    else if (faseActual == FaseJefe::FASE_UNO) {
        switch(estadoF1) {
            case EstadoFaseUno::PAUSANDO:
            case EstadoFaseUno::APUNTANDO_EMBESTIDA:
                animacionActual = &texF1Caminando;
                velocidadAnimacion = 6.0f;
                rotation = (frameActual % 2 == 0) ? -5.0f : 5.0f;
                break;

            case EstadoFaseUno::EMBISTIENDO:
                animacionActual = &texF1Embestida;
                velocidadAnimacion = 12.0f;
                frameActual = 1;
                break;

            case EstadoFaseUno::ATURDIDO_EMBESTIDA:
                animacionActual = &texF1Embestida;
                frameActual = 3;
                break;

            case EstadoFaseUno::ESTIRANDO_BRAZO:
                animacionActual = &texF1Brazo;
                if (texF1Brazo.size() > 0) {
                    if (estadoBrazo == EstadoEstirarBrazo::CARGANDO_AVISO) {
                        frameActual = 0;
                    } else {
                        float progreso = 1.0f - (temporizadorEstado / TIEMPO_BRAZO_EXTENDIENDO);
                        progreso = Clamp(progreso, 0.0f, 1.0f);
                        frameActual = (int)(progreso * (texF1Brazo.size() - 1));
                    }
                }
                break;
            case EstadoFaseUno::SALTANDO:
                animacionActual = &texF1Salto;
                if (progresoSalto < 0.2f) frameActual = 0;
                else if (progresoSalto < 0.8f) frameActual = 2;
                else frameActual = 5;
                break;
            case EstadoFaseUno::TIRANDO_CARNE:
                animacionActual = &texF1Atacando;
                break;
        }
    }
    // 4. FASE 2
    else if (faseActual == FaseJefe::FASE_DOS) {
        switch(estadoF2) {
            case EstadoFaseDos::PAUSANDO:
            case EstadoFaseDos::APUNTANDO_EMBESTIDA:
                animacionActual = &texF2Caminando; break;

            // --- VISUALES EMBESTIDA F2 ---
            case EstadoFaseDos::EMBISTIENDO:
                animacionActual = &texF2Embestida;
                if (frameActual > 2) frameActual = 2;
                break;
            case EstadoFaseDos::RECUPERANDO_EMBESTIDA:
                animacionActual = &texF2Embestida;
                frameActual = 4;
                break;
            case EstadoFaseDos::ATURDIDO_EMBESTIDA:
                animacionActual = &texF2Embestida;
                frameActual = 5;
                break;
            // -----------------------------

            case EstadoFaseDos::DISPARO_ESCOPETA:
            case EstadoFaseDos::PLANTANDO_MINAS:
            case EstadoFaseDos::PULSO_RADIAL:
                animacionActual = &texF2Disparando; break;
            case EstadoFaseDos::TELETRANSPORTANDO:
                animacionActual = &texF2Teleport; break;
        }
    }
    // 5. MURIENDO/BULLET HELL
    else if (faseActual == FaseJefe::MURIENDO) {
        animacionActual = &texF2Enloqueciendo;
        frameActual = 0;
        offsetDibujo.x = (float)GetRandomValue(-2, 2);
        offsetDibujo.y = (float)GetRandomValue(-2, 2);
    }
    else if (faseActual == FaseJefe::TRANSFORMANDO) {
        animacionActual = &texF2Enloqueciendo;
    }
    else if (faseActual == FaseJefe::BULLET_HELL) {
        animacionActual = &texF2Enloqueciendo;
        escalaActual = ESCALA_F2 * 1.2f;
        frameActual = (int)texF2Enloqueciendo.size() - 1;
        offsetDibujo.x = (float)GetRandomValue(-6, 6);
        offsetDibujo.y = (float)GetRandomValue(-6, 6);
        float t = GetTime() * 10.0f;
        tint = (sinf(t) > 0) ? RED : (Color){255, 100, 255, 255};
    }

    // --- RENDER ---
    if (animacionActual && !animacionActual->empty()) {

        bool controlManual = (faseActual == FaseJefe::FASE_UNO &&
             (estadoF1 == EstadoFaseUno::ESTIRANDO_BRAZO || estadoF1 == EstadoFaseUno::SALTANDO));

        bool freezeF1Stun = (estadoF1 == EstadoFaseUno::ATURDIDO_EMBESTIDA);
        bool freezeF2Dash = (estadoF2 == EstadoFaseDos::RECUPERANDO_EMBESTIDA || estadoF2 == EstadoFaseDos::ATURDIDO_EMBESTIDA);
        bool freezeBH = (faseActual == FaseJefe::MURIENDO || faseActual == FaseJefe::BULLET_HELL);

        if (!controlManual && !freezeF1Stun && !freezeF2Dash && !freezeBH && introFinalizada) {
            timerAnimacion += GetFrameTime();
            if (timerAnimacion >= (1.0f / velocidadAnimacion)) {
                frameActual++;
                timerAnimacion = 0.0f;
            }
        }

        // Avance específico arranque dash F2
        if (estadoF2 == EstadoFaseDos::EMBISTIENDO && frameActual < 2) {
             timerAnimacion += GetFrameTime();
             if (timerAnimacion >= (1.0f / 12.0f)) {
                 frameActual++;
                 timerAnimacion = 0.0f;
             }
        }

        // Clamps y Loops
        if (freezeF1Stun || freezeF2Dash || !introFinalizada) {
            // Control manual
        }
        else if (estadoF1 == EstadoFaseUno::TIRANDO_CARNE || faseActual == FaseJefe::TRANSFORMANDO) {
            frameActual = Clamp(frameActual, 0, (int)animacionActual->size() - 1);
        }
        else if (enTransicion) {
             frameActual = frameActual % static_cast<int>(animacionActual->size());
        }
        else if (!controlManual && !freezeBH && frameActual >= static_cast<int>(animacionActual->size())) {
            frameActual = 0;
        }

        if (frameActual >= static_cast<int>(animacionActual->size())) frameActual = 0;

        Texture2D tex = (*animacionActual)[frameActual];

        float width = (float)tex.width;
        float height = (float)tex.height;
        if (direccionVista.x < 0) width = -width;

        Rectangle sourceRec = { 0.0f, 0.0f, width, height };
        Rectangle destRec = {
            posicion.x + offsetDibujo.x,
            posicion.y + offsetDibujo.y,
            (float)tex.width * escalaActual,
            (float)tex.height * escalaActual
        };

        Vector2 origin = { destRec.width / 2.0f, destRec.height / 2.0f };

        // --- MEJORA BRAZO ---
        if (faseActual == FaseJefe::FASE_UNO && estadoF1 == EstadoFaseUno::ESTIRANDO_BRAZO) {
            float angleRad = atan2f(objetivoBrazo.y, objetivoBrazo.x);
            float targetRot = (angleRad * RAD2DEG) - 90.0f;

            if (direccionVista.x < 0) {
                float relativeAngle = targetRot + 90.0f;
                targetRot = -90.0f - relativeAngle;
            }

            anguloBrazoVisual += (targetRot - anguloBrazoVisual) * 0.2f;
            rotation = anguloBrazoVisual;

            origin.y = destRec.height * 0.5f;
            if (direccionVista.x < 0) origin.x = destRec.width * 0.75f;
            else origin.x = destRec.width * 0.25f;
        }

        DrawTexturePro(tex, sourceRec, destRec, origin, rotation, tint);
    }
    else if (faseActual == FaseJefe::DERROTADO) {
        float w = (float)texMuerto.width;
        if (direccionVista.x < 0) w = -w;
        Rectangle source = {0, 0, w, (float)texMuerto.height};
        float escalaMuerte = ESCALA_F2 * 0.7f;
        Rectangle dest = {posicion.x, posicion.y, fabsf(w)*escalaMuerte, (float)texMuerto.height*escalaMuerte};
        DrawTexturePro(texMuerto, source, dest, {dest.width/2, dest.height/2}, 0.0f, WHITE);
    }

    if (esInvulnerable && !enTransicion && !introFinalizada && faseActual < FaseJefe::MURIENDO) {
        DrawCircleV(posicion, 50 * escalaActual * 10.0f, Fade(WHITE, 0.3f));
    }
}
