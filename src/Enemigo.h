#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>

class Protagonista;
class Mapa;

enum class EstadoIA {
    PATRULLANDO,
    PERSIGUIENDO,
    ATACANDO
};

class Enemigo {
protected:
    Vector2 posicion;
    int vida;
    int danio;
    float velocidad;
    float radio;
    Vector2 direccion;

    float rangoVision;
    float anguloVision;
    float rangoEscucha;

    EstadoIA estadoActual;
    Vector2 destinoPatrulla;
    float temporizadorPatrulla;

    float rangoAtaque;
    float rangoDmg;
    float temporizadorAtaque;
    float temporizadorPausaAtaque;

    float temporizadorDanio;

    bool puedeVearAlJugador(Vector2 posJugador);
    bool puedeEscucharAlJugador(Vector2 posJugador);

    virtual void elegirNuevoDestinoPatrulla(const Mapa& mapa);

public:
    static bool batallaJefeIniciada;

    Enemigo(Vector2 pos, int vida, int danio, float vel, float rad,
            float rangoV, float anguloV, float rangoE);

    virtual ~Enemigo() {}

    virtual void actualizarBase();
    virtual void actualizarIA(Vector2 posJugador, const Mapa& mapa) = 0;
    virtual void dibujar() = 0;
    virtual void atacar(Protagonista& jugador) = 0;

    virtual void recibirDanio(int cantidad);
    void setPosicion(Vector2 nuevaPos);
    void setDireccion(Vector2 nuevaDir);

    bool estaListoParaAtacar() const;
    EstadoIA getEstadoIA() const;

    bool estaVivo() const;
    Vector2 getPosicion() const;
    Rectangle getRect() const;
    int getDanio() const;
    int getVida() const;
    float getVelocidad() const;
    Vector2 getDireccion() const;
    float getRadio() const;

    virtual bool estaMuerto() const;
    virtual bool estaConsumido() const;
};
