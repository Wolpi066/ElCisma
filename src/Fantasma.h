#pragma once
#include "Enemigo.h"
#include "raylib.h" // Necesario para Texture2D

class Mapa;

class Fantasma : public Enemigo {
private:
    Vector2 movimientoRecursivo(int profundidad);

    // --- Texturas Estáticas (Para cargar solo una vez) ---
    static Texture2D texNormalDer;
    static Texture2D texNormalIzq;
    static Texture2D texAtaqueDer;
    static Texture2D texAtaqueIzq;
    static bool texturasCargadas;
    // ----------------------------------------------------

public:
    // Variables de estado del Fantasma
    static bool despertado;
    static bool modoFuria;   // Para aura roja
    static bool modoDialogo; // Para no atacar

    // Logica del Fantasma
    static bool jefeEnCombate;

    // --- Estado de "Susto" ---
    static bool estaAsustando;
    static float temporizadorSusto;
    static Vector2 posSustoInicio;
    static Vector2 posSustoFin;

    // --- Estado "Despertando" ---
    static bool estaDespertando;
    static float temporizadorDespertar;

    Fantasma(Vector2 pos);
    // Destructor para asegurar limpieza si fuera necesario (aunque usamos estaticas)
    virtual ~Fantasma() {}

    // --- Métodos Estáticos de Gestión de Assets ---
    static void CargarTexturas();
    static void DescargarTexturas();
    // ---------------------------------------------

    virtual void actualizarIA(Vector2 posJugador, const Mapa& mapa) override;
    virtual void dibujar() override;
    virtual void atacar(Protagonista& jugador) override;
};
