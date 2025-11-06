#include "Protagonista.h"
#include "raymath.h"
#include "Consumible.h"
#include "Constantes.h"

Protagonista::Protagonista(Vector2 pos) :
    posicion(pos),
    vida(Constantes::VIDA_MAX_JUGADOR),
    municion(Constantes::MUNICION_MAX),
    bateria(Constantes::BATERIA_MAX),
    tieneArmadura(false),
    direccionVista({ 1.0f, 0.0f }), // Inicia mirando a la derecha
    anguloVista(0.0f),
    temporizadorDisparo(0.0f),
    tiempoInmune(0.0f),
    temporizadorBateria(2.0f),
    tieneLlave(false),
    anguloCono(75.0f / 2.0f),       // Valor base (a 100%)
    alcanceLinterna(300.0f),    // Valor base (a 100%)
    radio(15.0f),
    // --- NUEVO: Inicializar linterna ---
    linternaEncendida(true),
    temporizadorFlicker(0.0f),
    // --- 　NUEVO!! Inicializar Knockback ---
    knockbackVelocidad({0, 0}),
    knockbackTimer(0.0f)
    // ---------------------------------
{
}

void Protagonista::actualizarInterno(Camera2D camera) {

    // --- Drenaje de Bateria ---
    if (temporizadorBateria > 0) {
        temporizadorBateria -= GetFrameTime();
    } else {
        if (bateria > 0) bateria--;
        temporizadorBateria = 2.0f; // Drena cada 2 segundos
    }

    // --- Timers ---
    if (tiempoInmune > 0) {
        tiempoInmune -= GetFrameTime();
    }
    if (temporizadorDisparo > 0) {
        temporizadorDisparo -= GetFrameTime();
    }
    // --- 　NUEVO!! Timer de Knockback ---
    if (knockbackTimer > 0) {
        knockbackTimer -= GetFrameTime();
    }
    // -----------------------------------

    // --- 　BLOQUE DE ROTACION MODIFICADO CON DELAY!! ---
    // 1. Obtener la posicion del mouse en el mundo
    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
    // 2. Calcular la direccion "objetivo" (hacia donde apunta el mouse)
    Vector2 dirObjetivo = Vector2Normalize(Vector2Subtract(mousePos, posicion));

    // 3. Interpolar suavemente la direccion de vista actual hacia la objetivo
    // Un factor de suavizado mas bajo (ej. 5.0f) lo hace mas "pesado"
    // Un factor mas alto (ej. 20.0f) lo hace mas rapido y reactivo
    float factorSuavizado = 10.0f;
    direccionVista = Vector2Lerp(direccionVista, dirObjetivo, factorSuavizado * GetFrameTime());

    // 4. Es VITAL re-normalizar el vector resultante del Lerp
    direccionVista = Vector2Normalize(direccionVista);

    // 5. El angulo se calcula en base a la direccion de vista SUAVIZADA
    anguloVista = atan2f(direccionVista.y, direccionVista.x) * RAD2DEG;
    // --- FIN DEL BLOQUE MODIFICADO ---


    // --- NUEVO: Logica de Flicker (Parpadeo) ---
    if (bateria <= 30 && bateria > 0)
    {
        temporizadorFlicker -= GetFrameTime();
        if (temporizadorFlicker <= 0)
        {
            // Parpadea
            linternaEncendida = !linternaEncendida;

            // Si se apago, que dure poco apagada
            if (!linternaEncendida) {
                temporizadorFlicker = (float)GetRandomValue(5, 15) / 100.0f; // 0.05s a 0.15s
            } else {
            // Si se encendio, que dure mas encendida
                temporizadorFlicker = (float)GetRandomValue(20, 100) / 100.0f; // 0.20s a 1.0s
            }
        }
    }
    else if (bateria > 30)
    {
        linternaEncendida = true; // Luz solida
    }
    // (Si la bateria es 0, getAlcanceLinterna se encarga)
    // --------------------------------------------
}

void Protagonista::setPosicion(Vector2 nuevaPos)
{
    this->posicion = nuevaPos;
}

bool Protagonista::intentarDisparar(bool quiereDisparar) {
    if (quiereDisparar && temporizadorDisparo <= 0 && municion > 0) {
        municion--;
        temporizadorDisparo = 0.5f;
        return true;
    }
    return false;
}

