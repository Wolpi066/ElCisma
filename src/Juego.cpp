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
      temporizadorSpawnJefe(0.0f)
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
        case EstadoJuego::FIN_JUEGO_GANADO:
        case EstadoJuego::FIN_JUEGO_MUERTO:
            break;
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
            case EstadoJuego::FIN_JUEGO_GANADO:
            case EstadoJuego::FIN_JUEGO_MUERTO:
                dibujarFinJuego();
                break;
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

    jugador.actualizarInterno(renderizador.getCamera());

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
        Fantasma::modoFuria = true; // (para que se acerque)
        Fantasma::modoDialogo = true; // (para que no ataque)
        Fantasma::estaAsustando = false;
        // ----------------------------------------------

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
    jugador.actualizarInterno(renderizador.getCamera());
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
            // (Aquí iría la lógica de diálogo, por ahora solo ganamos)
            estadoActual = EstadoJuego::FIN_JUEGO_GANADO;
        }
    } else {
        // Si el fantasma no se encuentra (¿error?), ganamos igual.
        estadoActual = EstadoJuego::FIN_JUEGO_GANADO;
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

    // --- TAREA 3: Llenar las Notas ---
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
