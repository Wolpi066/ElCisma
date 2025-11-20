#pragma once
#include "Enemigo.h"
#include <vector>
#include <string>

class Zombie : public Enemigo
{
private:
    // --- SISTEMA DE ANIMACIÓN ---
    static std::vector<Texture2D> animIdle;
    static std::vector<Texture2D> animCaminando;
    static std::vector<Texture2D> animAtaque;
    static std::vector<Texture2D> animMuerte;
    static bool texturasCargadas;

    // Estado visual individual
    int frameActual;
    float tiempoAnimacion;

    // Control de Muerte
    bool estaMuriendo;
    bool animacionMuerteTerminada;

    // Puntero a la animación activa
    std::vector<Texture2D>* animacionActual;

public:
    Zombie(Vector2 pos);
    virtual ~Zombie();

    static void CargarTexturas();
    static void DescargarTexturas();

    // Overrides
    void actualizarIA(Vector2 posJugador, const Mapa& mapa) override;
    void dibujar() override;
    void atacar(Protagonista& jugador) override;
    void recibirDanio(int cantidad) override;

    // CRÍTICO: Esto evita que el Gestor lo borre antes de tiempo
    bool estaMuerto() const override;
};
