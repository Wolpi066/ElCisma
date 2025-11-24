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

enum class EstadoJuego {
    MENU_INICIAL = 0,
    CREDITOS,
    INTRO_CINEMATICA,
    JUGANDO,
    PAUSA,
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
    FIN_JUEGO_MUERTO,
    MENU_MUERTE,
    ITEM_OBTENIDO
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

    Music musicaAmbiente;
    Music musicaJefe;
    Music musicaFinal;
    Sound fxUIHover;
    Sound fxUIClick;
    Sound fxPuerta;

    // --- NUEVO: Control de Fade Out ---
    float volumenMusicaJefe;
    // ----------------------------------

    int opcionDialogo;
    float temporizadorDialogo;

    // Variables Cinemáticas
    Texture2D texIntro;
    float timerIntro;
    int faseIntro;
    int letrasIntroMostradas;
    float timerEscrituraIntro;
    float timerDespertar;

    Texture2D texFinalSacrificio;
    Texture2D texFinalHuir;
    float alphaFinal;
    float timerEscritura;
    int letrasMostradas;
    int faseFinal;

    // Popup
    Texture2D texPopupItem;
    const char* nombrePopupItem;
    const char* descPopupItem;
    float escalaPopup;

    // Menús
    Texture2D texFondoMenuInicio;
    Texture2D texBtnJugar;
    Texture2D texBtnJugarSel;
    Texture2D texBtnCreditos;
    Texture2D texBtnCreditosSel;
    Texture2D texBtnSalirInicio;
    Texture2D texBtnSalirInicioSel;
    int opcionMenuInicial;

    Texture2D texFondoCreditos;

    Texture2D texFondoMuerte;
    Texture2D texBtnReintentar;
    Texture2D texBtnReintentarSel;
    Texture2D texBtnMenu;
    Texture2D texBtnMenuSel;
    Texture2D texBtnSalirMuerte;
    Texture2D texBtnSalirMuerteSel;
    int opcionMenuMuerteSeleccionada;

    int opcionMenuPausa;

    void ResetSustoFantasma();
    void reiniciarJuego();
    void procesarCheats();

    void actualizar();
    void dibujar();

    void actualizarMenuInicial();
    void actualizarCreditos();
    void actualizarIntro();
    void actualizarJugando();
    void actualizarPausa();
    void actualizarLeyendoNota();
    void actualizarIniciandoJefe();
    void actualizarDialogo();
    void actualizarDialogoIntro();
    void actualizarDialogoPreguntas();
    void actualizarDialogoRespuesta();
    void actualizarDialogoDecisionFinal();
    void actualizarFinJuego();
    void actualizarMenuMuerte();
    void actualizarItemObtenido();

    void dibujarMenuInicial();
    void dibujarCreditos();
    void dibujarIntro();
    void dibujarJugando();
    void dibujarPausa();
    void dibujarLeyendoNota();
    void dibujarIniciandoJefe();
    void dibujarDialogo();
    void dibujarDialogoIntro();
    void dibujarDialogoPreguntas();
    void dibujarDialogoRespuesta();
    void dibujarDialogoDecisionFinal();
    void dibujarFinJuego();
    void dibujarMenuMuerte(float alpha = 1.0f);
    void dibujarItemObtenido();

public:
    Juego();
    ~Juego();
    void run();
};
