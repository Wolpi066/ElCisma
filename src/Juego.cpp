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
#include "BalaDeRifle.h"
#include "BalaMonstruosa.h"
#include "Spawner.h"

// --- CONSTRUCTOR ---
Juego::Juego()
    : jugador({0, 500}),
      miMapa(),
      gestor(),
      renderizador(),
      estadoActual(EstadoJuego::JUGANDO),
      temporizadorPartida(0.0f),
      temporizadorSustoFantasma(0.0f),
      notaActualID(0)
{
    miMapa.poblarMundo(gestor);
    ResetSustoFantasma();

    // --- ¢B¢BNUEVO!! ---
    // Le decimos al renderizador que dibuje el mapa en la textura
    // (Lo hacemos DESPUES de poblar el mapa, por si acaso)
    renderizador.inicializarMinimapa(miMapa);
    // -----------------
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
        case EstadoJuego::LEYENDO_NOTA:
            actualizarLeyendoNota(); // <-- ¡¡TYPO CORREGIDO!!
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
    if (!jugador.estaVivo()) {
        estadoActual = EstadoJuego::FIN_JUEGO_MUERTO;
        return;
    }

    Vector2 direccionMovimiento = SistemaInput::getDireccionMovimiento();
    bool quiereDisparar = SistemaInput::quiereDisparar();
    bool quiereInteractuar = SistemaInput::quiereInteractuar();

    jugador.actualizarInterno(renderizador.getCamera());

    // ¡¡MODIFICACIÓN!! Pasamos el mapa
    gestor.actualizarIAEntidades(jugador, miMapa);

    // --- ¡¡AÑADIDO!! Actualizar la niebla de guerra ---
    renderizador.actualizarNieblaMinimapa(jugador);
    // ----------------------------------------------


    // --- 4. LOGICA DE INTERACCION (TECLA 'E')!! ---
    if (quiereInteractuar)
    {
        // ¡¡MODIFICADO!! Radio aumentado
        float radioInteraccion = 75.0f; // Antes 50.0f
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
                    case 99: gestor.registrarConsumible(Spawner<Llave>::Spawn(posDrop)); break; // ¡¡CORREGIDO!! La llave spawnea
                    default: break;
                }
            }
            // Opcion 3: Es una Nota
            else if (Nota* nota = dynamic_cast<Nota*>(itemMasCercano))
            {
                this->notaActualID = nota->usar(jugador); // 'usar' devuelve el ID
                this->estadoActual = EstadoJuego::LEYENDO_NOTA; // Pausa el juego
            }
            // --- ¡¡LÓGICA ACTUALIZADA!! ---
            // Opcion 4: Es un item de recoleccion (Llave, Botiquin, Bateria, etc.)
            else if (
                dynamic_cast<Llave*>(itemMasCercano) ||
                dynamic_cast<Botiquin*>(itemMasCercano) ||
                dynamic_cast<Bateria*>(itemMasCercano) ||
                dynamic_cast<CajaDeMuniciones*>(itemMasCercano) ||
                dynamic_cast<Armadura*>(itemMasCercano)
            )
            {
                itemMasCercano->usar(jugador); // Lo recoge/consume
            }
        }
    }
    // --- FIN DE LA LOGICA DE INTERACCION ---


    // 5. EJECUTAR FISICA (contra muros/cajas)
    MotorFisica::moverJugador(jugador, direccionMovimiento, miMapa.getMuros(), miMapa.getCajas(), miMapa.getPuertaJefe(), miMapa.estaPuertaAbierta());
    MotorFisica::moverEnemigos(gestor.getEnemigos(), miMapa.getMuros(), miMapa.getCajas(), miMapa.getPuertaJefe(), miMapa.estaPuertaAbierta());
    MotorFisica::moverJefes(gestor.getJefes(), miMapa.getMuros(), miMapa.getCajas(), miMapa.getPuertaJefe(), miMapa.estaPuertaAbierta());
    MotorFisica::moverBalas(gestor.getBalas(), miMapa.getMuros(), miMapa.getCajas(), miMapa.getPuertaJefe(), miMapa.estaPuertaAbierta());

    // --- ¡¡NUEVO!! 5b. RESOLVER COLISIONES DINAMICAS ---
    // (Empuja a los enemigos fuera del jugador)
    MotorFisica::resolverColisionesDinamicas(jugador, gestor.getEnemigos());
    // --------------------------------------------------

    // 6. Disparos
    if (jugador.intentarDisparar(quiereDisparar)) {

        Vector2 dirDisparo = jugador.getDireccionVista();
        bool estaMoviendo = (Vector2LengthSqr(direccionMovimiento) > 0.0f);

        if (estaMoviendo)
        {
            float anguloPreciso = atan2f(dirDisparo.y, dirDisparo.x);
            float desviacionGrados = (float)GetRandomValue(-200, 200) / 10.0f; // -20.0 a +20.0
            float desviacionRadianes = desviacionGrados * DEG2RAD;
            float anguloImpreciso = anguloPreciso + desviacionRadianes;
            dirDisparo = { cosf(anguloImpreciso), sinf(anguloImpreciso) };
        }
        gestor.registrarBala(new BalaDeRifle(jugador.getPosicion(), dirDisparo));
    }

    for (Jefe* jefe : gestor.getJefes()) {
        if (jefe->quiereDisparar()) {
            Vector2 dir = Vector2Normalize(Vector2Subtract(jugador.getPosicion(), jefe->getPosicion()));
            gestor.registrarBala(new BalaMonstruosa(jefe->getPosicion(), dir));
        }
    }

    // 7. Colisiones (Balas, Ataques de IA)
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

