#include "Juego.h"
#include "Constantes.h"
#include "raymath.h"
#include <stdexcept>
#include "Fantasma.h"
#include "IndicadorPuerta.h"
#include "Llave.h"
#include "Cofre.h"
#include "Nota.h"

#include "Botiquin.h"
#include "Bateria.h"
#include "CajaDeMuniciones.h"
#include "Armadura.h"

#include "BalaDeRifle.h"
#include "BalaMonstruosa.h"
#include "Spawner.h"
#include "Jefe.h"

// Funcion auxiliar para texto con efecto glitch
void DibujarTextoGlitch(const char* texto, int posX, int posY, int fontSize, Color colorPrincipal)
{
    float offset = sin(GetTime() * 15.0f) * 1.0f;
    float offset2 = cos(GetTime() * 10.0f) * 1.0f;
    DrawText(texto, posX + (int)offset, posY + (int)offset2, fontSize, Fade((Color){255, 0, 100, 255}, 0.2f));
    DrawText(texto, posX - (int)offset, posY - (int)offset2, fontSize, Fade((Color){0, 255, 200, 255}, 0.2f));
    DrawText(texto, posX, posY, fontSize, colorPrincipal);
}

Juego::Juego()
    : jugador({0, 500}),
      miMapa(),
      gestor(),
      renderizador(),
      estadoActual(EstadoJuego::MENU_INICIAL), // Inicia en el Menu
      temporizadorPartida(0.0f),
      temporizadorSustoFantasma(0.0f),
      notaActualID(0),
      jefeHaSpawned(false),
      triggerRectJefe({ -200, -200, 400, 400 }),
      temporizadorSpawnJefe(0.0f),
      opcionDialogo(1),
      temporizadorDialogo(0.0f),
      opcionMenuInicial(0),
      opcionMenuMuerteSeleccionada(0)
{
    // Precarga del mundo
    miMapa.poblarMundo(gestor);
    ResetSustoFantasma();
    renderizador.inicializarMinimapa(miMapa);

    // ===========================================================
    // CARGA DE ASSETS - MENU DE INICIO
    // ===========================================================
    // Fondo (Raiz de HUD)
    texFondoMenuInicio = LoadTexture("assets/HUD/MenuInicio.png");

    // Botones (Ruta: assets/HUD/Botones/MenuInicio/...)
    texBtnJugar          = LoadTexture("assets/HUD/Botones/MenuInicio/Jugar.png");
    texBtnJugarSel       = LoadTexture("assets/HUD/Botones/MenuInicio/JugarSeleccionado.png");

    texBtnCreditos       = LoadTexture("assets/HUD/Botones/MenuInicio/Creditos.png");
    texBtnCreditosSel    = LoadTexture("assets/HUD/Botones/MenuInicio/CreditosSeleccionado.png");

    texBtnSalirInicio    = LoadTexture("assets/HUD/Botones/MenuInicio/Salir.png");
    texBtnSalirInicioSel = LoadTexture("assets/HUD/Botones/MenuInicio/SalirSeleccionado.png");

    // ===========================================================
    // CARGA DE ASSETS - PANTALLA CREDITOS (NUEVO)
    // ===========================================================
    texFondoCreditos = LoadTexture("assets/HUD/Creditos.png");

    // ===========================================================
    // CARGA DE ASSETS - PANTALLA DE MUERTE
    // ===========================================================
    // Fondo (Raiz de HUD)
    texFondoMuerte       = LoadTexture("assets/HUD/PantallaMuerte.png");

    // Botones (Ruta: assets/HUD/Botones/PantallaMuerte/...)
    texBtnReintentar     = LoadTexture("assets/HUD/Botones/PantallaMuerte/Reintentar.png");
    texBtnReintentarSel  = LoadTexture("assets/HUD/Botones/PantallaMuerte/ReintentarSeleccionado.png");

    texBtnMenu           = LoadTexture("assets/HUD/Botones/PantallaMuerte/Menu.png");
    texBtnMenuSel        = LoadTexture("assets/HUD/Botones/PantallaMuerte/MenuSeleccionado.png");

    texBtnSalirMuerte    = LoadTexture("assets/HUD/Botones/PantallaMuerte/Salir.png");
    texBtnSalirMuerteSel = LoadTexture("assets/HUD/Botones/PantallaMuerte/SalirSeleccionado.png");
}

Juego::~Juego()
{
    // Descargar texturas Menu Inicio
    UnloadTexture(texFondoMenuInicio);
    UnloadTexture(texBtnJugar);
    UnloadTexture(texBtnJugarSel);
    UnloadTexture(texBtnCreditos);
    UnloadTexture(texBtnCreditosSel);
    UnloadTexture(texBtnSalirInicio);
    UnloadTexture(texBtnSalirInicioSel);

    // Descargar textura Creditos
    UnloadTexture(texFondoCreditos);

    // Descargar texturas Pantalla Muerte
    UnloadTexture(texFondoMuerte);
    UnloadTexture(texBtnReintentar);
    UnloadTexture(texBtnReintentarSel);
    UnloadTexture(texBtnMenu);
    UnloadTexture(texBtnMenuSel);
    UnloadTexture(texBtnSalirMuerte);
    UnloadTexture(texBtnSalirMuerteSel);

    gestor.limpiarTodo();
}

void Juego::run()
{
    while (!WindowShouldClose()) {
        actualizar();
        dibujar();
    }
}

void Juego::ResetSustoFantasma() {
    temporizadorSustoFantasma = 0.0f;
    proximoSustoFantasma = (float)GetRandomValue(15, 30);
}

void Juego::reiniciarJuego()
{
    // Reinicio completo de variables para nueva partida
    jugador.reset();
    jugador.setPosicion({0, 500});

    gestor.limpiarTodo();
    miMapa.poblarMundo(gestor);

    estadoActual = EstadoJuego::JUGANDO;
    temporizadorPartida = 0.0f;
    notaActualID = 0;
    jefeHaSpawned = false;
    temporizadorSpawnJefe = 0.0f;

    miMapa.abrirPuerta();

    ResetSustoFantasma();
    Fantasma::despertado = false;
    Fantasma::estaDespertando = false;
    Fantasma::estaAsustando = false;
    Fantasma::modoFuria = false;
    Fantasma::modoDialogo = false;
    Fantasma::jefeEnCombate = false;

    HideCursor();
}

