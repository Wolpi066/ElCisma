#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include "Protagonista.h"

// --- TAREA 2: Añadida la nueva bala ---
#include "BalaInfernal.h"
// ----------------------------------

class Mapa;
class Bala;
class Consumible;

struct DropInfo {
    Vector2 pos;
    int tipo; // 1:Vida, 3:Municion, 4:Armadura
};

enum class FaseJefe {
    FASE_UNO,
    FASE_DOS,
    MURIENDO,
    TRANSFORMANDO,
    BULLET_HELL,
    DERROTADO
};

enum class EstadoFaseUno {
    PAUSANDO,
    APUNTANDO_EMBESTIDA,
    EMBISTIENDO,
    ATURDIDO_EMBESTIDA,
    SALTANDO,
    ESTIRANDO_BRAZO,
    TIRANDO_CARNE
};

enum class EstadoEstirarBrazo {
    CARGANDO_AVISO,
    EXTENDIENDO
};

enum class EstadoFaseDos {
    PAUSANDO,
    TELETRANSPORTANDO,
    PLANTANDO_MINAS,
    DISPARO_ESCOPETA,
    PULSO_RADIAL,
    APUNTANDO_EMBESTIDA,
    EMBISTIENDO,
    ATURDIDO_EMBESTIDA
};


class Jefe {
private:
    Vector2 posicion;
    Vector2 velocidadActual;
    Vector2 direccionVista;

    int vidaMaxima;
    int vida;
    int danioContacto;

    float radioHitbox;
    float velocidadLenta;

    FaseJefe faseActual;
    bool enTransicion;
    bool esInvulnerable;

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

    float bulletHellBaseDirection; // Usado para el offset de las olas/pulsos
    int bulletHellSpawnTimer; // Contador de frames (INT)
    float bulletHellAngleEspirales; // Ángulo separado para las espirales

    std::vector<Bala*> balasGeneradas;

    // --- TAREA 2: Constantes para el NUEVO Bullet Hell (Rediseño 100s) ---
    // (Ajustadas para 5 fases)

    // Fase 1 (Doble Pulso)
    const int BH_PULSO_COOLDOWN = 50; // (frames) Un pulso cada 0.83s
    const int BH_PULSO_DELAY = 10; // (frames) El segundo pulso
    const int BH_PULSO_CANTIDAD_BALAS = 12;

    // Fase 2 (Ráfagas Giratorias)
    const int BH_RAFAGA_COOLDOWN = 30; // (frames) Una ráfaga cada 0.5s
    const int BH_RAFAGA_CANTIDAD_BALAS = 4;
    const float BH_RAFAGA_SEPARACION = 0.15f; // (radianes)

    // Fase 3 (Lluvia de Olas - ¡FIXED!)
    const int BH_OLA_COOLDOWN = 30; // (frames) Una nueva línea de ola cada 0.5s
    const float BH_OLA_AMPLITUD = 190.0f; // Ancho de la ola (ej: -190 a 190) (¡Arreglado!)
    const int BH_OLA_CANTIDAD_BALAS = 8; // Balas por ola
    const float BH_OLA_HUECO = 80.0f; // Tamaño del hueco seguro
    const float BH_OLA_Y_ORIGEN_OFFSET = -190.0f; // Offset Y (¡Arreglado!)

    // Fase 5 (Espiral - Más fácil)
    const int BH_ESPIRAL_COOLDOWN = 8; // (frames) Más lento
    const int BH_ESPIRAL_FILAS = 4; // (4 en lugar de 5, más fácil)
    const float BH_ESPIRAL_ROTACION = 6.0f; // Grados
    // -----------------------------------------------------------------

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
    virtual ~Jefe() {}

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
