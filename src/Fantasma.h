#pragma once
#include "Enemigo.h"

class Fantasma : public Enemigo {
private:
    Vector2 movimientoRecursivo(int profundidad);

public:
    // Variables de estado del Fantasma
    static bool despertado;
    static bool modoFuria;   // Para aura roja
    static bool modoDialogo; // Para no atacar

    // --- Estado de "Susto" ---
    static bool estaAsustando;       // True si esta en el evento de susto
    static float temporizadorSusto;  // Cuanto dura el susto
    static Vector2 posSustoInicio;   // Posicion inicial del susto
    static Vector2 posSustoFin;      // Posicion final del susto

    // --- ¡¡NUEVO!!: Estado "Despertando" ---
    static bool estaDespertando;
    static float temporizadorDespertar;
    // -----------------------------------

    // --- CORREGIDO: Llama al constructor de 8 args de Enemigo ---
    Fantasma(Vector2 pos);

    // --- CORREGIDO: Firma de la funcion ---
    virtual void actualizarIA(Vector2 posJugador) override;
    virtual void dibujar() override;
    virtual void atacar(Protagonista& jugador) override;
};
