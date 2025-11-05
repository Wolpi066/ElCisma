#pragma once

#include "raylib.h"
#include "Protagonista.h"
#include "Mapa.h"
#include "GestorEntidades.h"
#include "SistemaRender.h"
// Incluimos los sistemas estaticos
#include "MotorColisiones.h"
#include "MotorFisica.h"
#include "SistemaInput.h"
#include "Constantes.h"

enum class EstadoJuego {
    JUGANDO,
    LEYENDO_NOTA, // <-- ¢B¢BNUEVO!!
    DIALOGO_FINAL,
    FIN_JUEGO_GANADO,
    FIN_JUEGO_MUERTO
};

class Juego
{
public:
    Juego();
    ~Juego();
    void run();

private:
    Protagonista jugador;
    Mapa miMapa;
    GestorEntidades gestor;
    SistemaRender renderizador;

    EstadoJuego estadoActual;
    float temporizadorPartida;
    const float TIEMPO_LIMITE_FANTASMA = 210.0f; // 3.5 minutos

    float temporizadorSustoFantasma;
    float proximoSustoFantasma;
    void ResetSustoFantasma();

    // --- ¢B¢BNUEVO!! ---
    int notaActualID; // Guarda el ID de la nota que estamos leyendo
    // -----------------

    void actualizar();
    void dibujar();

    // --- ¢B¢BNOMBRES CAMBIADOS!! ---
    void actualizarJugando();
    void actualizarDialogo();
    void actualizarLeyendoNota(); // <-- ¢B¢BNUEVO!!

    void dibujarJugando();
    void dibujarDialogo();
    void dibujarLeyendoNota(); // <-- ¢B¢BNUEVO!!
    void dibujarFinJuego();
};
