#pragma once
#include "raylib.h"
#include "Constantes.h"

class Consumible;

class Protagonista {
private:
    // --- MIEMBROS REORDENADOS ---
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
    float anguloCono;       // <-- Valor base (maximo)
    float alcanceLinterna;  // <-- Valor base (maximo)
    float radio;

    // --- Estado de la Linterna ---
    bool linternaEncendida;
    float temporizadorFlicker;

    // --- ¢B¢BNUEVO!! Estado de Knockback ---
    Vector2 knockbackVelocidad;
    float knockbackTimer;
    // ------------------------------------

public:
    Protagonista(Vector2 pos);

    void actualizarInterno(Camera2D camera);
    bool intentarDisparar(bool quiereDisparar);
    void setPosicion(Vector2 nuevaPos);

    void dibujar();
    void recibirDanio(int cantidad);

    // --- ¢B¢BNUEVO!! ---
    void matar(); // Muerte instantanea
    // ------------------

    // --- ¢B¢BNUEVO!! Knockback ---
    void aplicarKnockback(Vector2 direccion, float fuerza, float duracion);
    // -------------------------

    // --- METODOS DE CONSUMIBLES (CORREGIDOS: Usan const int&) ---
    void recargarBateria(const int& cantidad);
    void curarVida(const int& cantidad);
    void recargarMunicion(const int& cantidad);
    void recibirArmadura();
    void recibirLlave();

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
    float getTiempoInmune() const; // <-- ¢B¢BNUEVO GETTER!!
    float getKnockbackTimer() const; // <-- ¢B¢BNUEVO GETTER!!
    Vector2 getVelocidadKnockback() const; // <-- ¢B¢BNUEVO GETTER!!

    // --- GETTERS ACTUALIZADOS (Ahora son dinamicos) ---
    float getAnguloCono() const;
    float getAlcanceLinterna() const;
};
