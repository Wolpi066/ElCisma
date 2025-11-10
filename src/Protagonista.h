#pragma once
#include "raylib.h"
#include "Constantes.h"

class Consumible;

class Protagonista {
private:
    Vector2 posicion;
    int vida;
    int municion;
    int bateria;
    bool tieneArmadura;
    Vector2 direccionVista;
    float anguloVista;
    float temporizadorDisparo;
    float tiempoInmune;
    float temporizadorBateria;
    bool tieneLlave;
    float anguloCono;
    float alcanceLinterna;
    float radio;

    bool linternaEncendida;
    float temporizadorFlicker;

    Vector2 knockbackVelocidad; // (Velocidad por SEGUNDO)
    float knockbackTimer;

    bool proximoDisparoEsCheat;
    bool bateriaCongelada;

public:
    Protagonista(Vector2 pos);

    void actualizarInterno(Camera2D camera);
    int intentarDisparar(bool quiereDisparar);
    void setPosicion(Vector2 nuevaPos);

    void dibujar();
    void recibirDanio(int cantidad);
    void matar();

    void aplicarKnockback(Vector2 direccion, float fuerza, float duracion);

    void recargarBateria(const int& cantidad);
    void curarVida(const int& cantidad);
    void recargarMunicion(const int& cantidad);
    void recibirArmadura();
    void recibirLlave();
    void quitarLlave();

    void activarCheatDisparo();
    void setBateriaCongelada(bool congelada);

    bool estaVivo() const;
    Vector2 getPosicion() const;
    Vector2 getDireccionVista() const;
    Rectangle getRect() const;
    int getVida() const;
    int getMunicion() const;
    int getBateria() const;
    bool getTieneLlave() const;
    float getRadio() const;
    float getAnguloVista() const;
    float getTiempoInmune() const;
    float getKnockbackTimer() const;
    Vector2 getVelocidadKnockback() const;
    float getAnguloCono() const;
    float getAlcanceLinterna() const;
};
