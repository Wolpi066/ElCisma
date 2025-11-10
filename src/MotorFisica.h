#pragma once

#include "raylib.h"
#include <vector>
#include <list>
#include "Protagonista.h"
#include "Enemigo.h"
#include "Jefe.h"
#include "Bala.h"
#include "Mapa.h"
#include "Fantasma.h"

class MotorFisica
{
private:
    static Vector2 calcularMovimientoValido(Vector2 posActual, Vector2 velActual, Rectangle rectColision,
                                            const std::vector<Rectangle>& muros,
                                            const std::vector<Rectangle>& cajas,
                                            const Rectangle& puerta, bool puertaEstaAbierta,
                                            bool& choco);

public:
    static void moverJugador(Protagonista& jugador, Vector2 dirMovimiento,
                             const std::vector<Rectangle>& muros,
                             const std::vector<Rectangle>& cajas,
                             const Rectangle& puerta, bool puertaEstaAbierta,
                             const std::vector<Bala*>& balas);

    static void moverEnemigos(std::vector<Enemigo*>& enemigos,
                              const std::vector<Rectangle>& muros,
                              const std::vector<Rectangle>& cajas,
                              const Rectangle& puerta, bool puertaEstaAbierta);

    static void moverJefes(std::vector<Jefe*>& jefes,
                           const std::vector<Rectangle>& muros,
                           const std::vector<Rectangle>& cajas,
                           const Rectangle& puerta, bool puertaEstaAbierta);

    static void moverBalas(std::vector<Bala*>& balas,
                           const std::vector<Rectangle>& muros,
                           const std::vector<Rectangle>& cajas,
                           const Rectangle& puerta, bool puertaEstaAbierta);

    static void resolverColisionesDinamicas(Protagonista& jugador, std::vector<Enemigo*>& enemigos);
};
