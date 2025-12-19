#pragma once
#include "Enemigo.h"
#include "raylib.h"

class Mapa;

class Fantasma : public Enemigo {
private:
    Vector2 movimientoRecursivo(int profundidad);

    // Texturas Estáticas
    static Texture2D texNormalDer;
    static Texture2D texNormalIzq;
    static Texture2D texAtaqueDer;
    static Texture2D texAtaqueIzq;
    static bool texturasCargadas;

public:
    static bool despertado;
    static bool modoFuria;
    static bool modoDialogo;

    static bool jefeEnCombate;

    static bool estaAsustando;
    static float temporizadorSusto;
    static Vector2 posSustoInicio;
    static Vector2 posSustoFin;

    static bool estaDespertando;
    static float temporizadorDespertar;

    Fantasma(Vector2 pos);
    virtual ~Fantasma() {}

    static void CargarTexturas();
    static void DescargarTexturas();

    virtual void actualizarIA(Vector2 posJugador, const Mapa& mapa) override;
    virtual void dibujar() override;
    virtual void atacar(Protagonista& jugador) override;
};