void Juego::actualizar()
{
    switch (estadoActual)
    {
        case EstadoJuego::MENU_INICIAL:
            actualizarMenuInicial();
            break;
        case EstadoJuego::CREDITOS:
            actualizarCreditos();
            break;
        case EstadoJuego::JUGANDO:
            actualizarJugando();
            break;
        case EstadoJuego::INICIANDO_JEFE:
            actualizarIniciandoJefe();
            break;
        case EstadoJuego::LEYENDO_NOTA:
            actualizarLeyendoNota();
            break;
        case EstadoJuego::DIALOGO_FINAL:
            actualizarDialogo();
            break;
        case EstadoJuego::DIALOGO_INTRO:
            actualizarDialogoIntro();
            break;
        case EstadoJuego::DIALOGO_PREGUNTAS:
            actualizarDialogoPreguntas();
            break;
        case EstadoJuego::DIALOGO_RESPUESTA_1:
        case EstadoJuego::DIALOGO_RESPUESTA_2:
            actualizarDialogoRespuesta();
            break;
        case EstadoJuego::DIALOGO_DECISION_FINAL:
            actualizarDialogoDecisionFinal();
            break;
        case EstadoJuego::FIN_JUEGO_SACRIFICIO:
        case EstadoJuego::FIN_JUEGO_HUIR:
            actualizarFinJuego();
            break;
        case EstadoJuego::FIN_JUEGO_MUERTO:
            // Transicion breve
            estadoActual = EstadoJuego::MENU_MUERTE;
            temporizadorDialogo = 0.0f;
            ShowCursor();
            opcionMenuMuerteSeleccionada = 0;
            break;
        case EstadoJuego::MENU_MUERTE:
            actualizarMenuMuerte();
            break;
    }
}

void Juego::dibujar()
{
    BeginDrawing();
        ClearBackground(BLACK);
        switch (estadoActual)
        {
            case EstadoJuego::MENU_INICIAL:
                dibujarMenuInicial();
                break;
            case EstadoJuego::CREDITOS:
                dibujarCreditos();
                break;
            case EstadoJuego::JUGANDO:
                dibujarJugando();
                break;
            case EstadoJuego::INICIANDO_JEFE:
                dibujarIniciandoJefe();
                break;
            case EstadoJuego::LEYENDO_NOTA:
                dibujarLeyendoNota();
                break;
            case EstadoJuego::DIALOGO_FINAL:
                dibujarDialogo();
                break;
            case EstadoJuego::DIALOGO_INTRO:
                dibujarDialogoIntro();
                break;
            case EstadoJuego::DIALOGO_PREGUNTAS:
                dibujarDialogoPreguntas();
                break;
            case EstadoJuego::DIALOGO_RESPUESTA_1:
            case EstadoJuego::DIALOGO_RESPUESTA_2:
                dibujarDialogoRespuesta();
                break;
            case EstadoJuego::DIALOGO_DECISION_FINAL:
                dibujarDialogoDecisionFinal();
                break;
            case EstadoJuego::FIN_JUEGO_SACRIFICIO:
            case EstadoJuego::FIN_JUEGO_HUIR:
                dibujarFinJuego();
                break;
            case EstadoJuego::FIN_JUEGO_MUERTO:
                dibujarMenuMuerte(0.0f);
                break;
            case EstadoJuego::MENU_MUERTE:
                {
                     float alpha = Clamp(temporizadorDialogo / 2.0f, 0.0f, 1.0f);
                     dibujarMenuMuerte(alpha);
                }
                break;
        }
    EndDrawing();
}

// ============================================================================
// SECCION: MENU INICIAL
// ============================================================================

void Juego::actualizarMenuInicial()
{
    ShowCursor();

    // --- TAMAÑO AJUSTADO: Mas ancho (300) y misma altura (80) ---
    float btnW = 300.0f;
    float btnH = 80.0f;
    // ------------------------------------------------------------

    float spacing = 25.0f;
    // Centrado vertical en la mitad inferior
    float startY = (float)Constantes::ALTO_PANTALLA * 0.5f;
    float centerX = ((float)Constantes::ANCHO_PANTALLA - btnW) / 2.0f;

    Rectangle rectJugar    = { centerX, startY, btnW, btnH };
    Rectangle rectCreditos = { centerX, startY + btnH + spacing, btnW, btnH };
    Rectangle rectSalir    = { centerX, startY + (btnH + spacing) * 2, btnW, btnH };

    // 1. Teclado
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        opcionMenuInicial--;
        if (opcionMenuInicial < 0) opcionMenuInicial = 2;
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        opcionMenuInicial++;
        if (opcionMenuInicial > 2) opcionMenuInicial = 0;
    }

    // 2. Raton (Hover)
    Vector2 mouse = GetMousePosition();
    if (CheckCollisionPointRec(mouse, rectJugar)) opcionMenuInicial = 0;
    else if (CheckCollisionPointRec(mouse, rectCreditos)) opcionMenuInicial = 1;
    else if (CheckCollisionPointRec(mouse, rectSalir)) opcionMenuInicial = 2;

    // 3. Seleccion
    bool clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    bool enter = (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_E));
    bool clickValido = false;

    if (clicked) {
        if (opcionMenuInicial == 0 && CheckCollisionPointRec(mouse, rectJugar)) clickValido = true;
        else if (opcionMenuInicial == 1 && CheckCollisionPointRec(mouse, rectCreditos)) clickValido = true;
        else if (opcionMenuInicial == 2 && CheckCollisionPointRec(mouse, rectSalir)) clickValido = true;
    }

    if (enter || clickValido)
    {
        if (opcionMenuInicial == 0) {
            reiniciarJuego(); // JUGAR
        }
        else if (opcionMenuInicial == 1) {
            estadoActual = EstadoJuego::CREDITOS;
        }
        else if (opcionMenuInicial == 2) {
            CloseWindow();
        }
    }
}

void Juego::dibujarMenuInicial()
{
    // 1. Dibujar Fondo
    if (texFondoMenuInicio.id != 0) {
        // Escalamos el fondo para que cubra toda la pantalla
        Rectangle src = { 0, 0, (float)texFondoMenuInicio.width, (float)texFondoMenuInicio.height };
        Rectangle dest = { 0, 0, (float)Constantes::ANCHO_PANTALLA, (float)Constantes::ALTO_PANTALLA };
        DrawTexturePro(texFondoMenuInicio, src, dest, {0,0}, 0.0f, WHITE);
    } else {
        ClearBackground(BLACK);
        DibujarTextoGlitch("ERROR: FONDO NO ENCONTRADO", 50, 50, 20, RED);
    }

    // 2. Configurar Dimensiones FIJAS (Coinciden con actualizar: 300x80)
    float btnW = 300.0f;
    float btnH = 80.0f;
    float spacing = 25.0f;
    float startY = (float)Constantes::ALTO_PANTALLA * 0.5f;
    float centerX = ((float)Constantes::ANCHO_PANTALLA - btnW) / 2.0f;

    // --- TINTE OSCURO PARA LOS BOTONES ---
    Color tinteBoton = { 180, 180, 180, 255 };

    // Helper Lambda para dibujar boton escalado
    auto DibujarBotonSeguro = [&](Texture2D& tex, int yOffset, bool seleccionado, const char* textoError) {
        float posY = startY + yOffset * (btnH + spacing);

        if (tex.id != 0) {
            // ESCALADO: Usamos DrawTexturePro
            Rectangle source = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
            Rectangle dest   = { centerX, posY, btnW, btnH };

            // Si esta seleccionado, lo dibujamos a full brillo (WHITE), si no, con el tinte oscuro
            Color colorFinal = seleccionado ? WHITE : tinteBoton;

            DrawTexturePro(tex, source, dest, {0,0}, 0.0f, colorFinal);
        } else {
            // FALLBACK
            Color color = seleccionado ? MAGENTA : DARKGRAY;
            DrawRectangle((int)centerX, (int)posY, (int)btnW, (int)btnH, Fade(color, 0.7f));
            DrawRectangleLines((int)centerX, (int)posY, (int)btnW, (int)btnH, RED);
            DrawText(textoError, (int)centerX + 10, (int)posY + 20, 20, WHITE);
        }
    };

    // BOTON JUGAR
    Texture2D tJugar = (opcionMenuInicial == 0) ? texBtnJugarSel : texBtnJugar;
    DibujarBotonSeguro(tJugar, 0, (opcionMenuInicial == 0), "ERR: Jugar.png");

    // BOTON CREDITOS
    Texture2D tCreditos = (opcionMenuInicial == 1) ? texBtnCreditosSel : texBtnCreditos;
    DibujarBotonSeguro(tCreditos, 1, (opcionMenuInicial == 1), "ERR: Creditos.png");

    // BOTON SALIR
    Texture2D tSalir = (opcionMenuInicial == 2) ? texBtnSalirInicioSel : texBtnSalirInicio;
    DibujarBotonSeguro(tSalir, 2, (opcionMenuInicial == 2), "ERR: Salir.png");
}

