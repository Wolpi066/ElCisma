#include "Iluminacion.h"
#include "Constantes.h"
#include "rlgl.h"
#include "Protagonista.h"

#ifndef RL_SRC_ALPHA
    #define RL_SRC_ALPHA 0x0302
#endif
#ifndef RL_MIN
    #define RL_MIN 0x8007
#endif
#ifndef RL_MAX
    #define RL_MAX 0x8008
#endif

void Iluminacion::ComputeShadowVolumeForEdge(LightInfo* light, Vector2 sp, Vector2 ep)
{
    if (light->shadowCount >= MAX_SHADOWS) return;
    float extension = light->radius * 3;
    Vector2 spVector = Vector2Normalize(Vector2Subtract(sp, light->position));
    Vector2 spProjection = Vector2Add(sp, Vector2Scale(spVector, extension));
    Vector2 epVector = Vector2Normalize(Vector2Subtract(ep, light->position));
    Vector2 epProjection = Vector2Add(ep, Vector2Scale(epVector, extension));
    light->shadows[light->shadowCount].vertices[0] = sp;
    light->shadows[light->shadowCount].vertices[1] = ep;
    light->shadows[light->shadowCount].vertices[2] = epProjection;
    light->shadows[light->shadowCount].vertices[3] = spProjection;
    light->shadowCount++;
}

void Iluminacion::DrawLightMask(LightInfo* light, Camera2D& camera, Protagonista& jugador)
{
    BeginTextureMode(light->mask);
        ClearBackground(WHITE);
        BeginMode2D(camera);

            rlSetBlendFactors(RL_SRC_ALPHA, RL_SRC_ALPHA, RL_MIN);
            rlSetBlendMode(BLEND_CUSTOM);

            if (light->valid)
            {
                float radioHalo = 80.0f;
                DrawCircleGradient((int)light->position.x, (int)light->position.y, radioHalo, ColorAlpha(WHITE, 0), WHITE);

                float anguloVista = jugador.getAnguloVista();
                float anguloCono = jugador.getAnguloCono();
                float alcance = jugador.getAlcanceLinterna();

                float bateriaPct = (float)jugador.getBateria() / (float)Constantes::BATERIA_MAX;
                unsigned char alphaCono = (unsigned char)Lerp(180, 0, bateriaPct);
                DrawCircleSector(light->position, alcance, anguloVista - anguloCono, anguloVista + anguloCono, 32, ColorAlpha(WHITE, (float)alphaCono / 255.0f));
                rlDrawRenderBatchActive();
            }

            rlSetBlendMode(BLEND_ALPHA);
            for (int i = 0; i < light->shadowCount; i++) {
                DrawTriangleFan(light->shadows[i].vertices, 4, BLACK);
            }
            rlDrawRenderBatchActive();

        EndMode2D();
    EndTextureMode();
}

void Iluminacion::UpdateLightShadows(
    LightInfo* light,
    const std::vector<Rectangle>& muros,
    const Rectangle& puerta,
    bool puertaEstaAbierta,
    Camera2D& camera,
    Protagonista& jugador
)
{
    light->shadowCount = 0;
    light->valid = true;

    for (const auto& box : muros) {
        if (CheckCollisionPointRec(light->position, box)) {
            light->valid = false;
            break;
        }
    }
    if (light->valid && !puertaEstaAbierta) {
        if (CheckCollisionPointRec(light->position, puerta)) {
            light->valid = false;
        }
    }

    if (light->valid)
    {
        std::vector<Rectangle> bloqueadores = muros;
        if (!puertaEstaAbierta) {
            bloqueadores.push_back(puerta);
        }

        for (const auto& box : bloqueadores)
        {
            if (!CheckCollisionRecs(light->bounds, box)) continue;

            Vector2 sp = { box.x, box.y };
            Vector2 ep = { box.x + box.width, box.y };
            if (light->position.y > ep.y) ComputeShadowVolumeForEdge(light, sp, ep);
            sp = ep;
            ep.y += box.height;
            if (light->position.x < ep.x) ComputeShadowVolumeForEdge(light, sp, ep);
            sp = ep;
            ep.x -= box.width;
            if (light->position.y < ep.y) ComputeShadowVolumeForEdge(light, sp, ep);
            sp = ep;
            ep.y -= box.height;
            if (light->position.x > ep.x) ComputeShadowVolumeForEdge(light, sp, ep);

            if (light->shadowCount < MAX_SHADOWS) {
                light->shadows[light->shadowCount].vertices[0] = (Vector2){ box.x, box.y };
                light->shadows[light->shadowCount].vertices[1] = (Vector2){ box.x, box.y + box.height };
                light->shadows[light->shadowCount].vertices[2] = (Vector2){ box.x + box.width, box.y + box.height };
                light->shadows[light->shadowCount].vertices[3] = (Vector2){ box.x + box.width, box.y };
                light->shadowCount++;
            }
        }
    }

    DrawLightMask(light, camera, jugador);
}

void Iluminacion::SetupLight(LightInfo* light, Vector2 pos, Color color, float radius, float intensity)
{
    light->position = pos;
    light->color = color;
    light->radius = radius;
    light->intensity = intensity;
    light->shadowCount = 0;
    light->active = true;
    light->valid = false;
    light->mask = LoadRenderTexture(Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA);
    light->bounds = (Rectangle){ pos.x - radius, pos.y - radius, radius*2, radius*2 };
}

void Iluminacion::MoveLight(LightInfo* light, Vector2 pos)
{
    light->position = pos;
    light->bounds.x = pos.x - light->radius;
    light->bounds.y = pos.y - light->radius;
}
