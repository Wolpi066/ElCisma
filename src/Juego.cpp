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
      estadoActual(EstadoJuego::JUGANDO),
      temporizadorPartida(0.0f),
      temporizadorSustoFantasma(0.0f),
      notaActualID(0),
      jefeHaSpawned(false),
      triggerRectJefe({ -200, -200, 400, 400 }),
      temporizadorSpawnJefe(0.0f),
      opcionDialogo(1), // (Inicializamos la opción)
      temporizadorDialogo(0.0f) // (Inicializamos el timer)
{
    miMapa.poblarMundo(gestor);
    ResetSustoFantasma();
    renderizador.inicializarMinimapa(miMapa);
}

Juego::~Juego()
{
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

        // --- TAREA FINAL: Nuevos estados ---
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

        // Los estados finales ahora usan un temporizador
        case EstadoJuego::FIN_JUEGO_SACRIFICIO:
        case EstadoJuego::FIN_JUEGO_HUIR:
            actualizarFinJuego(); // ¡NUEVO!
            break;
        case EstadoJuego::FIN_JUEGO_MUERTO:
            break; // Este no hace nada
        // ---------------------------------
    }
}

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

            // --- TAREA FINAL: Nuevos estados ---
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
            case EstadoJuego::FIN_JUEGO_MUERTO:
                dibujarFinJuego();
                break;
            // ---------------------------------
        }
    EndDrawing();
}


void Juego::actualizarJugando()
{
    procesarCheats();

    if (!jugador.estaVivo()) {
        estadoActual = EstadoJuego::FIN_JUEGO_MUERTO;
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

            // --- TAREA 1: Desactivar Fantasma ---
            Fantasma::jefeEnCombate = true;
            // ------------------------------------

            return;
        }
    }

    Vector2 direccionMovimiento = SistemaInput::getDireccionMovimiento();
    bool quiereDisparar = SistemaInput::quiereDisparar();
    bool quiereInteractuar = SistemaInput::quiereInteractuar();

    // --- ¡MODIFICADO! Pasamos la dirección de movimiento ---
    jugador.actualizarInterno(renderizador.getCamera(), direccionMovimiento);
    // ----------------------------------------------------

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
        // --- TAREA 1: Reactivar Fantasma para diálogo ---
        Fantasma::jefeEnCombate = false; // ¡Reactivada!
        Fantasma::estaDespertando = false;
        Fantasma::despertado = true;
        Fantasma::modoFuria = false; // (Que se acerque lento)
        Fantasma::modoDialogo = true; // (para que no ataque)
        Fantasma::estaAsustando = false;
        // ----------------------------------------------

        estadoActual = EstadoJuego::DIALOGO_FINAL;
        temporizadorDialogo = 3.0f; // ¡Inicia el temporizador para que se acerque!
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
    // --- ¡MODIFICADO! El jugador no se mueve
    jugador.actualizarInterno(renderizador.getCamera(), {0, 0});

    if (SistemaInput::quiereInteractuar() || IsKeyPressed(KEY_ESCAPE))
    {
        estadoActual = EstadoJuego::JUGANDO;
        notaActualID = 0;
    }
}

void Juego::actualizarIniciandoJefe()
{
    // --- ¡MODIFICADO! El jugador no se mueve
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
    // --- TAREA 3: Lógica de Diálogo Arreglada ---
    // Este estado ahora SOLO es una pausa cinemática.
    jugador.actualizarInterno(renderizador.getCamera(), {0, 0}); // El jugador puede mirar, no moverse

    // El fantasma se mueve (lo gestiona gestor.actualizarIAEntidades)
    gestor.actualizarIAEntidades(jugador, miMapa);

    temporizadorDialogo -= GetFrameTime();
    if (temporizadorDialogo <= 0)
    {
        estadoActual = EstadoJuego::DIALOGO_INTRO; // Pasa al monólogo
        opcionDialogo = 1;

        // Teletransporta a Elana frente al jugador para asegurar la escena
        for (Enemigo* enemigo : gestor.getEnemigos()) {
            if (dynamic_cast<Fantasma*>(enemigo)) {
                enemigo->setPosicion(Vector2Add(jugador.getPosicion(), Vector2Scale(jugador.getDireccionVista(), 80.0f)));
                break;
            }
        }
    }
    // --- FIN TAREA 3 ---
}

// --- TAREA 3: NUEVA FUNCIÓN ---
void Juego::actualizarDialogoIntro()
{
    jugador.actualizarInterno(renderizador.getCamera(), {0, 0});
    if (SistemaInput::quiereInteractuar())
    {
        estadoActual = EstadoJuego::DIALOGO_PREGUNTAS;
        opcionDialogo = 1; // Resetea a la primera pregunta
    }
}

// --- TAREA 3: NUEVA FUNCIÓN ---
void Juego::actualizarDialogoPreguntas()
{
    jugador.actualizarInterno(renderizador.getCamera(), {0, 0});

    // Mover la selección (3 opciones)
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

    // Confirmar selección
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
        else // Opcion 3 (Callado)
        {
            estadoActual = EstadoJuego::DIALOGO_DECISION_FINAL;
            opcionDialogo = 1; // Resetea para la decisión final
        }
    }
}