void Juego::actualizarCreditos()
{
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_E) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        estadoActual = EstadoJuego::MENU_INICIAL;
    }
}

void Juego::dibujarCreditos()
{
    // --- AQUI ES DONDE DIBUJAMOS LA IMAGEN DE FONDO ---
    if (texFondoCreditos.id != 0) {
        Rectangle src = { 0, 0, (float)texFondoCreditos.width, (float)texFondoCreditos.height };
        Rectangle dest = { 0, 0, (float)Constantes::ANCHO_PANTALLA, (float)Constantes::ALTO_PANTALLA };
        DrawTexturePro(texFondoCreditos, src, dest, {0,0}, 0.0f, WHITE);
    } else {
        ClearBackground(BLACK);
        // Si no hay imagen, dibujamos el titulo manual
        DibujarTextoGlitch("CREDITOS", 50, 50, 40, PURPLE);
    }

    // Ajustamos la posicion Y para que no pise el titulo de la imagen
    int startY = 220;
    int stepY = 35;

    // --- DESARROLLO ---
    DrawText("Desarrollo:", 50, startY, 20, WHITE);
    DrawText("Mailen Acosta Vera", 50, startY + stepY, 20, GRAY);
    DrawText("Emiliano Volpino", 50, startY + stepY * 2, 20, GRAY);

    startY += 130;

    // --- ARTE ---
    DrawText("Arte:", 50, startY, 20, WHITE);
    DrawText("Generado por IA / Assets Propios", 50, startY + stepY, 20, GRAY);

    startY += 100;

    // --- MOTOR ---
    DrawText("Motor:", 50, startY, 20, WHITE);
    DrawText("C++ & Raylib", 50, startY + stepY, 20, GRAY);

    startY += 100;

    // --- AGRADECIMIENTOS ---
    DrawText("Agradecimientos Especiales:", 50, startY, 20, WHITE);
    DrawText("A nuestros profesores y testers", 50, startY + stepY, 20, GRAY);

    // Pie
    const char* msg = "Volver [E]";
    DrawText(msg, Constantes::ANCHO_PANTALLA - MeasureText(msg, 20) - 40, Constantes::ALTO_PANTALLA - 60, 20, WHITE);
}


// ============================================================================
// SECCION: PANTALLA MUERTE
// ============================================================================

void Juego::actualizarMenuMuerte()
{
    if (temporizadorDialogo < 2.0f) {
        temporizadorDialogo += GetFrameTime();
    }

    // Layout
    float btnY = (float)Constantes::ALTO_PANTALLA - 150.0f;
    float spacing = 30.0f;

    // TAMAÑO FIJO PARA PANTALLA MUERTE TAMBIEN
    float btnW = 240.0f;
    float btnH = 80.0f;

    float totalW = (btnW * 3) + (spacing * 2);
    float startX = ((float)Constantes::ANCHO_PANTALLA - totalW) / 2.0f;

    Rectangle rectReintentar = { startX, btnY, btnW, btnH };
    Rectangle rectMenu       = { startX + btnW + spacing, btnY, btnW, btnH };
    Rectangle rectSalir      = { startX + (btnW + spacing) * 2, btnY, btnW, btnH };

    if (temporizadorDialogo > 0.1f) {
        // Inputs
        if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
            opcionMenuMuerteSeleccionada--;
            if (opcionMenuMuerteSeleccionada < 0) opcionMenuMuerteSeleccionada = 2;
        }
        if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
            opcionMenuMuerteSeleccionada++;
            if (opcionMenuMuerteSeleccionada > 2) opcionMenuMuerteSeleccionada = 0;
        }

        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, rectReintentar)) opcionMenuMuerteSeleccionada = 0;
        else if (CheckCollisionPointRec(mouse, rectMenu))  opcionMenuMuerteSeleccionada = 1;
        else if (CheckCollisionPointRec(mouse, rectSalir)) opcionMenuMuerteSeleccionada = 2;

        bool clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        bool enter = (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_E) || IsKeyPressed(KEY_SPACE));
        bool clickValido = false;

        if (clicked) {
            if (opcionMenuMuerteSeleccionada == 0 && CheckCollisionPointRec(mouse, rectReintentar)) clickValido = true;
            else if (opcionMenuMuerteSeleccionada == 1 && CheckCollisionPointRec(mouse, rectMenu)) clickValido = true;
            else if (opcionMenuMuerteSeleccionada == 2 && CheckCollisionPointRec(mouse, rectSalir)) clickValido = true;
        }

        if (enter || clickValido)
        {
            if (opcionMenuMuerteSeleccionada == 0) reiniciarJuego();
            else if (opcionMenuMuerteSeleccionada == 1) {
                estadoActual = EstadoJuego::MENU_INICIAL; // Volver al inicio
            }
            else if (opcionMenuMuerteSeleccionada == 2) CloseWindow();
        }
    }
}

