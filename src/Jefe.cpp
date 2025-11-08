#include "Jefe.h"
#include "Protagonista.h"
#include "Mapa.h"
#include "Constantes.h"
#include <cstdlib>
#include "BalaMonstruosa.h"
#include "MinaEnemiga.h"

static const float VELOCIDAD_EMBESTIDA_JEFE_REDUCIDA = 450.0f;

static const float TIEMPO_SALTO_JEFE = 2.0f;
static const float TIEMPO_BRAZO_CARGANDO_AVISO = 0.5f;
static const float TIEMPO_BRAZO_EXTENDIENDO = 1.0f;
static const float LARGO_BRAZO_MAXIMO = 300.0f;
static const float EXTENSION_CARNE_RELATIVA = 0.9f;
static const float EXTENSION_HUESO_RELATIVA = 0.1f;

static const float COOLDOWN_MINAS = 2.0f;
static const float COOLDOWN_ESCOPETA_F2 = 1.0f;
static const float COOLDOWN_PULSO_F2 = 1.5f;


Jefe::Jefe(Vector2 pos) :
    posicion(pos),
    velocidadActual({0, 0}),
    direccionVista({0, 1}),
    vidaMaxima(ConstantesJefe::VIDA_JEFE),
    vida(ConstantesJefe::VIDA_JEFE),
    danioContacto(20),
    radioHitbox(ConstantesJefe::RADIO_JEFE),
    velocidadLenta(50.0f),
    faseActual(FaseJefe::FASE_UNO),
    enTransicion(false),
    esInvulnerable(false),
    estadoF1(EstadoFaseUno::PAUSANDO),
    estadoF2(EstadoFaseDos::PAUSANDO),
    temporizadorEstado(2.5f),
    objetivoEmbestida({0, 0}),
    temporizadorEmbestida(0.0f),
    objetivoSalto({0,0}),
    // --- ¡¡FIX SALTO!! ---
    inicioSalto({0,0}),
    progresoSalto(0.0f),
    // -------------------
    objetivoBrazo({0,0}),
    extensionBrazo(0.0f),
    estadoBrazo(EstadoEstirarBrazo::CARGANDO_AVISO)
{
    this->vidaMaxima = ConstantesJefe::VIDA_JEFE;
    this->vida = this->vidaMaxima;
    balasGeneradas.clear();
}

