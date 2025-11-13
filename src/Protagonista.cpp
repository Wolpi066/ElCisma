#include "Protagonista.h"
#include "raymath.h"
#include "Constantes.h"
#include <vector> // <-- ¡AÑADIDO!

static const float ALCANCE_LINTERNA_MIN = 80.0f;
static const float ANGULO_CONO_MIN = 0.1f;


Protagonista::Protagonista(Vector2 pos) :
    posicion(pos),
    vida(Constantes::VIDA_MAX_JUGADOR),
    municion(Constantes::MUNICION_MAX),
    bateria(Constantes::BATERIA_MAX),
    tieneArmadura(false),
    direccionVista({ 0.0f, 1.0f }),
    anguloVista(0.0f),
    temporizadorDisparo(0.0f),
    tiempoInmune(0.0f),
    temporizadorBateria(0.0f),
    tieneLlave(false),
    anguloCono(Constantes::ANCHO_CONO_LINTERNA * 0.8f),
    alcanceLinterna(Constantes::ALCANCE_LINTERNA * 0.8f),
    radio(16.0f),
    linternaEncendida(true),
    temporizadorFlicker(0.0f),
    knockbackVelocidad({0.0f, 0.0f}),
    knockbackTimer(0.0f),
    proximoDisparoEsCheat(false),
    bateriaCongelada(false),
    // --- ¡NUEVO! Inicializar Animación ---
    animFrameCounter(0),
    animCurrentFrame(0),
    animFramesSpeed(8), // 8 FPS, como en tu ejemplo
    estaMoviendo(false)
{
    // --- ¡NUEVO! Cargar tus 5 texturas (CON NOMBRES CORREGIDOS) ---
    texWalkSouth.push_back(LoadTexture("caminando1.png")); // Frame 0 (Idle)
    texWalkSouth.push_back(LoadTexture("caminando2.png")); // Frame 1
    texWalkSouth.push_back(LoadTexture("caminando3.png")); // Frame 2
    texWalkSouth.push_back(LoadTexture("caminando4.png")); // Frame 3
    texWalkSouth.push_back(LoadTexture("caminando5.png")); // Frame 4
}

// --- ¡NUEVO! Destructor ---
Protagonista::~Protagonista()
{
    // Descarga las 5 texturas que cargamos
    for (Texture2D tex : texWalkSouth)
    {
        UnloadTexture(tex);
    }
}

// --- ¡MODIFICADO! ---
void Protagonista::actualizarInterno(Camera2D camera, Vector2 direccionMovimiento)
{
    if (temporizadorDisparo > 0) {
        temporizadorDisparo -= GetFrameTime();
    }
    if (tiempoInmune > 0) {
        tiempoInmune -= GetFrameTime();
    }
    if (knockbackTimer > 0) {
        knockbackTimer -= GetFrameTime();
    }

    Vector2 posMouse = GetScreenToWorld2D(GetMousePosition(), camera);
    Vector2 dirDeseada = Vector2Normalize(Vector2Subtract(posMouse, posicion));
    direccionVista = Vector2Lerp(direccionVista, dirDeseada, Constantes::VELOCIDAD_LINTERNA * GetFrameTime());
    direccionVista = Vector2Normalize(direccionVista);
    anguloVista = atan2f(direccionVista.y, direccionVista.x) * RAD2DEG;

    if (!bateriaCongelada && IsKeyPressed(KEY_F)) {
        linternaEncendida = !linternaEncendida;
    }

    if (!bateriaCongelada && linternaEncendida && bateria > 0) {
        temporizadorBateria += GetFrameTime();
        if (temporizadorBateria >= (1.0f / Constantes::BATERIA_CONSUMO_SEGUNDO))
        {
            bateria--;
            temporizadorBateria = 0.0f;
        }
    }

    if (bateria <= 0) {
        linternaEncendida = false;
    }

    if (linternaEncendida && bateria < Constantes::BATERIA_FLICKER_THRESHOLD)
    {
        temporizadorFlicker -= GetFrameTime();
        if (temporizadorFlicker < 0.0f) {
            float ratio = (bateria / Constantes::BATERIA_FLICKER_THRESHOLD);
            temporizadorFlicker = (float)GetRandomValue(0, 100) / 100.0f * (0.1f + ratio * 0.4f);
        }
    }

    // --- ¡NUEVO! Lógica de Animación ---

    // 1. Comprobar si el jugador se está moviendo
    estaMoviendo = (Vector2LengthSqr(direccionMovimiento) > 0.0f);

    // 2. Comprobar si se mueve hacia abajo (Top-Down)
    bool caminaSur = (estaMoviendo && direccionMovimiento.y > 0.5f);

    if (caminaSur)
    {
        // 3. Actualizar la lógica de frames (copiado de tu ejemplo)
        animFrameCounter++;
        if (animFrameCounter >= (60 / animFramesSpeed))
        {
            animFrameCounter = 0;
            animCurrentFrame++;

            // 4. Bucle de 5 frames (0-4)
            if (animCurrentFrame > 4)
            {
                animCurrentFrame = 1; // Volvemos al frame 1, no al 0 (Idle)
            }
            // (Si el frame 0 NO es parte del ciclo de caminar, empezamos en 1)
            if (animCurrentFrame == 0) animCurrentFrame = 1;
        }
    }
    else
    {
        // 5. Si no se mueve (o se mueve a otro lado), vuelve al frame 0 (Idle)
        animCurrentFrame = 0;
    }
    // --- Fin de Animación ---
}