void Juego::dibujarMenuMuerte(float alpha)
{
    // Fondo
    if (texFondoMuerte.id != 0) {
        Rectangle src = { 0, 0, (float)texFondoMuerte.width, (float)texFondoMuerte.height };
        Rectangle dest = { 0, 0, (float)Constantes::ANCHO_PANTALLA, (float)Constantes::ALTO_PANTALLA };
        DrawTexturePro(texFondoMuerte, src, dest, {0,0}, 0.0f, Fade(WHITE, alpha));
    } else {
        ClearBackground(BLACK);
        DibujarTextoGlitch("HAS MUERTO", 200, 200, 60, RED);
    }

    // Layout
    float btnY = (float)Constantes::ALTO_PANTALLA - 150.0f;
    float spacing = 30.0f;
    float btnW = 240.0f;
    float btnH = 80.0f;
    float totalW = (btnW * 3) + (spacing * 2);
    float startX = ((float)Constantes::ANCHO_PANTALLA - totalW) / 2.0f;

    // Funcion lambda para dibujar botones de muerte de forma segura Y ESCALADA
    auto DibujarBtnMuerte = [&](Texture2D& tex, float x, float y) {
        if (tex.id != 0) {
            Rectangle source = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
            Rectangle dest   = { x, y, btnW, btnH };
            DrawTexturePro(tex, source, dest, {0,0}, 0.0f, Fade(WHITE, alpha));
        } else {
            DrawRectangle((int)x, (int)y, (int)btnW, (int)btnH, Fade(RED, alpha));
            DrawText("ERROR IMG", (int)x+10, (int)y+20, 20, WHITE);
        }
    };

    // Reintentar
    Texture2D tReintentar = (opcionMenuMuerteSeleccionada == 0) ? texBtnReintentarSel : texBtnReintentar;
    DibujarBtnMuerte(tReintentar, startX, btnY);

    // Menu
    Texture2D tMenu = (opcionMenuMuerteSeleccionada == 1) ? texBtnMenuSel : texBtnMenu;
    DibujarBtnMuerte(tMenu, startX + btnW + spacing, btnY);

    // Salir
    Texture2D tSalir = (opcionMenuMuerteSeleccionada == 2) ? texBtnSalirMuerteSel : texBtnSalirMuerte;
    DibujarBtnMuerte(tSalir, startX + (btnW + spacing) * 2, btnY);
}


void Juego::actualizarJugando()
{
    procesarCheats();

    if (!jugador.estaVivo()) {
        jugador.actualizarInterno(renderizador.getCamera(), {0, 0});

        if (jugador.haFinalizadoAnimacionMuerte()) {
            estadoActual = EstadoJuego::FIN_JUEGO_MUERTO;
            temporizadorDialogo = 0.0f;
        }
        return;
    }

    if (!jefeHaSpawned && miMapa.estaPuertaAbierta())
    {
        if (CheckCollisionRecs(jugador.getRect(), triggerRectJefe))
        {
            estadoActual = EstadoJuego::INICIANDO_JEFE;
            temporizadorSpawnJefe = 3.0f;
            miMapa.cerrarPuerta();
            jugador.quitarLlave();
            jugador.setBateriaCongelada(true);

            Fantasma::jefeEnCombate = true;
            return;
        }
    }

    Vector2 direccionMovimiento = SistemaInput::getDireccionMovimiento();
    bool quiereDisparar = SistemaInput::quiereDisparar();
    bool quiereInteractuar = SistemaInput::quiereInteractuar();

    jugador.actualizarInterno(renderizador.getCamera(), direccionMovimiento);

    gestor.actualizarIAEntidades(jugador, miMapa);

    renderizador.actualizarNieblaMinimapa(jugador);

    if (quiereInteractuar)
    {
        float radioInteraccion = 75.0f;
        Vector2 posJugador = jugador.getPosicion();
        Consumible* itemMasCercano = nullptr;
        float distMasCercana = 9999.0f;
        for (Consumible* item : gestor.getConsumibles())
        {
            if (item->esInteraccionPorTecla() && !item->estaConsumido())
            {
                Rectangle itemRect = item->getRect();
                Vector2 itemCenter = { itemRect.x + itemRect.width / 2, itemRect.y + itemRect.height / 2 };
                float dist = Vector2Distance(posJugador, itemCenter);
                if (dist < distMasCercana)
                {
                    distMasCercana = dist;
                    itemMasCercano = item;
                }
            }
        }
        if (itemMasCercano != nullptr && distMasCercana <= radioInteraccion)
        {
            IndicadorPuerta* indicador = dynamic_cast<IndicadorPuerta*>(itemMasCercano);
            if (indicador)
            {
                if (jugador.getTieneLlave())
                {
                    miMapa.abrirPuerta();
                    for (Consumible* itemPuerta : gestor.getConsumibles()) {
                        IndicadorPuerta* ind = dynamic_cast<IndicadorPuerta*>(itemPuerta);
                        if (ind) ind->consumir();
                    }
                }
            }
            else if (Cofre* cofre = dynamic_cast<Cofre*>(itemMasCercano))
            {
                int lootID = cofre->usar(jugador);
                Vector2 posDrop = cofre->getPosicion();
                switch (lootID)
                {
                    case 1: gestor.registrarConsumible(Spawner<Botiquin>::Spawn(posDrop)); break;
                    case 2: gestor.registrarConsumible(Spawner<Bateria>::Spawn(posDrop)); break;
                    case 3: gestor.registrarConsumible(Spawner<CajaDeMuniciones>::Spawn(posDrop)); break;
                    case 4: gestor.registrarConsumible(Spawner<Armadura>::Spawn(posDrop)); break;
                    case 99: gestor.registrarConsumible(Spawner<Llave>::Spawn(posDrop)); break;
                    default: break;
                }
            }
            else if (Nota* nota = dynamic_cast<Nota*>(itemMasCercano))
            {
                this->notaActualID = nota->usar(jugador);
                this->estadoActual = EstadoJuego::LEYENDO_NOTA;
            }
            else if (
                dynamic_cast<Llave*>(itemMasCercano) ||
                dynamic_cast<Botiquin*>(itemMasCercano) ||
                dynamic_cast<Bateria*>(itemMasCercano) ||
                dynamic_cast<CajaDeMuniciones*>(itemMasCercano) ||
                dynamic_cast<Armadura*>(itemMasCercano)
            )
            {
                itemMasCercano->usar(jugador);
            }
        }
    }

    MotorFisica::moverJugador(
        jugador,
        direccionMovimiento,
        miMapa.getMuros(),
        miMapa.getCajas(),
        miMapa.getPuertaJefe(),
        miMapa.estaPuertaAbierta(),
        gestor.getBalas()
    );

    MotorFisica::moverEnemigos(gestor.getEnemigos(), miMapa.getMuros(), miMapa.getCajas(), miMapa.getPuertaJefe(), miMapa.estaPuertaAbierta());
    MotorFisica::moverJefes(gestor.getJefes(), miMapa.getMuros(), miMapa.getCajas(), miMapa.getPuertaJefe(), miMapa.estaPuertaAbierta());
    MotorFisica::moverBalas(gestor.getBalas(), miMapa.getMuros(), miMapa.getCajas(), miMapa.getPuertaJefe(), miMapa.estaPuertaAbierta());

    MotorFisica::resolverColisionesDinamicas(jugador, gestor.getEnemigos());

    int tipoDisparo = jugador.intentarDisparar(quiereDisparar);
    if (tipoDisparo > 0)
    {
        Vector2 dirDisparo = jugador.getDireccionVista();
        bool estaMoviendo = (Vector2LengthSqr(direccionMovimiento) > 0.0f);
        bool esUnCheat = (tipoDisparo == 2);
        if (estaMoviendo && !esUnCheat)
        {
            float anguloPreciso = atan2f(dirDisparo.y, dirDisparo.x);
            float desviacionGrados = (float)GetRandomValue(-200, 200) / 10.0f;
            float desviacionRadianes = desviacionGrados * DEG2RAD;
            float anguloImpreciso = anguloPreciso + desviacionRadianes;
            dirDisparo = { cosf(anguloImpreciso), sinf(anguloImpreciso) };
        }
        gestor.registrarBala(new BalaDeRifle(jugador.getPosicion(), dirDisparo, esUnCheat));
    }

    MotorColisiones::procesar(jugador, gestor);

    gestor.recolectarBasura();

    temporizadorPartida += GetFrameTime();

    if (jefeHaSpawned && !gestor.getJefes().empty() && gestor.getJefes()[0]->estaEnFaseFinal())
    {
        Fantasma::jefeEnCombate = false;
        Fantasma::estaDespertando = false;
        Fantasma::despertado = true;
        Fantasma::modoFuria = false;
        Fantasma::modoDialogo = true;
        Fantasma::estaAsustando = false;

        estadoActual = EstadoJuego::DIALOGO_FINAL;
        temporizadorDialogo = 3.0f;
        return;
    }

    if (!Fantasma::despertado && !Fantasma::estaDespertando)
    {
        if (!Fantasma::estaAsustando) {
            temporizadorSustoFantasma += GetFrameTime();
            if (temporizadorSustoFantasma > proximoSustoFantasma) {
                Fantasma::estaAsustando = true;
                Fantasma::temporizadorSusto = 0.75f;
                Enemigo* fantasmaPtr = nullptr;
                for (Enemigo* e : gestor.getEnemigos()) {
                    if (dynamic_cast<Fantasma*>(e)) {
                        fantasmaPtr = e;
                        break;
                    }
                }
                if (fantasmaPtr) {
                    Camera2D cam = renderizador.getCamera();
                    float screenLeft = cam.target.x - cam.offset.x / cam.zoom;
                    float screenRight = cam.target.x + cam.offset.x / cam.zoom;
                    float screenTop = cam.target.y - cam.offset.y / cam.zoom;
                    float screenBottom = cam.target.y + cam.offset.y / cam.zoom;
                    if (GetRandomValue(0, 1) == 0) {
                        Fantasma::posSustoInicio = { screenLeft - 50, (float)GetRandomValue((int)screenTop, (int)screenBottom) };
                        Fantasma::posSustoFin = { screenRight + 50, (float)GetRandomValue((int)screenTop, (int)screenBottom) };
                    } else {
                        Fantasma::posSustoInicio = { screenRight + 50, (float)GetRandomValue((int)screenTop, (int)screenBottom) };
                        Fantasma::posSustoFin = { screenLeft - 50, (float)GetRandomValue((int)screenTop, (int)screenBottom) };
                    }
                    fantasmaPtr->setPosicion(Fantasma::posSustoInicio);
                }
                ResetSustoFantasma();
            }
        }

        bool despertarFantasma = false;
        if (jugador.getBateria() <= 0) {
            despertarFantasma = true;
            Fantasma::modoFuria = true;
        }
        else if (temporizadorPartida > TIEMPO_LIMITE_FANTASMA && !jugador.getTieneLlave()) {
            despertarFantasma = true;
            Fantasma::modoFuria = false;
        }

        if (despertarFantasma)
        {
            Fantasma::estaDespertando = true;
            Fantasma::temporizadorDespertar = 3.0f;
            Fantasma::estaAsustando = false;
            Enemigo* fantasmaPtr = nullptr;
            for (Enemigo* e : gestor.getEnemigos()) {
                if (dynamic_cast<Fantasma*>(e)) {
                    fantasmaPtr = e;
                    break;
                }
            }
            if (fantasmaPtr) {
                Camera2D cam = renderizador.getCamera();
                float screenLeft = cam.target.x - cam.offset.x / cam.zoom;
                float screenRight = cam.target.x + cam.offset.x / cam.zoom;
                float screenTop = cam.target.y - cam.offset.y / cam.zoom;
                float screenBottom = cam.target.y + cam.offset.y / cam.zoom;
                int borde = GetRandomValue(0, 3);
                Vector2 posInicio = {0,0};
                if (borde == 0) posInicio = { screenLeft - 50, (float)GetRandomValue((int)screenTop, (int)screenBottom) };
                else if (borde == 1) posInicio = { screenRight + 50, (float)GetRandomValue((int)screenTop, (int)screenBottom) };
                else if (borde == 2) posInicio = { (float)GetRandomValue((int)screenLeft, (int)screenRight), screenTop - 50 };
                else posInicio = { (float)GetRandomValue((int)screenLeft, (int)screenRight), screenBottom + 50 };
                fantasmaPtr->setPosicion(posInicio);
            }
        }
    }
    else
    {
        if (!Fantasma::modoFuria && !Fantasma::modoDialogo && jugador.getTieneLlave()) {
            Fantasma::despertado = false;
        }
    }
}

