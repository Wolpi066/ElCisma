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
    minimapaTextura = LoadRenderTexture( (int)(3200 * minimapaZoom), (int)(3200 * minimapaZoom) );
    // ------------------------------------
}

SistemaRender::~SistemaRender()
{
    UnloadRenderTexture(linterna.mask);
    UnloadRenderTexture(minimapaTextura); // ¡¡NUEVO!!
}

// --- ¡¡NUEVA FUNCION!! ---
void SistemaRender::inicializarMinimapa(Mapa& mapa)
{
    // Creamos una camara especial solo para el minimapa
    // Centrada en 0,0 y con el zoom alejado
    Camera2D minimapaCamera = { 0 };
    minimapaCamera.target = { 0, 0 };
    minimapaCamera.offset = { (3200 * minimapaZoom) / 2, (3200 * minimapaZoom) / 2 }; // Centramos la textura
    minimapaCamera.zoom = minimapaZoom;

    // Dibujamos el mapa COMPLETO una sola vez en nuestra textura
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
}
// -----------------------------


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


    // --- ¡¡NUEVO!! DIBUJAR MINIMAPA Y JUGADOR ---
    // (Dibujamos fuera de cualquier modo de camara, en pixeles de pantalla)

    // 1. Dibujar el mapa (la textura que ya creamos)
    DrawTextureRec(
        minimapaTextura.texture,
        (Rectangle){ 0, 0, (float)minimapaTextura.texture.width, (float)-minimapaTextura.texture.height }, // Invertir Y
        minimapaOffset, // Posicion en pantalla
        WHITE
    );
    DrawRectangleLinesEx(
        (Rectangle){minimapaOffset.x, minimapaOffset.y, (float)minimapaTextura.texture.width, (float)minimapaTextura.texture.height},
        1.0f,
        GRAY
    ); // Borde

    // 2. Calcular la posicion del jugador en el minimapa
    // (Posicion_Mundo * zoom) + (Centro_Mundo * zoom) + Offset_Pantalla
    Vector2 posJugadorEnMapa = Vector2Scale(jugador.getPosicion(), minimapaZoom);
    Vector2 centroMinimapa = { (3200 * minimapaZoom) / 2, (3200 * minimapaZoom) / 2 };
    Vector2 posFinalJugador = Vector2Add(Vector2Add(posJugadorEnMapa, centroMinimapa), minimapaOffset);

    // 3. Dibujar al jugador (punto rojo)
    DrawCircleV(posFinalJugador, 3.0f, RED);

    // ------------------------------------------

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
