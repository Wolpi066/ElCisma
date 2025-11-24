#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <string>
#include "Protagonista.h"
#include "BalaInfernal.h"

class Mapa;
class Bala;
class Consumible;

struct DropInfo {
    Vector2 pos;
    int tipo;
};

enum class FaseJefe {
    FASE_UNO, FASE_DOS, MURIENDO, TRANSFORMANDO, BULLET_HELL, DERROTADO
};

enum class EstadoFaseUno {
    PAUSANDO, APUNTANDO_EMBESTIDA, EMBISTIENDO, ATURDIDO_EMBESTIDA,
    SALTANDO, ESTIRANDO_BRAZO, TIRANDO_CARNE
};

enum class EstadoEstirarBrazo { CARGANDO_AVISO, EXTENDIENDO };

enum class EstadoFaseDos {
    PAUSANDO, TELETRANSPORTANDO, PLANTANDO_MINAS, DISPARO_ESCOPETA,
    PULSO_RADIAL, APUNTANDO_EMBESTIDA, EMBISTIENDO, RECUPERANDO_EMBESTIDA,
    ATURDIDO_EMBESTIDA
};

class Jefe {
private:
    Vector2 posicion;
    Vector2 velocidadActual;
    Vector2 direccionVista;

    int vidaMaxima, vida, danioContacto;
    float radioHitbox, velocidadLenta;

    FaseJefe faseActual;
    bool enTransicion;
    bool esInvulnerable;

    // --- AUDIO ---
    Sound fxApareciendo;
    Sound fxBulletHell;
    float timerAudioLoopBH;
    Sound fxCaida;
    Sound fxCaminando;
    Sound fxChoque;
    Sound fxEmbestida;
    Sound fxEstirarBrazo;
    Sound fxSalto;
    Sound fxEscopetazo;
    Sound fxExplosion; // Usado para minas o impacto fuerte
    Sound fxMinaActiva;
    Sound fxTeleport;

    // Control de audio pasos
    float timerPasos;
    // -------------

    // --- VISUALES ---
    bool introFinalizada;
    float timerAnimacion;
    int frameActual;
    float anguloBrazoVisual;
    int pasoIntro;
    float timerIntroSequence;

    // Texturas
    std::vector<Texture2D> texF1Apareciendo;
    std::vector<Texture2D> texF1Caminando;
    std::vector<Texture2D> texF1Atacando;
    std::vector<Texture2D> texF1Embestida;
    std::vector<Texture2D> texF1Brazo;
    std::vector<Texture2D> texF1Salto;
    std::vector<Texture2D> texTransformacion;
    std::vector<Texture2D> texF2Caminando;
    std::vector<Texture2D> texF2Disparando;
    std::vector<Texture2D> texF2Embestida;
    std::vector<Texture2D> texF2Teleport;
    std::vector<Texture2D> texF2Enloqueciendo;
    Texture2D texMuerto;

    // Estados y Timers
    EstadoFaseUno estadoF1;
    EstadoFaseDos estadoF2;
    float temporizadorEstado;
    Vector2 objetivoEmbestida;
    float temporizadorEmbestida;
    Vector2 objetivoSalto;
    Vector2 inicioSalto;
    float progresoSalto;
    Vector2 objetivoBrazo;
    float extensionBrazo;
    EstadoEstirarBrazo estadoBrazo;
    bool haSoltadoLootStun;
    std::vector<DropInfo> dropsGenerados;
    float progresoMuerte;
    float bulletHellBaseDirection;
    int bulletHellSpawnTimer;
    float bulletHellAngleEspirales;
    std::vector<Bala*> balasGeneradas;

    // Constantes
    const int BH_PULSO_COOLDOWN = 90;
    const int BH_PULSO_CANTIDAD_BALAS = 10;
    const int BH_RAFAGA_COOLDOWN = 50;
    const int BH_RAFAGA_CANTIDAD_BALAS = 3;
    const float BH_RAFAGA_SEPARACION = 0.20f;
    const int BH_OLA_COOLDOWN = 45;
    const float BH_OLA_AMPLITUD = 220.0f;
    const int BH_OLA_CANTIDAD_BALAS = 8;
    const float BH_OLA_HUECO = 110.0f;
    const float BH_OLA_Y_ORIGEN_OFFSET = -190.0f;
    const int BH_ESPIRAL_COOLDOWN = 10;
    const int BH_ESPIRAL_FILAS = 4;
    const float BH_ESPIRAL_ROTACION = 4.0f;
    const int BH_PULSO_DOBLE_COOLDOWN = 70;
    const int BH_PULSO_DOBLE_DELAY = 12;

    // Métodos Internos
    void CargarTexturas();
    void DescargarTexturas();
    void CargarSonidos();     // <--- NUEVO
    void DescargarSonidos();  // <--- NUEVO
    bool verificarColisionMuros(Vector2 pos, const Mapa& mapa);

    void actualizarFaseUno(Protagonista& jugador, const Mapa& mapa);
    void actualizarFaseDos(Protagonista& jugador, const Mapa& mapa);
    void actualizarFaseMuerte(Protagonista& jugador, const Mapa& mapa);
    void transicionAFaseDos();

    void ejecutarPausaF1(Protagonista& jugador);
    void ejecutarDisparoEscopeta();
    void ejecutarPulsoRadial();
    void ejecutarSalto(Protagonista& jugador);
    void ejecutarEstirarBrazo(Protagonista& jugador);
    void ejecutarTirarCarne(Protagonista& jugador);

    void ejecutarPausaF2(Protagonista& jugador);
    void ejecutarTeletransporte();
    void ejecutarPlantandoMinas();
    void ejecutarAtaqueSombra(Protagonista& jugador);
    void ejecutarDisparoInteligente(Protagonista& jugador);

public:
    Jefe(Vector2 pos);
    virtual ~Jefe();

    void actualizar(Protagonista& jugador, const Mapa& mapa);
    void dibujar();
    void recibirDanio(int cantidad, Vector2 posicionJugador);
    void forzarFaseDos();
    void forzarMuerte();

    std::vector<Bala*>& getBalasGeneradas();
    void limpiarBalasGeneradas();
    std::vector<DropInfo>& getDropsGenerados();
    void limpiarDropsGenerados();
    Rectangle getHitboxBrazo() const;

    void setPosicion(Vector2 nuevaPos);
    void setVelocidad(Vector2 vel);
    bool estaVivo() const;
    Rectangle getRect() const;
    Vector2 getPosicion() const;
    Vector2 getVelocidadActual() const;
    int getDanioContacto() const;
    FaseJefe getFase() const;
    bool esInvulnerableActualmente() const;
    bool estaEnFaseFinal() const;

    EstadoFaseUno getEstadoF1() const;
    void setEstadoF1(EstadoFaseUno nuevoEstado);
    EstadoFaseDos getEstadoF2() const;
    void setEstadoF2(EstadoFaseDos nuevoEstado);
    void setTemporizadorEstado(float tiempo);

    virtual bool estaMuerto() const;
    virtual bool estaConsumido() const;
};