// --- ¢B¢BNUEVA FUNCION!! ---
void Juego::actualizarLeyendoNota()
{
    // Si el jugador presiona 'E' de nuevo, o ESC, cierra la nota
    if (SistemaInput::quiereInteractuar() || IsKeyPressed(KEY_ESCAPE))
    {
        estadoActual = EstadoJuego::JUGANDO;
        notaActualID = 0; // Limpiamos el ID
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
        // ¡MODIFICACIÓN SUTIL PERO IMPORTANTE!
        // Le pasamos miMapa, aunque el fantasma no la use ahora,
        // la firma de actualizarIA lo requiere.
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

// --- ¢B¢BNUEVA FUNCION CON HUD PROFESIONAL!! ---
void Juego::dibujarLeyendoNota()
{
    // 1. Dibujamos el juego pausado detras
    // --- ¢B¢BBUG CORREGIDO!! ---
    // Llamamos a renderizador.dibujarTodo() directamente
    // para que se dibuje *antes* del EndDrawing()
    renderizador.dibujarTodo(jugador, miMapa, gestor);

    // 2. Dibujamos el velo oscuro
    DrawRectangle(0, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, Fade(BLACK, 0.85f));

    // 3. Definimos el area de la nota (un HUD profesional)
    Rectangle frame = {
        Constantes::ANCHO_PANTALLA * 0.15f, // Margen 15%
        Constantes::ALTO_PANTALLA * 0.2f,  // Margen 20%
        Constantes::ANCHO_PANTALLA * 0.7f,  // Ancho 70%
        Constantes::ALTO_PANTALLA * 0.6f   // Alto 60%
    };
    // Fondo de "papel"
    DrawRectangleRec(frame, (Color){ 245, 245, 245, 255 }); // Blanco hueso
    DrawRectangleLinesEx(frame, 3.0f, BLACK); // Borde grueso

    // 4. Dibujamos el texto de la nota
    const char* textoTitulo = "";
    const char* textoNota = "";
    switch (notaActualID)
    {
        case 1:
            textoTitulo = "Nota #1: Entrada de Laboratorio";
            textoNota = "Hola mundo";
            break;
        // ... mas notas ...
        default:
            textoTitulo = "ERROR";
            textoNota = "NOTA CORRUPTA";
            break;
    }

    // Dibujar Titulo
    DrawText(textoTitulo,
             (int)(frame.x + 20),
             (int)(frame.y + 20),
             20, GRAY); // Fuente 20, gris

    DrawLine((int)(frame.x + 20), (int)(frame.y + 45), (int)(frame.x + frame.width - 20), (int)(frame.y + 45), LIGHTGRAY);

    // Dibujar Texto Principal
    DrawText(textoNota,
             (int)(frame.x + 20),
             (int)(frame.y + 70),
             30, BLACK); // Fuente 30, negro

    // 5. Instruccion de salida
    const char* textoCerrar = "Presiona 'E' para cerrar";
    DrawText(textoCerrar,
             (int)(frame.x + frame.width - MeasureText(textoCerrar, 20) - 20),
             (int)(frame.y + frame.height - 40),
             20, DARKGRAY);
}

void Juego::dibujarDialogo()
{
    // ¢B¢BCORREGIDO!!
    renderizador.dibujarTodo(jugador, miMapa, gestor);

    DrawRectangle(0, Constantes::ALTO_PANTALLA - 100, Constantes::ANCHO_PANTALLA, 100, Fade(BLACK, 0.8f));
    DrawText("El 'Fantasma' se acerca... su movimiento es erratico pero decidido...", 20, Constantes::ALTO_PANTALLA - 80, 20, WHITE);
    DrawText("Sientes una presencia abrumadora... no es hostil, es... triste.", 20, Constantes::ALTO_PANTALLA - 50, 20, WHITE);
}

void Juego::dibujarFinJuego()
{
    // ¢B¢BCORREGIDO!!
    renderizador.dibujarTodo(jugador, miMapa, gestor);

    DrawRectangle(0, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, Fade(BLACK, 0.7f));

    if (estadoActual == EstadoJuego::FIN_JUEGO_GANADO)
    {
        DrawText("VICTORIA",
                 Constantes::ANCHO_PANTALLA / 2 - MeasureText("VICTORIA", 60) / 2,
                 Constantes::ALTO_PANTALLA / 2 - 40, 60, LIME);
        DrawText("Has escapado del Nexo... o quizas solo has encontrado la paz.",
                 Constantes::ANCHO_PANTALLA / 2 - MeasureText("Has escapado del Nexo... o quizas solo has encontrado la paz.", 20) / 2,
                 Constantes::ALTO_PANTALLA / 2 + 30, 20, WHITE);
    }
    else // FIN_JUEGO_MUERTO
    {
        DrawText("GAME OVER",
                 Constantes::ANCHO_PANTALLA / 2 - MeasureText("GAME OVER", 60) / 2,
                 Constantes::ALTO_PANTALLA / 2 - 40, 60, RED);
        DrawText("Tu eco se une a los demas...",
                 Constantes::ANCHO_PANTALLA / 2 - MeasureText("Tu eco se une a los demas...", 20) / 2,
                 Constantes::ALTO_PANTALLA / 2 + 30, 20, GRAY);
    }
}
