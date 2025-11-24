#include "Juego.h"
#include "Constantes.h"
#include "raylib.h"
#include "raymath.h"
#include <stdexcept>
#include "Fantasma.h"
#include "IndicadorPuerta.h"
#include "Llave.h"
#include "Cofre.h"
#include "Nota.h"
#include <cstring>

#include "Botiquin.h"
#include "Bateria.h"
#include "CajaDeMuniciones.h"
#include "Armadura.h"

#include "BalaDeRifle.h"
#include "BalaMonstruosa.h"
#include "Spawner.h"
#include "Jefe.h"

// --- Helpers Visuales ---
void DibujarTextoGlitch(const char* texto, int posX, int posY, int fontSize, Color colorPrincipal) {
    float offset = sin(GetTime() * 15.0f) * 1.0f;
    float offset2 = cos(GetTime() * 10.0f) * 1.0f;
    DrawText(texto, posX + (int)offset, posY + (int)offset2, fontSize, Fade((Color){255, 0, 100, 255}, 0.2f));
    DrawText(texto, posX - (int)offset, posY - (int)offset2, fontSize, Fade((Color){0, 255, 200, 255}, 0.2f));
    DrawText(texto, posX, posY, fontSize, colorPrincipal);
}

void DibujarTextoConBorde(const char* texto, int x, int y, int fontSize, Color color) {
    int offset = 2;
    DrawText(texto, x - offset, y - offset, fontSize, BLACK);
    DrawText(texto, x + offset, y - offset, fontSize, BLACK);
    DrawText(texto, x - offset, y + offset, fontSize, BLACK);
    DrawText(texto, x + offset, y + offset, fontSize, BLACK);
    DrawText(texto, x, y, fontSize, color);
}

void DibujarCajaDialogo() {
    DrawRectangle(0, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, Fade(BLACK, 0.85f));
    Rectangle frame = { Constantes::ANCHO_PANTALLA * 0.1f, Constantes::ALTO_PANTALLA * 0.25f, Constantes::ANCHO_PANTALLA * 0.8f, Constantes::ALTO_PANTALLA * 0.5f };
    DrawRectangleRec(frame, Fade((Color){10, 20, 30, 255}, 0.90f));
    DrawRectangleLinesEx(frame, 2.0f, Fade(PURPLE, 0.5f));
}

// --- CONSTRUCTOR ---
Juego::Juego()
    : jugador({0, 500}),
      miMapa(),
      gestor(),
      renderizador(),
      estadoActual(EstadoJuego::MENU_INICIAL),
      temporizadorPartida(0.0f),
      temporizadorSustoFantasma(0.0f),
      proximoSustoFantasma(0.0f),
      notaActualID(0),
      jefeHaSpawned(false),
      triggerRectJefe({ -200, -200, 400, 400 }),
      temporizadorSpawnJefe(0.0f),
      // --- ORDEN CORRECTO DE INICIALIZACIÓN (SEGÚN .H) ---
      volumenMusicaJefe(1.0f), // 1. Volumen antes que dialogos
      opcionDialogo(1),        // 2. Dialogos
      temporizadorDialogo(0.0f),
      timerIntro(0.0f),        // 3. Cinemáticas
      faseIntro(0),
      letrasIntroMostradas(0),
      timerEscrituraIntro(0.0f),
      timerDespertar(0.0f),
      alphaFinal(0.0f),
      timerEscritura(0.0f),
      letrasMostradas(0),
      faseFinal(0),
      texPopupItem({0}),       // 4. Popup
      nombrePopupItem(""),
      descPopupItem(""),
      escalaPopup(0.0f),
      opcionMenuInicial(0),    // 5. Menús
      opcionMenuMuerteSeleccionada(0),
      opcionMenuPausa(0)       // Al final
      // ---------------------------------------------------
{
    SetExitKey(KEY_NULL);

    //miMapa.poblarMundo(gestor);
    ResetSustoFantasma();
    renderizador.inicializarMinimapa(miMapa);

    // --- CARGA DE ASSETS VISUALES ---
    texFondoMenuInicio = LoadTexture("assets/HUD/MenuInicio.png");
    texBtnJugar = LoadTexture("assets/HUD/Botones/MenuInicio/Jugar.png");
    texBtnJugarSel = LoadTexture("assets/HUD/Botones/MenuInicio/JugarSeleccionado.png");
    texBtnCreditos = LoadTexture("assets/HUD/Botones/MenuInicio/Creditos.png");
    texBtnCreditosSel = LoadTexture("assets/HUD/Botones/MenuInicio/CreditosSeleccionado.png");
    texBtnSalirInicio = LoadTexture("assets/HUD/Botones/MenuInicio/Salir.png");
    texBtnSalirInicioSel = LoadTexture("assets/HUD/Botones/MenuInicio/SalirSeleccionado.png");

    texFondoCreditos = LoadTexture("assets/HUD/Creditos.png");

    texFondoMuerte = LoadTexture("assets/HUD/PantallaMuerte.png");
    texBtnReintentar = LoadTexture("assets/HUD/Botones/PantallaMuerte/Reintentar.png");
    texBtnReintentarSel = LoadTexture("assets/HUD/Botones/PantallaMuerte/ReintentarSeleccionado.png");
    texBtnMenu = LoadTexture("assets/HUD/Botones/PantallaMuerte/Menu.png");
    texBtnMenuSel = LoadTexture("assets/HUD/Botones/PantallaMuerte/MenuSeleccionado.png");
    texBtnSalirMuerte = LoadTexture("assets/HUD/Botones/PantallaMuerte/Salir.png");
    texBtnSalirMuerteSel = LoadTexture("assets/HUD/Botones/PantallaMuerte/SalirSeleccionado.png");

    texFinalSacrificio = LoadTexture("assets/FinalSacrificio.png");
    texFinalHuir = LoadTexture("assets/FinalHuir.png");
    texIntro = LoadTexture("assets/Intro.png");

    // --- CARGA DE AUDIO ---
    musicaAmbiente = LoadMusicStream("assets/Audio/Musica/Ambientacion.wav");
    musicaJefe = LoadMusicStream("assets/Audio/Musica/Jefe.mp3");
    musicaFinal = LoadMusicStream("assets/Audio/Musica/Final.mp3");

    fxUIHover = LoadSound("assets/Audio/Sonidos/HUD/Hover.wav");
    fxUIClick = LoadSound("assets/Audio/Sonidos/HUD/Click.wav");
    fxPuerta = LoadSound("assets/Audio/Sonidos/PuertaAbierta.wav");

    // NOTA: NO iniciamos música ambiente aquí para que el menú esté en silencio
    // Se iniciará en actualizarMenuInicial() al pulsar JUGAR.
}

Juego::~Juego() {
    UnloadTexture(texFondoMenuInicio); UnloadTexture(texBtnJugar); UnloadTexture(texBtnJugarSel);
    UnloadTexture(texBtnCreditos); UnloadTexture(texBtnCreditosSel);
    UnloadTexture(texBtnSalirInicio); UnloadTexture(texBtnSalirInicioSel);
    UnloadTexture(texFondoCreditos);
    UnloadTexture(texFondoMuerte); UnloadTexture(texBtnReintentar); UnloadTexture(texBtnReintentarSel);
    UnloadTexture(texBtnMenu); UnloadTexture(texBtnMenuSel);
    UnloadTexture(texBtnSalirMuerte); UnloadTexture(texBtnSalirMuerteSel);
    UnloadTexture(texFinalSacrificio); UnloadTexture(texFinalHuir);
    UnloadTexture(texIntro);

    UnloadMusicStream(musicaAmbiente);
    UnloadMusicStream(musicaJefe);
    UnloadMusicStream(musicaFinal);
    UnloadSound(fxUIHover);
    UnloadSound(fxUIClick);
    UnloadSound(fxPuerta);

    gestor.limpiarTodo();
}

void Juego::run() {
    while (!WindowShouldClose()) {
        actualizar();
        dibujar();
    }
}

void Juego::ResetSustoFantasma() {
    temporizadorSustoFantasma = 0.0f;
    proximoSustoFantasma = (float)GetRandomValue(15, 30);
}

