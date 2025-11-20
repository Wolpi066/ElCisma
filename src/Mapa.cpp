#include "Mapa.h"
#include "Constantes.h"
#include "raymath.h"
#include <algorithm>
#include <random>
#include <chrono>
#include <numeric>
#include "GestorEntidades.h"
#include "Spawner.h"
#include "Zombie.h"
#include "MonstruoObeso.h"
#include "Jefe.h"
#include "Bateria.h"
#include "Armadura.h"
#include "Botiquin.h"
#include "CajaDeMuniciones.h"
#include "Llave.h"
#include "Fantasma.h"
#include "IndicadorPuerta.h"
#include "Cofre.h"
#include "Nota.h"

Mapa::Mapa()
    : mundoRect({ -1500, -1500, 3000, 3000 }),
      estadoPuerta(EstadoPuerta::CERRADA),
      temporizadorAnimacionPuerta(0.0f)
{
    Image img = GenImageChecked(64, 64, 32, 32, GRAY, DARKGRAY);
    pisoTexture = LoadTextureFromImage(img);
    UnloadImage(img);

    texPuertaCerrada = LoadTexture("assets/PuertaCerrada.png");
    texPuertaAbriendose = LoadTexture("assets/PuertaAbriendose.png");
    texPuertaAbierta = LoadTexture("assets/PuertaAbierta.png");

    cargarMapa();
}

Mapa::~Mapa()
{
    UnloadTexture(pisoTexture);
    UnloadTexture(texPuertaCerrada);
    UnloadTexture(texPuertaAbriendose);
    UnloadTexture(texPuertaAbierta);
}

void Mapa::actualizar(float dt)
{
    if (estadoPuerta == EstadoPuerta::ABRIENDO)
    {
        temporizadorAnimacionPuerta -= dt;
        if (temporizadorAnimacionPuerta <= 0.0f) {
            estadoPuerta = EstadoPuerta::ABIERTA;
        }
    }
}

void Mapa::dibujar()
{
    dibujarPiso();

    for (const auto& muro : muros) {
        DrawRectangleRec(muro, DARKGRAY);
    }

    for (const auto& caja : cajas) {
        DrawRectangleRec(caja, DARKBROWN);
    }

    // --- DIBUJO PUERTA 2.5D ---
    Texture2D* texPuerta = &texPuertaCerrada;

    if (estadoPuerta == EstadoPuerta::ABRIENDO) texPuerta = &texPuertaAbriendose;
    else if (estadoPuerta == EstadoPuerta::ABIERTA) texPuerta = &texPuertaAbierta;

    if (texPuerta->id != 0) {
        // Calcular dimensiones manteniendo proporción
        float aspect = (float)texPuerta->width / (float)texPuerta->height;
        float finalW = puertaJefe.width;
        float finalH = finalW / aspect;

        // IMPORTANTE: Dibujamos anclado a la BASE física de la puerta.
        // (puertaJefe.y + puertaJefe.height) es el "suelo" donde toca la puerta.
        // Restamos finalH para dibujar hacia arriba.
        float posY = (puertaJefe.y + puertaJefe.height) - finalH;

        // Ajuste fino: Le damos 2 píxeles de overlap sobre el suelo para que no flote
        posY += 2.0f;

        Rectangle dest = { puertaJefe.x, posY, finalW, finalH };
        Rectangle source = {0, 0, (float)texPuerta->width, (float)texPuerta->height};

        DrawTexturePro(*texPuerta, source, dest, {0,0}, 0.0f, WHITE);
    } else {
        if (estadoPuerta != EstadoPuerta::ABIERTA) {
             DrawRectangleRec(puertaJefe, GOLD);
        }
    }
}

const std::vector<Rectangle>& Mapa::getMuros() const { return muros; }
const std::vector<Rectangle>& Mapa::getCajas() const { return cajas; }

void Mapa::abrirPuerta() {
    if (estadoPuerta == EstadoPuerta::CERRADA) {
        estadoPuerta = EstadoPuerta::ABRIENDO;
        temporizadorAnimacionPuerta = 0.5f;
    }
}

void Mapa::cerrarPuerta() {
    estadoPuerta = EstadoPuerta::CERRADA;
}

bool Mapa::estaPuertaAbierta() const {
    return estadoPuerta == EstadoPuerta::ABIERTA;
}

Rectangle Mapa::getPuertaJefe() const { return puertaJefe; }

void Mapa::dibujarPiso()
{
    float tileW = (float)pisoTexture.width;
    float tileH = (float)pisoTexture.height;
    Rectangle sourceRect = { 0, 0, tileW, tileH };

    for (float y = mundoRect.y; y < mundoRect.y + mundoRect.height; y += tileH)
    {
        for (float x = mundoRect.x; x < mundoRect.x + mundoRect.width; x += tileW)
        {
            DrawTextureRec(pisoTexture, sourceRect, (Vector2){x, y}, WHITE);
        }
    }
}

