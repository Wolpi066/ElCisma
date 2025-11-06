#pragma once
#include "raylib.h"
#include <vector>
#include "raymath.h" // <-- ¡AÑADIDO!

class Protagonista;
class Mapa; // <-- ¡AÑADIDO! Forward declaration

// ¡NUEVO! Estados de la IA
enum class EstadoIA {
    PATRULLANDO,
    PERSIGUIENDO,
    ATACANDO // (Lo dejamos listo para el futuro)
};

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

    // --- ¡MODIFICADO! Reemplazamos el bool por la FSM ---
    EstadoIA estadoActual;
    Vector2 destinoPatrulla;
    float temporizadorPatrulla;
    // --------------------------------------------------

    // --- ¡¡NUEVO!! Estado de Ataque ---
    float rangoAtaque;
    float rangoDmg;
    float temporizadorAtaque;
    float temporizadorPausaAtaque;

    // --- ¡¡NUEVO!! Estado de Daño ---
    float temporizadorDanio; // Para el flash rojo
    // --------------------------------

    bool puedeVearAlJugador(Vector2 posJugador);
    bool puedeEscucharAlJugador(Vector2 posJugador);

    // ¡NUEVO! Función virtual para patrullar
    virtual void elegirNuevoDestinoPatrulla(const Mapa& mapa);

public:
    // --- CONSTRUCTOR RESTAURADO (8 args) ---
    Enemigo(Vector2 pos, int vida, int danio, float vel, float rad,
            float rangoV, float anguloV, float rangoE);

    virtual ~Enemigo() {}

    // --- MÉTODOS RESTAURADOS ---
    virtual void actualizarBase(); // <-- ¡¡NUEVO!! Para timers
    virtual void actualizarIA(Vector2 posJugador, const Mapa& mapa) = 0;
    virtual void dibujar() = 0;
    virtual void atacar(Protagonista& jugador) = 0;

    virtual void recibirDanio(int cantidad);
    void setPosicion(Vector2 nuevaPos);
    void setDireccion(Vector2 nuevaDir);

    // --- ¡¡NUEVO!! ---
    bool estaListoParaAtacar() const;
    EstadoIA getEstadoIA() const;
    // -----------------

    // Getters
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
