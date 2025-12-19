#pragma once
#include "Enemigo.h"
#include <vector>
#include <string>

class Zombie : public Enemigo
{
private:
    static std::vector<Texture2D> animCaminando;
    static std::vector<Texture2D> animAtaque;
    static std::vector<Texture2D> animMuerte;
    static bool texturasCargadas;

    static Sound fxRugido;
    static bool recursosSonidoCargados;

    int frameActual;
    float tiempoAnimacion;

    bool estaMuriendo;
    bool animacionMuerteTerminada;
    float temporizadorCadaver;

    bool haDaniadoEnEsteAtaque;

    bool haRugidoInicial;

    std::vector<Texture2D>* animacionActual;

public:
    Zombie(Vector2 pos);
    virtual ~Zombie();

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