void Juego::reiniciarJuego() {
    jugador.reset();
    jugador.setPosicion({0, 500});
    gestor.limpiarTodo();
    miMapa.poblarMundo(gestor);
    estadoActual = EstadoJuego::JUGANDO;
    temporizadorPartida = 0.0f;
    notaActualID = 0;
    jefeHaSpawned = false;
    temporizadorSpawnJefe = 0.0f;
    miMapa.cerrarPuerta();
    ResetSustoFantasma();
    Fantasma::despertado = false;
    Fantasma::estaDespertando = false;
    Fantasma::estaAsustando = false;
    Fantasma::modoFuria = false;
    Fantasma::modoDialogo = false;
    Fantasma::jefeEnCombate = false;
    alphaFinal = 0.0f;
    faseFinal = 0;
    letrasMostradas = 0;
    timerEscritura = 0.0f;
    timerDespertar = 4.0f;

    // Reset Audio
    volumenMusicaJefe = 1.0f;
    SetMusicVolume(musicaJefe, 1.0f);
    StopMusicStream(musicaJefe);
    StopMusicStream(musicaFinal);

    // Al reiniciar (volvemos a jugar), arranca el ambiente
    PlayMusicStream(musicaAmbiente);

    HideCursor();
}

void Juego::actualizar() {
    // --- GESTIÓN DE MÚSICA (Streams) ---
    // Es obligatorio llamar a esto en cada frame para que la música avance
    UpdateMusicStream(musicaAmbiente);
    UpdateMusicStream(musicaJefe);
    UpdateMusicStream(musicaFinal);

    // 1. LÓGICA DE FADE OUT (Música Jefe)
    // Si el jefe ha muerto, bajamos el volumen gradualmente
    if (jefeHaSpawned && !gestor.getJefes().empty()) {
        Jefe* jefe = gestor.getJefes()[0];
        if (jefe->getFase() == FaseJefe::DERROTADO) {
            if (volumenMusicaJefe > 0.0f) {
                volumenMusicaJefe -= GetFrameTime() * 0.3f; // Baja en aprox 3 segundos
                if (volumenMusicaJefe < 0.0f) volumenMusicaJefe = 0.0f;
                SetMusicVolume(musicaJefe, volumenMusicaJefe);
            } else {
                StopMusicStream(musicaJefe); // Detener al llegar a 0
            }
        }
    }

    // 2. INICIO MÚSICA JEFE
    // Al entrar a la sala del jefe, cortamos ambiente y lanzamos rock
    if (estadoActual == EstadoJuego::INICIANDO_JEFE && !IsMusicStreamPlaying(musicaJefe)) {
        StopMusicStream(musicaAmbiente);
        volumenMusicaJefe = 1.0f;
        SetMusicVolume(musicaJefe, 1.0f);
        PlayMusicStream(musicaJefe);
    }

    // 3. INICIO MÚSICA FINAL
    // Al terminar el juego (Huida o Sacrificio)
    if ((estadoActual == EstadoJuego::FIN_JUEGO_SACRIFICIO || estadoActual == EstadoJuego::FIN_JUEGO_HUIR)
        && !IsMusicStreamPlaying(musicaFinal))
    {
        StopMusicStream(musicaJefe);
        StopMusicStream(musicaAmbiente);
        PlayMusicStream(musicaFinal);
    }

    // 4. LIMPIEZA EN MENÚ INICIAL
    // Si volvemos al menú, silencio total (respetando tu petición de que ambiente solo suene al jugar)
    if (estadoActual == EstadoJuego::MENU_INICIAL) {
        if (IsMusicStreamPlaying(musicaJefe)) StopMusicStream(musicaJefe);
        if (IsMusicStreamPlaying(musicaFinal)) StopMusicStream(musicaFinal);
        if (IsMusicStreamPlaying(musicaAmbiente)) StopMusicStream(musicaAmbiente);
    }

    // 5. LIMPIEZA EN MUERTE
    // Si morimos, cortar la música del jefe inmediatamente
    if (estadoActual == EstadoJuego::MENU_MUERTE) {
        if (IsMusicStreamPlaying(musicaJefe)) StopMusicStream(musicaJefe);
    }

    // --- MÁQUINA DE ESTADOS DEL JUEGO ---
    switch (estadoActual) {
        case EstadoJuego::MENU_INICIAL: actualizarMenuInicial(); break;
        case EstadoJuego::CREDITOS: actualizarCreditos(); break;
        case EstadoJuego::INTRO_CINEMATICA: actualizarIntro(); break;
        case EstadoJuego::JUGANDO: actualizarJugando(); break;
        case EstadoJuego::PAUSA: actualizarPausa(); break;
        case EstadoJuego::INICIANDO_JEFE: actualizarIniciandoJefe(); break;
        case EstadoJuego::LEYENDO_NOTA: actualizarLeyendoNota(); break;
        case EstadoJuego::DIALOGO_FINAL: actualizarDialogo(); break;
        case EstadoJuego::DIALOGO_INTRO: actualizarDialogoIntro(); break;
        case EstadoJuego::DIALOGO_PREGUNTAS: actualizarDialogoPreguntas(); break;
        case EstadoJuego::DIALOGO_RESPUESTA_1:
        case EstadoJuego::DIALOGO_RESPUESTA_2: actualizarDialogoRespuesta(); break;
        case EstadoJuego::DIALOGO_DECISION_FINAL: actualizarDialogoDecisionFinal(); break;
        case EstadoJuego::FIN_JUEGO_SACRIFICIO:
        case EstadoJuego::FIN_JUEGO_HUIR: actualizarFinJuego(); break;
        case EstadoJuego::FIN_JUEGO_MUERTO:
            estadoActual = EstadoJuego::MENU_MUERTE;
            temporizadorDialogo = 0.0f;
            ShowCursor();
            opcionMenuMuerteSeleccionada = 0;
            break;
        case EstadoJuego::MENU_MUERTE: actualizarMenuMuerte(); break;
        case EstadoJuego::ITEM_OBTENIDO: actualizarItemObtenido(); break;
    }
}

void Juego::dibujar() {
    BeginDrawing();
    ClearBackground(BLACK);
    switch (estadoActual) {
        case EstadoJuego::MENU_INICIAL: dibujarMenuInicial(); break;
        case EstadoJuego::CREDITOS: dibujarCreditos(); break;
        case EstadoJuego::INTRO_CINEMATICA: dibujarIntro(); break;
        case EstadoJuego::JUGANDO: dibujarJugando(); break;
        case EstadoJuego::PAUSA: dibujarPausa(); break;
        case EstadoJuego::INICIANDO_JEFE: dibujarIniciandoJefe(); break;
        case EstadoJuego::LEYENDO_NOTA: dibujarLeyendoNota(); break;
        case EstadoJuego::DIALOGO_FINAL: dibujarDialogo(); break;
        case EstadoJuego::DIALOGO_INTRO: dibujarDialogoIntro(); break;
        case EstadoJuego::DIALOGO_PREGUNTAS: dibujarDialogoPreguntas(); break;
        case EstadoJuego::DIALOGO_RESPUESTA_1:
        case EstadoJuego::DIALOGO_RESPUESTA_2: dibujarDialogoRespuesta(); break;
        case EstadoJuego::DIALOGO_DECISION_FINAL: dibujarDialogoDecisionFinal(); break;
        case EstadoJuego::FIN_JUEGO_SACRIFICIO:
        case EstadoJuego::FIN_JUEGO_HUIR: dibujarFinJuego(); break;
        case EstadoJuego::FIN_JUEGO_MUERTO: dibujarMenuMuerte(0.0f); break;
        case EstadoJuego::MENU_MUERTE: dibujarMenuMuerte(Clamp(temporizadorDialogo / 2.0f, 0.0f, 1.0f)); break;
        case EstadoJuego::ITEM_OBTENIDO: dibujarItemObtenido(); break;
    }
    EndDrawing();
}