void Jefe::actualizar(Protagonista& jugador, const Mapa& mapa) {

    if (!estaVivo()) {
        velocidadActual = {0,0};
        return;
    }

    velocidadActual = {0, 0};

    if (estadoF1 == EstadoFaseUno::EMBISTIENDO ||
        estadoF2 == EstadoFaseDos::TELETRANSPORTANDO)
    {
         velocidadActual = getVelocidadActual();
    }

    limpiarBalasGeneradas();

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
        if (estadoF1 == EstadoFaseUno::PAUSANDO || estadoF1 == EstadoFaseUno::APUNTANDO_EMBESTIDA ||
            estadoF2 == EstadoFaseDos::PAUSANDO ||
            (estadoF1 == EstadoFaseUno::ESTIRANDO_BRAZO && estadoBrazo == EstadoEstirarBrazo::CARGANDO_AVISO)) // Sigue mirando en el aviso
        {
            if (Vector2DistanceSqr(jugador.getPosicion(), posicion) > 1.0f) {
                direccionVista = Vector2Normalize(Vector2Subtract(jugador.getPosicion(), posicion));
            }
        }

        if (faseActual == FaseJefe::FASE_UNO && estadoF1 == EstadoFaseUno::PAUSANDO)
        {
            velocidadActual = Vector2Scale(direccionVista, velocidadLenta * GetFrameTime());
        }
        else if (faseActual == FaseJefe::FASE_DOS && estadoF2 == EstadoFaseDos::PAUSANDO)
        {
            velocidadActual = Vector2Scale(direccionVista, (velocidadLenta * 1.5f) * GetFrameTime());
        }
    }


    switch (faseActual) {
        case FaseJefe::FASE_UNO:
            actualizarFaseUno(jugador, mapa);
            break;
        case FaseJefe::FASE_DOS:
            actualizarFaseDos(jugador, mapa);
            break;
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
            if (temporizadorEstado > 0.0f) {
                 objetivoEmbestida = direccionVista;
            }
            if (temporizadorEstado <= 0) {
                estadoF1 = EstadoFaseUno::EMBISTIENDO;
                // --- ¡¡FIX GIRO DE CABEZA!! ---
                direccionVista = objetivoEmbestida; // Fija la mirada AHORA
                // -----------------------------
                temporizadorEmbestida = ConstantesJefe::TIEMPO_MAX_EMBESTIDA;
            }
            break;

        case EstadoFaseUno::EMBISTIENDO:
        {
            float velPorFrame = VELOCIDAD_EMBESTIDA_JEFE_REDUCIDA * GetFrameTime();
            velocidadActual = Vector2Scale(objetivoEmbestida, velPorFrame);

            temporizadorEmbestida -= GetFrameTime();
            if (temporizadorEmbestida <= 0) {
                estadoF1 = EstadoFaseUno::PAUSANDO;
                temporizadorEstado = 1.0f;
            }
            break;
        }

        case EstadoFaseUno::ATURDIDO_EMBESTIDA:
            if (temporizadorEstado <= 0) {
                estadoF1 = EstadoFaseUno::PAUSANDO;
                temporizadorEstado = ConstantesJefe::COOLDOWN_ATURDIMIENTO;
            }
            break;

        case EstadoFaseUno::SALTANDO:
        {
            // --- ¡¡FIX SALTO!! ---
            // (Ya no es invulnerable)
            float tiempoMax = TIEMPO_SALTO_JEFE;
            float tiempoPasado = tiempoMax - temporizadorEstado;
            progresoSalto = Clamp(tiempoPasado / tiempoMax, 0.0f, 1.0f); // 0.0 -> 1.0

            if (temporizadorEstado <= 0) {
                progresoSalto = 1.0f;
                posicion = objetivoSalto; // Aterriza
                ejecutarPulsoRadial();
                estadoF1 = EstadoFaseUno::PAUSANDO;
                temporizadorEstado = 1.5f;
            }
            break;
            // -------------------
        }

        case EstadoFaseUno::ESTIRANDO_BRAZO:
        {
            if (estadoBrazo == EstadoEstirarBrazo::CARGANDO_AVISO) {
                if (temporizadorEstado <= 0) {
                    estadoBrazo = EstadoEstirarBrazo::EXTENDIENDO_CARNE;
                    temporizadorEstado = TIEMPO_BRAZO_EXTENDIENDO;
                    objetivoBrazo = direccionVista; // Fija la dirección del brazo
                    extensionBrazo = 0.0f;
                }
                extensionBrazo = 0.0f;
            } else {
                float duracionEstirar = temporizadorEstado;
                float tiempoMax = TIEMPO_BRAZO_EXTENDIENDO;
                float tiempoPasado = tiempoMax - duracionEstirar;

                // La extensión total va de 0 a 1.0
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
    }
}

void Jefe::ejecutarPausaF1(Protagonista& jugador) {
    int ataque = GetRandomValue(0, 3);
    if (ataque == 0) {
        estadoF1 = EstadoFaseUno::APUNTANDO_EMBESTIDA;
        temporizadorEstado = ConstantesJefe::TIEMPO_APUNTAR_EMBESTIDA;
    } else if (ataque == 1) {
        ejecutarSalto(jugador);
    } else {
        ejecutarEstirarBrazo(jugador);
    }
}

void Jefe::ejecutarSalto(Protagonista& jugador)
{
    estadoF1 = EstadoFaseUno::SALTANDO;
    temporizadorEstado = TIEMPO_SALTO_JEFE;
    objetivoSalto = jugador.getPosicion();
    // --- ¡¡FIX SALTO!! ---
    inicioSalto = posicion;   // Guarda dónde empieza
    progresoSalto = 0.0f;     // Reinicia el progreso
    // -------------------
}

void Jefe::ejecutarEstirarBrazo(Protagonista& jugador)
{
    estadoF1 = EstadoFaseUno::ESTIRANDO_BRAZO;
    estadoBrazo = EstadoEstirarBrazo::CARGANDO_AVISO;
    temporizadorEstado = TIEMPO_BRAZO_CARGANDO_AVISO;
    extensionBrazo = 0.0f;
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
                ejecutarPausaF2();
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
    }
}

