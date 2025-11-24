#pragma once
#include "Enemigo.h"
#include <vector>
#include <string>

class MonstruoObeso : public Enemigo
{
private:
    // Animación
    static std::vector<Texture2D> animCaminando;
    static std::vector<Texture2D> animAtaque;
    static std::vector<Texture2D> animMuerte;
    static bool texturasCargadas;

    // Audio
    static Sound fxGrito;
    static bool recursosSonidoCargados;

    // Estado visual
    int frameActual;
    float tiempoAnimacion;

    // Muerte
    bool estaMuriendo;
    bool animacionMuerteTerminada;
    float temporizadorCadaver;

    // Ataque
    bool haDaniadoEnEsteAtaque;

    // Audio Proximidad
    bool haRugidoInicial; // <--- NUEVO

    std::vector<Texture2D>* animacionActual;

public:
    MonstruoObeso(Vector2 pos);
    virtual ~MonstruoObeso();

    static void CargarTexturas();
    static void DescargarTexturas();
    static void CargarSonidos();
    static void DescargarSonidos();

    void actualizarIA(Vector2 posJugador, const Mapa& mapa) override;
    void dibujar() override;
    void atacar(Protagonista& jugador) override;
    void recibirDanio(int cantidad) override;
    bool estaMuerto() const override;
};
