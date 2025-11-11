#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include "Protagonista.h"

// --- TAREA 2: Usar la bala del BH ---
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

    // --- TAREA 2: Constantes para el NUEVO Bullet Hell (Rediseño 60s - FÁCIL) ---

    // Fase 1 (Pulsos)
    const int BH_PULSO_COOLDOWN = 90; // (frames) 1.5s
    const int BH_PULSO_CANTIDAD_BALAS = 10;

    // Fase 2 (Ráfagas)
    const int BH_RAFAGA_COOLDOWN = 50; // (frames) ~0.8s
    const int BH_RAFAGA_CANTIDAD_BALAS = 3; // (Menos balas)
    const float BH_RAFAGA_SEPARACION = 0.20f; // (Más separadas)

    // Fase 3 (Lluvia de Olas - ¡FIXED!)
    const int BH_OLA_COOLDOWN = 45; // (frames) 0.75s (¡Más lento/espaciado!)
    const float BH_OLA_AMPLITUD = 220.0f; // (frames) (¡Más ancho!)
    const int BH_OLA_CANTIDAD_BALAS = 8;
    const float BH_OLA_HUECO = 110.0f; // (Hueco más grande)
    const float BH_OLA_Y_ORIGEN_OFFSET = -190.0f;

    // Fase 4 (Espiral)
    const int BH_ESPIRAL_COOLDOWN = 10; // (Más lento)
    const int BH_ESPIRAL_FILAS = 4; // (Menos filas)
    const float BH_ESPIRAL_ROTACION = 4.0f; // (Más lento)

    // Fase 5 (Doble Pulso)
    const int BH_PULSO_DOBLE_COOLDOWN = 70; // (Más lento)
    const int BH_PULSO_DOBLE_DELAY = 12; // (frames)
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
