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
    MENU_INICIAL = 0,
    CREDITOS,
    JUGANDO,
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
    FIN_JUEGO_MUERTO, // Animacion de muerte (Fade-in)
    MENU_MUERTE       // Menu interactivo de muerte
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

    // Variables de dialogo
    int opcionDialogo;
    float temporizadorDialogo;

    // --- ASSETS MENU INICIO ---
    Texture2D texFondoMenuInicio;

    // Botones Menu Inicio (Normal y Seleccionado)
    Texture2D texBtnJugar;
    Texture2D texBtnJugarSel;
    Texture2D texBtnCreditos;
    Texture2D texBtnCreditosSel;
    Texture2D texBtnSalirInicio;
    Texture2D texBtnSalirInicioSel;

    int opcionMenuInicial; // 0: Jugar, 1: Creditos, 2: Salir

    // --- ASSETS PANTALLA MUERTE ---
    Texture2D texFondoMuerte;

    // Botones Menu Muerte (Normal y Seleccionado)
    Texture2D texBtnReintentar;
    Texture2D texBtnReintentarSel;
    Texture2D texBtnMenu;
    Texture2D texBtnMenuSel;
    Texture2D texBtnSalirMuerte;
    Texture2D texBtnSalirMuerteSel;

    int opcionMenuMuerteSeleccionada; // 0: Reintentar, 1: Menu, 2: Salir

    // Metodos internos
    void ResetSustoFantasma();
    void reiniciarJuego();
    void procesarCheats();

    void actualizar();
    void dibujar();

    // Metodos de Actualizacion por Estado
    void actualizarMenuInicial();
    void actualizarCreditos();
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

    // Metodos de Dibujado por Estado
    void dibujarMenuInicial();
    void dibujarCreditos();
    void dibujarJugando();
    void dibujarLeyendoNota();
    void dibujarIniciandoJefe();
    void dibujarDialogo();
    void dibujarDialogoIntro();
    void dibujarDialogoPreguntas();
    void dibujarDialogoRespuesta();
    void dibujarDialogoDecisionFinal();
    void dibujarFinJuego();

    void dibujarMenuMuerte(float alpha = 1.0f);

public:
    Juego();
    ~Juego();
    void run();
};