int Protagonista::intentarDisparar(bool quiereDisparar)
{
    if (quiereDisparar && temporizadorDisparo <= 0 && municion > 0) {
        municion--;
        temporizadorDisparo = Constantes::TIEMPO_RECARGA_DISPARO;
        if (proximoDisparoEsCheat) {
            proximoDisparoEsCheat = false;
            return 2; // Cheat
        }
        return 1; // Normal
    }
    return 0; // No disparó
}

void Protagonista::setPosicion(Vector2 nuevaPos) {
    this->posicion = nuevaPos;
}


void Protagonista::dibujar()
{
    if (!estaVivo()) return;

    // --- ¡MODIFICADO! Dibujar Sprite ---

    // 1. Obtener la textura actual
    Texture2D texActual;

    // (Asumimos que 'caminando1.png' (índice 0) es el IDLE)
    // (Asumimos que 'caminando2-5.png' (índices 1-4) son el ciclo)
    if (estaMoviendo && direccionVista.y > 0.5f) // (Simple check para "walking south")
    {
        texActual = texWalkSouth[animCurrentFrame]; // Usa el frame 1, 2, 3 o 4
    }
    else
    {
        texActual = texWalkSouth[0]; // Frame 0 (Idle)
    }

    // 2. Definir rectángulos de dibujado
    float frameWidth = (float)texActual.width;
    float frameHeight = (float)texActual.height;

    Rectangle sourceRec = { 0.0f, 0.0f, frameWidth, frameHeight };
    Rectangle destRec = { posicion.x, posicion.y, frameWidth, frameHeight };
    Vector2 origen = { frameWidth / 2.0f, frameHeight / 2.0f }; // Centrar el sprite

    // 3. Definir tinte (para efecto de daño)
    Color tinte = WHITE;
    if (tiempoInmune > 0) {
        if ((int)(tiempoInmune * 10) % 2 == 0) {
            tinte = RED; // Efecto de parpadeo rojo
        }
    }

    // 4. Dibujar la textura
    DrawTexturePro(texActual,
                   sourceRec,
                   destRec,
                   origen,
                   0.0f, // No rotamos el sprite, solo la linterna
                   tinte);

    // --- Fin de Modificación ---
}

void Protagonista::recibirDanio(int cantidad)
{
    if (tiempoInmune > 0 || !estaVivo()) return;

    if (tieneArmadura) {
        tieneArmadura = false;
    } else {
        vida -= cantidad;
    }

    if (vida <= 0) {
        vida = 0;
        matar();
    } else {
        tiempoInmune = Constantes::TIEMPO_INMUNIDAD_DANIO;
    }
}

void Protagonista::matar()
{
    vida = 0;
}

void Protagonista::aplicarKnockback(Vector2 direccion, float fuerza, float duracion)
{
    if (!estaVivo()) return;
    // --- ¡¡VACA FIX 4.0!! (Velocidad en px/frame) ---
    knockbackVelocidad = Vector2Scale(direccion, fuerza);
    // ---------------------------------------------
    knockbackTimer = duracion;
}

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

void Protagonista::quitarLlave() {
    tieneLlave = false;
}

void Protagonista::activarCheatDisparo()
{
    proximoDisparoEsCheat = true;
}

void Protagonista::setBateriaCongelada(bool congelada)
{
    bateriaCongelada = congelada;
}

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
    return vida;
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
float Protagonista::getRadio() const {
    return radio;
}
float Protagonista::getAnguloVista() const {
    return anguloVista;
}
float Protagonista::getTiempoInmune() const {
    return tiempoInmune;
}
float Protagonista::getKnockbackTimer() const {
    return knockbackTimer;
}
Vector2 Protagonista::getVelocidadKnockback() const {
    return knockbackVelocidad;
}


float Protagonista::getAnguloCono() const
{
    if (!linternaEncendida) return 0.0f;
    float bateriaNorm = (float)bateria / (float)Constantes::BATERIA_MAX;
    bateriaNorm = Clamp(bateriaNorm, 0.0f, 1.0f);
    float anguloCalculado = ANGULO_CONO_MIN + (anguloCono - ANGULO_CONO_MIN) * bateriaNorm;
    if (bateria < Constantes::BATERIA_FLICKER_THRESHOLD) {
        if (temporizadorFlicker < 0.05f) {
            return anguloCalculado * 0.7f;
        }
    }
    return anguloCalculado;
}

float Protagonista::getAlcanceLinterna() const
{
    if (!linternaEncendida) return 0.0f;
    float bateriaNorm = (float)bateria / (float)Constantes::BATERIA_MAX;
    bateriaNorm = Clamp(bateriaNorm, 0.0f, 1.0f);
    float alcanceCalculado = ALCANCE_LINTERNA_MIN + (alcanceLinterna - ALCANCE_LINTERNA_MIN) * bateriaNorm;
    if (bateria < Constantes::BATERIA_FLICKER_THRESHOLD) {
        if (temporizadorFlicker < 0.05f) {
            return alcanceCalculado * 0.8f;
        }
    }
    return alcanceCalculado;
}
