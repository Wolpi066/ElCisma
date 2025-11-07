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
    LEYENDO_NOTA,
    // --- ¢B¢BNUEVO ESTADO!! ---
    INICIANDO_JEFE, // Para la animaci«Qn de spawn
    // ------------------------
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

    int notaActualID;

    // --- ¢B¢BNUEVOS MIEMBROS PARA SECUENCIA DE JEFE!! ---
    bool jefeHaSpawned;
    Rectangle triggerRectJefe; // El "trigger" para empezar la pelea
    float temporizadorSpawnJefe; // El timer para la animaci«Qn
    // --------------------------------------------------

    void actualizar();
    void dibujar();

    void actualizarJugando();
    void actualizarDialogo();
    void actualizarLeyendoNota();
    // --- ¢B¢BNUEVAS FUNCIONES!! ---
    void actualizarIniciandoJefe();
    void dibujarIniciandoJefe();
    // ----------------------------

    void dibujarJugando();
    void dibujarDialogo();
    void dibujarLeyendoNota();
    void dibujarFinJuego();

    void procesarCheats();
};