// --- JUEGO PRINCIPAL ---
void Juego::actualizarJugando() {
    procesarCheats();
    if (timerDespertar > 0.0f) timerDespertar -= GetFrameTime();

    if (!jugador.estaVivo()) {
        jugador.actualizarInterno(renderizador.getCamera(), {0, 0});
        if (jugador.haFinalizadoAnimacionMuerte()) {
            estadoActual = EstadoJuego::FIN_JUEGO_MUERTO;
            temporizadorDialogo = 0.0f;
        }
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) {
        estadoActual = EstadoJuego::PAUSA;
        opcionMenuPausa = 0;
        return;
    }

    miMapa.actualizar(GetFrameTime());

    if (!jefeHaSpawned && miMapa.estaPuertaAbierta()) {
        if (CheckCollisionRecs(jugador.getRect(), triggerRectJefe)) {
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

    if (quiereInteractuar) {
        float radioInteraccion = 75.0f; Vector2 posJugador = jugador.getPosicion();
        Consumible* itemMasCercano = nullptr; float distMasCercana = 9999.0f;
        for (Consumible* item : gestor.getConsumibles()) {
            if (!item->estaConsumido()) {
                if (Cofre* c = dynamic_cast<Cofre*>(item)) { if (c->estaAbierto()) continue; }
                Rectangle itemRect = item->getRect();
                Vector2 itemCenter = { itemRect.x + itemRect.width / 2, itemRect.y + itemRect.height / 2 };
                float dist = Vector2Distance(posJugador, itemCenter);
                if (dist < distMasCercana) { distMasCercana = dist; itemMasCercano = item; }
            }
        }
        if (itemMasCercano != nullptr && distMasCercana <= radioInteraccion) {
            if (dynamic_cast<IndicadorPuerta*>(itemMasCercano)) {
                if (jugador.getTieneLlave()) {
                    miMapa.abrirPuerta();
                    PlaySound(fxPuerta); // Sonido puerta
                    for (Consumible* itemPuerta : gestor.getConsumibles()) {
                        if (dynamic_cast<IndicadorPuerta*>(itemPuerta)) itemPuerta->consumir();
                    }
                }
            }
            else if (Cofre* cofre = dynamic_cast<Cofre*>(itemMasCercano)) {
                int lootID = cofre->usar(jugador);
                Vector2 posDrop = cofre->getPosicion(); posDrop.y += 40.0f;
                switch (lootID) {
                    case 1: gestor.registrarConsumible(Spawner<Botiquin>::Spawn(posDrop)); break;
                    case 2: gestor.registrarConsumible(Spawner<Bateria>::Spawn(posDrop)); break;
                    case 3: gestor.registrarConsumible(Spawner<CajaDeMuniciones>::Spawn(posDrop)); break;
                    case 4: gestor.registrarConsumible(Spawner<Armadura>::Spawn(posDrop)); break;
                    case 99: gestor.registrarConsumible(Spawner<Llave>::Spawn(posDrop)); break;
                }
            }
            else if (Nota* nota = dynamic_cast<Nota*>(itemMasCercano)) {
                this->notaActualID = nota->usar(jugador);
                this->estadoActual = EstadoJuego::LEYENDO_NOTA;
            }
            else {
                texPopupItem = itemMasCercano->getTextura();
                nombrePopupItem = itemMasCercano->getNombre();
                descPopupItem = itemMasCercano->getDescripcion();
                escalaPopup = 0.0f;
                itemMasCercano->usar(jugador);
                if (itemMasCercano->estaConsumido()) estadoActual = EstadoJuego::ITEM_OBTENIDO;
            }
        }
    }

    MotorFisica::moverJugador(jugador, direccionMovimiento, miMapa.getMuros(), miMapa.getCajas(), miMapa.getPuertaJefe(), miMapa.estaPuertaAbierta(), gestor.getBalas());
    MotorFisica::moverEnemigos(gestor.getEnemigos(), miMapa.getMuros(), miMapa.getCajas(), miMapa.getPuertaJefe(), miMapa.estaPuertaAbierta());
    MotorFisica::moverJefes(gestor.getJefes(), miMapa.getMuros(), miMapa.getCajas(), miMapa.getPuertaJefe(), miMapa.estaPuertaAbierta());
    MotorFisica::moverBalas(gestor.getBalas(), miMapa.getMuros(), miMapa.getCajas(), miMapa.getPuertaJefe(), miMapa.estaPuertaAbierta());
    MotorFisica::resolverColisionesDinamicas(jugador, gestor.getEnemigos());

    int tipoDisparo = jugador.intentarDisparar(quiereDisparar);
    if (tipoDisparo > 0) {
        Vector2 dirDisparo = jugador.getDireccionVista();
        bool estaMoviendo = (Vector2LengthSqr(direccionMovimiento) > 0.0f);
        bool esUnCheat = (tipoDisparo == 2);
        if (estaMoviendo && !esUnCheat) {
            float anguloPreciso = atan2f(dirDisparo.y, dirDisparo.x);
            float desviacionGrados = (float)GetRandomValue(-200, 200) / 10.0f;
            dirDisparo = { cosf(anguloPreciso + desviacionGrados * DEG2RAD), sinf(anguloPreciso + desviacionGrados * DEG2RAD) };
        }
        gestor.registrarBala(new BalaDeRifle(jugador.getPosicion(), dirDisparo, esUnCheat));
    }

    MotorColisiones::procesar(jugador, gestor);
    gestor.recolectarBasura();
    temporizadorPartida += GetFrameTime();

    // Lógica Fantasma / Final
    if (jefeHaSpawned && !gestor.getJefes().empty() && gestor.getJefes()[0]->estaEnFaseFinal()) {
        Fantasma::jefeEnCombate = false; Fantasma::despertado = true; Fantasma::modoDialogo = true;
        estadoActual = EstadoJuego::DIALOGO_FINAL; temporizadorDialogo = 3.0f;
        return;
    }
    if (!Fantasma::despertado && !Fantasma::estaDespertando) {
        if (!Fantasma::estaAsustando) {
            temporizadorSustoFantasma += GetFrameTime();
            if (temporizadorSustoFantasma > proximoSustoFantasma) {
                Fantasma::estaAsustando = true; Fantasma::temporizadorSusto = 0.75f;
                Enemigo* f = nullptr; for (auto* e : gestor.getEnemigos()) if (dynamic_cast<Fantasma*>(e)) f = e;
                if (f) {
                    Camera2D cam = renderizador.getCamera();
                    float sl = cam.target.x - cam.offset.x / cam.zoom; float sr = cam.target.x + cam.offset.x / cam.zoom;
                    float st = cam.target.y - cam.offset.y / cam.zoom; float sb = cam.target.y + cam.offset.y / cam.zoom;
                    f->setPosicion({ (GetRandomValue(0,1)==0 ? sl-50 : sr+50), (float)GetRandomValue((int)st, (int)sb) });
                }
                ResetSustoFantasma();
            }
        }
        bool despertar = (jugador.getBateria() <= 0) || (temporizadorPartida > TIEMPO_LIMITE_FANTASMA && !jugador.getTieneLlave());
        if (despertar) {
            Fantasma::estaDespertando = true; Fantasma::temporizadorDespertar = 3.0f; Fantasma::estaAsustando = false;
            if (jugador.getBateria() <= 0) Fantasma::modoFuria = true;
        }
    }
}

void Juego::dibujarJugando() {
    renderizador.dibujarTodo(jugador, miMapa, gestor);
    if (timerDespertar > 0.0f) {
        float progress = timerDespertar / 4.0f;
        DrawRectangle(0, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, Fade(BLACK, progress));
        float alturaOjo = (Constantes::ALTO_PANTALLA / 2) * progress;
        DrawRectangle(0, 0, Constantes::ANCHO_PANTALLA, (int)alturaOjo, BLACK);
        DrawRectangle(0, Constantes::ALTO_PANTALLA - (int)alturaOjo, Constantes::ANCHO_PANTALLA, (int)alturaOjo, BLACK);
    }
}

// --- PAUSA ---
void Juego::actualizarPausa() {
    ShowCursor();
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) { estadoActual = EstadoJuego::JUGANDO; HideCursor(); return; }
    float btnW = 300.0f; float btnH = 80.0f; float spacing = 30.0f; float totalH = (btnH * 2) + spacing;
    float startY = ((float)Constantes::ALTO_PANTALLA - totalH) / 2.0f; float startX = ((float)Constantes::ANCHO_PANTALLA - btnW) / 2.0f;
    Rectangle rJugar = { startX, startY, btnW, btnH }; Rectangle rSalir = { startX, startY + btnH + spacing, btnW, btnH };

    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        opcionMenuPausa--;
        if (opcionMenuPausa < 0) opcionMenuPausa = 1;
        PlaySound(fxUIHover);
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        opcionMenuPausa++;
        if (opcionMenuPausa > 1) opcionMenuPausa = 0;
        PlaySound(fxUIHover);
    }

    Vector2 m = GetMousePosition();
    if (CheckCollisionPointRec(m, rJugar)) opcionMenuPausa = 0; else if (CheckCollisionPointRec(m, rSalir)) opcionMenuPausa = 1;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (opcionMenuPausa == 0 && (CheckCollisionPointRec(m, rJugar) || IsKeyPressed(KEY_ENTER))) {
            PlaySound(fxUIClick);
            estadoActual = EstadoJuego::JUGANDO; HideCursor();
        }
        if (opcionMenuPausa == 1 && (CheckCollisionPointRec(m, rSalir) || IsKeyPressed(KEY_ENTER))) {
            PlaySound(fxUIClick);
            estadoActual = EstadoJuego::MENU_INICIAL;
        }
    }
}

