#pragma once
#include "raylib.h"
#include "Constantes.h"
#include <vector>

class Consumible;

class Protagonista {
private:
    Vector2 posicion;
    int vida;
    int municion;
    int bateria;
    bool tieneArmadura;

    // Lógica de Vista
    Vector2 direccionVista;
    float anguloVista;
    float anguloCono;
    float alcanceLinterna;

    // Lógica de Juego
    float temporizadorDisparo;
    float tiempoInmune;
    float temporizadorBateria;
    bool tieneLlave;
    float radio;

    bool linternaEncendida;
    bool luzApagadaPorFlicker;
    float timerFlicker;

    Vector2 knockbackVelocidad;
    float knockbackTimer;

    bool proximoDisparoEsCheat;
    bool bateriaCongelada;

    // --- VISUALES ---
    Texture2D texCaminando;
    Texture2D texDisparando;
    Texture2D texMuerto;

    // --- AUDIO ---
    Sound fxCaminando;
    Sound fxDisparo;
    Sound fxGolpe;
    Sound fxLinterna;
    Sound fxPocaVida;
    Sound fxRecargando;
    Sound fxSinBalas; // <--- NUEVO

    float timerPasos;

    float timerVisualDisparo;
    float timerAnimacionMuerte;

    void CargarRecursos();
    void DescargarRecursos();

public:
    Protagonista(Vector2 pos);
    ~Protagonista();

    void actualizarInterno(Camera2D camera, Vector2 direccionMovimiento);

    int intentarDisparar(bool quiereDisparar);
    void setPosicion(Vector2 nuevaPos);
    void reset();

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

    // --- Getters ---
    bool estaVivo() const;
    bool haFinalizadoAnimacionMuerte() const;

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
