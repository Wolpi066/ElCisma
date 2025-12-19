#include "SistemaRender.h"
#include "Constantes.h"
#include "Fantasma.h"
#include "raymath.h"
#include <string>
#include <cstdio>


// HELPERS UI
void DibujarMarcoTech(Rectangle rect, Color colorBorde, const char* titulo = nullptr) {
    DrawRectangleGradientV((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height, Fade(BLACK, 0.85f), Fade(DARKBLUE, 0.40f));

    for(int y = (int)rect.y; y < (int)(rect.y + rect.height); y+=4) {
        DrawLine((int)rect.x, y, (int)(rect.x+rect.width), y, Fade(colorBorde, 0.05f));
    }

    DrawRectangleLinesEx(rect, 1.0f, Fade(colorBorde, 0.5f));

    float thick = 2.0f;
    float cornerLen = 10.0f;
    Color cCorner = Fade(colorBorde, 0.9f);

    DrawRectangle((int)rect.x, (int)rect.y, (int)cornerLen, (int)thick, cCorner);
    DrawRectangle((int)rect.x, (int)rect.y, (int)thick, (int)cornerLen, cCorner);
    DrawRectangle((int)(rect.x + rect.width - cornerLen), (int)rect.y, (int)cornerLen, (int)thick, cCorner);
    DrawRectangle((int)(rect.x + rect.width - thick), (int)rect.y, (int)thick, (int)cornerLen, cCorner);
    DrawRectangle((int)rect.x, (int)(rect.y + rect.height - thick), (int)cornerLen, (int)thick, cCorner);
    DrawRectangle((int)rect.x, (int)(rect.y + rect.height - cornerLen), (int)thick, (int)cornerLen, cCorner);
    DrawRectangle((int)(rect.x + rect.width - cornerLen), (int)(rect.y + rect.height - thick), (int)cornerLen, (int)thick, cCorner);
    DrawRectangle((int)(rect.x + rect.width - thick), (int)(rect.y + rect.height - cornerLen), (int)thick, (int)cornerLen, cCorner);

    if (titulo) {
        int textW = MeasureText(titulo, 10);
        DrawRectangle((int)rect.x, (int)rect.y - 12, textW + 8, 12, cCorner);
        DrawText(titulo, (int)rect.x + 4, (int)rect.y - 10, 10, BLACK);
    }
}

void DrawTextDigital(const char* text, int x, int y, int fontSize, Color color) {
    int textW = MeasureText(text, fontSize);
    DrawRectangle(x - 3, y - 1, textW + 6, fontSize + 2, Fade(BLACK, 0.7f));
    DrawText(text, x + 1, y + 1, fontSize, BLACK);
    DrawText(text, x, y, fontSize, color);
}

void DrawTextShadow(const char* text, int x, int y, int fontSize, Color color) {
    DrawText(text, x + 1, y + 1, fontSize, BLACK);
    DrawText(text, x, y, fontSize, color);
}

SistemaRender::SistemaRender() : camera({0})
{
    camera.offset = (Vector2){ Constantes::ANCHO_PANTALLA / 2.0f, Constantes::ALTO_PANTALLA / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    float alcanceInicial = 300.0f;
    Iluminacion::SetupLight(&linterna, {0,0}, WHITE, alcanceInicial, 1.0f);

    minimapaZoom = 0.07f;
    minimapaOffset = {
        Constantes::ANCHO_PANTALLA - (3200 * minimapaZoom) - 20,
        Constantes::ALTO_PANTALLA - (3200 * minimapaZoom) - 20
    };
    minimapaTextura = LoadRenderTexture( (int)(3200 * minimapaZoom), (int)(3200 * minimapaZoom) );
    nieblaMinimapa = LoadRenderTexture( (int)(3200 * minimapaZoom), (int)(3200 * minimapaZoom) );

    texBateria[0] = LoadTexture("assets/HUD/BateriaMuerta.png");
    texBateria[1] = LoadTexture("assets/HUD/BateriaPeligro.png");
    texBateria[2] = LoadTexture("assets/HUD/BateriaBaja.png");
    texBateria[3] = LoadTexture("assets/HUD/BateriaNormal.png");
    texBateria[4] = LoadTexture("assets/HUD/BateriaBien.png");
    texBateria[5] = LoadTexture("assets/HUD/BateriaLlena.png");

    texVida[0] = LoadTexture("assets/HUD/Vida0.png");
    texVida[1] = LoadTexture("assets/HUD/Vida20.png");
    texVida[2] = LoadTexture("assets/HUD/Vida50.png");
    texVida[3] = LoadTexture("assets/HUD/Vida100.png");

    texIconoMunicion = LoadTexture("assets/HUD/IconoMunicion.png");
    texMinimapaFrame = LoadTexture("assets/HUD/MinimapaFrame.png");
}

SistemaRender::~SistemaRender()
{
    UnloadRenderTexture(linterna.mask);
    UnloadRenderTexture(minimapaTextura);
    UnloadRenderTexture(nieblaMinimapa);

    for(int i=0; i<6; i++) UnloadTexture(texBateria[i]);
    for(int i=0; i<4; i++) UnloadTexture(texVida[i]);
    UnloadTexture(texIconoMunicion);
    UnloadTexture(texMinimapaFrame);
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
            for (const auto& muro : mapa.getMuros()) DrawRectangleRec(muro, DARKGRAY);
            for (const auto& caja : mapa.getCajas()) DrawRectangleRec(caja, DARKBROWN);
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
            if (alcance > 0.0f) {
                float angulo = jugador.getAnguloCono();
                float anguloVista = jugador.getAnguloVista();
                DrawRing(jugador.getPosicion(), radioProximidad * 0.8f, alcance, anguloVista - angulo, anguloVista + angulo, 16, Fade(WHITE, 0.05f));
            }
        EndMode2D();
    EndTextureMode();
}

Camera2D SistemaRender::getCamera() const { return camera; }

float CalcularIntensidadLuzEnObjeto(const Protagonista& jugador, Rectangle objeto)
{
    float alcance = jugador.getAlcanceLinterna();
    float anguloCono = jugador.getAnguloCono();
    if (alcance <= 0.1f || anguloCono <= 0.01f) return 0.0f;

    Vector2 centroObjeto = { objeto.x + objeto.width/2, objeto.y + objeto.height/2 };
    Vector2 vectorHaciaObjeto = Vector2Subtract(centroObjeto, jugador.getPosicion());
    float distancia = Vector2Length(vectorHaciaObjeto);

    if (distancia > alcance) return 0.0f;
    Vector2 dirHaciaObjeto = Vector2Normalize(vectorHaciaObjeto);
    Vector2 dirVista = jugador.getDireccionVista();
    float dot = Vector2DotProduct(dirVista, dirHaciaObjeto);
    float umbralAngulo = cosf(anguloCono);

    if (dot >= umbralAngulo) {
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

    Iluminacion::UpdateLightShadows(&linterna, mapa.getMuros(), mapa.getPuertaJefe(), mapa.estaPuertaAbierta(), camera, jugador);

    BeginMode2D(camera);
        mapa.dibujar();
        gestor.dibujarEntidades();
    EndMode2D();

    BeginBlendMode(BLEND_ALPHA);
    DrawTextureRec(linterna.mask.texture, (Rectangle){0, 0, (float)linterna.mask.texture.width, (float)-linterna.mask.texture.height}, Vector2Zero(), BLACK);
    EndBlendMode();

    BeginMode2D(camera);
        float intensidadPuerta = CalcularIntensidadLuzEnObjeto(jugador, mapa.getPuertaJefe());
        if (intensidadPuerta > 0.0f) mapa.dibujarPuerta(intensidadPuerta);
        jugador.dibujar();
    EndMode2D();

    int vidaActual = jugador.getVida();
    if (vidaActual <= 3 && jugador.estaVivo()) {
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
    for (Enemigo* enemigo : gestor.getEnemigos()) {
        if (dynamic_cast<Fantasma*>(enemigo)) {
            if (Fantasma::estaAsustando && !Fantasma::despertado) enemigo->dibujar();
            if (Fantasma::estaDespertando) enemigo->dibujar();
        }
    }
    EndMode2D();

    DrawTextureRec(minimapaTextura.texture, (Rectangle){ 0, 0, (float)minimapaTextura.texture.width, (float)-minimapaTextura.texture.height }, minimapaOffset, WHITE);
    BeginBlendMode(BLEND_MULTIPLIED);
    DrawTextureRec(nieblaMinimapa.texture, (Rectangle){ 0, 0, (float)nieblaMinimapa.texture.width, (float)-nieblaMinimapa.texture.height }, minimapaOffset, WHITE);
    EndBlendMode();

    if (texMinimapaFrame.id != 0) {
        float frameScale = 1.25f;
        float frameW = (float)minimapaTextura.texture.width * frameScale;
        float frameH = (float)minimapaTextura.texture.height * frameScale;
        float offX = (frameW - minimapaTextura.texture.width) / 2.0f;
        float offY = (frameH - minimapaTextura.texture.height) / 2.0f;
        Rectangle destFrame = { minimapaOffset.x - offX, minimapaOffset.y - offY, frameW, frameH };
        DrawTexturePro(texMinimapaFrame, {0,0,(float)texMinimapaFrame.width,(float)texMinimapaFrame.height}, destFrame, {0,0}, 0.0f, WHITE);
    } else {
        DrawRectangleLinesEx((Rectangle){minimapaOffset.x, minimapaOffset.y, (float)minimapaTextura.texture.width, (float)minimapaTextura.texture.height}, 2.0f, DARKGRAY);
    }

    Vector2 posJugadorEnMapa = Vector2Scale(jugador.getPosicion(), minimapaZoom);
    Vector2 centroMinimapa = { (3200 * minimapaZoom) / 2, (3200 * minimapaZoom) / 2 };
    Vector2 posFinalJugador = Vector2Add(Vector2Add(posJugadorEnMapa, centroMinimapa), minimapaOffset);
    DrawCircleV(posFinalJugador, 3.0f, RED);

    dibujarHUD(jugador);
}

void SistemaRender::dibujarMundo(const Rectangle& cameraView, Mapa& mapa, GestorEntidades& gestor, Protagonista& jugador) {
    mapa.dibujar();
    gestor.dibujarEntidades();
}

void SistemaRender::dibujarHUD(Protagonista& jugador)
{
    float scaleIconLarge = 0.17f;
    float scaleIconBattery = 0.22f;
    float scaleAmmo = 0.09f;

    int margin = 20;
    Color colorCyan = (Color){0, 255, 255, 255};

    // PANEL ESTADO
    Rectangle rectStatus = { (float)margin, (float)margin, 200, 145 };
    DibujarMarcoTech(rectStatus, colorCyan, "STATUS");

    // VIDA
    int vida = jugador.getVida();
    int maxVida = Constantes::VIDA_MAX_JUGADOR;
    float pctVida = (float)vida / maxVida;
    int idxVida = (vida <= 0) ? 0 : (pctVida < 0.3f ? 1 : (pctVida < 0.6f ? 2 : 3));

    Texture2D tVida = texVida[idxVida];

    int iconX = (int)rectStatus.x + 5;
    int iconY = (int)rectStatus.y + 10;

    if (tVida.id != 0) {
        DrawTextureEx(tVida, {(float)iconX, (float)iconY}, 0.0f, scaleIconLarge, WHITE);
    } else {
        DrawTextDigital(TextFormat("HP: %d", vida), iconX, iconY, 20, RED);
    }

    // BATERÍA
    int bateria = jugador.getBateria();
    int idxBat = (bateria >= 80) ? 5 : (bateria >= 60 ? 4 : (bateria >= 40 ? 3 : (bateria >= 20 ? 2 : (bateria > 0 ? 1 : 0))));
    Texture2D tBat = texBateria[idxBat];

    int batX = iconX;
    int batY = (int)(rectStatus.y + rectStatus.height) - (int)(tBat.height * scaleIconBattery) - 5;

    if (tBat.id != 0) {
        // Icono Batería
        DrawTextureEx(tBat, {(float)batX, (float)batY}, 0.0f, scaleIconBattery, WHITE);

        // BARRA PWR
        int barX = batX + (int)(tBat.width * scaleIconBattery) + 15;
        int barY = batY + (int)((tBat.height * scaleIconBattery) / 2) + 5;

        int barW = 140;
        int barH = 16;

        float pctBat = (float)bateria / 100.0f;

        DrawRectangle(barX, barY, barW, barH, Fade(BLACK, 0.6f));
        DrawRectangle(barX + 2, barY + 2, (int)((barW - 4) * pctBat), barH - 4, YELLOW);
        DrawRectangleLines(barX, barY, barW, barH, Fade(YELLOW, 0.6f));

        DrawText("PWR", barX, barY - 12, 10, YELLOW);

    } else {
        DrawTextDigital(TextFormat("BAT: %d%%", bateria), batX, batY, 20, YELLOW);
    }

    // PANEL MUNICIÓN
    Rectangle rectAmmo = { (float)margin, (float)Constantes::ALTO_PANTALLA - 70 - margin, 220, 70 };
    DibujarMarcoTech(rectAmmo, colorCyan, "WEAPON");

    if (texIconoMunicion.id != 0) {
        float hMun = texIconoMunicion.height * scaleAmmo;
        float wMun = texIconoMunicion.width * scaleAmmo;

        // Icono
        int munX = (int)rectAmmo.x + 15;
        int munY = (int)(rectAmmo.y + (rectAmmo.height - hMun) / 2) + 5;

        DrawTextureEx(texIconoMunicion, {(float)munX, (float)munY}, 0.0f, scaleAmmo, WHITE);

        Color cMunicion = (jugador.getMunicion() < 5) ? RED : colorCyan;

        int textX = munX + (int)wMun + 20;
        DrawTextDigital(TextFormat("x %d", jugador.getMunicion()), textX, munY + 8, 30, cMunicion);
    } else {
        DrawTextDigital(TextFormat("BALAS: %d", jugador.getMunicion()), (int)rectAmmo.x + 10, (int)rectAmmo.y + 20, 20, SKYBLUE);
    }

    // TARJETA (Top-Right)
    if (jugador.getTieneLlave()) {
        const char* txt = "KEY";
        Rectangle rectKey = { (float)minimapaOffset.x - 60, (float)margin + 10, 50, 30 };
        DibujarMarcoTech(rectKey, GREEN, nullptr);
        DrawTextDigital(txt, (int)rectKey.x + 10, (int)rectKey.y + 8, 10, GREEN);
        if ((int)(GetTime()*4)%2==0) DrawRectangle((int)rectKey.x + 35, (int)rectKey.y + 10, 8, 8, GREEN);
    }
}