void Mapa::cargarMapa()
{
    muros.clear();
    cajas.clear();

    // (Lógica de muros base igual que antes)
    float mundoW = 1500; float mundoH = 1500;
    float doorW = 100; float halfDoor = doorW / 2; float pasilloW = 150;

    muros.push_back({ -mundoW, -mundoH, mundoW*2, 20 });
    muros.push_back({ -mundoW, mundoH-20, mundoW*2, 20 });
    muros.push_back({ -mundoW, -mundoH, 20, mundoH*2 });
    muros.push_back({ mundoW-20, -mundoH, 20, mundoH*2 });

    float bossW = 800; float bossH = 800;
    float halfBossW = bossW / 2; float halfBossH = bossH / 2;
    muros.push_back({ -halfBossW, -halfBossH, bossW, 20 });
    muros.push_back({ -halfBossW, -halfBossH, 20, bossH });
    muros.push_back({ halfBossW-20, -halfBossH, 20, bossH });
    muros.push_back({ -halfBossW, halfBossH-20, halfBossW - halfDoor, 20 });
    muros.push_back({ halfDoor, halfBossH-20, halfBossW - halfDoor, 20 });

    // Puerta física
    puertaJefe = { -halfDoor, halfBossH - 20, doorW, 20 };

    // (Resto de muros... copia aquí todos los push_back de tu versión completa)
    // ... [INSERTA AQUI LOS MUROS DE SIEMPRE] ...
    // (Si quieres que te los vuelva a pegar todos dímelo, pero asumo que tienes el bloque grande)

    // Solo para que compile y funcione el ejemplo base:
    float anilloExt = 600;
    muros.push_back({ -anilloExt, -anilloExt, anilloExt - halfDoor, 20 });
    muros.push_back({ halfDoor, -anilloExt, anilloExt - halfDoor, 20 });
}

void Mapa::poblarMundo(GestorEntidades& gestor)
{
    spawnsCofres.clear();

    auto registrarCofreConColision = [&](GestorEntidades& gestor, const SpawnCofre& spawn, int lootID) {
        gestor.registrarConsumible(new Cofre(spawn.pos, lootID, spawn.orient));
        Rectangle cofreRect;
        if (spawn.orient == CofreOrientacion::HORIZONTAL) {
            cofreRect = { spawn.pos.x - 12.5f, spawn.pos.y - 7.5f, 25, 15 };
        } else {
            cofreRect = { spawn.pos.x - 7.5f, spawn.pos.y - 12.5f, 15, 25 };
        }
        this->cajas.push_back(cofreRect);
    };

    // ... (Tu lógica de spawns completa va aqui) ...

    // --- PICAPORTES CENTRADOS E INVISIBLES ---
    float posYPicaportes = puertaJefe.y + (puertaJefe.height / 2);
    Vector2 posPicaporteIzq = { puertaJefe.x + 20, posYPicaportes };
    Vector2 posPicaporteDer = { puertaJefe.x + puertaJefe.width - 20, posYPicaportes };

    gestor.registrarConsumible(Spawner<IndicadorPuerta>::Spawn(posPicaporteIzq));
    gestor.registrarConsumible(Spawner<IndicadorPuerta>::Spawn(posPicaporteDer));

    gestor.registrarConsumible(new Nota({0, 550}, 0));
}

bool Mapa::esAreaValida(Vector2 pos) {
    if (Vector2Distance(pos, {0.0f, 500.0f}) < Constantes::RADIO_SPAWN_SEGURO_JUGADOR) return false;
    Rectangle areaCheck = { pos.x - 32, pos.y - 32, 64, 64 };
    for (const auto& muro : muros) if (CheckCollisionRecs(areaCheck, muro)) return false;
    for (const auto& caja : cajas) if (CheckCollisionRecs(areaCheck, caja)) return false;
    if (!estaPuertaAbierta()) if (CheckCollisionRecs(areaCheck, puertaJefe)) return false;
    return true;
}
bool Mapa::esAreaValida(Vector2 pos, CofreOrientacion orient) {
    Rectangle areaCheck;
    if (orient == CofreOrientacion::HORIZONTAL) areaCheck = { pos.x - 12.5f, pos.y - 7.5f, 25, 15 };
    else areaCheck = { pos.x - 7.5f, pos.y - 12.5f, 15, 25 };
    for (const auto& muro : muros) if (CheckCollisionRecs(areaCheck, muro)) return false;
    if (!estaPuertaAbierta()) if (CheckCollisionRecs(areaCheck, puertaJefe)) return false;
    Rectangle areaInflada = { areaCheck.x - 1, areaCheck.y - 1, areaCheck.width + 2, areaCheck.height + 2 };
    bool tocandoMuro = false;
    for (const auto& muro : muros) { if (CheckCollisionRecs(areaInflada, muro)) { tocandoMuro = true; break; } }
    if (!tocandoMuro) return false;
    return true;
}
Vector2 Mapa::getPosicionSpawnValida(Rectangle zona) {
    Vector2 pos; int intentos = 0;
    do { pos.x = (float)GetRandomValue(zona.x, zona.x + zona.width); pos.y = (float)GetRandomValue(zona.y, zona.y + zona.height); intentos++; if (intentos > 100) return {zona.x, zona.y}; } while (!esAreaValida(pos));
    return pos;
}
SpawnCofre Mapa::getSpawnCofrePegadoAPared(Rectangle zona) {
    SpawnCofre spawn; spawn.pos = getPosicionSpawnValida(zona); spawn.orient = CofreOrientacion::HORIZONTAL; return spawn;
}
Vector2 Mapa::getPosicionSpawnNota(const Rectangle& caja) { return { caja.x + caja.width / 2, caja.y + caja.height / 2 }; }
