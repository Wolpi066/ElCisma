#pragma once
#include "raylib.h"
#include "raymath.h"
#include "Constantes.h"

class Protagonista;

enum class EstadoJefe {
    ESPERANDO,
    MOVIENDO,
    ATACANDO_RAFAGA
};

class Jefe {
private:
    Vector2 posicion;
    int vida;
    int danioContacto;

    EstadoJefe estado;
    float temporizadorEstado;
    int rafagaRestante;

    bool disparoSolicitado;
    Vector2 velocidadActual;

public:
    Jefe(Vector2 pos);
    virtual ~Jefe() {}

    void actualizar(Protagonista& jugador);
    void dibujar();
    void recibirDanio(int cantidad);

    bool quiereDisparar() const;
    void setPosicion(Vector2 nuevaPos);

    bool estaVivo() const;
    Rectangle getRect() const;
    Vector2 getPosicion() const;
    Vector2 getVelocidadActual() const;

    virtual bool estaMuerto() const;
    virtual bool estaConsumido() const;
};