void Juego::dibujarPausa() {
    renderizador.dibujarTodo(jugador, miMapa, gestor);
    DrawRectangle(0, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, Fade(BLACK, 0.90f));
    const char* titulo = "PAUSA"; int wTitulo = MeasureText(titulo, 40); DibujarTextoGlitch(titulo, Constantes::ANCHO_PANTALLA/2 - wTitulo/2, 100, 40, WHITE);
    float btnW = 300.0f; float btnH = 80.0f; float spacing = 30.0f; float totalH = (btnH * 2) + spacing; float startY = ((float)Constantes::ALTO_PANTALLA - totalH) / 2.0f; float startX = ((float)Constantes::ANCHO_PANTALLA - btnW) / 2.0f;
    Texture2D tJugar = (opcionMenuPausa == 0) ? texBtnJugarSel : texBtnJugar; Texture2D tSalir = (opcionMenuPausa == 1) ? texBtnSalirInicioSel : texBtnSalirInicio;
    auto DibujarBtn = [&](Texture2D& tex, float x, float y, const char* textoFallback) {
        if (tex.id != 0) { Rectangle src = { 0.0f, 0.0f, (float)tex.width, (float)tex.height }; Rectangle dest = { x, y, btnW, btnH }; DrawTexturePro(tex, src, dest, {0,0}, 0.0f, WHITE); }
        else {
            Color c = GRAY; if (strcmp(textoFallback, "JUGAR")==0) c = (opcionMenuPausa==0) ? GREEN : DARKGREEN; if (strcmp(textoFallback, "SALIR")==0) c = (opcionMenuPausa==1) ? RED : MAROON;
            DrawRectangle((int)x, (int)y, (int)btnW, (int)btnH, c); DrawText(textoFallback, (int)x + 20, (int)y + 25, 20, BLACK);
        }
    };
    DibujarBtn(tJugar, startX, startY, "JUGAR"); DibujarBtn(tSalir, startX, startY + btnH + spacing, "SALIR");
}

// --- CINEMÁTICA INTRO (DESPERTAR) ---
void Juego::actualizarIntro() {
    float dt = GetFrameTime();
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) { reiniciarJuego(); return; }

    if (faseIntro == 0) { // Texto Boot
        timerEscrituraIntro += dt;
        if (timerEscrituraIntro >= 0.05f) { letrasIntroMostradas++; timerEscrituraIntro = 0.0f; }
        if (letrasIntroMostradas > 120) {
            timerIntro += dt;
            if (timerIntro > 2.0f) { faseIntro = 1; timerIntro = 0.0f; }
        }
    }
    else if (faseIntro == 1) { // Fade In Imagen
        timerIntro += dt;
        if (timerIntro > 3.0f) { faseIntro = 2; timerIntro = 0.0f; }
    }
    else if (faseIntro == 2) { // Hold
        timerIntro += dt;
        if (timerIntro > 2.0f) { faseIntro = 3; timerIntro = 0.0f; }
    }
    else if (faseIntro == 3) { // Fade Out
        timerIntro += dt;
        if (timerIntro > 1.0f) { reiniciarJuego(); }
    }
}

void Juego::dibujarIntro() {
    ClearBackground(BLACK);
    if (faseIntro == 0) {
        const char* bootText = "SISTEMA DE SOPORTE VITAL: ACTIVO\n\n> SUJETO: P-07 [EL REMANENTE]\n> CONSTANTES VITALES: INESTABLES\n> UBICACION: [DATOS CORRUPTOS]\n> MEMORIA: NO DISPONIBLE\n\nIniciando protocolo de despertar...";
        char buffer[512]; int len = strlen(bootText); int c = (letrasIntroMostradas > len) ? len : letrasIntroMostradas;
        strncpy(buffer, bootText, c); buffer[c] = '\0';
        DrawText(buffer, 50, 50, 20, GREEN);
    } else {
        if (texIntro.id != 0) {
            float alpha = 1.0f;
            if (faseIntro == 1) alpha = timerIntro / 3.0f;
            if (faseIntro == 3) alpha = 1.0f - (timerIntro / 1.0f);
            DrawTexturePro(texIntro, {0,0,(float)texIntro.width,(float)texIntro.height}, {0,0,(float)Constantes::ANCHO_PANTALLA,(float)Constantes::ALTO_PANTALLA}, {0,0}, 0.0f, Fade(WHITE, alpha));
            for (int y = 0; y < Constantes::ALTO_PANTALLA; y+=4) DrawLine(0, y, Constantes::ANCHO_PANTALLA, y, Fade(BLACK, 0.3f * alpha));
            if (faseIntro >= 1 && (int)(GetTime()*2)%2==0) DrawText("REC 00:00:01", 50, 50, 20, Fade(RED, alpha));
        }
    }
}

// --- CINEMÁTICA FINAL ---
void Juego::actualizarFinJuego() {
    float dt = GetFrameTime();
    if (faseFinal == 0) { alphaFinal += dt * 0.5f; if (alphaFinal >= 1.0f) { alphaFinal = 0.0f; faseFinal = 1; } }
    else if (faseFinal == 1) { alphaFinal += dt * 0.5f; if (alphaFinal >= 1.0f) { alphaFinal = 1.0f; faseFinal = 2; } }
    else if (faseFinal == 2) {
        timerEscritura += dt; if (timerEscritura >= 0.05f) { letrasMostradas++; timerEscritura = 0.0f; }
        if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_SPACE)) { letrasMostradas = 9999; faseFinal = 3; }
        if (letrasMostradas > 300) faseFinal = 3;
    }
    else if (faseFinal == 3) { if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { estadoActual = EstadoJuego::MENU_INICIAL; } }
}

void Juego::dibujarFinJuego() {
    if (faseFinal == 0) { renderizador.dibujarTodo(jugador, miMapa, gestor); DrawRectangle(0, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, Fade(BLACK, alphaFinal)); return; }

    ClearBackground(BLACK);
    Texture2D* texFinal = nullptr;
    const char* titulo = "";
    const char* desc = "";
    Color colorTitulo = WHITE;

    if (estadoActual == EstadoJuego::FIN_JUEGO_SACRIFICIO) {
        texFinal = &texFinalSacrificio;
        titulo = "EL NUEVO ARQUITECTO";
        desc = "Aceptas el trono. El Nexo se estabiliza, pero te conviertes\nen su nuevo corazon. Las almas estan atrapadas, pero en calma.\n\n...esperando al proximo Remanente.";
        colorTitulo = (Color){0, 255, 255, 255};
    }
    else {
        texFinal = &texFinalHuir;
        titulo = "EL CICLO SE REPITE";
        desc = "Escapas del Vortice. Detras de ti, el Nexo ruge y Elana grita.\nEl Cisma ha muerto, pero el trono esta vacio...\n\nY el Vortice odia el vacio.";
        colorTitulo = RED;
    }

    float alphaImg = (faseFinal == 1) ? alphaFinal : 1.0f;
    if (texFinal && texFinal->id != 0) {
        Rectangle src = {0, 0, (float)texFinal->width, (float)texFinal->height};
        Rectangle dest = {0, 0, (float)Constantes::ANCHO_PANTALLA, (float)Constantes::ALTO_PANTALLA};
        DrawTexturePro(*texFinal, src, dest, {0,0}, 0.0f, Fade(WHITE, alphaImg));
    } else {
        DrawRectangleGradientV(0, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, BLACK, Fade(colorTitulo, 0.2f * alphaImg));
    }

    // TEXTOS MEJORADOS
    if (faseFinal >= 2) {
        // TÍTULO
        int fontSizeTitulo = 60;
        int wTit = MeasureText(titulo, fontSizeTitulo);
        int xTit = Constantes::ANCHO_PANTALLA/2 - wTit/2;
        int yTit = 80;

        DibujarTextoConBorde(titulo, xTit, yTit, fontSizeTitulo, colorTitulo);

        // DESCRIPCIÓN
        char buffer[512];
        int len = strlen(desc);
        int count = (letrasMostradas > len) ? len : letrasMostradas;
        strncpy(buffer, desc, count);
        buffer[count] = '\0';

        int fontSizeDesc = 24;
        int xDesc = Constantes::ANCHO_PANTALLA/2 - 350;
        int yDesc = Constantes::ALTO_PANTALLA - 250;

        DrawRectangle(0, yDesc - 20, Constantes::ANCHO_PANTALLA, 150, Fade(BLACK, 0.6f));
        DibujarTextoConBorde(buffer, xDesc, yDesc, fontSizeDesc, WHITE);
    }

    if (faseFinal == 3) {
        const char* msg = "Presiona 'E' para volver al menu";
        float alphaPulse = 0.5f + sin(GetTime() * 3.0f) * 0.5f;
        int wMsg = MeasureText(msg, 20);
        int xMsg = Constantes::ANCHO_PANTALLA/2 - wMsg/2;
        int yMsg = Constantes::ALTO_PANTALLA - 50;

        DibujarTextoConBorde(msg, xMsg, yMsg, 20, Fade(GRAY, alphaPulse));
    }
}