void Protagonista::dibujar() {
    Color colorJugador = DARKBLUE;
    if (tiempoInmune > 0) {
        if ( (int)(tiempoInmune * 10) % 2 == 0) {
            colorJugador = BLANK;
        }
    }
    DrawCircleV(posicion, radio, colorJugador);
}

// --- CORREGIDO: Nombre y tipo ---
void Protagonista::recibirDanio(int cantidad) {
    if (tiempoInmune > 0) return;

    if (tieneArmadura) {
        tieneArmadura = false;
    } else {
        vida -= cantidad;
        if (vida < 0) vida = 0;
    }

    tiempoInmune = 1.0f;
}

// --- 　NUEVO!! ---
void Protagonista::matar()
{
    // Muerte instantanea, ignora armadura e inmunidad
    this->vida = 0;
}
// ------------------

// --- 　NUEVA FUNCION!! ---
void Protagonista::aplicarKnockback(Vector2 direccion, float fuerza, float duracion)
{
    // Solo aplica un nuevo knockback si no esta ya en uno
    if (knockbackTimer <= 0.0f)
    {
        this->knockbackVelocidad = Vector2Scale(direccion, fuerza);
        this->knockbackTimer = duracion;
    }
}
// -------------------------

// --- Metodos de Consumibles ---
void Protagonista::recargarBateria(const int& cantidad) {
    bateria += cantidad;
    if (bateria > Constantes::BATERIA_MAX) bateria = Constantes::BATERIA_MAX;
}
void Protagonista::curarVida(const int& cantidad) {
    vida += cantidad;
    if (vida > Constantes::VIDA_MAX_JUGADOR) vida = Constantes::VIDA_MAX_JUGADOR;
}
void Protagonista::recargarMunicion(const int& cantidad) {
    municion += cantidad;
    if (municion > Constantes::MUNICION_MAX) municion = Constantes::MUNICION_MAX;
}
void Protagonista::recibirArmadura() {
    tieneArmadura = true;
}
void Protagonista::recibirLlave() {
    tieneLlave = true;
}

// --- Getters ---
bool Protagonista::estaVivo() const {
    return vida > 0;
}
Vector2 Protagonista::getPosicion() const {
    return posicion;
}
Vector2 Protagonista::getDireccionVista() const {
    return direccionVista;
}
Rectangle Protagonista::getRect() const {
    return { posicion.x - radio, posicion.y - radio, radio * 2, radio * 2 };
}
int Protagonista::getVida() const {
    return (int)vida;
}
int Protagonista::getMunicion() const {
    return municion;
}
int Protagonista::getBateria() const {
    return bateria;
}
bool Protagonista::getTieneLlave() const {
    return tieneLlave;
}
float Protagonista::getAnguloVista() const {
    return anguloVista;
}
float Protagonista::getRadio() const {
    return radio;
}
// --- 　NUEVOS GETTERS!! ---
float Protagonista::getTiempoInmune() const {
    return tiempoInmune;
}
float Protagonista::getKnockbackTimer() const {
    return knockbackTimer;
}
Vector2 Protagonista::getVelocidadKnockback() const {
    return knockbackVelocidad;
}
// ---------------------------


// --- 　GETTERS DINAMICOS ACTUALIZADOS!! ---

float Protagonista::getAnguloCono() const {
    // Si no hay bateria o esta parpadeando apagada, no hay cono
    if (bateria <= 0 || !linternaEncendida) {
        return 0.0f;
    }

    // Calcular el porcentaje de bateria (0.0 a 1.0)
    float bateriaPct = (float)bateria / (float)Constantes::BATERIA_MAX;

    // El angulo minimo sera la mitad del angulo base
    float anguloMin = anguloCono / 2.0f;

    // Interpolar (Lerp) entre el angulo minimo y el maximo
    return Lerp(anguloMin, anguloCono, bateriaPct);
}

float Protagonista::getAlcanceLinterna() const {
    // Si no hay bateria o esta parpadeando apagada, no hay cono
    if (bateria <= 0 || !linternaEncendida) {
        return 0.0f;
    }

    // Calcular el porcentaje de bateria (0.0 a 1.0)
    float bateriaPct = (float)bateria / (float)Constantes::BATERIA_MAX;

    // El alcance minimo sera un tercio del alcance base
    float alcanceMin = alcanceLinterna / 3.0f;

    // Interpolar (Lerp) entre el alcance minimo y el maximo
    return Lerp(alcanceMin, alcanceLinterna, bateriaPct);
}
