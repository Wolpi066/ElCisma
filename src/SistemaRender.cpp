#include "SistemaRender.h"
#include "Constantes.h"
#include "Fantasma.h"
#include "raymath.h" // Para Vector2Add

// (Los #includes de Iluminacion.h y .cpp ya tienen los #define de rlgl)


SistemaRender::SistemaRender() : camera({0})
{
    camera.offset = (Vector2){ Constantes::ANCHO_PANTALLA / 2.0f, Constantes::ALTO_PANTALLA / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    float alcanceInicial = 300.0f;
    Iluminacion::SetupLight(&linterna, {0,0}, WHITE, alcanceInicial, 1.0f);

    // --- ¡¡NUEVO!! Inicializar Minimapa ---
    // (El tamaño del mundo es 3000x3000. Una escala de 0.07 lo hace 210x210)
    minimapaZoom = 0.07f;
    // Lo posicionamos en la esquina inf-der con un margen
    minimapaOffset = {
        Constantes::ANCHO_PANTALLA - (3000 * minimapaZoom) - 10, // x
        Constantes::ALTO_PANTALLA - (3000 * minimapaZoom) - 10  // y
    };

    // Creamos el lienzo del minimapa (un poco mas grande que el mapa para los bordes)
    // --- ¡¡MODIFICADO!! Creamos AMBAS texturas ---
    minimapaTextura = LoadRenderTexture( (int)(3200 * minimapaZoom), (int)(3200 * minimapaZoom) );
    nieblaMinimapa = LoadRenderTexture( (int)(3200 * minimapaZoom), (int)(3200 * minimapaZoom) );
    // ------------------------------------
}

SistemaRender::~SistemaRender()
{
    UnloadRenderTexture(linterna.mask);
    UnloadRenderTexture(minimapaTextura); // ¡¡MODIFICADO!!
    UnloadRenderTexture(nieblaMinimapa);  // ¡¡AÑADIDO!!
}

// --- ¡¡FUNCION MODIFICADA!! ---
void SistemaRender::inicializarMinimapa(Mapa& mapa)
{
    // Creamos una camara especial solo para el minimapa
    // Centrada en 0,0 y con el zoom alejado
    Camera2D minimapaCamera = { 0 };
    minimapaCamera.target = { 0, 0 };
    minimapaCamera.offset = { (3200 * minimapaZoom) / 2, (3200 * minimapaZoom) / 2 }; // Centramos la textura
    minimapaCamera.zoom = minimapaZoom;

    // --- 1. Dibujamos el mapa ESTATICO ---
    BeginTextureMode(minimapaTextura);
        ClearBackground(Fade(BLACK, 0.5f)); // Fondo semitransparente
        BeginMode2D(minimapaCamera);

            // Dibujamos solo los obstaculos, sin items ni enemigos
            for (const auto& muro : mapa.getMuros()) {
                DrawRectangleRec(muro, DARKGRAY);
            }
            for (const auto& caja : mapa.getCajas()) {
                DrawRectangleRec(caja, DARKBROWN);
            }

            // Dibujamos los picaportes (¡importante para la orientacion!)
            if (!mapa.estaPuertaAbierta())
            {
                Rectangle puerta = mapa.getPuertaJefe();
                float handleSize = 10;
                Rectangle picaporteIzq = { puerta.x + (puerta.width/2) - handleSize - 5, puerta.y + (puerta.height / 2) - (handleSize / 2), handleSize, handleSize };
                Rectangle picaporteDer = { puerta.x + (puerta.width/2) + 5, puerta.y + (puerta.height / 2) - (handleSize / 2), handleSize, handleSize };
                DrawRectangleRec(picaporteIzq, GOLD);
                DrawRectangleRec(picaporteDer, GOLD);
            }

        EndMode2D();
    EndTextureMode();

    // --- ¡¡AÑADIDO!! 2. Inicializamos la NIEBLA ---
    // La empezamos completamente en NEGRO (oculto)
    BeginTextureMode(nieblaMinimapa);
        ClearBackground(BLACK);
    EndTextureMode();
    // ---------------------------------------------
}
// -----------------------------

// --- ¡¡NUEVA FUNCION!! ---
void SistemaRender::actualizarNieblaMinimapa(const Protagonista& jugador)
{
    // 1. Recrear la camara del minimapa (centrada en 0,0)
    // Es vital que sea identica a la de inicializarMinimapa
    Camera2D minimapaCamera = { 0 };
    minimapaCamera.target = { 0, 0 };
    minimapaCamera.offset = { (3200 * minimapaZoom) / 2, (3200 * minimapaZoom) / 2 };
    minimapaCamera.zoom = minimapaZoom;

    // 2. Dibujar sobre la textura de la niebla SIN borrar
    // Esto hace que la revelacion sea permanente y acumulativa
    BeginTextureMode(nieblaMinimapa);
        BeginMode2D(minimapaCamera);

            // 3. Dibujar el "aura" de proximidad del jugador
            // Usamos un color blanco muy tenue. Se acumulara
            // con el tiempo, creando el efecto "fade-in".
            float radioProximidad = 100.0f; // 100 unidades del mundo
            DrawCircleV(jugador.getPosicion(), radioProximidad, Fade(WHITE, 0.05f)); // 5% alpha

            // 4. Dibujar el cono de la linterna
            float alcance = jugador.getAlcanceLinterna();
            if (alcance > 0.0f)
            {
                float angulo = jugador.getAnguloCono(); // El angulo *mitad*
                float anguloVista = jugador.getAnguloVista(); // Angulo en grados

                // Dibujamos un "anillo" (un cono)
                DrawRing(
                    jugador.getPosicion(),       // center
                    radioProximidad * 0.8f,      // innerRadius (mas chico que el aura)
                    alcance,                     // outerRadius
                    anguloVista - angulo,        // startAngle
                    anguloVista + angulo,        // endAngle
                    16,                          // segments
                    Fade(WHITE, 0.05f)           // color (tambien 5% alpha)
                );
            }

        EndMode2D();
    EndTextureMode();
}
// --------------------------


Camera2D SistemaRender::getCamera() const
{
    return camera;
}

void SistemaRender::dibujarTodo(Protagonista& jugador, Mapa& mapa, GestorEntidades& gestor)
{
    camera.target = jugador.getPosicion();
    Iluminacion::MoveLight(&linterna, jugador.getPosicion());

    float alcanceCono = jugador.getAlcanceLinterna();
    float radioHalo = 80.0f;
    linterna.radius = (alcanceCono > radioHalo) ? alcanceCono : radioHalo;

    Iluminacion::UpdateLightShadows(
        &linterna,
        mapa.getMuros(),
        mapa.getPuertaJefe(),
        mapa.estaPuertaAbierta(),
        camera,
        jugador
    );

    // BeginDrawing() y EndDrawing() estan en Juego.cpp

    BeginMode2D(camera);
        dibujarMundo(mapa, gestor, jugador);
    EndMode2D();

    // --- APLICAMOS LA MASCARA DE LUZ ---
    BeginBlendMode(BLEND_ALPHA);
    DrawTextureRec(
        linterna.mask.texture,
        (Rectangle){0, 0, (float)linterna.mask.texture.width, (float)-linterna.mask.texture.height},
        Vector2Zero(),
        BLACK
    );
    EndBlendMode();
    // ------------------------------------

    // --- ¡¡NUEVO!! EFECTOS DE POCA VIDA (VENAS Y GLITCH) ---
    int vidaActual = jugador.getVida();
    if (vidaActual <= 5 && jugador.estaVivo())
    {
        // --- 1. EFECTO "VENAS VIOLETAS" (Lore) ---
        // Intensidad: 0.0 (5 vida) a 1.0 (0 vida).
        // ¡¡CORREGIDO!! El maximo de vida es 10. El umbral es 5.
        // Queremos que con 5 de vida la intensidad sea baja, y con 1 sea alta.
        float intensidad = 1.0f - ((float)(vidaActual - 1) / 4.0f); // 0.0 (con 5 vida) a 1.0 (con 1 vida)
        if (intensidad < 0.0f) intensidad = 0.0f;
        if (intensidad > 1.0f) intensidad = 1.0f;

        float grosor = 1.0f + (intensidad * 4.0f); // Grosor de 1 a 5
        float alfa = 0.2f + (intensidad * 0.5f); // Alfa de 20% a 70%
        Color colorVenas = Fade(DARKPURPLE, alfa);

        Vector2 centro = { Constantes::ANCHO_PANTALLA / 2, Constantes::ALTO_PANTALLA / 2 };
        float t = GetTime() * 0.5f; // Tiempo para la animacion de "pulso"

        // Dibujamos 8 venas (Bezier Cuadraticas) desde las esquinas/lados
        Vector2 anclas[8] = {
            {0, 0}, {Constantes::ANCHO_PANTALLA / 2, 0}, {Constantes::ANCHO_PANTALLA, 0},
            {Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA / 2},
            {Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA},
            {Constantes::ANCHO_PANTALLA / 2, Constantes::ALTO_PANTALLA},
            {0, Constantes::ALTO_PANTALLA},
            {0, Constantes::ALTO_PANTALLA / 2}
        };

        // Puntos de control (para la curva) que "vibran"
        Vector2 control[8] = {
            {150 + sin(t*1.1f)*50, 150 + cos(t*1.2f)*50}, {centro.x + sin(t)*100, 200 + cos(t)*100}, {1024-150 - sin(t*1.3f)*50, 150 + cos(t*1.1f)*50},
            {1024-200 - sin(t)*100, centro.y + cos(t)*100}, {1024-150 - sin(t*1.2f)*50, 768-150 - cos(t*1.3f)*50},
            {centro.x - sin(t)*100, 768-200 - cos(t)*100}, {150 + sin(t*1.1f)*50, 768-150 - cos(t*1.2f)*50},
            {200 + sin(t)*100, centro.y - cos(t)*100}
        };

        // --- ¡¡ERROR CORREGIDO!! Bucle manual para Bezier (no usa DrawLineBezierQuad) ---
        for (int i = 0; i < 8; i++)
        {
            Vector2 ancla = anclas[i];
            Vector2 ctrl = control[i];
            Vector2 p_anterior = ancla;

            for (int j = 1; j <= 10; j++) // 10 segmentos por vena
            {
                float t_step = (float)j / 10.0f;

                // Formula de Bezier Cuadratica usando Lerp (Vector2Lerp anidado)
                Vector2 p_lerp_1 = Vector2Lerp(ancla, ctrl, t_step);
                Vector2 p_lerp_2 = Vector2Lerp(ctrl, centro, t_step);
                Vector2 p_actual = Vector2Lerp(p_lerp_1, p_lerp_2, t_step);

                DrawLineEx(p_anterior, p_actual, grosor, colorVenas);
                p_anterior = p_actual; // La siguiente linea empieza donde termino esta
            }
        }
        // --- FIN DE LA CORRECCION ---

        // --- 2. EFECTO "GLITCH" (a 2 de vida o menos) ---
        if (vidaActual <= 2)
        {
            // 10% de probabilidad cada frame de mostrar un glitch
            if (GetRandomValue(0, 100) > 90)
            {
                int numGlitches = GetRandomValue(1, 4);
                for (int i = 0; i < numGlitches; i++)
                {
                    int x = GetRandomValue(0, Constantes::ANCHO_PANTALLA);
                    int y = GetRandomValue(0, Constantes::ALTO_PANTALLA);
                    int w = GetRandomValue(20, 100);
                    int h = GetRandomValue(5, 30);
                    Color c = (GetRandomValue(0, 1) == 0) ? RED : (Color){0, 255, 128, 255}; // Rojo o Verde-Terminal
                    DrawRectangle(x, y, w, h, Fade(c, 0.8f));
                }
            }
        }
    }
    // --- FIN DE EFECTOS POCA VIDA ---


    // --- DIBUJO DE FANTASMAS (Susto y Despertando) ---
    BeginMode2D(camera);
    for (Enemigo* enemigo : gestor.getEnemigos())
    {
        if (dynamic_cast<Fantasma*>(enemigo))
        {
            if (Fantasma::estaAsustando && !Fantasma::despertado)
            {
                enemigo->dibujar();
            }
            if (Fantasma::estaDespertando)
            {
                enemigo->dibujar();
            }
        }
    }
    EndMode2D();
    // --- FIN DEL BLOQUE ---


    // --- ¡¡BLOQUE DE MINIMAPA MODIFICADO!! ---

    // 1. Dibujar el mapa (la textura que ya creamos)
    DrawTextureRec(
        minimapaTextura.texture,
        (Rectangle){ 0, 0, (float)minimapaTextura.texture.width, (float)-minimapaTextura.texture.height }, // Invertir Y
        minimapaOffset, // Posicion en pantalla
        WHITE
    );

    // 2. ¡¡NUEVO!! Aplicar la niebla de guerra
    // Usamos BLEND_MULTIPLY: Mapa * Niebla.
    BeginBlendMode(BLEND_MULTIPLIED);
    DrawTextureRec(
        nieblaMinimapa.texture,
        (Rectangle){ 0, 0, (float)nieblaMinimapa.texture.width, (float)-nieblaMinimapa.texture.height }, // Invertir Y
        minimapaOffset, // Posicion en pantalla
        WHITE           // La textura ya tiene el color (negro -> blanco)
    );
    EndBlendMode();

    // 3. Dibujar el borde (despues de la niebla)
    DrawRectangleLinesEx(
        (Rectangle){minimapaOffset.x, minimapaOffset.y, (float)minimapaTextura.texture.width, (float)minimapaTextura.texture.height},
        1.0f,
        GRAY
    ); // Borde

    // 4. Calcular la posicion del jugador en el minimapa
    Vector2 posJugadorEnMapa = Vector2Scale(jugador.getPosicion(), minimapaZoom);
    Vector2 centroMinimapa = { (3200 * minimapaZoom) / 2, (3200 * minimapaZoom) / 2 };
    Vector2 posFinalJugador = Vector2Add(Vector2Add(posJugadorEnMapa, centroMinimapa), minimapaOffset);

    // 5. Dibujar al jugador (punto rojo)
    DrawCircleV(posFinalJugador, 3.0f, RED);

    // ------------------------------------------

    // --- ¡¡NUEVO!! EFECTO VIGNETTE DE DAÑO (CORREGIDO) ---
    float tiempoInmune = jugador.getTiempoInmune();
    if (tiempoInmune > 0.0f)
    {
        // El timer va de 1.0 a 0.0. Lo usamos como alfa.
        // Usamos MAROON (bordo) en lugar de RED brillante.
        // Multiplicamos por 0.8 para que no sea 100% opaco en el pico.
        float alpha = tiempoInmune * 0.8f;
        Color colorBorde = Fade(MAROON, alpha);
        Color colorCentro = Fade(BLANK, 0.0f); // Centro transparente

        // Dibujamos 4 gradientes para crear la viñeta
        int grosorBorde = 200; // Grosor del borde en pixeles

        // Arriba
        DrawRectangleGradientV(0, 0, Constantes::ANCHO_PANTALLA, grosorBorde, colorBorde, colorCentro);
        // Abajo
        DrawRectangleGradientV(0, Constantes::ALTO_PANTALLA - grosorBorde, Constantes::ANCHO_PANTALLA, grosorBorde, colorCentro, colorBorde);
        // Izquierda
        DrawRectangleGradientH(0, 0, grosorBorde, Constantes::ALTO_PANTALLA, colorBorde, colorCentro);
        // Derecha
        DrawRectangleGradientH(Constantes::ANCHO_PANTALLA - grosorBorde, 0, grosorBorde, Constantes::ALTO_PANTALLA, colorCentro, colorBorde);
    }
    // --------------------------------------

    dibujarHUD(jugador);
}


void SistemaRender::dibujarMundo(Mapa& mapa, GestorEntidades& gestor, Protagonista& jugador)
{
    mapa.dibujar();
    gestor.dibujarEntidades();
    jugador.dibujar();
}

void SistemaRender::dibujarHUD(Protagonista& jugador)
{
    DrawText(TextFormat("Vida: %d", (int)jugador.getVida()), 10, 10, 20, LIME);
    DrawText(TextFormat("Municion: %d", jugador.getMunicion()), 10, 30, 20, SKYBLUE);
    DrawText(TextFormat("Bateria: %d", jugador.getBateria()), 10, 50, 20, YELLOW);
    DrawText(TextFormat("Llave: %s", jugador.getTieneLlave() ? "SI" : "NO"), 10, 70, 20, ORANGE);

    if (!jugador.estaVivo()) {
        DrawText("GAME OVER", Constantes::ANCHO_PANTALLA / 2 - 150, Constantes::ALTO_PANTALLA / 2 - 50, 60, RED);
    }
}