void Jefe::ejecutarPausaF2() {
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
    balasGeneradas.push_back(new MinaEnemiga(posicion, Vector2Rotate(dir, -0.3f)));
    balasGeneradas.push_back(new MinaEnemiga(posicion, Vector2Rotate(dir, 0.3f)));
    balasGeneradas.push_back(new MinaEnemiga(posicion, Vector2Rotate(dir, -0.6f)));
    balasGeneradas.push_back(new MinaEnemiga(posicion, Vector2Rotate(dir, 0.6f)));
}

void Jefe::ejecutarAtaqueSombra(Protagonista& jugador) {}
void Jefe::ejecutarDisparoInteligente(Protagonista& jugador) {}

// -------------------------------------------------------------------
// --- Lógica de Daño y Getters
// -------------------------------------------------------------------

void Jefe::forzarFaseDos()
{
    if (faseActual == FaseJefe::FASE_DOS || !estaVivo() || enTransicion) {
        return;
    }
    int vidaFaseDos = (int)(vidaMaxima * ConstantesJefe::VIDA_FASE_DOS_TRIGGER);
    vida = vidaFaseDos;
    transicionAFaseDos();
}


void Jefe::recibirDanio(int cantidad, Vector2 posicionJugador) {
    if (!estaVivo() || enTransicion || esInvulnerable) return;
    Vector2 dirHaciaJugador = Vector2Normalize(Vector2Subtract(posicionJugador, posicion));
    float dot = Vector2DotProduct(direccionVista, dirHaciaJugador);
    int danioFinal = 0;
    if (dot > 0.707f) {
        danioFinal = (int)(cantidad * ConstantesJefe::MULT_DANIO_FRENTE);
    } else {
        danioFinal = (int)(cantidad * ConstantesJefe::MULT_DANIO_ESPALDA);
    }
    vida -= danioFinal;
    if (vida < 0) vida = 0;
}

