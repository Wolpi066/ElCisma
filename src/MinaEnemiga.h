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

    // --- ¡¡VACA FIX 4.0!! ---
    float velocidadInicial;
    Vector2 direccionNormalizada;
    // --------------------

public:
    MinaEnemiga(Vector2 pos, Vector2 dir);

    void actualizar(Protagonista& jugador, const Mapa& mapa) override;
    void dibujar() override;

    void recibirDanio(int cantidad, OrigenBala origenDanio) override;
    void explotar(bool porJugador);
    bool estaExplotando() const;
    bool explosionPuedeHerirJefe() const;
};