void Juego::actualizarLeyendoNota()
{
    jugador.actualizarInterno(renderizador.getCamera(), {0, 0});

    if (SistemaInput::quiereInteractuar() || IsKeyPressed(KEY_ESCAPE))
    {
        estadoActual = EstadoJuego::JUGANDO;
        notaActualID = 0;
    }
}

void Juego::actualizarIniciandoJefe()
{
    jugador.actualizarInterno(renderizador.getCamera(), {0, 0});

    temporizadorSpawnJefe -= GetFrameTime();
    if (temporizadorSpawnJefe <= 0)
    {
        gestor.registrarJefe(new Jefe({0.0f, 0.0f}));
        jefeHaSpawned = true;
        estadoActual = EstadoJuego::JUGANDO;
    }
}

void Juego::actualizarDialogo()
{
    jugador.actualizarInterno(renderizador.getCamera(), {0, 0});

    gestor.actualizarIAEntidades(jugador, miMapa);

    temporizadorDialogo -= GetFrameTime();
    if (temporizadorDialogo <= 0)
    {
        estadoActual = EstadoJuego::DIALOGO_INTRO;
        opcionDialogo = 1;

        for (Enemigo* enemigo : gestor.getEnemigos()) {
            if (dynamic_cast<Fantasma*>(enemigo)) {
                enemigo->setPosicion(Vector2Add(jugador.getPosicion(), Vector2Scale(jugador.getDireccionVista(), 80.0f)));
                break;
            }
        }
    }
}

void Juego::actualizarDialogoIntro()
{
    jugador.actualizarInterno(renderizador.getCamera(), {0, 0});
    if (SistemaInput::quiereInteractuar())
    {
        estadoActual = EstadoJuego::DIALOGO_PREGUNTAS;
        opcionDialogo = 1;
    }
}

