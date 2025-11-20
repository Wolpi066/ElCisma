#include "SistemaRender.h"
#include "Constantes.h"
#include "Fantasma.h"
#include "raymath.h"

SistemaRender::SistemaRender() : camera({0})
{
    camera.offset = (Vector2){ Constantes::ANCHO_PANTALLA / 2.0f, Constantes::ALTO_PANTALLA / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    float alcanceInicial = 300.0f;
    Iluminacion::SetupLight(&linterna, {0,0}, WHITE, alcanceInicial, 1.0f);

    minimapaZoom = 0.07f;
    minimapaOffset = {
        Constantes::ANCHO_PANTALLA - (3200 * minimapaZoom) - 10,
        Constantes::ALTO_PANTALLA - (3200 * minimapaZoom) - 10
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

            Rectangle p = mapa.getPuertaJefe();
            if (!mapa.estaPuertaAbierta()) DrawRectangleRec(p, GOLD);
            else DrawRectangleLinesEx(p, 1.0f, GREEN);

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

Camera2D SistemaRender::getCamera() const { return camera; }

// --- CÁLCULO DE LUZ (ESTRICTO: SOLO CONO) ---
float CalcularIntensidadLuzEnObjeto(const Protagonista& jugador, Rectangle objeto)
{
    float alcance = jugador.getAlcanceLinterna();
    float anguloCono = jugador.getAnguloCono();

    if (alcance <= 0.1f || anguloCono <= 0.01f) return 0.0f;

    Vector2 centroObjeto = { objeto.x + objeto.width/2, objeto.y + objeto.height/2 };
    Vector2 vectorHaciaObjeto = Vector2Subtract(centroObjeto, jugador.getPosicion());
    float distancia = Vector2Length(vectorHaciaObjeto);

    if (distancia > alcance) return 0.0f;

    // NOTA: Eliminado el check de Halo (proximidad) para que sea oscuridad total si no apuntas.

    // Check Ángulo
    Vector2 dirHaciaObjeto = Vector2Normalize(vectorHaciaObjeto);
    Vector2 dirVista = jugador.getDireccionVista();

    float dot = Vector2DotProduct(dirVista, dirHaciaObjeto);

    // Convertimos ángulo (radianes) a umbral de coseno
    float umbralAngulo = cosf(anguloCono);

    if (dot >= umbralAngulo) {
        // Factor de atenuación en los bordes del cono para suavizar
        float factorBorde = (dot - umbralAngulo) / (1.0f - umbralAngulo);
        if (factorBorde > 1.0f) factorBorde = 1.0f;
        return factorBorde;
    }

    return 0.0f;
}

void SistemaRender::dibujarTodo(Protagonista& jugador, Mapa& mapa, GestorEntidades& gestor)
{
    camera.target = jugador.getPosicion();
    Iluminacion::MoveLight(&linterna, jugador.getPosicion());

    float alcanceCono = jugador.getAlcanceLinterna();
    float radioHalo = 80.0f;
    linterna.radius = (alcanceCono > radioHalo) ? alcanceCono : radioHalo;

    // 1. CALCULAR SOMBRAS
    Iluminacion::UpdateLightShadows(
        &linterna,
        mapa.getMuros(),
        mapa.getPuertaJefe(),
        mapa.estaPuertaAbierta(),
        camera,
        jugador
    );

    // 2. CAPA MUNDO BASE
    BeginMode2D(camera);
        mapa.dibujar();
        gestor.dibujarEntidades();
        // (Jugador no se dibuja aquí)
    EndMode2D();

    // 3. APLICAR MÁSCARA DE LUZ
    BeginBlendMode(BLEND_ALPHA);
    DrawTextureRec(
        linterna.mask.texture,
        (Rectangle){0, 0, (float)linterna.mask.texture.width, (float)-linterna.mask.texture.height},
        Vector2Zero(),
        BLACK
    );
    EndBlendMode();

    // 4. CAPA OVERLAY (Solo iluminados)
    BeginMode2D(camera);

        // Lógica "Sandwich": Solo dibujamos la puerta si el cono la ilumina
        float intensidadPuerta = CalcularIntensidadLuzEnObjeto(jugador, mapa.getPuertaJefe());

        if (intensidadPuerta > 0.0f) {
            mapa.dibujarPuerta(intensidadPuerta);
        }

        jugador.dibujar();
    EndMode2D();


    // 5. HUD Y EFECTOS
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
    }

    BeginMode2D(camera);
    for (Enemigo* enemigo : gestor.getEnemigos())
    {
        if (dynamic_cast<Fantasma*>(enemigo))
        {
            if (Fantasma::estaAsustando && !Fantasma::despertado) enemigo->dibujar();
            if (Fantasma::estaDespertando) enemigo->dibujar();
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
    DrawRectangleLinesEx((Rectangle){minimapaOffset.x, minimapaOffset.y, (float)minimapaTextura.texture.width, (float)minimapaTextura.texture.height}, 1.0f, GRAY);

    Vector2 posJugadorEnMapa = Vector2Scale(jugador.getPosicion(), minimapaZoom);
    Vector2 centroMinimapa = { (3200 * minimapaZoom) / 2, (3200 * minimapaZoom) / 2 };
    Vector2 posFinalJugador = Vector2Add(Vector2Add(posJugadorEnMapa, centroMinimapa), minimapaOffset);
    DrawCircleV(posFinalJugador, 3.0f, RED);

    dibujarHUD(jugador);
}

void SistemaRender::dibujarMundo(const Rectangle& cameraView, Mapa& mapa, GestorEntidades& gestor, Protagonista& jugador)
{
    mapa.dibujar();
    gestor.dibujarEntidades();
}

void SistemaRender::dibujarHUD(Protagonista& jugador)
{
    DrawText(TextFormat("Vida: %d", (int)jugador.getVida()), 10, 10, 20, LIME);
    DrawText(TextFormat("Municion: %d", jugador.getMunicion()), 10, 30, 20, SKYBLUE);
    DrawText(TextFormat("Bateria: %d", (int)jugador.getBateria()), 10, 50, 20, YELLOW);
    DrawText(TextFormat("Llave: %s", jugador.getTieneLlave() ? "SI" : "NO"), 10, 70, 20, ORANGE);
}
