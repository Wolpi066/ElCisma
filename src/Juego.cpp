#include "Juego.h"
#include "Constantes.h"
#include "raymath.h"
#include <stdexcept>
#include "Fantasma.h"
#include "IndicadorPuerta.h"
#include "Llave.h"
#include "Cofre.h"
#include "Nota.h"

// Items que el Cofre puede soltar
#include "Botiquin.h"
#include "Bateria.h"
#include "CajaDeMuniciones.h"
#include "Armadura.h"

// Spawners
#include "BalaDeRifle.h" // ¡¡NECESARIO!!
#include "BalaMonstruosa.h"
#include "Spawner.h"
#include "Jefe.h"

// --- Helper para el texto glitch ---
void DibujarTextoGlitch(const char* texto, int posX, int posY, int fontSize, Color colorPrincipal)
{
    float offset = sin(GetTime() * 15.0f) * 1.0f;
    float offset2 = cos(GetTime() * 10.0f) * 1.0f;
    DrawText(texto, posX + (int)offset, posY + (int)offset2, fontSize, Fade((Color){255, 0, 100, 255}, 0.2f));
    DrawText(texto, posX - (int)offset, posY - (int)offset2, fontSize, Fade((Color){0, 255, 200, 255}, 0.2f));
    DrawText(texto, posX, posY, fontSize, colorPrincipal);
}
// ------------------------------------------


// --- CONSTRUCTOR ---
Juego::Juego()
    : jugador({0, 500}),
      miMapa(),
      gestor(),
      renderizador(),
      estadoActual(EstadoJuego::JUGANDO),
      temporizadorPartida(0.0f),
      temporizadorSustoFantasma(0.0f),
      notaActualID(0),
      jefeHaSpawned(false),
      triggerRectJefe({ -200, -200, 400, 400 }),
      temporizadorSpawnJefe(0.0f)
{
    miMapa.poblarMundo(gestor);
    ResetSustoFantasma();

    renderizador.inicializarMinimapa(miMapa);
}

// --- DESTRUCTOR ---
Juego::~Juego()
{
    gestor.limpiarTodo();
}

// --- BUCLE PRINCIPAL (Publico) ---
void Juego::run()
{
    while (!WindowShouldClose()) {
        actualizar();
        dibujar();
    }
}

// --- Helper para Susto ---
void Juego::ResetSustoFantasma() {
    temporizadorSustoFantasma = 0.0f;
    proximoSustoFantasma = (float)GetRandomValue(15, 30);
}

// --- ACTUALIZAR (Principal) ---
void Juego::actualizar()
{
    switch (estadoActual)
    {
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
        case EstadoJuego::FIN_JUEGO_GANADO:
        case EstadoJuego::FIN_JUEGO_MUERTO:
            break;
    }
}

// --- DIBUJAR (Principal) ---
void Juego::dibujar()
{
    BeginDrawing();
        ClearBackground(BLACK);

        switch (estadoActual)
        {
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
            case EstadoJuego::FIN_JUEGO_GANADO:
            case EstadoJuego::FIN_JUEGO_MUERTO:
                dibujarFinJuego();
                break;
        }

    EndDrawing();
}


// --- METODOS DE LOGICA PRIVADOS ---