void Juego::actualizarDialogoPreguntas()
{
    jugador.actualizarInterno(renderizador.getCamera(), {0, 0});

    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
    {
        opcionDialogo--;
        if (opcionDialogo < 1) opcionDialogo = 3;
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
    {
        opcionDialogo++;
        if (opcionDialogo > 3) opcionDialogo = 1;
    }

    if (SistemaInput::quiereInteractuar())
    {
        if (opcionDialogo == 1)
        {
            estadoActual = EstadoJuego::DIALOGO_RESPUESTA_1;
        }
        else if (opcionDialogo == 2)
        {
            estadoActual = EstadoJuego::DIALOGO_RESPUESTA_2;
        }
        else
        {
            estadoActual = EstadoJuego::DIALOGO_DECISION_FINAL;
            opcionDialogo = 1;
        }
    }
}

void Juego::actualizarDialogoRespuesta()
{
    jugador.actualizarInterno(renderizador.getCamera(), {0, 0});
    if (SistemaInput::quiereInteractuar())
    {
        estadoActual = EstadoJuego::DIALOGO_PREGUNTAS;
        opcionDialogo = 1;
    }
}

void Juego::actualizarDialogoDecisionFinal()
{
    jugador.actualizarInterno(renderizador.getCamera(), {0, 0});

    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
    {
        opcionDialogo = 1;
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
    {
        opcionDialogo = 2;
    }

    if (SistemaInput::quiereInteractuar())
    {
        if (opcionDialogo == 1)
        {
            estadoActual = EstadoJuego::FIN_JUEGO_SACRIFICIO;
        }
        else
        {
            estadoActual = EstadoJuego::FIN_JUEGO_HUIR;
        }
        temporizadorDialogo = 3.0f;
    }
}

void Juego::actualizarFinJuego()
{
    temporizadorDialogo -= GetFrameTime();
    if (temporizadorDialogo <= 0)
    {
        CloseWindow();
    }
}


void Juego::dibujarJugando()
{
    renderizador.dibujarTodo(jugador, miMapa, gestor);
}

void Juego::dibujarIniciandoJefe()
{
    dibujarJugando();
    const char* textoSpawn = "LA PUERTA SE HA SELLADO.\n\nSIENTES UNA DISTORSION EN EL CENTRO DE LA SALA...";
    float alpha = 1.0f;
    if (temporizadorSpawnJefe < 1.0f) {
        alpha = temporizadorSpawnJefe;
    }
    DibujarTextoGlitch(textoSpawn,
                       Constantes::ANCHO_PANTALLA / 2 - MeasureText(textoSpawn, 20) / 2,
                       Constantes::ALTO_PANTALLA - 150,
                       20, Fade(RED, alpha));
}


void Juego::dibujarLeyendoNota()
{
    renderizador.dibujarTodo(jugador, miMapa, gestor);
    DrawRectangle(0, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, Fade(BLACK, 0.8f));
    Rectangle frame = {
        Constantes::ANCHO_PANTALLA * 0.1f,
        Constantes::ALTO_PANTALLA * 0.15f,
        Constantes::ANCHO_PANTALLA * 0.8f,
        Constantes::ALTO_PANTALLA * 0.7f
    };
    DrawRectangleRec(frame, Fade((Color){10, 20, 30, 255}, 0.80f));
    DrawRectangleLinesEx(frame, 2.0f, Fade((Color){0, 255, 128, 100}, 0.5f));
    for (int y = (int)frame.y; y < (int)(frame.y + frame.height); y += 3)
    {
        DrawLine(frame.x, y, frame.x + frame.width, y, Fade(BLACK, 0.5f));
    }

    const char* textoTitulo = "REGISTRO";
    const char* textoNota = "...";

    switch(notaActualID)
    {
        case 0:
            textoTitulo = "...Estas despierto?";
            textoNota = "No hay tiempo. La Fusion es inminente. El Arquitecto...\n"
                        "El Cisma... esta en el centro. Debes llegar a el.\n\n"
                        "Usa la linterna, busca la Llave... y por favor, ten cuidado\n"
                        "con mis... ecos. No dejes que la bateria se agote.\n\n- E.";
            break;
        case 1:
            textoTitulo = "REG-001: EL NEXO";
            textoNota = "Dia 1. Lo llamamos 'El Nexo'. No es un lugar, es un estado.\n"
                        "Un Vortice de psique pura. El Arquitecto cree que podemos\n"
                        "estabilizarlo... pero la materia fisica que introducimos...\n"
                        "se esta corrompiendo. Los sujetos de prueba (Z-00)\n"
                        "son... maleables.";
            break;
        case 2:
            textoTitulo = "REG-014: EL ETER";
            textoNota = "El Eter es mas que energia. Tiene... voluntad.\n"
                        "Las almas de los que murieron en el primer test de Fusion\n"
                        "siguen aqui. Podemos oirlas. Elana (Dra. Reyes) insiste\n"
                        "en que estan sufriendo. El Arquitecto la ha apartado\n"
                        "del proyecto.";
            break;
        case 3:
            textoTitulo = "DIARIO DEL ARQUITECTO (El Cisma)";
            textoNota = "Funciona! Puedo sentirlo! El Desdoblamiento es la clave!\n"
                        "Unir la materia y el espiritu! Seremos dioses!\n\n"
                        "Elana no lo entiende. Llama 'corrupcion' a lo que yo\n"
                        "llamo 'creacion'. Ella es... un estorbo.";
            break;
        case 4:
            textoTitulo = "ALERTA DE SEGURIDAD: Z-08";
            textoNota = "...brecha de contencion. El Sujeto Z-08 (Apodo: 'Obeso')\n"
                        "ha roto el sello. Ha... absorbido a dos tecnicos.\n"
                        "La carne se pliega sobre si misma. Las balas estandar\n"
                        "no son suficientes. Solicitando municion perforante.";
            break;
        case 5:
            textoTitulo = "MI INVESTIGACION";
            textoNota = "El esta ciego. El Vortice no se puede controlar,\n"
                        "solo se puede... cerrar. La Fusion no es ascension,\n"
                        "es un cancer. He creado una 'Llave' de frecuencia.\n"
                        "Una anulacion. Si el Cisma se estabiliza, todo estara\n"
                        "perdido. Debo esconderla...";
            break;
        case 6:
            textoTitulo = "TRANSMISION CORRUPTA";
            textoNota = "...no puedo... no puedo ver... la carne... se retuerce...\n"
                        "huesos... cables... donde... donde esta mi cara?\n\n"
                        "ELANA! POR QUE ME HAS ENCERRADO AQUI?!\n"
                        "POR QUE?! m... mi... rostro...";
            break;
        case 7:
            textoTitulo = "REPORTE DE ENERGIA";
            textoNota = "La bateria principal es inestable. Sigue atrayendo...\n"
                        "cosas... del Eter. Fantasmas. Los llamamos 'Ecos'.\n"
                        "Apagar la luz ayuda, pero cuando la bateria se agota...\n"
                        "la oscuridad los enfurece. Se vuelven... rapidos.";
            break;
        case 8:
            textoTitulo = "REG-045: EL REMANENTE";
            textoNota = "Sujeto P-07 (El Remanente). Es el unico... hibrido estable.\n"
                        "Puede transitar ambos planos. Por que el? Es su psique?\n"
                        "El Arquitecto esta... celoso. Dice que P-07 es la\n"
                        "'Mascara' perfecta que necesita.";
            break;
        case 9:
            textoTitulo = "CUARENTENA: CAMARA 2";
            textoNota = "La Camara del Canto esta perdida. Los Ecos... han matado\n"
                        "a todos. No podemos danarlos. No... fisicamente.\n"
                        "El Arquitecto se rie. Dice que son 'energia pura'.\n"
                        "Son... almas puras. Gritando.";
            break;
        case 10:
            textoTitulo = "---";
            textoNota = "...solo oxido y mascaras...\n"
                        "...El Vortice reclama...\n"
                        "...El Cisma debe...\n"
                        "...Elana... perdoname...";
            break;
        default:
            textoTitulo = "ERROR DE DATOS";
            textoNota = "El archivo esta corrupto. No se puede leer el registro.";
            break;
    }

    DibujarTextoGlitch(textoTitulo, (int)(frame.x + 20), (int)(frame.y + 20), 20, (Color){0, 255, 128, 255});
    DrawLine((int)(frame.x + 20), (int)(frame.y + 45), (int)(frame.x + frame.width - 20), (int)(frame.y + 45), Fade(GREEN, 0.2f));
    DibujarTextoGlitch(textoNota, (int)(frame.x + 20), (int)(frame.y + 70), 20, (Color){200, 255, 220, 255});
    const char* textoCerrar = "Presiona 'E' para cerrar";
    float alphaCerrar = (sin(GetTime() * 2.0f) + 1.0f) / 2.0f;
    DrawText(textoCerrar, (int)(frame.x + frame.width - MeasureText(textoCerrar, 20) - 20), (int)(frame.y + frame.height - 40), 20, Fade(GREEN, 0.5f + alphaCerrar * 0.5f));
}

void Juego::dibujarDialogo()
{
    renderizador.dibujarTodo(jugador, miMapa, gestor);
    DrawRectangle(0, Constantes::ALTO_PANTALLA - 100, Constantes::ANCHO_PANTALLA, 100, Fade(BLACK, 0.8f));
    DrawText("El 'Fantasma' se acerca... su movimiento es erratico pero decidido...", 20, Constantes::ALTO_PANTALLA - 80, 20, WHITE);
    DrawText("Sientes una presencia abrumadora... no es hostil, es... triste.", 20, Constantes::ALTO_PANTALLA - 50, 20, WHITE);
}


void DibujarCajaDialogo()
{
    DrawRectangle(0, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, Fade(BLACK, 0.85f));
    Rectangle frame = {
        Constantes::ANCHO_PANTALLA * 0.1f,
        Constantes::ALTO_PANTALLA * 0.25f,
        Constantes::ANCHO_PANTALLA * 0.8f,
        Constantes::ALTO_PANTALLA * 0.5f
    };
    DrawRectangleRec(frame, Fade((Color){10, 20, 30, 255}, 0.90f));
    DrawRectangleLinesEx(frame, 2.0f, Fade(PURPLE, 0.5f));
}

void Juego::dibujarDialogoIntro()
{
    renderizador.dibujarTodo(jugador, miMapa, gestor);
    DibujarCajaDialogo();

    Rectangle frame = { Constantes::ANCHO_PANTALLA * 0.1f, Constantes::ALTO_PANTALLA * 0.25f, Constantes::ANCHO_PANTALLA * 0.8f, Constantes::ALTO_PANTALLA * 0.5f };

    const char* t1 = "ELANA: Remanente... P-07... estas... vivo.";
    const char* t2 = "Lo... lo detuviste. El Cisma... El Arquitecto...";
    const char* t3 = "Por fin ha callado.";

    DibujarTextoGlitch(t1, (int)(frame.x + 20), (int)(frame.y + 30), 20, (Color){200, 200, 255, 255});
    DibujarTextoGlitch(t2, (int)(frame.x + 20), (int)(frame.y + 60), 20, (Color){200, 200, 255, 255});
    DibujarTextoGlitch(t3, (int)(frame.x + 20), (int)(frame.y + 90), 20, (Color){200, 200, 255, 255});

    const char* textoCerrar = "Presiona 'E' para continuar...";
    float alphaCerrar = (sin(GetTime() * 2.0f) + 1.0f) / 2.0f;
    DrawText(textoCerrar, (int)(frame.x + frame.width - MeasureText(textoCerrar, 20) - 20), (int)(frame.y + frame.height - 40), 20, Fade(PURPLE, 0.5f + alphaCerrar * 0.5f));
}

void Juego::dibujarDialogoPreguntas()
{
    renderizador.dibujarTodo(jugador, miMapa, gestor);
    DibujarCajaDialogo();
    Rectangle frame = { Constantes::ANCHO_PANTALLA * 0.1f, Constantes::ALTO_PANTALLA * 0.25f, Constantes::ANCHO_PANTALLA * 0.8f, Constantes::ALTO_PANTALLA * 0.5f };

    const char* t1 = "ELANA: ... (Espera tu respuesta)";
    DibujarTextoGlitch(t1, (int)(frame.x + 20), (int)(frame.y + 30), 20, (Color){200, 200, 255, 255});
    DrawLine((int)(frame.x + 20), (int)(frame.y + 70), (int)(frame.x + frame.width - 20), (int)(frame.y + 70), Fade(PURPLE, 0.2f));

    const char* textoOpcion1 = "[1] Quien eres tu?";
    const char* textoOpcion2 = "[2] Que es este lugar?";
    const char* textoOpcion3 = "[3] ... (Quedarse callado)";

    Color color1 = (opcionDialogo == 1) ? YELLOW : GRAY;
    Color color2 = (opcionDialogo == 2) ? YELLOW : GRAY;
    Color color3 = (opcionDialogo == 3) ? YELLOW : GRAY;

    DibujarTextoGlitch(textoOpcion1, (int)(frame.x + 40), (int)(frame.y + 100), 20, color1);
    DibujarTextoGlitch(textoOpcion2, (int)(frame.x + 40), (int)(frame.y + 130), 20, color2);
    DibujarTextoGlitch(textoOpcion3, (int)(frame.x + 40), (int)(frame.y + 160), 20, color3);

    if(opcionDialogo == 1) DibujarTextoGlitch(">", (int)(frame.x + 20), (int)(frame.y + 100), 20, YELLOW);
    else if(opcionDialogo == 2) DibujarTextoGlitch(">", (int)(frame.x + 20), (int)(frame.y + 130), 20, YELLOW);
    else DibujarTextoGlitch(">", (int)(frame.x + 20), (int)(frame.y + 160), 20, YELLOW);
}

void Juego::dibujarDialogoRespuesta()
{
    renderizador.dibujarTodo(jugador, miMapa, gestor);
    DibujarCajaDialogo();
    Rectangle frame = { Constantes::ANCHO_PANTALLA * 0.1f, Constantes::ALTO_PANTALLA * 0.25f, Constantes::ANCHO_PANTALLA * 0.8f, Constantes::ALTO_PANTALLA * 0.5f };

    const char* t1 = "...";

    if (estadoActual == EstadoJuego::DIALOGO_RESPUESTA_1)
    {
        t1 = "ELANA: Soy... era... la Dra. Elana Reyes. Yo... intente\n"
             "detenerlo. Cree la Llave para anular la fusion, pero\n"
             "el me encontro... y me convirtio en esto. Un Eco.";
    }
    else if (estadoActual == EstadoJuego::DIALOGO_RESPUESTA_2)
    {
        t1 = "ELANA: Es el Nexo. El laboratorio del Arquitecto.\n"
             "Un Vortice donde intento forzar la fusion entre la materia\n"
             "y el eter. Es... un cancer en la realidad.";
    }

    DibujarTextoGlitch(t1, (int)(frame.x + 20), (int)(frame.y + 30), 20, (Color){200, 200, 255, 255});

    const char* textoCerrar = "Presiona 'E' para continuar...";
    float alphaCerrar = (sin(GetTime() * 2.0f) + 1.0f) / 2.0f;
    DrawText(textoCerrar, (int)(frame.x + frame.width - MeasureText(textoCerrar, 20) - 20), (int)(frame.y + frame.height - 40), 20, Fade(PURPLE, 0.5f + alphaCerrar * 0.5f));
}

void Juego::dibujarDialogoDecisionFinal()
{
    renderizador.dibujarTodo(jugador, miMapa, gestor);
    DibujarCajaDialogo();
    Rectangle frame = { Constantes::ANCHO_PANTALLA * 0.1f, Constantes::ALTO_PANTALLA * 0.25f, Constantes::ANCHO_PANTALLA * 0.8f, Constantes::ALTO_PANTALLA * 0.5f };

    const char* t1 = "ELANA: El Cisma esta muerto, pero el Vortice sigue aqui.\n"
                     "Inestable. Remanente, tu eres el unico hibrido estable.\n"
                     "Puedes tomar su lugar... o puedes huir.\n\n"
                     "Si te vas, esto volvera a empezar. El Nexo... siempre reclama.";

    DibujarTextoGlitch(t1, (int)(frame.x + 20), (int)(frame.y + 30), 20, (Color){200, 200, 255, 255});
    DrawLine((int)(frame.x + 20), (int)(frame.y + 150), (int)(frame.x + frame.width - 20), (int)(frame.y + 150), Fade(PURPLE, 0.2f));

    const char* textoOpcion1 = "[1] Me quedare. Tomare el lugar del Cisma.";
    const char* textoOpcion2 = "[2] No es mi problema. Me voy de aqui.";

    Color color1 = (opcionDialogo == 1) ? YELLOW : GRAY;
    Color color2 = (opcionDialogo == 2) ? YELLOW : GRAY;

    DibujarTextoGlitch(textoOpcion1, (int)(frame.x + 40), (int)(frame.y + 180), 20, color1);
    DibujarTextoGlitch(textoOpcion2, (int)(frame.x + 40), (int)(frame.y + 210), 20, color2);

    if(opcionDialogo == 1) DibujarTextoGlitch(">", (int)(frame.x + 20), (int)(frame.y + 180), 20, YELLOW);
    else DibujarTextoGlitch(">", (int)(frame.x + 20), (int)(frame.y + 210), 20, YELLOW);

    const char* textoCerrar = "Presiona 'E' para decidir";
    float alphaCerrar = (sin(GetTime() * 2.0f) + 1.0f) / 2.0f;
    DrawText(textoCerrar, (int)(frame.x + frame.width - MeasureText(textoCerrar, 20) - 20), (int)(frame.y + frame.height - 40), 20, Fade(PURPLE, 0.5f + alphaCerrar * 0.5f));
}


void Juego::dibujarFinJuego()
{
    renderizador.dibujarTodo(jugador, miMapa, gestor);
    DrawRectangle(0, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, Fade(BLACK, 0.8f));

    if (estadoActual == EstadoJuego::FIN_JUEGO_SACRIFICIO)
    {
        const char* t1 = "EL NUEVO ARQUITECTO";
        const char* t2 = "Aceptas el trono. El Nexo se estabiliza, pero te conviertes\n"
                         "en su nuevo corazon. Las almas estan atrapadas, pero en calma.\n\n"
                         "...esperando al proximo Remanente.";
        DibujarTextoGlitch(t1, Constantes::ANCHO_PANTALLA / 2 - MeasureText(t1, 40) / 2, Constantes::ALTO_PANTALLA / 2 - 60, 40, LIME);
        DibujarTextoGlitch(t2, Constantes::ANCHO_PANTALLA / 2 - MeasureText(t2, 20) / 2, Constantes::ALTO_PANTALLA / 2 + 20, 20, WHITE);
    }
    else if (estadoActual == EstadoJuego::FIN_JUEGO_HUIR)
    {
        const char* t1 = "EL CICLO SE REPITE";
        const char* t2 = "Escapas del Vortice. Detras de ti, el Nexo ruge y Elana grita.\n"
                         "El Cisma ha muerto, pero el trono esta vacio...\n\n"
                         "Y el Vortice odia el vacio.";
        DibujarTextoGlitch(t1, Constantes::ANCHO_PANTALLA / 2 - MeasureText(t1, 40) / 2, Constantes::ALTO_PANTALLA / 2 - 60, 40, RED);
        DibujarTextoGlitch(t2, Constantes::ANCHO_PANTALLA / 2 - MeasureText(t2, 20) / 2, Constantes::ALTO_PANTALLA / 2 + 20, 20, GRAY);
    }
}

void Juego::procesarCheats()
{
    if (!IsKeyDown(KEY_LEFT_SHIFT)) {
        return;
    }
    if (IsKeyPressed(KEY_H)) {
        jugador.curarVida(100);
    }
    if (IsKeyPressed(KEY_A)) {
        jugador.recibirArmadura();
    }
    if (IsKeyPressed(KEY_M)) {
        jugador.recargarMunicion(50);
    }
    if (IsKeyPressed(KEY_B)) {
        jugador.recargarBateria(100);
    }
    if (IsKeyPressed(KEY_K)) {
        jugador.recibirLlave();
    }
    if (IsKeyPressed(KEY_J)) {
        jugador.activarCheatDisparo();
    }
    if (IsKeyPressed(KEY_L))
    {
        if (jefeHaSpawned && !gestor.getJefes().empty())
        {
            gestor.getJefes()[0]->forzarMuerte();
        }
    }
}
