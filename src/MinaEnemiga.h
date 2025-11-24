#pragma once
#include "Bala.h"

class Protagonista;
class Mapa;

class MinaEnemiga : public Bala {
private:
    float temporizadorActivacion;
    float temporizadorVida;
    bool estaArmada;
    bool estaMuriendo;
    float temporizadorExplosion;
    bool explosionLastimaJefe;
    float velocidadInicial;
    Vector2 direccionNormalizada;

    // Texturas
    static Texture2D texMinaMov; // Mina1.png
    static Texture2D texMinaArmada; // Mina2.png

public:
    MinaEnemiga(Vector2 pos, Vector2 dir);

    void actualizar(Protagonista& jugador, const Mapa& mapa) override;
    void dibujar() override;
    void recibirDanio(int cantidad, OrigenBala origenDanio) override;
    void explotar(bool porJugador);
    bool estaExplotando() const;
    bool explosionPuedeHerirJefe() const;

    static void CargarRecursos();
    static void DescargarRecursos();
};