// --- NOTAS ---
void Juego::actualizarLeyendoNota() { jugador.actualizarInterno(renderizador.getCamera(), {0, 0}); if (SistemaInput::quiereInteractuar() || IsKeyPressed(KEY_ESCAPE)) { estadoActual = EstadoJuego::JUGANDO; notaActualID = 0; } }
void Juego::dibujarLeyendoNota() {
    renderizador.dibujarTodo(jugador, miMapa, gestor); DrawRectangle(0, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, Fade(BLACK, 0.8f));
    Rectangle frame = { Constantes::ANCHO_PANTALLA * 0.1f, Constantes::ALTO_PANTALLA * 0.15f, Constantes::ANCHO_PANTALLA * 0.8f, Constantes::ALTO_PANTALLA * 0.7f }; DrawRectangleRec(frame, Fade((Color){10, 20, 30, 255}, 0.80f)); DrawRectangleLinesEx(frame, 2.0f, Fade((Color){0, 255, 128, 100}, 0.5f));
    for (int y = (int)frame.y; y < (int)(frame.y + frame.height); y += 3) DrawLine(frame.x, y, frame.x + frame.width, y, Fade(BLACK, 0.5f));
    const char* textoTitulo = "REGISTRO"; const char* textoNota = "...";
    switch(notaActualID) {
        case 0: textoTitulo = "...Estas despierto?"; textoNota = "No hay tiempo. La Fusion es inminente. El Arquitecto...\nEl Cisma... esta en el centro. Debes llegar a el.\n\nUsa la linterna, busca la Llave... y por favor, ten cuidado\ncon mis... ecos. No dejes que la bateria se agote.\n\n- E."; break;
        case 1: textoTitulo = "REG-001: EL NEXO"; textoNota = "Dia 1. Lo llamamos 'El Nexo'. No es un lugar, es un estado.\nUn Vortice de psique pura. El Arquitecto cree que podemos\nestabilizarlo... pero la materia fisica que introducimos...\nse esta corrompiendo. Los sujetos de prueba (Z-00)\nson... maleables."; break;
        case 2: textoTitulo = "REG-014: EL ETER"; textoNota = "El Eter es mas que energia. Tiene... voluntad.\nLas almas de los que murieron en el primer test de Fusion\nsiguen aqui. Podemos oirlas. Elana (Dra. Reyes) insiste\nen que estan sufriendo. El Arquitecto la ha apartado\ndel proyecto."; break;
        case 3: textoTitulo = "DIARIO DEL ARQUITECTO"; textoNota = "Funciona! Puedo sentirlo! El Desdoblamiento es la clave!\nUnir la materia y el espiritu! Seremos dioses!\n\nElana no lo entiende. Llama 'corrupcion' a lo que yo\nllamo 'creacion'. Ella es... un estorbo."; break;
        case 4: textoTitulo = "ALERTA DE SEGURIDAD"; textoNota = "...brecha de contencion. El Sujeto Z-08 (Apodo: 'Obeso')\nha roto el sello. Ha... absorbido a dos tecnicos.\nLa carne se pliega sobre si misma. Las balas estandar\nno son suficientes. Solicitando municion perforante."; break;
        case 5: textoTitulo = "MI INVESTIGACION"; textoNota = "El esta ciego. El Vortice no se puede controlar,\nsolo se puede... cerrar. La Fusion no es ascension,\nes un cancer. He creado una 'Llave' de frecuencia.\nUna anulacion. Si el Cisma se estabiliza, todo estara\nperdido. Debo esconderla..."; break;
        case 6: textoTitulo = "TRANSMISION CORRUPTA"; textoNota = "...no puedo... no puedo ver... la carne... se retuerce...\nhuesos... cables... donde... donde esta mi cara?\n\nELANA! POR QUE ME HAS ENCERRADO AQUI?!\nPOR QUE?! m... mi... rostro..."; break;
        case 7: textoTitulo = "REPORTE DE ENERGIA"; textoNota = "La bateria principal es inestable. Sigue atrayendo...\ncosas... del Eter. Fantasmas. Los llamamos 'Ecos'.\nApagar la luz ayuda, pero cuando la bateria se agota...\nla oscuridad los enfurece. Se vuelven... rapidos."; break;
        case 8: textoTitulo = "REG-045: EL REMANENTE"; textoNota = "Sujeto P-07 (El Remanente). Es el unico... hibrido estable.\nPuede transitar ambos planos. Por que el? Es su psique?\nEl Arquitecto esta... celoso. Dice que P-07 es la\n'Mascara' perfecta que necesita."; break;
        case 9: textoTitulo = "CUARENTENA: CAMARA 2"; textoNota = "La Camara del Canto esta perdida. Los Ecos... han matado\na todos. No podemos danarlos. No... fisicamente.\nEl Arquitecto se rie. Dice que son 'energia pura'.\nSon... almas puras. Gritando."; break;
        case 10: textoTitulo = "---"; textoNota = "...solo oxido y mascaras...\n...El Vortice reclama...\n...El Cisma debe...\n...Elana... perdoname..."; break;
        default: textoTitulo = "ERROR DE DATOS"; textoNota = "El archivo esta corrupto. No se puede leer el registro."; break;
    }
    DibujarTextoGlitch(textoTitulo, (int)(frame.x + 20), (int)(frame.y + 20), 20, (Color){0, 255, 128, 255});
    DrawLine((int)(frame.x + 20), (int)(frame.y + 45), (int)(frame.x + frame.width - 20), (int)(frame.y + 45), Fade(GREEN, 0.2f));
    DibujarTextoGlitch(textoNota, (int)(frame.x + 20), (int)(frame.y + 70), 20, (Color){200, 255, 220, 255});
    const char* textoCerrar = "Presiona 'E' para cerrar"; float alphaCerrar = (sin(GetTime() * 2.0f) + 1.0f) / 2.0f;
    DrawText(textoCerrar, (int)(frame.x + frame.width - MeasureText(textoCerrar, 20) - 20), (int)(frame.y + frame.height - 40), 20, Fade(GREEN, 0.5f + alphaCerrar * 0.5f));
}

// --- JEFE ---
void Juego::actualizarIniciandoJefe() { jugador.actualizarInterno(renderizador.getCamera(), {0, 0}); temporizadorSpawnJefe -= GetFrameTime(); if (temporizadorSpawnJefe <= 0) { gestor.registrarJefe(new Jefe({0.0f, 0.0f})); jefeHaSpawned = true; estadoActual = EstadoJuego::JUGANDO; } }
void Juego::dibujarIniciandoJefe() { dibujarJugando(); const char* textoSpawn = "LA PUERTA SE HA SELLADO.\n\nSIENTES UNA DISTORSION EN EL CENTRO DE LA SALA..."; float alpha = 1.0f; if (temporizadorSpawnJefe < 1.0f) alpha = temporizadorSpawnJefe; DibujarTextoGlitch(textoSpawn, Constantes::ANCHO_PANTALLA / 2 - MeasureText(textoSpawn, 20) / 2, Constantes::ALTO_PANTALLA - 150, 20, Fade(RED, alpha)); }

