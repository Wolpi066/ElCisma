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
    FIN_JUEGO_MUERTO,
    MENU_MUERTE,
    ITEM_OBTENIDO     // <--- NUEVO: Estado Popup de objeto
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

    // --- VARIABLES POP-UP ITEM ---
    Texture2D texPopupItem;      // Textura del item actual
    const char* nombrePopupItem; // Nombre del item
    const char* descPopupItem;   // Descripcion
    float escalaPopup;           // Animacion
    // -----------------------------

    // Assets Menu Inicio
    Texture2D texFondoMenuInicio;
    Texture2D texBtnJugar;
    Texture2D texBtnJugarSel;
    Texture2D texBtnCreditos;
    Texture2D texBtnCreditosSel;
    Texture2D texBtnSalirInicio;
    Texture2D texBtnSalirInicioSel;
    int opcionMenuInicial;

    // Assets Creditos
    Texture2D texFondoCreditos;

    // Assets Muerte
    Texture2D texFondoMuerte;
    Texture2D texBtnReintentar;
    Texture2D texBtnReintentarSel;
    Texture2D texBtnMenu;
    Texture2D texBtnMenuSel;
    Texture2D texBtnSalirMuerte;
    Texture2D texBtnSalirMuerteSel;
    int opcionMenuMuerteSeleccionada;

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
    void actualizarItemObtenido(); // <--- NUEVO

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
    void dibujarItemObtenido(); // <--- NUEVO

public:
    Juego();
    ~Juego();
    void run();
};