std::vector<Bala*>& Jefe::getBalasGeneradas() {
    return balasGeneradas;
}
void Jefe::limpiarBalasGeneradas() {
    balasGeneradas.clear();
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
        largoCarneActual = LARGO_BRAZO_MAXIMO * extensionBrazo;
    } else {
        largoCarneActual = LARGO_BRAZO_MAXIMO * EXTENSION_CARNE_RELATIVA;
        float extensionRestante = extensionBrazo - EXTENSION_CARNE_RELATIVA;
        // --- ¡¡FIX HUESO GIGANTE!! ---
        largoHuesoActual = LARGO_BRAZO_MAXIMO * extensionRestante;
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
bool Jefe::estaVivo() const { return vida > 0; }
Rectangle Jefe::getRect() const { float r = (faseActual == FaseJefe::FASE_UNO) ? ConstantesJefe::RADIO_JEFE : ConstantesJefe::RADIO_JEFE * 0.8f; return { posicion.x - r, posicion.y - r, r * 2, r * 2 }; }
Vector2 Jefe::getPosicion() const { return posicion; }
int Jefe::getDanioContacto() const { return danioContacto; }
FaseJefe Jefe::getFase() const { return faseActual; }
bool Jefe::esInvulnerableActualmente() const { return enTransicion || esInvulnerable; }
bool Jefe::estaMuerto() const { return !estaVivo(); }
bool Jefe::estaConsumido() const { return false; }
EstadoFaseUno Jefe::getEstadoF1() const { return estadoF1; }
void Jefe::setEstadoF1(EstadoFaseUno nuevoEstado) { this->estadoF1 = nuevoEstado; }
void Jefe::setTemporizadorEstado(float tiempo) { this->temporizadorEstado = tiempo; }


void Jefe::dibujar() {

    float radioActual = (faseActual == FaseJefe::FASE_UNO) ? ConstantesJefe::RADIO_JEFE : ConstantesJefe::RADIO_JEFE * 0.8f;
    Color colorJefe = (faseActual == FaseJefe::FASE_UNO) ? MAROON : RED;
    Vector2 posDibujo = posicion;
    float radioDibujo = radioActual;

    if (!estaVivo()) {
        DrawCircleV(posDibujo, radioDibujo, GRAY);
        return;
    }

    if (faseActual == FaseJefe::FASE_UNO)
    {
        if (estadoF1 == EstadoFaseUno::APUNTANDO_EMBESTIDA) {
            float shake = 5.0f;
            posDibujo.x += (float)GetRandomValue(-shake, shake);
            posDibujo.y += (float)GetRandomValue(-shake, shake);
            colorJefe = YELLOW;
        }
        else if (estadoF1 == EstadoFaseUno::SALTANDO) {
            // --- ¡¡FIX SALTO!! ---
            float t = progresoSalto; // 0.0 -> 1.0
            float escalaT;
            float escalaSalto;

            if (t < 0.5f) {
                // Subiendo (t 0.0 -> 0.5)
                escalaT = t / 0.5f; // Mapea 0.0->0.5 a 0.0->1.0
                escalaSalto = Lerp(1.0f, 2.0f, escalaT);
                posDibujo = Vector2Lerp(inicioSalto, objetivoSalto, escalaT); // Mueve de A a B
            } else {
                // Bajando (t 0.5 -> 1.0)
                escalaT = (t - 0.5f) / 0.5f; // Mapea 0.5->1.0 a 0.0->1.0
                escalaSalto = Lerp(2.0f, 1.0f, escalaT);
                posDibujo = objetivoSalto; // Se queda en B
            }

            radioDibujo = radioActual * escalaSalto;
            float radioSombra = radioActual * escalaSalto;
            DrawCircleV(objetivoSalto, radioSombra, Fade(BLACK, 0.5f));
            // -------------------
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
                        largoCarneActual = LARGO_BRAZO_MAXIMO * extensionBrazo;
                    } else {
                        largoCarneActual = LARGO_BRAZO_MAXIMO * EXTENSION_CARNE_RELATIVA;
                        float extensionRestante = extensionBrazo - EXTENSION_CARNE_RELATIVA;
                        // --- ¡¡FIX HUESO GIGANTE!! ---
                        largoHuesoActual = LARGO_BRAZO_MAXIMO * extensionRestante;
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
    }

    if (enTransicion) colorJefe = WHITE;
    if (estadoF1 == EstadoFaseUno::ATURDIDO_EMBESTIDA) colorJefe = SKYBLUE;

    if (esInvulnerable && !enTransicion) {
        DrawCircleV(posDibujo, radioDibujo * 1.2f, Fade(BLACK, 0.3f));
        colorJefe = Fade(VIOLET, 0.5f);
    }

    // Dibujamos el cuerpo principal si no está parpadeando por el aviso del brazo
    if (!(estadoF1 == EstadoFaseUno::ESTIRANDO_BRAZO && estadoBrazo == EstadoEstirarBrazo::CARGANDO_AVISO))
    {
        DrawCircleV(posDibujo, radioDibujo, colorJefe);
    }

    // Dibujamos la cara si no está saltando
    if (estadoF1 != EstadoFaseUno::SALTANDO)
    {
        Vector2 frente = Vector2Add(posDibujo, Vector2Scale(direccionVista, radioDibujo));
        DrawCircleV(frente, radioDibujo / 4, YELLOW);
    }
}