// --- DIÁLOGOS ---
void Juego::actualizarDialogo() { jugador.actualizarInterno(renderizador.getCamera(), {0, 0}); gestor.actualizarIAEntidades(jugador, miMapa); temporizadorDialogo -= GetFrameTime(); if (temporizadorDialogo <= 0) { estadoActual = EstadoJuego::DIALOGO_INTRO; opcionDialogo = 1; for (Enemigo* enemigo : gestor.getEnemigos()) if (dynamic_cast<Fantasma*>(enemigo)) { enemigo->setPosicion(Vector2Add(jugador.getPosicion(), Vector2Scale(jugador.getDireccionVista(), 80.0f))); break; } } }
void Juego::dibujarDialogo() { renderizador.dibujarTodo(jugador, miMapa, gestor); DrawRectangle(0, Constantes::ALTO_PANTALLA - 100, Constantes::ANCHO_PANTALLA, 100, Fade(BLACK, 0.8f)); DrawText("El 'Fantasma' se acerca... su movimiento es erratico pero decidido...", 20, Constantes::ALTO_PANTALLA - 80, 20, WHITE); DrawText("Sientes una presencia abrumadora... no es hostil, es... triste.", 20, Constantes::ALTO_PANTALLA - 50, 20, WHITE); }
void Juego::actualizarDialogoIntro() { jugador.actualizarInterno(renderizador.getCamera(), {0, 0}); if (SistemaInput::quiereInteractuar()) { estadoActual = EstadoJuego::DIALOGO_PREGUNTAS; opcionDialogo = 1; } }
void Juego::dibujarDialogoIntro() { renderizador.dibujarTodo(jugador, miMapa, gestor); DibujarCajaDialogo(); Rectangle frame = { Constantes::ANCHO_PANTALLA * 0.1f, Constantes::ALTO_PANTALLA * 0.25f, Constantes::ANCHO_PANTALLA * 0.8f, Constantes::ALTO_PANTALLA * 0.5f }; DibujarTextoGlitch("ELANA: Remanente... P-07... estas... vivo.", (int)(frame.x + 20), (int)(frame.y + 30), 20, (Color){200, 200, 255, 255}); DibujarTextoGlitch("Lo... lo detuviste. El Cisma... El Arquitecto...", (int)(frame.x + 20), (int)(frame.y + 60), 20, (Color){200, 200, 255, 255}); DibujarTextoGlitch("Por fin ha callado.", (int)(frame.x + 20), (int)(frame.y + 90), 20, (Color){200, 200, 255, 255}); const char* textoCerrar = "Presiona 'E' para continuar..."; float alphaCerrar = (sin(GetTime() * 2.0f) + 1.0f) / 2.0f; DrawText(textoCerrar, (int)(frame.x + frame.width - MeasureText(textoCerrar, 20) - 20), (int)(frame.y + frame.height - 40), 20, Fade(PURPLE, 0.5f + alphaCerrar * 0.5f)); }
void Juego::actualizarDialogoPreguntas() { jugador.actualizarInterno(renderizador.getCamera(), {0, 0}); if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) { opcionDialogo--; if (opcionDialogo < 1) opcionDialogo = 3; } if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) { opcionDialogo++; if (opcionDialogo > 3) opcionDialogo = 1; } if (SistemaInput::quiereInteractuar()) { if (opcionDialogo == 1) estadoActual = EstadoJuego::DIALOGO_RESPUESTA_1; else if (opcionDialogo == 2) estadoActual = EstadoJuego::DIALOGO_RESPUESTA_2; else { estadoActual = EstadoJuego::DIALOGO_DECISION_FINAL; opcionDialogo = 1; } } }
void Juego::dibujarDialogoPreguntas() { renderizador.dibujarTodo(jugador, miMapa, gestor); DibujarCajaDialogo(); Rectangle frame = { Constantes::ANCHO_PANTALLA * 0.1f, Constantes::ALTO_PANTALLA * 0.25f, Constantes::ANCHO_PANTALLA * 0.8f, Constantes::ALTO_PANTALLA * 0.5f }; DibujarTextoGlitch("ELANA: ... (Espera tu respuesta)", (int)(frame.x + 20), (int)(frame.y + 30), 20, (Color){200, 200, 255, 255}); DrawLine((int)(frame.x + 20), (int)(frame.y + 70), (int)(frame.x + frame.width - 20), (int)(frame.y + 70), Fade(PURPLE, 0.2f)); Color color1 = (opcionDialogo == 1) ? YELLOW : GRAY; Color color2 = (opcionDialogo == 2) ? YELLOW : GRAY; Color color3 = (opcionDialogo == 3) ? YELLOW : GRAY; DibujarTextoGlitch("[1] Quien eres tu?", (int)(frame.x + 40), (int)(frame.y + 100), 20, color1); DibujarTextoGlitch("[2] Que es este lugar?", (int)(frame.x + 40), (int)(frame.y + 130), 20, color2); DibujarTextoGlitch("[3] ... (Quedarse callado)", (int)(frame.x + 40), (int)(frame.y + 160), 20, color3); if(opcionDialogo == 1) DibujarTextoGlitch(">", (int)(frame.x + 20), (int)(frame.y + 100), 20, YELLOW); else if(opcionDialogo == 2) DibujarTextoGlitch(">", (int)(frame.x + 20), (int)(frame.y + 130), 20, YELLOW); else DibujarTextoGlitch(">", (int)(frame.x + 20), (int)(frame.y + 160), 20, YELLOW); }
void Juego::actualizarDialogoRespuesta() { jugador.actualizarInterno(renderizador.getCamera(), {0, 0}); if (SistemaInput::quiereInteractuar()) estadoActual = EstadoJuego::DIALOGO_PREGUNTAS; }
void Juego::dibujarDialogoRespuesta() { renderizador.dibujarTodo(jugador, miMapa, gestor); DibujarCajaDialogo(); Rectangle frame = { Constantes::ANCHO_PANTALLA * 0.1f, Constantes::ALTO_PANTALLA * 0.25f, Constantes::ANCHO_PANTALLA * 0.8f, Constantes::ALTO_PANTALLA * 0.5f }; const char* t1 = "..."; if (estadoActual == EstadoJuego::DIALOGO_RESPUESTA_1) t1 = "ELANA: Soy... era... la Dra. Elana Reyes. Yo... intente\ndetenerlo. Cree la Llave para anular la fusion, pero\nel me encontro... y me convirtio en esto. Un Eco."; else if (estadoActual == EstadoJuego::DIALOGO_RESPUESTA_2) t1 = "ELANA: Es el Nexo. El laboratorio del Arquitecto.\nUn Vortice donde intento forzar la fusion entre la materia\ny el eter. Es... un cancer en la realidad."; DibujarTextoGlitch(t1, (int)(frame.x + 20), (int)(frame.y + 30), 20, (Color){200, 200, 255, 255}); const char* textoCerrar = "Presiona 'E' para continuar..."; float alphaCerrar = (sin(GetTime() * 2.0f) + 1.0f) / 2.0f; DrawText(textoCerrar, (int)(frame.x + frame.width - MeasureText(textoCerrar, 20) - 20), (int)(frame.y + frame.height - 40), 20, Fade(PURPLE, 0.5f + alphaCerrar * 0.5f)); }
void Juego::actualizarDialogoDecisionFinal() { jugador.actualizarInterno(renderizador.getCamera(), {0, 0}); if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_UP)) opcionDialogo = 1; if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_DOWN)) opcionDialogo = 2; if (SistemaInput::quiereInteractuar()) { if (opcionDialogo == 1) estadoActual = EstadoJuego::FIN_JUEGO_SACRIFICIO; else estadoActual = EstadoJuego::FIN_JUEGO_HUIR; faseFinal = 0; alphaFinal = 0.0f; } }
void Juego::dibujarDialogoDecisionFinal() { renderizador.dibujarTodo(jugador, miMapa, gestor); DibujarCajaDialogo(); Rectangle frame = { Constantes::ANCHO_PANTALLA * 0.1f, Constantes::ALTO_PANTALLA * 0.25f, Constantes::ANCHO_PANTALLA * 0.8f, Constantes::ALTO_PANTALLA * 0.5f }; DibujarTextoGlitch("ELANA: El Cisma esta muerto, pero el Vortice sigue aqui.\nInestable. Remanente, tu eres el unico hibrido estable.\nPuedes tomar su lugar... o puedes huir.\n\nSi te vas, esto volvera a empezar. El Nexo... siempre reclama.", (int)(frame.x + 20), (int)(frame.y + 30), 20, (Color){200, 200, 255, 255}); DrawLine((int)(frame.x + 20), (int)(frame.y + 150), (int)(frame.x + frame.width - 20), (int)(frame.y + 150), Fade(PURPLE, 0.2f)); Color color1 = (opcionDialogo == 1) ? YELLOW : GRAY; Color color2 = (opcionDialogo == 2) ? YELLOW : GRAY; DibujarTextoGlitch("[1] Me quedare. Tomare el lugar del Cisma.", (int)(frame.x + 40), (int)(frame.y + 180), 20, color1); DibujarTextoGlitch("[2] No es mi problema. Me voy de aqui.", (int)(frame.x + 40), (int)(frame.y + 210), 20, color2); if(opcionDialogo == 1) DibujarTextoGlitch(">", (int)(frame.x + 20), (int)(frame.y + 180), 20, YELLOW); else DibujarTextoGlitch(">", (int)(frame.x + 20), (int)(frame.y + 210), 20, YELLOW); const char* textoCerrar = "Presiona 'E' para decidir"; float alphaCerrar = (sin(GetTime() * 2.0f) + 1.0f) / 2.0f; DrawText(textoCerrar, (int)(frame.x + frame.width - MeasureText(textoCerrar, 20) - 20), (int)(frame.y + frame.height - 40), 20, Fade(PURPLE, 0.5f + alphaCerrar * 0.5f)); }

