#include "Jefe.h"
#include "Protagonista.h"
#include "Mapa.h"
#include "Constantes.h"
#include <cstdlib> // Para GetRandomValue

static const float VELOCIDAD_EMBESTIDA_JEFE_REDUCIDA = 450.0f;

Jefe::Jefe(Vector2 pos) :
    posicion(pos),
    velocidadActual({0, 0}),
    direccionVista({0, 1}),
    vidaMaxima(ConstantesJefe::VIDA_JEFE),
    vida(ConstantesJefe::VIDA_JEFE),
    danioContacto(20), // Hardcodeado a 20 (GDD)
    radioHitbox(ConstantesJefe::RADIO_JEFE),
    faseActual(FaseJefe::FASE_UNO),
    enTransicion(false),
    esInvulnerable(false),
    estadoF1(EstadoFaseUno::PAUSANDO),
    estadoF2(EstadoFaseDos::PAUSANDO),
    temporizadorEstado(2.5f),
    objetivoEmbestida({0, 0}),
    temporizadorEmbestida(0.0f),
    temporizadorAtaqueSombra(0.0f),
    objetivoAtaqueSombra({0,0})
{
    this->vidaMaxima = ConstantesJefe::VIDA_JEFE;
    this->vida = this->vidaMaxima;

    disparosSolicitados.clear();
}