void Juego::actualizarJugando()
{
    // --- ¡¡NUEVO!! PROCESAR CHEATS ---
    procesarCheats();
    // ---------------------------------

    if (!jugador.estaVivo()) {
        estadoActual = EstadoJuego::FIN_JUEGO_MUERTO;
        return;
    }

    // --- ¡¡NUEVA LÓGICA!! Disparamos la secuencia del jefe ---
    if (!jefeHaSpawned && miMapa.estaPuertaAbierta())
    {
        if (CheckCollisionRecs(jugador.getRect(), triggerRectJefe))
        {
            estadoActual = EstadoJuego::INICIANDO_JEFE;
            temporizadorSpawnJefe = 3.0f; // 3 segundos de animación
            miMapa.cerrarPuerta(); // ¡Encerrado!
            jugador.quitarLlave(); // ¡Sin salida!

            // --- ¡¡CONGELAR BATERÍA!! ---
            jugador.setBateriaCongelada(true);
            // -----------------------------

            return; // Salimos de actualizarJugando()
        }
    }
    // -----------------------------------------------------

    Vector2 direccionMovimiento = SistemaInput::getDireccionMovimiento();
    bool quiereDisparar = SistemaInput::quiereDisparar();
    bool quiereInteractuar = SistemaInput::quiereInteractuar();

    jugador.actualizarInterno(renderizador.getCamera());

    gestor.actualizarIAEntidades(jugador, miMapa);

    renderizador.actualizarNieblaMinimapa(jugador);


    // --- 4. LOGICA DE INTERACCION (TECLA 'E')!! ---
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
            // Opcion 1: Es un Indicador de Puerta
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
            // Opcion 2: Es un Cofre
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
            // Opcion 3: Es una Nota
            else if (Nota* nota = dynamic_cast<Nota*>(itemMasCercano))
            {
                this->notaActualID = nota->usar(jugador);
                this->estadoActual = EstadoJuego::LEYENDO_NOTA;
            }
            // Opcion 4: Es un item de recoleccion
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
    // --- FIN DE LA LOGICA DE INTERACCION ---


    // 5. EJECUTAR FISICA
    MotorFisica::moverJugador(jugador, direccionMovimiento, miMapa.getMuros(), miMapa.getCajas(), miMapa.getPuertaJefe(), miMapa.estaPuertaAbierta());
    MotorFisica::moverEnemigos(gestor.getEnemigos(), miMapa.getMuros(), miMapa.getCajas(), miMapa.getPuertaJefe(), miMapa.estaPuertaAbierta());
    MotorFisica::moverJefes(gestor.getJefes(), miMapa.getMuros(), miMapa.getCajas(), miMapa.getPuertaJefe(), miMapa.estaPuertaAbierta());
    MotorFisica::moverBalas(gestor.getBalas(), miMapa.getMuros(), miMapa.getCajas(), miMapa.getPuertaJefe(), miMapa.estaPuertaAbierta());

    // 5b. RESOLVER COLISIONES DINAMICAS
    MotorFisica::resolverColisionesDinamicas(jugador, gestor.getEnemigos());

    // --- 6. Disparos (MODIFICADO PARA CHEAT) ---
    int tipoDisparo = jugador.intentarDisparar(quiereDisparar);
    if (tipoDisparo > 0) // (1=Normal, 2=Cheat)
    {
        Vector2 dirDisparo = jugador.getDireccionVista();
        bool estaMoviendo = (Vector2LengthSqr(direccionMovimiento) > 0.0f);
        bool esUnCheat = (tipoDisparo == 2);

        if (estaMoviendo && !esUnCheat) // El cheat siempre es preciso
        {
            float anguloPreciso = atan2f(dirDisparo.y, dirDisparo.x);
            float desviacionGrados = (float)GetRandomValue(-200, 200) / 10.0f;
            float desviacionRadianes = desviacionGrados * DEG2RAD;
            float anguloImpreciso = anguloPreciso + desviacionRadianes;
            dirDisparo = { cosf(anguloImpreciso), sinf(anguloImpreciso) };
        }

        // Pasamos el bool 'esUnCheat' al constructor de BalaDeRifle
        gestor.registrarBala(new BalaDeRifle(jugador.getPosicion(), dirDisparo, esUnCheat));
    }
    // --- FIN MODIFICACIÓN ---

    // 7. Colisiones
    MotorColisiones::procesar(jugador, gestor);

    // 8. Recoleccion de basura
    gestor.recolectarBasura();

    // 9. LOGICA DE ESTADO Y FANTASMA
    temporizadorPartida += GetFrameTime();

    if (!gestor.getJefes().empty() && !gestor.getJefes()[0]->estaVivo())
    {
        Fantasma::estaDespertando = false;
        Fantasma::despertado = true;
        Fantasma::modoFuria = true;
        Fantasma::modoDialogo = true;
        Fantasma::estaAsustando = false;
        estadoActual = EstadoJuego::DIALOGO_FINAL;
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
    if (SistemaInput::quiereInteractuar() || IsKeyPressed(KEY_ESCAPE))
    {
        estadoActual = EstadoJuego::JUGANDO;
        notaActualID = 0;
    }
}

void Juego::actualizarIniciandoJefe()
{
    // El jugador puede mirar alrededor, pero no moverse
    jugador.actualizarInterno(renderizador.getCamera());

    // Contamos hacia atrás
    temporizadorSpawnJefe -= GetFrameTime();

    if (temporizadorSpawnJefe <= 0)
    {
        // ¡Spawnea el jefe!
        gestor.registrarJefe(new Jefe({0.0f, 0.0f}));
        jefeHaSpawned = true; // Doble seguridad

        // Vuelve al juego
        estadoActual = EstadoJuego::JUGANDO;
    }
}

void Juego::actualizarDialogo()
{
    jugador.actualizarInterno(renderizador.getCamera());

    Enemigo* fantasmaPtr = nullptr;
    for (Enemigo* enemigo : gestor.getEnemigos()) {
        if (dynamic_cast<Fantasma*>(enemigo)) {
            fantasmaPtr = enemigo;
            break;
        }
    }

    if (fantasmaPtr) {
        fantasmaPtr->actualizarIA(jugador.getPosicion(), miMapa);

        float distancia = Vector2Distance(jugador.getPosicion(), fantasmaPtr->getPosicion());
        if (distancia < 100.0f) {
            estadoActual = EstadoJuego::FIN_JUEGO_GANADO;
        }
    } else {
        estadoActual = EstadoJuego::FIN_JUEGO_GANADO;
    }
}


// --- METODOS DE DIBUJADO PRIVADOS ---

void Juego::dibujarJugando()
{
    renderizador.dibujarTodo(jugador, miMapa, gestor);
}

void Juego::dibujarIniciandoJefe()
{
    // Dibujamos el juego normal (jugador quieto, puerta cerrada)
    dibujarJugando();

    // Mostramos un texto de "peligro"
    const char* textoSpawn = "LA PUERTA SE HA SELLADO.\n\nSIENTES UNA DISTORSION EN EL CENTRO DE LA SALA...";

    float alpha = 1.0f;
    if (temporizadorSpawnJefe < 1.0f) {
        alpha = temporizadorSpawnJefe; // Fade out
    }

    DibujarTextoGlitch(textoSpawn,
                       Constantes::ANCHO_PANTALLA / 2 - MeasureText(textoSpawn, 20) / 2,
                       Constantes::ALTO_PANTALLA - 150,
                       20, Fade(RED, alpha));
}


void Juego::dibujarLeyendoNota()
{
    // 1. Dibujamos el juego pausado detras
    renderizador.dibujarTodo(jugador, miMapa, gestor);
    // 2. Dibujamos el velo oscuro
    DrawRectangle(0, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, Fade(BLACK, 0.85f));
    // 3. Definimos el area de la nota (terminal)
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

    // (Lógica de texto de notas omitida por brevedad, va aquí)
    const char* textoTitulo = "REGISTRO";
    const char* textoNota = "...";
    // ... tu switch(notaActualID) ...

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

void Juego::dibujarFinJuego()
{
    renderizador.dibujarTodo(jugador, miMapa, gestor);
    DrawRectangle(0, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, Fade(BLACK, 0.7f));
    if (estadoActual == EstadoJuego::FIN_JUEGO_GANADO)
    {
        DrawText("VICTORIA", Constantes::ANCHO_PANTALLA / 2 - MeasureText("VICTORIA", 60) / 2, Constantes::ALTO_PANTALLA / 2 - 40, 60, LIME);
        DrawText("Has escapado del Nexo... o quizas solo has encontrado la paz.", Constantes::ANCHO_PANTALLA / 2 - MeasureText("Has escapado del Nexo... o quizas solo has encontrado la paz.", 20) / 2, Constantes::ALTO_PANTALLA / 2 + 30, 20, WHITE);
    }
    else // FIN_JUEGO_MUERTO
    {
        DrawText("GAME OVER", Constantes::ANCHO_PANTALLA / 2 - MeasureText("GAME OVER", 60) / 2, Constantes::ALTO_PANTALLA / 2 - 40, 60, RED);
        DrawText("Tu eco se une a los demas...", Constantes::ANCHO_PANTALLA / 2 - MeasureText("Tu eco se une a los demas...", 20) / 2, Constantes::ALTO_PANTALLA / 2 + 30, 20, GRAY);
    }
}

// --- ¡¡IMPLEMENTACIÓN DE CHEATS MODIFICADA!! ---
void Juego::procesarCheats()
{
    if (!IsKeyDown(KEY_LEFT_SHIFT)) {
        return;
    }
    // SHIFT + H = Curar Vida
    if (IsKeyPressed(KEY_H)) {
        jugador.curarVida(100);
    }
    // SHIFT + A = Dar Armadura
    if (IsKeyPressed(KEY_A)) {
        jugador.recibirArmadura();
    }
    // SHIFT + M = Dar Munición
    if (IsKeyPressed(KEY_M)) {
        jugador.recargarMunicion(50);
    }
    // SHIFT + B = Recargar Batería
    if (IsKeyPressed(KEY_B)) {
        jugador.recargarBateria(100);
    }
    // SHIFT + K = Dar Llave
    if (IsKeyPressed(KEY_K)) {
        jugador.recibirLlave();
    }

    // --- ¡¡NUEVO CHEAT DE DAÑO!! ---
    // SHIFT + J = Próximo disparo hace 1000 de daño
    if (IsKeyPressed(KEY_J)) {
        jugador.activarCheatDisparo();
    }
    // -------------------------------
}