// --- ITEM POPUP ---
void Juego::actualizarItemObtenido() { if (escalaPopup < 1.0f) escalaPopup += GetFrameTime() * 3.0f; else escalaPopup = 1.0f; if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) estadoActual = EstadoJuego::JUGANDO; }
void Juego::dibujarItemObtenido() { renderizador.dibujarTodo(jugador, miMapa, gestor); DrawRectangle(0, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, Fade(BLACK, 0.85f)); DrawLine(0, Constantes::ALTO_PANTALLA/2, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA/2, Fade(GREEN, 0.3f)); if (texPopupItem.id != 0) { float tamanoMaximo = 250.0f; float aspect = (float)texPopupItem.width / (float)texPopupItem.height; float finalW, finalH; if (aspect >= 1.0f) { finalW = tamanoMaximo * escalaPopup; finalH = (tamanoMaximo / aspect) * escalaPopup; } else { finalH = tamanoMaximo * escalaPopup; finalW = (tamanoMaximo * aspect) * escalaPopup; } float rotacion = sin(GetTime() * 1.5f) * 10.0f; Rectangle source = { 0, 0, (float)texPopupItem.width, (float)texPopupItem.height }; Rectangle dest = { (float)Constantes::ANCHO_PANTALLA/2, (float)Constantes::ALTO_PANTALLA/2 - 80, finalW, finalH }; Vector2 origin = { dest.width/2, dest.height/2 }; DrawTexturePro(texPopupItem, source, dest, origin, rotacion, WHITE); DrawCircleGradient((int)dest.x, (int)dest.y, 100 * escalaPopup, Fade(GREEN, 0.2f), Fade(BLACK, 0.0f)); } else { DrawRectangle(Constantes::ANCHO_PANTALLA/2 - 50, Constantes::ALTO_PANTALLA/2 - 130, 100, 100, PURPLE); DrawText("NO IMAGE", Constantes::ANCHO_PANTALLA/2 - 40, Constantes::ALTO_PANTALLA/2 - 90, 20, WHITE); } const char* titulo = "OBJETO ENCONTRADO"; int wTitulo = MeasureText(titulo, 30); DrawText(titulo, Constantes::ANCHO_PANTALLA/2 - wTitulo/2, Constantes::ALTO_PANTALLA/2 + 60, 30, Fade(GREEN, escalaPopup)); if (nombrePopupItem) { int wNombre = MeasureText(nombrePopupItem, 40); DibujarTextoGlitch(nombrePopupItem, Constantes::ANCHO_PANTALLA/2 - wNombre/2, Constantes::ALTO_PANTALLA/2 + 100, 40, WHITE); } if (descPopupItem) { int wDesc = MeasureText(descPopupItem, 20); DrawText(descPopupItem, Constantes::ANCHO_PANTALLA/2 - wDesc/2, Constantes::ALTO_PANTALLA/2 + 150, 20, GRAY); } if (escalaPopup >= 1.0f) { const char* msg = "Continuar [E]"; DrawText(msg, Constantes::ANCHO_PANTALLA - MeasureText(msg, 20) - 30, Constantes::ALTO_PANTALLA - 40, 20, Fade(WHITE, 0.5f + sin(GetTime()*5)*0.5f)); } }

