#pragma once

#include "raylib.h"
#include "Protagonista.h"
#include "Mapa.h"
#include "GestorEntidades.h"
#include "SistemaRender.h"
#include "SistemaInput.h"
#include "MotorFisica.h"
#include "MotorColisiones.h"
#include "Constantes.h"
#include <vector>
#include <list>

// Estados del juego
enum class EstadoJuego {
    JUGANDO = 0,
    INICIANDO_JEFE,
    LEYENDO_NOTA,
    DIALOGO_FINAL,
    DIALOGO_INTRO,
    DIALOGO_PREGUNTAS,
    DIALOGO_RESPUESTA_1,
    DIALOGO_RESPUESTA_2,
    DIALOGO_DECISION_FINAL,
    FIN_JUEGO_SACRIFICIO,
    FIN_JUEGO_HUIR,
    FIN_JUEGO_MUERTO, // Animación de muerte (Fade-in)
    MENU_MUERTE       // Menú interactivo de muerte
};

class Juego
{
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

    int notaActualID;
    bool jefeHaSpawned;
    Rectangle triggerRectJefe;
    float temporizadorSpawnJefe;

    // Variables de diálogo
    int opcionDialogo;
    float temporizadorDialogo;

    // --- ASSETS PANTALLA MUERTE Y BOTONES ---
    Texture2D texPantallaMuerte;

    Texture2D texReintentar;
    Texture2D texReintentarSel;
    Texture2D texMenu;
    Texture2D texMenuSel;
    Texture2D texSalir;
    Texture2D texSalirSel;
    // ----------------------------------------

    int opcionMenuMuerteSeleccionada; // 0: Reintentar, 1: Menú, 2: Salir

    // Métodos internos
    void ResetSustoFantasma();
    void reiniciarJuego();
    void procesarCheats();

    void actualizar();
    void dibujar();

    // Métodos de Actualización por Estado
    void actualizarJugando();
    void actualizarLeyendoNota();
    void actualizarIniciandoJefe();
    void actualizarDialogo();
    void actualizarDialogoIntro();
    void actualizarDialogoPreguntas();
    void actualizarDialogoRespuesta();
    void actualizarDialogoDecisionFinal();
    void actualizarFinJuego();
    void actualizarMenuMuerte();

    // Métodos de Dibujado por Estado
    void dibujarJugando();
    void dibujarLeyendoNota();
    void dibujarIniciandoJefe();
    void dibujarDialogo();
    void dibujarDialogoIntro();
    void dibujarDialogoPreguntas();
    void dibujarDialogoRespuesta();
    void dibujarDialogoDecisionFinal();
    void dibujarFinJuego();

    // --- MODIFICADO: Acepta alpha para el fade-in ---
    void dibujarMenuMuerte(float alpha = 1.0f);
    // ------------------------------------------------

public:
    Juego();
    ~Juego();
    void run();
};
