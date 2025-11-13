#pragma once

#include "raylib.h"
#include "Protagonista.h"
#include "Mapa.h"
#include "GestorEntidades.h"
#include "SistemaRender.h"
#include "MotorColisiones.h"
#include "MotorFisica.h"
#include "SistemaInput.h"
#include "Constantes.h"

enum class EstadoJuego {
    JUGANDO,
    LEYENDO_NOTA,
    INICIANDO_JEFE,
    DIALOGO_FINAL,

    // --- TAREA FINAL: Nuevos estados de diálogo ---
    DIALOGO_INTRO,
    DIALOGO_PREGUNTAS,
    DIALOGO_RESPUESTA_1,
    DIALOGO_RESPUESTA_2,
    DIALOGO_DECISION_FINAL,

    FIN_JUEGO_SACRIFICIO,
    FIN_JUEGO_HUIR,
    // ---------------------------------------------

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
    const float TIEMPO_LIMITE_FANTASMA = 210.0f;

    float temporizadorSustoFantasma;
    float proximoSustoFantasma;
    void ResetSustoFantasma();

    int notaActualID;

    bool jefeHaSpawned;
    Rectangle triggerRectJefe;
    float temporizadorSpawnJefe;

    // --- TAREA FINAL: Diálogo ---
    int opcionDialogo; // Reutilizada para Q&A y Decisión Final
    float temporizadorDialogo;
    // -----------------------------

    void actualizar();
    void dibujar();

    void actualizarJugando();
    void actualizarDialogo();
    void actualizarLeyendoNota();
    void actualizarIniciandoJefe();
    void dibujarIniciandoJefe();

    void dibujarJugando();
    void dibujarDialogo();
    void dibujarLeyendoNota();
    void dibujarFinJuego();

    // --- TAREA FINAL: Nuevas funciones de estado ---
    void actualizarDialogoIntro();
    void actualizarDialogoPreguntas();
    void actualizarDialogoRespuesta();
    void actualizarDialogoDecisionFinal();
    void actualizarFinJuego(); // ¡NUEVO!

    void dibujarDialogoIntro();
    void dibujarDialogoPreguntas();
    void dibujarDialogoRespuesta();
    void dibujarDialogoDecisionFinal();
    // ----------------------------------------------

    void procesarCheats();
};