// --- MENU INICIO ---
void Juego::actualizarMenuInicial() {
    ShowCursor();

    float btnW = 300.0f;
    float btnH = 80.0f;
    float spacing = 25.0f;
    float startY = (float)Constantes::ALTO_PANTALLA * 0.5f;
    float centerX = ((float)Constantes::ANCHO_PANTALLA - btnW) / 2.0f;

    Rectangle rectJugar = { centerX, startY, btnW, btnH };
    Rectangle rectCreditos = { centerX, startY + btnH + spacing, btnW, btnH };
    Rectangle rectSalir = { centerX, startY + (btnH + spacing) * 2, btnW, btnH };

    int opcionAnterior = opcionMenuInicial;

    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        opcionMenuInicial--;
        if (opcionMenuInicial < 0) opcionMenuInicial = 2;
        PlaySound(fxUIHover);
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        opcionMenuInicial++;
        if (opcionMenuInicial > 2) opcionMenuInicial = 0;
        PlaySound(fxUIHover);
    }

    Vector2 mouse = GetMousePosition();
    bool mouseSobreAlgo = false;

    if (CheckCollisionPointRec(mouse, rectJugar)) {
        opcionMenuInicial = 0;
        mouseSobreAlgo = true;
    }
    else if (CheckCollisionPointRec(mouse, rectCreditos)) {
        opcionMenuInicial = 1;
        mouseSobreAlgo = true;
    }
    else if (CheckCollisionPointRec(mouse, rectSalir)) {
        opcionMenuInicial = 2;
        mouseSobreAlgo = true;
    }

    if (mouseSobreAlgo && opcionMenuInicial != opcionAnterior) {
        PlaySound(fxUIHover);
    }

    bool clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    bool enter = (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_E));
    bool clickValido = false;

    if (clicked && mouseSobreAlgo) clickValido = true;

    if (enter || clickValido) {
        PlaySound(fxUIClick);

        if (opcionMenuInicial == 0) {
            estadoActual = EstadoJuego::INTRO_CINEMATICA;
            faseIntro = 0;
            timerIntro = 0.0f;
            letrasIntroMostradas = 0;
            timerEscrituraIntro = 0.0f;
            // Iniciar música ambiente al entrar al juego
            PlayMusicStream(musicaAmbiente);
        }
        else if (opcionMenuInicial == 1) estadoActual = EstadoJuego::CREDITOS;
        else if (opcionMenuInicial == 2) CloseWindow();
    }
}
// ... (Resto de métodos dibujarMenuInicial, dibujarCreditos, etc. sin cambios) ...
void Juego::dibujarMenuInicial() { if (texFondoMenuInicio.id != 0) { Rectangle src = { 0, 0, (float)texFondoMenuInicio.width, (float)texFondoMenuInicio.height }; Rectangle dest = { 0, 0, (float)Constantes::ANCHO_PANTALLA, (float)Constantes::ALTO_PANTALLA }; DrawTexturePro(texFondoMenuInicio, src, dest, {0,0}, 0.0f, WHITE); } else { ClearBackground(BLACK); DibujarTextoGlitch("ERROR: FONDO NO ENCONTRADO", 50, 50, 20, RED); } float btnW = 300.0f; float btnH = 80.0f; float spacing = 25.0f; float startY = (float)Constantes::ALTO_PANTALLA * 0.5f; float centerX = ((float)Constantes::ANCHO_PANTALLA - btnW) / 2.0f; Color tinteBoton = { 180, 180, 180, 255 }; auto DibujarBotonSeguro = [&](Texture2D& tex, int yOffset, bool seleccionado, const char* textoError) { float posY = startY + yOffset * (btnH + spacing); if (tex.id != 0) { Rectangle source = { 0.0f, 0.0f, (float)tex.width, (float)tex.height }; Rectangle dest = { centerX, posY, btnW, btnH }; Color colorFinal = seleccionado ? WHITE : tinteBoton; DrawTexturePro(tex, source, dest, {0,0}, 0.0f, colorFinal); } else { Color color = seleccionado ? MAGENTA : DARKGRAY; DrawRectangle((int)centerX, (int)posY, (int)btnW, (int)btnH, Fade(color, 0.7f)); DrawText(textoError, (int)centerX + 10, (int)posY + 20, 20, WHITE); } }; DibujarBotonSeguro((opcionMenuInicial == 0) ? texBtnJugarSel : texBtnJugar, 0, (opcionMenuInicial == 0), "ERR: Jugar"); DibujarBotonSeguro((opcionMenuInicial == 1) ? texBtnCreditosSel : texBtnCreditos, 1, (opcionMenuInicial == 1), "ERR: Creditos"); DibujarBotonSeguro((opcionMenuInicial == 2) ? texBtnSalirInicioSel : texBtnSalirInicio, 2, (opcionMenuInicial == 2), "ERR: Salir"); }
void Juego::actualizarCreditos() { if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_E) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) estadoActual = EstadoJuego::MENU_INICIAL; }
void Juego::dibujarCreditos() { if (texFondoCreditos.id != 0) { Rectangle src = { 0, 0, (float)texFondoCreditos.width, (float)texFondoCreditos.height }; Rectangle dest = { 0, 0, (float)Constantes::ANCHO_PANTALLA, (float)Constantes::ALTO_PANTALLA }; DrawTexturePro(texFondoCreditos, src, dest, {0,0}, 0.0f, WHITE); } else { ClearBackground(BLACK); DibujarTextoGlitch("CREDITOS", 50, 50, 40, PURPLE); } int startY = 220; int stepY = 35; DrawText("Desarrollo:", 50, startY, 20, WHITE); DrawText("Mailen Acosta Vera", 50, startY + stepY, 20, GRAY); DrawText("Emiliano Volpino", 50, startY + stepY * 2, 20, GRAY); startY += 130; DrawText("Arte:", 50, startY, 20, WHITE); DrawText("Generado por IA / Assets Propios", 50, startY + stepY, 20, GRAY); startY += 100; DrawText("Motor:", 50, startY, 20, WHITE); DrawText("C++ & Raylib", 50, startY + stepY, 20, GRAY); startY += 100; DrawText("Agradecimientos Especiales:", 50, startY, 20, WHITE); DrawText("A Ignacio Rodriguez y Santi", 50, startY + stepY, 20, GRAY); const char* msg = "Volver [E]"; DrawText(msg, Constantes::ANCHO_PANTALLA - MeasureText(msg, 20) - 40, Constantes::ALTO_PANTALLA - 60, 20, WHITE); }
void Juego::actualizarMenuMuerte() { if (temporizadorDialogo < 2.0f) temporizadorDialogo += GetFrameTime(); float btnY = (float)Constantes::ALTO_PANTALLA - 150.0f; float spacing = 30.0f; float btnW = 240.0f; float btnH = 80.0f; float totalW = (btnW * 3) + (spacing * 2); float startX = ((float)Constantes::ANCHO_PANTALLA - totalW) / 2.0f; Rectangle rectReintentar = { startX, btnY, btnW, btnH }; Rectangle rectMenu = { startX + btnW + spacing, btnY, btnW, btnH }; Rectangle rectSalir = { startX + (btnW + spacing) * 2, btnY, btnW, btnH }; if (temporizadorDialogo > 0.1f) { if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) { opcionMenuMuerteSeleccionada--; if (opcionMenuMuerteSeleccionada < 0) opcionMenuMuerteSeleccionada = 2; } if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) { opcionMenuMuerteSeleccionada++; if (opcionMenuMuerteSeleccionada > 2) opcionMenuMuerteSeleccionada = 0; } Vector2 mouse = GetMousePosition(); if (CheckCollisionPointRec(mouse, rectReintentar)) opcionMenuMuerteSeleccionada = 0; else if (CheckCollisionPointRec(mouse, rectMenu)) opcionMenuMuerteSeleccionada = 1; else if (CheckCollisionPointRec(mouse, rectSalir)) opcionMenuMuerteSeleccionada = 2; bool clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON); bool enter = (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_E) || IsKeyPressed(KEY_SPACE)); bool clickValido = false; if (clicked) { if (opcionMenuMuerteSeleccionada == 0 && CheckCollisionPointRec(mouse, rectReintentar)) clickValido = true; else if (opcionMenuMuerteSeleccionada == 1 && CheckCollisionPointRec(mouse, rectMenu)) clickValido = true; else if (opcionMenuMuerteSeleccionada == 2 && CheckCollisionPointRec(mouse, rectSalir)) clickValido = true; } if (enter || clickValido) { if (opcionMenuMuerteSeleccionada == 0) reiniciarJuego(); else if (opcionMenuMuerteSeleccionada == 1) estadoActual = EstadoJuego::MENU_INICIAL; else if (opcionMenuMuerteSeleccionada == 2) estadoActual = EstadoJuego::MENU_INICIAL; } } }
void Juego::dibujarMenuMuerte(float alpha) { if (texFondoMuerte.id != 0) { Rectangle src = { 0, 0, (float)texFondoMuerte.width, (float)texFondoMuerte.height }; Rectangle dest = { 0, 0, (float)Constantes::ANCHO_PANTALLA, (float)Constantes::ALTO_PANTALLA }; DrawTexturePro(texFondoMuerte, src, dest, {0,0}, 0.0f, Fade(WHITE, alpha)); } else { ClearBackground(BLACK); DibujarTextoGlitch("HAS MUERTO", 200, 200, 60, RED); } float btnY = (float)Constantes::ALTO_PANTALLA - 150.0f; float spacing = 30.0f; float btnW = 240.0f; float btnH = 80.0f; float totalW = (btnW * 3) + (spacing * 2); float startX = ((float)Constantes::ANCHO_PANTALLA - totalW) / 2.0f; auto DibujarBtnMuerte = [&](Texture2D& tex, float x, float y) { if (tex.id != 0) { Rectangle source = { 0.0f, 0.0f, (float)tex.width, (float)tex.height }; Rectangle dest = { x, y, btnW, btnH }; DrawTexturePro(tex, source, dest, {0,0}, 0.0f, Fade(WHITE, alpha)); } else { DrawRectangle((int)x, (int)y, (int)btnW, (int)btnH, Fade(RED, alpha)); DrawText("ERROR IMG", (int)x+10, (int)y+20, 20, WHITE); } }; Texture2D tReintentar = (opcionMenuMuerteSeleccionada == 0) ? texBtnReintentarSel : texBtnReintentar; DibujarBtnMuerte(tReintentar, startX, btnY); Texture2D tMenu = (opcionMenuMuerteSeleccionada == 1) ? texBtnMenuSel : texBtnMenu; DibujarBtnMuerte(tMenu, startX + btnW + spacing, btnY); Texture2D tSalir = (opcionMenuMuerteSeleccionada == 2) ? texBtnSalirMuerteSel : texBtnSalirMuerte; DibujarBtnMuerte(tSalir, startX + (btnW + spacing) * 2, btnY); }

void Juego::procesarCheats() { if (!IsKeyDown(KEY_LEFT_SHIFT)) return; if (IsKeyPressed(KEY_H)) jugador.curarVida(100); if (IsKeyPressed(KEY_A)) jugador.recibirArmadura(); if (IsKeyPressed(KEY_M)) jugador.recargarMunicion(50); if (IsKeyPressed(KEY_B)) jugador.recargarBateria(100); if (IsKeyPressed(KEY_K)) jugador.recibirLlave(); if (IsKeyPressed(KEY_J)) jugador.activarCheatDisparo(); if (IsKeyPressed(KEY_L)) { if (jefeHaSpawned && !gestor.getJefes().empty()) gestor.getJefes()[0]->forzarMuerte(); } }
