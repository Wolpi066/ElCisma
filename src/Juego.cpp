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

// --- ¡¡NUEVO!! Helper para el texto glitch ---
// Dibuja texto con un efecto de "aberracion cromatica" o glitch
void DibujarTextoGlitch(const char* texto, int posX, int posY, int fontSize, Color colorPrincipal)
{
    // 1. Canales de color desfasados (el "glitch")
    // (Usamos el tiempo para que el offset sea dinamico y "vibre")
    float offset = sin(GetTime() * 15.0f) * 1.0f; // Vibracion sutil
    float offset2 = cos(GetTime() * 10.0f) * 1.0f;
    DrawText(texto, posX + (int)offset, posY + (int)offset2, fontSize, Fade((Color){255, 0, 100, 255}, 0.2f));
    DrawText(texto, posX - (int)offset, posY - (int)offset2, fontSize, Fade((Color){0, 255, 200, 255}, 0.2f));

    // 2. Texto principal
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

// --- ¡¡FUNCION COMPLETAMENTE REDISEÑADA!! ---
void Juego::dibujarLeyendoNota()
{
    // 1. Dibujamos el juego pausado detras
    renderizador.dibujarTodo(jugador, miMapa, gestor);

    // 2. Dibujamos el velo oscuro
    DrawRectangle(0, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, Fade(BLACK, 0.85f));

    // 3. Definimos el area de la nota (terminal)
    Rectangle frame = {
        Constantes::ANCHO_PANTALLA * 0.1f,  // Margen 10%
        Constantes::ALTO_PANTALLA * 0.15f, // Margen 15%
        Constantes::ANCHO_PANTALLA * 0.8f,  // Ancho 80%
        Constantes::ALTO_PANTALLA * 0.7f   // Alto 70%
    };

    // --- ¡¡MODIFICADO!! Fondo de la terminal (ahora 80% opaco) ---
    DrawRectangleRec(frame, Fade((Color){10, 20, 30, 255}, 0.80f));
    // Borde tematico (verde cyber)
    DrawRectangleLinesEx(frame, 2.0f, Fade((Color){0, 255, 128, 100}, 0.5f));

    // --- Scanlines (para el efecto CRT) ---
    for (int y = (int)frame.y; y < (int)(frame.y + frame.height); y += 3)
    {
        DrawLine(frame.x, y, frame.x + frame.width, y, Fade(BLACK, 0.5f));
    }
    // ---------------------------------------

    // 4. Dibujamos el texto de la nota (¡con el lore!)
    const char* textoTitulo = "REGISTRO CORRUPTO";
    const char* textoNota = "DATOS NO ENCONTRADOS.\n\n[ERROR: 0x3F_Desync_Eter]\nLA FUENTE SOLICITADA NO EXISTE.";

    // --- ¡¡NUEVO LORE REVISADO!! ---
    switch (notaActualID)
    {
        case 0: // La nueva nota inicial de Elara
            textoTitulo = "MENSAJE DE ETER (¿Elara?)";
            textoNota = "¿Quien eres? ¿Otro eco, como yo?\n\nNo... tu... tu eres real. Estas vivo.\n\nDebes... debes calmarlo. El ancla. El Dr. Aris.\nEsta en el centro. El Cisma lo consumio, y su dolor\nnos mantiene atrapados a todos aqui.\n\nPor favor... liberalo. Liberalo y podremos... descansar.";
            break;
        case 1:
            textoTitulo = "LOG CIENTIFICO: 734-A";
            textoNota = "No es un lugar. Es... una costura. Un pliegue entre\nlo que *es* y lo que *piensa*.\n\nNo debimos tirar del hilo.";
            break;
        case 2:
            textoTitulo = "MEMO DE SEGURIDAD: INCIDENTE";
            textoNota = "Protocolo Cisma activado. El Dr. Aris es... el ancla.\nLa estructura del Nexo se esta colapsando sobre su\nconciencia. No podemos sellar la brecha mientras el...\nbueno, mientras *el* siga ahi.";
            break;
        case 3:
            textoTitulo = "NOTA DE DIARIO: DIA ???";
            textoNota = "¿Cuanto tiempo llevo aqui? La luz de la linterna parpadea,\npero nunca se apaga. Vi a... algo... hoy. No tenia\nforma fisica. Era solo... un grito con sombra.\n\nCreo que era Elara, de observacion psionica.";
            break;
        case 4:
            textoTitulo = "INFORME DE CONTENCION";
            textoNota = "Los cadaveres reanimados son un problema\nmenor. Son ecos fisicos. Predecibles. Lentos.\nDisparad a la cabeza y seguid moviendoos.\n\nEl verdadero peligro es el Eter.";
            break;
        case 5:
            // --- ¡¡LA NOTA DEL FANTASMA!! ---
            textoTitulo = "FRAGMENTO DE ETER (¿Elara?)";
            textoNota = "...frio... no puedo... no puedo parar.\nEl me ve. El ancla. Me atrae.\n\nNo quiero hacerles dano. No puedo parar.\n\n...Aris... por favor... para... duele...";
            break;
        case 6:
            textoTitulo = "LOG DE MANTENIMIENTO: CAMARAS";
            textoNota = "Fallo catastrofico en las camaras de contencion\nemocional. El 'Canto' y el 'Oxido' se desbordaron.\nEstan... filtrandose. La arquitectura esta\n*sintiendo*.\n\nNo dejen que el miedo los controle. El Nexo se alimenta.";
            break;
        case 7:
            textoTitulo = "ORDEN DE EVACUACION (Parcial)";
            textoNota = "...energia al minimo. La bateria de la\nlinterna es vuestra vida. Usadla con cuidado.\nEl Eter reacciona a la luz. Atraera a los ecos.\n\nSi os quedais a oscuras... rezad para que solo\nla Materia os encuentre.";
            break;
        case 8:
            textoTitulo = "MEMO DE SEGURIDAD: PUERTA DEL NEXO";
            textoNota = "La puerta principal esta bajo sello psionico.\nEs una medida de seguridad de Aris. Solo se abrira\nsi el 'ancla' del Cisma se calma.\n\nBuena suerte con eso. La llave esta en la Camara\nde la Memoria. O estaba.";
            break;
        case 9:
            textoTitulo = "PENSAMIENTO TRANSCRITO (¿MIO?)";
            textoNota = "No estoy aqui. Soy un eco. Morí en el pasillo.\nSenti el... Cisma. Me desgarro.\n\nPero sigo caminando. Sigo respirando.\nEsto es un suenno. Tengo que despertar.\nTengo que despertar.";
            break;
        case 10:
            textoTitulo = "REGISTRO DE SEGURIDAD #119";
            textoNota = "Los Cofres estan asegurados. Los suministros deberian\ndurar. Pero estas... *cosas*... no necesitan comer.\nSolo odian.\n\nVi a uno gordo. Carne podrida. Se movia tan...\n...lento. Pero no se detuvo. Nunca se detuvo.";
            break;
        default:
            // Se queda como "REGISTRO CORRUPTO"
            break;
    }
    // --- FIN DEL LORE ---

    // Dibujar Titulo (con efecto glitch)
    DibujarTextoGlitch(textoTitulo,
             (int)(frame.x + 20),
             (int)(frame.y + 20),
             20, (Color){0, 255, 128, 255}); // Verde terminal

    DrawLine((int)(frame.x + 20), (int)(frame.y + 45), (int)(frame.x + frame.width - 20), (int)(frame.y + 45), Fade(GREEN, 0.2f));

    // Dibujar Texto Principal (con efecto glitch)
    DibujarTextoGlitch(textoNota,
             (int)(frame.x + 20),
             (int)(frame.y + 70),
             20, (Color){200, 255, 220, 255}); // Fuente 20 (verde-blanco)

    // 5. Instruccion de salida
    const char* textoCerrar = "Presiona 'E' para cerrar";
    // Parpadeo suave para la instruccion
    float alphaCerrar = (sin(GetTime() * 2.0f) + 1.0f) / 2.0f; // 0.0 a 1.0
    DrawText(textoCerrar,
             (int)(frame.x + frame.width - MeasureText(textoCerrar, 20) - 20),
             (int)(frame.y + frame.height - 40),
             20, Fade(GREEN, 0.5f + alphaCerrar * 0.5f));
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
