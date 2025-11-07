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

    Vector2 knockbackVelocidad;
    float knockbackTimer;

    // --- ¡¡NUEVOS FLAGS!! ---
    bool proximoDisparoEsCheat;
    bool bateriaCongelada;
    // ------------------------

public:
    Protagonista(Vector2 pos);

    void actualizarInterno(Camera2D camera);

    // --- ¡¡MODIFICADO!! ---
    // Devuelve int: 0=No disparó, 1=Disparo Normal, 2=Disparo Cheat
    int intentarDisparar(bool quiereDisparar);
    // ----------------------

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

    // --- ¡¡NUEVOS SETTERS!! ---
    void activarCheatDisparo();
    void setBateriaCongelada(bool congelada);
    // -------------------------

    // --- Getters ---
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
