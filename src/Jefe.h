#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include "Protagonista.h"

// FWD Declarations
class Mapa;

enum class FaseJefe {
    FASE_UNO,
    FASE_DOS
};

enum class EstadoFaseUno {
    PAUSANDO,
    APUNTANDO_EMBESTIDA,
    EMBISTIENDO,
    ATURDIDO_EMBESTIDA,
    DISPARO_ESCOPETA,
    PULSO_RADIAL
};

enum class EstadoFaseDos {
    PAUSANDO,
    APUNTANDO_SOMBRA,
    ATAQUE_SOMBRA_MOVIENDO,
    DISPARO_INTELIGENTE,
    LLUVIA_DE_BALAS
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

    FaseJefe faseActual;
    bool enTransicion;
    bool esInvulnerable;

    EstadoFaseUno estadoF1;
    EstadoFaseDos estadoF2;
    float temporizadorEstado;

    Vector2 objetivoEmbestida;
    float temporizadorEmbestida;

    float temporizadorAtaqueSombra;
    Vector2 objetivoAtaqueSombra;

    std::vector<Vector2> disparosSolicitados;

    void actualizarFaseUno(Protagonista& jugador, const Mapa& mapa);
    void actualizarFaseDos(Protagonista& jugador, const Mapa& mapa);
    void transicionAFaseDos();

    void ejecutarPausaF1();
    void ejecutarDisparoEscopeta();
    void ejecutarPulsoRadial();

    void ejecutarPausaF2();
    void ejecutarAtaqueSombra(Protagonista& jugador);
    void ejecutarDisparoInteligente(Protagonista& jugador);


public:
    Jefe(Vector2 pos);
    virtual ~Jefe() {}

    void actualizar(Protagonista& jugador, const Mapa& mapa);
    void dibujar();

    void recibirDanio(int cantidad, Vector2 posicionJugador);

    // --- ¡¡NUEVA FUNCIÓN PARA CHEAT!! ---
    void forzarFaseDos();
    // ------------------------------------

    std::vector<Vector2>& getDisparosSolicitados();
    void limpiarDisparosSolicitados();

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
