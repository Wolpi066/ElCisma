#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include "Protagonista.h"

// FWD Declarations
class Mapa;
class Bala;

enum class FaseJefe {
    FASE_UNO,
    FASE_DOS
};

enum class EstadoFaseUno {
    PAUSANDO,
    APUNTANDO_EMBESTIDA,
    EMBISTIENDO,
    ATURDIDO_EMBESTIDA,
    SALTANDO,
    ESTIRANDO_BRAZO
};

enum class EstadoEstirarBrazo {
    CARGANDO_AVISO,
    EXTENDIENDO_CARNE,
    EXTENDIENDO_HUESO
};

enum class EstadoFaseDos {
    PAUSANDO,
    TELETRANSPORTANDO,
    PLANTANDO_MINAS,
    DISPARO_ESCOPETA,
    PULSO_RADIAL
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

    // --- ¡¡FIX SALTO!! ---
    Vector2 objetivoSalto;
    Vector2 inicioSalto; // <-- ¡NUEVO!
    float progresoSalto; // <-- (Reemplaza escalaSalto)
    // -------------------

    Vector2 objetivoBrazo;
    float extensionBrazo;

    EstadoEstirarBrazo estadoBrazo;

    std::vector<Bala*> balasGeneradas;

    void actualizarFaseUno(Protagonista& jugador, const Mapa& mapa);
    void actualizarFaseDos(Protagonista& jugador, const Mapa& mapa);
    void transicionAFaseDos();

    void ejecutarPausaF1(Protagonista& jugador);
    void ejecutarDisparoEscopeta();
    void ejecutarPulsoRadial();
    void ejecutarSalto(Protagonista& jugador);
    void ejecutarEstirarBrazo(Protagonista& jugador);

    void ejecutarPausaF2();
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

    std::vector<Bala*>& getBalasGeneradas();
    void limpiarBalasGeneradas();

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

    EstadoFaseUno getEstadoF1() const;
    void setEstadoF1(EstadoFaseUno nuevoEstado);
    void setTemporizadorEstado(float tiempo);

    virtual bool estaMuerto() const;
    virtual bool estaConsumido() const;
};
