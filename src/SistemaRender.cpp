#include "SistemaRender.h"
#include "Constantes.h"
#include "Fantasma.h"
#include "raymath.h" // Para Vector2Add

SistemaRender::SistemaRender() : camera({0})
{
    camera.offset = (Vector2){ Constantes::ANCHO_PANTALLA / 2.0f, Constantes::ALTO_PANTALLA / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Volvemos a la inicialización simple. El radio se calculará en dibujarTodo.
    float alcanceInicial = 300.0f;
    Iluminacion::SetupLight(&linterna, {0,0}, WHITE, alcanceInicial, 1.0f);

    // --- Inicializar Minimapa ---
    minimapaZoom = 0.07f;
    minimapaOffset = {
        Constantes::ANCHO_PANTALLA - (3200 * minimapaZoom) - 10, // x
        Constantes::ALTO_PANTALLA - (3200 * minimapaZoom) - 10  // y
    };
    minimapaTextura = LoadRenderTexture( (int)(3200 * minimapaZoom), (int)(3200 * minimapaZoom) );
    nieblaMinimapa = LoadRenderTexture( (int)(3200 * minimapaZoom), (int)(3200 * minimapaZoom) );
}

SistemaRender::~SistemaRender()
{
    UnloadRenderTexture(linterna.mask);
    UnloadRenderTexture(minimapaTextura);
    UnloadRenderTexture(nieblaMinimapa);
}

Rectangle SistemaRender::getCameraViewRect(const Camera2D& cam)
{
    Vector2 topLeft = GetScreenToWorld2D({0, 0}, cam);
    float viewWidth = Constantes::ANCHO_PANTALLA / cam.zoom;
    float viewHeight = Constantes::ALTO_PANTALLA / cam.zoom;

    return { topLeft.x, topLeft.y, viewWidth, viewHeight };
}


void SistemaRender::inicializarMinimapa(Mapa& mapa)
{
    Camera2D minimapaCamera = { 0 };
    minimapaCamera.target = { 0, 0 };
    minimapaCamera.offset = { (3200 * minimapaZoom) / 2, (3200 * minimapaZoom) / 2 };
    minimapaCamera.zoom = minimapaZoom;

    BeginTextureMode(minimapaTextura);
        ClearBackground(Fade(BLACK, 0.5f));
        BeginMode2D(minimapaCamera);

            for (const auto& muro : mapa.getMuros()) {
                DrawRectangleRec(muro, DARKGRAY);
            }
            for (const auto& caja : mapa.getCajas()) {
                DrawRectangleRec(caja, DARKBROWN);
            }

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

    BeginTextureMode(nieblaMinimapa);
        ClearBackground(BLACK);
    EndTextureMode();
}

void SistemaRender::actualizarNieblaMinimapa(const Protagonista& jugador)
{
    Camera2D minimapaCamera = { 0 };
    minimapaCamera.target = { 0, 0 };
    minimapaCamera.offset = { (3200 * minimapaZoom) / 2, (3200 * minimapaZoom) / 2 };
    minimapaCamera.zoom = minimapaZoom;

    BeginTextureMode(nieblaMinimapa);
        BeginMode2D(minimapaCamera);

            float radioProximidad = 100.0f;
            DrawCircleV(jugador.getPosicion(), radioProximidad, Fade(WHITE, 0.05f));

            // Esta lógica es correcta, asumimos que Protagonista
            // ahora devolverá los valores ajustados por la batería.
            float alcance = jugador.getAlcanceLinterna();
            if (alcance > 0.0f)
            {
                float angulo = jugador.getAnguloCono();
                float anguloVista = jugador.getAnguloVista();

                DrawRing(
                    jugador.getPosicion(),
                    radioProximidad * 0.8f,
                    alcance,
                    anguloVista - angulo,
                    anguloVista + angulo,
                    16,
                    Fade(WHITE, 0.05f)
                );
            }

        EndMode2D();
    EndTextureMode();
}


Camera2D SistemaRender::getCamera() const
{
    return camera;
}

void SistemaRender::dibujarTodo(Protagonista& jugador, Mapa& mapa, GestorEntidades& gestor)
{
    camera.target = jugador.getPosicion();
    Iluminacion::MoveLight(&linterna, jugador.getPosicion());

    // --- LÓGICA DE BATERÍA (REVERTIDA) ---
    // Esta es la lógica que tenías antes.
    // Ahora, 'getAlcanceLinterna()' debe ser modificado DENTRO
    // de Protagonista para que devuelva el valor según la batería.
    float alcanceCono = jugador.getAlcanceLinterna();
    float radioHalo = 80.0f; // Este era el valor mínimo que tenías
    linterna.radius = (alcanceCono > radioHalo) ? alcanceCono : radioHalo;

    // La línea que daba error (linterna.fov = ...) se ha eliminado.
    // ------------------------------------

    Rectangle cameraView = getCameraViewRect(camera);

    Iluminacion::UpdateLightShadows(
        &linterna,
        mapa.getMuros(), // <-- FIX DE SOMBRAS (CORRECTO)
        mapa.getPuertaJefe(),
        mapa.estaPuertaAbierta(),
        camera,
        jugador // El sistema de iluminación leerá el ángulo desde 'jugador'
    );

    BeginMode2D(camera);
        dibujarMundo(cameraView, mapa, gestor, jugador);
    EndMode2D();

    BeginBlendMode(BLEND_ALPHA);
    DrawTextureRec(
        linterna.mask.texture,
        (Rectangle){0, 0, (float)linterna.mask.texture.width, (float)-linterna.mask.texture.height},
        Vector2Zero(),
        BLACK
    );
    EndBlendMode();

    int vidaActual = jugador.getVida();
    if (vidaActual <= 5 && jugador.estaVivo())
    {
        float intensidad = 1.0f - ((float)vidaActual / 3.0f);
        float freq = 10.0f + (intensidad * 20.0f);
        float amp = 1.0f + (intensidad * 4.0f);
        float offset = sin(GetTime() * freq) * amp;

        BeginBlendMode(BLEND_ADDITIVE);
        DrawRectangle(offset, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, Fade((Color){255, 0, 100, 255}, 0.05f + (intensidad * 0.1f)));
        DrawRectangle(-offset, 0, Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, Fade((Color){0, 255, 200, 255}, 0.05f + (intensidad * 0.1f)));
        EndBlendMode();

        if (vidaActual <= 2)
        {
            if (GetRandomValue(0, 100) > 90)
            {
                int numGlitches = GetRandomValue(1, 4);
                for (int i = 0; i < numGlitches; i++)
                {
                    int x = GetRandomValue(0, Constantes::ANCHO_PANTALLA);
                    int y = GetRandomValue(0, Constantes::ALTO_PANTALLA);
                    int w = GetRandomValue(20, 100);
                    int h = GetRandomValue(5, 30);
                    Color c = (GetRandomValue(0, 1) == 0) ? RED : (Color){0, 255, 128, 255};
                    DrawRectangle(x, y, w, h, Fade(c, 0.8f));
                }
            }
        }
    }

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

    DrawTextureRec(
        minimapaTextura.texture,
        (Rectangle){ 0, 0, (float)minimapaTextura.texture.width, (float)-minimapaTextura.texture.height },
        minimapaOffset,
        WHITE
    );

    BeginBlendMode(BLEND_MULTIPLIED);
    DrawTextureRec(
        nieblaMinimapa.texture,
        (Rectangle){ 0, 0, (float)nieblaMinimapa.texture.width, (float)-nieblaMinimapa.texture.height },
        minimapaOffset,
        WHITE
    );
    EndBlendMode();

    DrawRectangleLinesEx(
        (Rectangle){minimapaOffset.x, minimapaOffset.y, (float)minimapaTextura.texture.width, (float)minimapaTextura.texture.height},
        1.0f,
        GRAY
    );

    Vector2 posJugadorEnMapa = Vector2Scale(jugador.getPosicion(), minimapaZoom);
    Vector2 centroMinimapa = { (3200 * minimapaZoom) / 2, (3200 * minimapaZoom) / 2 };
    Vector2 posFinalJugador = Vector2Add(Vector2Add(posJugadorEnMapa, centroMinimapa), minimapaOffset);

    DrawCircleV(posFinalJugador, 3.0f, RED);

    float tiempoInmune = jugador.getTiempoInmune();
    if (tiempoInmune > 0.0f)
    {
        float alpha = tiempoInmune * 0.8f;
        Color colorBorde = Fade(MAROON, alpha);
        Color colorCentro = Fade(BLANK, 0.0f);
        int grosorBorde = 200;

        DrawRectangleGradientV(0, 0, Constantes::ANCHO_PANTALLA, grosorBorde, colorBorde, colorCentro);
        DrawRectangleGradientV(0, Constantes::ALTO_PANTALLA - grosorBorde, Constantes::ANCHO_PANTALLA, grosorBorde, colorCentro, colorBorde);
        DrawRectangleGradientH(0, 0, grosorBorde, Constantes::ALTO_PANTALLA, colorBorde, colorCentro);
        DrawRectangleGradientH(Constantes::ANCHO_PANTALLA - grosorBorde, 0, grosorBorde, Constantes::ALTO_PANTALLA, colorCentro, colorBorde);
    }

    dibujarHUD(jugador);
}


void SistemaRender::dibujarMundo(const Rectangle& cameraView, Mapa& mapa, GestorEntidades& gestor, Protagonista& jugador)
{
    mapa.dibujarPiso();

    for (const auto& muro : mapa.getMuros()) {
        if (CheckCollisionRecs(cameraView, muro)) {
            DrawRectangleRec(muro, DARKGRAY);
        }
    }
    for (const auto& caja : mapa.getCajas()) {
        if (CheckCollisionRecs(cameraView, caja)) {
            DrawRectangleRec(caja, DARKBROWN);
        }
    }

    gestor.dibujarEntidades();
    jugador.dibujar(); // <--- Esta línea llama al Protagonista::dibujar() modificado
}

void SistemaRender::dibujarHUD(Protagonista& jugador)
{
    DrawText(TextFormat("Vida: %d", (int)jugador.getVida()), 10, 10, 20, LIME);
    DrawText(TextFormat("Municion: %d", jugador.getMunicion()), 10, 30, 20, SKYBLUE);
    DrawText(TextFormat("Bateria: %d", (int)jugador.getBateria()), 10, 50, 20, YELLOW);
    DrawText(TextFormat("Llave: %s", jugador.getTieneLlave() ? "SI" : "NO"), 10, 70, 20, ORANGE);

    if (!jugador.estaVivo()) {
        DrawText("GAME OVER", Constantes::ANCHO_PANTALLA / 2 - 150, Constantes::ALTO_PANTALLA / 2 - 50, 60, RED);
    }
}