// --- TAREA 3: NUEVA FUNCIÓN ---
void Juego::actualizarDialogoRespuesta()
{
    jugador.actualizarInterno(renderizador.getCamera(), {0, 0});
    // Espera a que el jugador presione 'E' para volver a las preguntas
    if (SistemaInput::quiereInteractuar())
    {
        estadoActual = EstadoJuego::DIALOGO_PREGUNTAS;
        opcionDialogo = 1; // Resetea a la primera pregunta
    }
}

// --- TAREA 3: NUEVA FUNCIÓN ---
void Juego::actualizarDialogoDecisionFinal()
{
    jugador.actualizarInterno(renderizador.getCamera(), {0, 0});

    // Mover la selección (2 opciones)
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
    {
        opcionDialogo = 1;
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
    {
        opcionDialogo = 2;
    }

    // Confirmar selección
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
        temporizadorDialogo = 3.0f; // Inicia el temporizador de 3 seg para cerrar
    }
}

// --- TAREA 3: NUEVA FUNCIÓN ---
void Juego::actualizarFinJuego()
{
    // Esta función ahora maneja el cierre automático
    temporizadorDialogo -= GetFrameTime();
    if (temporizadorDialogo <= 0)
    {
        CloseWindow(); // Cierra el juego
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
    DrawRectangle(0, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, Fade(BLACK, 0.85f));
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

    // --- TAREA 3: Llenar las Notas (YA IMPLEMENTADO) ---
    const char* textoTitulo = "REGISTRO";
    const char* textoNota = "...";

    switch(notaActualID)
    {
        case 0: // Nota Inicial (Spawn)
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
            textoNota = "¡Funciona! ¡Puedo sentirlo! ¡El Desdoblamiento es la clave!\n"
                        "¡Unir la materia y el espiritu! ¡Seremos dioses!\n\n"
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
        case 5: // Nota de Elana (Fantasma)
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
                        "¡ELANA! ¡¿POR QUE ME HAS ENCERRADO AQUI?!\n"
                        "¡¿POR QUE?! ¡m... mi... rostro...";
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
                        "Puede transitar ambos planos. ¿Por que el? ¿Es su psique?\n"
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
    // --- FIN TAREA 3 ---


    DibujarTextoGlitch(textoTitulo, (int)(frame.x + 20), (int)(frame.y + 20), 20, (Color){0, 255, 128, 255});
    DrawLine((int)(frame.x + 20), (int)(frame.y + 45), (int)(frame.x + frame.width - 20), (int)(frame.y + 45), Fade(GREEN, 0.2f));
    DibujarTextoGlitch(textoNota, (int)(frame.x + 20), (int)(frame.y + 70), 20, (Color){200, 255, 220, 255});
    const char* textoCerrar = "Presiona 'E' para cerrar";
    float alphaCerrar = (sin(GetTime() * 2.0f) + 1.0f) / 2.0f;
    DrawText(textoCerrar, (int)(frame.x + frame.width - MeasureText(textoCerrar, 20) - 20), (int)(frame.y + frame.height - 40), 20, Fade(GREEN, 0.5f + alphaCerrar * 0.5f));
}

void Juego::dibujarDialogo()
{
    // Este estado ahora SOLO es el fantasma acercándose
    renderizador.dibujarTodo(jugador, miMapa, gestor);
    DrawRectangle(0, Constantes::ALTO_PANTALLA - 100, Constantes::ANCHO_PANTALLA, 100, Fade(BLACK, 0.8f));
    DrawText("El 'Fantasma' se acerca... su movimiento es erratico pero decidido...", 20, Constantes::ALTO_PANTALLA - 80, 20, WHITE);
    DrawText("Sientes una presencia abrumadora... no es hostil, es... triste.", 20, Constantes::ALTO_PANTALLA - 50, 20, WHITE);
}

// --- TAREA 3: Funciones de dibujado del nuevo diálogo ---

void DibujarCajaDialogo()
{
    DrawRectangle(0, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, Fade(BLACK, 0.85f));
    Rectangle frame = {
        Constantes::ANCHO_PANTALLA * 0.1f,
        Constantes::ALTO_PANTALLA * 0.25f, // Más centrado
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

    // Opciones
    const char* textoOpcion1 = "[1] ¿Quien eres tu?";
    const char* textoOpcion2 = "[2] ¿Que es este lugar?";
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

    // Opciones
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
// --- FIN TAREA 3 ---


void Juego::dibujarFinJuego()
{
    renderizador.dibujarTodo(jugador, miMapa, gestor);
    DrawRectangle(0, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, Fade(BLACK, 0.85f));

    // --- TAREA 3: Finales Múltiples (Impactantes) ---
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
    else // FIN_JUEGO_MUERTO
    {
        const char* t1 = "TE HAS UNIDO AL ECO";
        const char* t2 = "Tu luz se apaga. Tu alma se une a los gritos del Nexo.";
        DibujarTextoGlitch(t1, Constantes::ANCHO_PANTALLA / 2 - MeasureText(t1, 50) / 2, Constantes::ALTO_PANTALLA / 2 - 40, 50, RED);
        DibujarTextoGlitch(t2, Constantes::ANCHO_PANTALLA / 2 - MeasureText(t2, 20) / 2, Constantes::ALTO_PANTALLA / 2 + 40, 20, GRAY);
    }
    // --- FIN TAREA 3 ---
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
    // --- ¡¡NUEVO CHEAT DE MUERTE!! ---
    if (IsKeyPressed(KEY_L))
    {
        if (jefeHaSpawned && !gestor.getJefes().empty())
        {
            gestor.getJefes()[0]->forzarMuerte();
        }
    }
    // ---------------------------------
}
