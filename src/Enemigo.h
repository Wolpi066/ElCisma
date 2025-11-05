#pragma once
#include "raylib.h"
#include <vector>

class Protagonista;

class Enemigo {
protected:
    // --- MIEMBROS REORDENADOS ---
    Vector2 posicion;
    int vida;
    int danio;
    float velocidad;
    float radio;
    Vector2 direccion;
    // ---------------------------

    float rangoVision;
    float anguloVision;
    float rangoEscucha;
    bool haDetectadoAlJugador;

    bool puedeVearAlJugador(Vector2 posJugador);
    bool puedeEscucharAlJugador(Vector2 posJugador);

public:
    // --- CONSTRUCTOR RESTAURADO (8 args) ---
    Enemigo(Vector2 pos, int vida, int danio, float vel, float rad,
            float rangoV, float anguloV, float rangoE);

    virtual ~Enemigo() {}

    // --- MÉTODOS RESTAURADOS ---
    virtual void actualizarIA(Vector2 posJugador) = 0;
    virtual void dibujar() = 0;
    virtual void atacar(Protagonista& jugador) = 0;

    // --- CORREGIDO: Nombre y tipo ---
    virtual void recibirDanio(int cantidad);
    void setPosicion(Vector2 nuevaPos);

    // Getters
    bool estaVivo() const;
    Vector2 getPosicion() const;
    Rectangle getRect() const;
    int getDanio() const;
    int getVida() const;
    float getVelocidad() const;
    Vector2 getDireccion() const;

    virtual bool estaMuerto() const;
    virtual bool estaConsumido() const;
};