void Jefe::actualizar(Protagonista& jugador, const Mapa& mapa) {

    if (!estaVivo()) {
        velocidadActual = {0,0};
        return;
    }

    if (estadoF1 != EstadoFaseUno::EMBISTIENDO &&
        estadoF2 != EstadoFaseDos::ATAQUE_SOMBRA_MOVIENDO)
    {
        velocidadActual = {0, 0};
    }

    limpiarDisparosSolicitados();

    esInvulnerable = false;

    temporizadorEstado -= GetFrameTime();

    // La transición SÓLO ocurre si la vida está baja, PERO MÁS DE 0.
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

    // Fix Bloqueo de Mira
    if (estadoF1 != EstadoFaseUno::ATURDIDO_EMBESTIDA &&
        estadoF1 != EstadoFaseUno::EMBISTIENDO)
    {
        if (Vector2DistanceSqr(jugador.getPosicion(), posicion) > 1.0f) {
            direccionVista = Vector2Normalize(Vector2Subtract(jugador.getPosicion(), posicion));
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
                ejecutarPausaF1();
            }
            break;

        case EstadoFaseUno::APUNTANDO_EMBESTIDA:
            if (temporizadorEstado > 0.0f) {
                 objetivoEmbestida = direccionVista;
            }
            if (temporizadorEstado <= 0) {
                estadoF1 = EstadoFaseUno::EMBISTIENDO;
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

        case EstadoFaseUno::DISPARO_ESCOPETA:
            if (temporizadorEstado <= 0) {
                ejecutarDisparoEscopeta();
                estadoF1 = EstadoFaseUno::PAUSANDO;
                temporizadorEstado = ConstantesJefe::COOLDOWN_ESCOPETA;
            }
            break;

        case EstadoFaseUno::PULSO_RADIAL:
             if (temporizadorEstado <= 0) {
                ejecutarPulsoRadial();
                estadoF1 = EstadoFaseUno::PAUSANDO;
                temporizadorEstado = ConstantesJefe::COOLDOWN_PULSO;
            }
            break;
    }
}

void Jefe::ejecutarPausaF1() {
    int ataque = GetRandomValue(0, 2);
    if (ataque == 0) {
        estadoF1 = EstadoFaseUno::APUNTANDO_EMBESTIDA;
        temporizadorEstado = ConstantesJefe::TIEMPO_APUNTAR_EMBESTIDA;
    } else if (ataque == 1) {
        estadoF1 = EstadoFaseUno::DISPARO_ESCOPETA;
        temporizadorEstado = ConstantesJefe::TIEMPO_PREPARAR_ESCOPETA;
    } else {
        estadoF1 = EstadoFaseUno::PULSO_RADIAL;
        temporizadorEstado = ConstantesJefe::TIEMPO_PREPARAR_PULSO;
    }
}

void Jefe::ejecutarDisparoEscopeta() {
    Vector2 dir = direccionVista;
    disparosSolicitados.push_back(dir);
    disparosSolicitados.push_back(Vector2Rotate(dir, -0.2f));
    disparosSolicitados.push_back(Vector2Rotate(dir, 0.2f));
    disparosSolicitados.push_back(Vector2Rotate(dir, -0.4f));
    disparosSolicitados.push_back(Vector2Rotate(dir, 0.4f));
}

void Jefe::ejecutarPulsoRadial() {
    disparosSolicitados.push_back({0, 1});
    disparosSolicitados.push_back({0, -1});
    disparosSolicitados.push_back({1, 0});
    disparosSolicitados.push_back({-1, 0});
    disparosSolicitados.push_back(Vector2Normalize({1, 1}));
    disparosSolicitados.push_back(Vector2Normalize({1, -1}));
    disparosSolicitados.push_back(Vector2Normalize({-1, 1}));
    disparosSolicitados.push_back(Vector2Normalize({-1, -1}));
}


// -------------------------------------------------------------------
// --- FASE 2
// -------------------------------------------------------------------
void Jefe::actualizarFaseDos(Protagonista& jugador, const Mapa& mapa) {
    // (Placeholder - Reutiliza F1 pero más rápido)
    if (estadoF1 == EstadoFaseUno::PAUSANDO && temporizadorEstado <= 0)
    {
         ejecutarPausaF1();
         temporizadorEstado *= 0.5f;
    }
    actualizarFaseUno(jugador, mapa);
}

void Jefe::ejecutarPausaF2() {
    // (PENDIENTE)
}
void Jefe::ejecutarAtaqueSombra(Protagonista& jugador) {
    // (PENDIENTE)
}
void Jefe::ejecutarDisparoInteligente(Protagonista& jugador) {
    // (PENDIENTE)
}

// -------------------------------------------------------------------
// --- Lógica de Daño y Cheat
// -------------------------------------------------------------------

// --- ¡¡NUEVA FUNCIÓN!! ---
void Jefe::forzarFaseDos()
{
    // Si ya está en fase 2, muerto, o en transición, no hace nada
    if (faseActual == FaseJefe::FASE_DOS || !estaVivo() || enTransicion) {
        return;
    }

    // Calcula la vida de la fase 2
    int vidaFaseDos = (int)(vidaMaxima * ConstantesJefe::VIDA_FASE_DOS_TRIGGER); // Ej: 800 * 0.3 = 240

    // Setea la vida al umbral
    vida = vidaFaseDos;

    // ¡Forzamos la transición AHORA!
    transicionAFaseDos();
}
// -------------------------


void Jefe::recibirDanio(int cantidad, Vector2 posicionJugador) {
    if (!estaVivo() || enTransicion || esInvulnerable) return;

    Vector2 dirHaciaJugador = Vector2Normalize(Vector2Subtract(posicionJugador, posicion));
    float dot = Vector2DotProduct(direccionVista, dirHaciaJugador);
    int danioFinal = 0;

    if (dot > 0.707f) { // "Delante"
        danioFinal = (int)(cantidad * ConstantesJefe::MULT_DANIO_FRENTE);
    } else { // "Costados" y "Espalda"
        danioFinal = (int)(cantidad * ConstantesJefe::MULT_DANIO_ESPALDA);
    }

    vida -= danioFinal;
    if (vida < 0) vida = 0;
}

// (Resto de Getters/Setters sin cambios)
// ...
std::vector<Vector2>& Jefe::getDisparosSolicitados() { return disparosSolicitados; }
void Jefe::limpiarDisparosSolicitados() { disparosSolicitados.clear(); }
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
// ...

// --- Dibujo (con fix de muerte) ---
void Jefe::dibujar() {

    float radioActual = (faseActual == FaseJefe::FASE_UNO) ? ConstantesJefe::RADIO_JEFE : ConstantesJefe::RADIO_JEFE * 0.8f;
    Color colorJefe = (faseActual == FaseJefe::FASE_UNO) ? MAROON : RED;

    Vector2 posDibujo = posicion;

    if (estadoF1 == EstadoFaseUno::APUNTANDO_EMBESTIDA) {
        float shake = 5.0f;
        posDibujo.x += (float)GetRandomValue(-shake, shake);
        posDibujo.y += (float)GetRandomValue(-shake, shake);
        colorJefe = YELLOW;
    }

    if (enTransicion) colorJefe = WHITE;
    if (estadoF1 == EstadoFaseUno::ATURDIDO_EMBESTIDA) colorJefe = SKYBLUE;

    if (esInvulnerable && !enTransicion) {
        DrawCircleV(posDibujo, radioActual * 1.2f, Fade(BLACK, 0.3f));
        colorJefe = Fade(VIOLET, 0.5f);
    }

    // Fix de desaparición:
    if (!estaVivo()) {
        colorJefe = GRAY;
        DrawCircleV(posDibujo, radioActual, colorJefe);
        return; // No dibujamos la cara amarilla si está muerto
    }

    DrawCircleV(posDibujo, radioActual, colorJefe);

    Vector2 frente = Vector2Add(posDibujo, Vector2Scale(direccionVista, radioActual));
    DrawCircleV(frente, radioActual / 4, YELLOW);
}
