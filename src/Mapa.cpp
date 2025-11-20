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

    // --- CARGA DE TEXTURAS ---
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

// --- DIBUJO AJUSTADO ---
void Mapa::dibujarPuerta(float alpha)
{
    Texture2D* texPuerta = &texPuertaCerrada;

    if (estadoPuerta == EstadoPuerta::ABRIENDO) texPuerta = &texPuertaAbriendose;
    else if (estadoPuerta == EstadoPuerta::ABIERTA) texPuerta = &texPuertaAbierta;

    if (texPuerta->id != 0) {
        float aspect = (float)texPuerta->width / (float)texPuerta->height;

        // Ajuste lateral para tapar rendijas
        float extraWidth = 45.0f;
        float finalW = puertaJefe.width + extraWidth;
        float finalH = finalW / aspect;

        // Posicionamos: Centrado en X, Base en Y
        float posX = puertaJefe.x - (extraWidth / 2.0f);
        float posY = (puertaJefe.y + puertaJefe.height) - finalH;
        posY += 2.0f;

        Rectangle dest = { posX, posY, finalW, finalH };
        Rectangle source = {0, 0, (float)texPuerta->width, (float)texPuerta->height};

        DrawTexturePro(*texPuerta, source, dest, {0,0}, 0.0f, Fade(WHITE, alpha));
    } else {
        // Fallback
        if (estadoPuerta != EstadoPuerta::ABIERTA) {
             DrawRectangleRec(puertaJefe, Fade(GOLD, alpha));
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

    // No dibujamos la puerta en la capa base
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

    float mundoW = 1500;
    float mundoH = 1500;
    float doorW = 100;
    float halfDoor = doorW / 2;
    float pasilloW = 150;

    // Paredes exteriores
    muros.push_back({ -mundoW, -mundoH, mundoW*2, 20 });
    muros.push_back({ -mundoW, mundoH-20, mundoW*2, 20 });
    muros.push_back({ -mundoW, -mundoH, 20, mundoH*2 });
    muros.push_back({ mundoW-20, -mundoH, 20, mundoH*2 });

    // --- ZONA JEFE: PAREDES MASIVAS (x3) ---
    float bossW = 800;
    float bossH = 800;
    float halfBossW = bossW / 2;
    float halfBossH = bossH / 2;

    // --- CAMBIO: Pared 3 veces más gruesa (180px) ---
    float bossWallThickness = 180.0f;

    muros.push_back({ -halfBossW, -halfBossH, bossW, 20 }); // Norte
    muros.push_back({ -halfBossW, -halfBossH, 20, bossH }); // Oeste
    muros.push_back({ halfBossW-20, -halfBossH, 20, bossH }); // Este

    // Pared Sur (Izquierda y Derecha de la puerta) - MASIVA
    // Ajustamos la Y para que crezca hacia "arriba" (adentro de la sala)
    muros.push_back({ -halfBossW, halfBossH-bossWallThickness, halfBossW - halfDoor, bossWallThickness });
    muros.push_back({ halfDoor, halfBossH-bossWallThickness, halfBossW - halfDoor, bossWallThickness });

    // Puerta lógica (se mantiene alineada al borde exterior)
    puertaJefe = { -halfDoor, halfBossH - 20, doorW, 20 };

    // --- Resto del mapa (igual que antes) ---
    float anilloExt = 600;
    muros.push_back({ -anilloExt, -anilloExt, anilloExt - halfDoor, 20 });
    muros.push_back({ halfDoor, -anilloExt, anilloExt - halfDoor, 20 });
    muros.push_back({ -anilloExt, anilloExt-20, anilloExt - halfDoor, 20 });
    muros.push_back({ halfDoor, anilloExt-20, anilloExt - halfDoor, 20 });
    muros.push_back({ -anilloExt, -anilloExt, 20, anilloExt - halfDoor });
    muros.push_back({ -anilloExt, halfDoor, 20, anilloExt - halfDoor });
    muros.push_back({ anilloExt-20, -anilloExt, 20, anilloExt - halfDoor });
    muros.push_back({ anilloExt-20, halfDoor, 20, anilloExt - halfDoor });

    float salaW = 600;
    float salaH = 600;
    float salaX = 900;
    float salaY = 900;
    muros.push_back({ -salaX-salaW, -salaY-salaH, salaW, 20 });
    muros.push_back({ -salaX-salaW, -salaY-salaH, 20, salaH });
    muros.push_back({ -salaX-salaW, -salaY, (salaW-doorW)/2, 20 });
    muros.push_back({ -salaX-salaW + (salaW+doorW)/2, -salaY, (salaW-doorW)/2, 20 });
    muros.push_back({ -salaX, -salaY-salaH, 20, (salaH-doorW)/2 });
    muros.push_back({ -salaX, -salaY-salaH + (salaH+doorW)/2, 20, (salaH-doorW)/2 });
    muros.push_back({ salaX, -salaY-salaH, salaW, 20 });
    muros.push_back({ salaX+salaW-20, -salaY-salaH, 20, salaH });
    muros.push_back({ salaX, -salaY, (salaW-doorW)/2, 20 });
    muros.push_back({ salaX+(salaW+doorW)/2, -salaY, (salaW-doorW)/2, 20 });
    muros.push_back({ salaX, -salaY-salaH, 20, (salaH-doorW)/2 });
    muros.push_back({ salaX, -salaY-salaH+(salaH+doorW)/2, 20, (salaH-doorW)/2 });
    muros.push_back({ -salaX-salaW, salaY+salaH-20, salaW, 20 });
    muros.push_back({ -salaX-salaW, salaY, 20, salaH });
    muros.push_back({ -salaX-salaW, salaY, (salaW-doorW)/2, 20 });
    muros.push_back({ -salaX-salaW+(salaW+doorW)/2, salaY, (salaW-doorW)/2, 20 });
    muros.push_back({ -salaX, salaY, 20, (salaH-doorW)/2 });
    muros.push_back({ -salaX, salaY+(salaH+doorW)/2, 20, (salaH-doorW)/2 });
    muros.push_back({ salaX, salaY+salaH-20, salaW, 20 });
    muros.push_back({ salaX+salaW-20, salaY, 20, salaH });
    muros.push_back({ salaX, salaY, (salaW-doorW)/2, 20 });
    muros.push_back({ salaX+(salaW+doorW)/2, salaY, (salaW-doorW)/2, 20 });
    muros.push_back({ salaX, salaY, 20, (salaH-doorW)/2 });
    muros.push_back({ salaX, salaY+(salaH+doorW)/2, 20, (salaH-doorW)/2 });

    muros.push_back({ -salaX, -salaY, pasilloW, salaY - (bossH/2) });
    muros.push_back({ -salaX, (bossH/2), pasilloW, salaY - (bossH/2) });
    muros.push_back({ salaX-pasilloW, -salaY, pasilloW, salaY - (bossH/2) });
    muros.push_back({ salaX-pasilloW, (bossH/2), pasilloW, salaY - (bossH/2) });
    muros.push_back({ -salaX, -salaY, salaX - (bossW/2), pasilloW });
    muros.push_back({ (bossW/2), -salaY, salaX - (bossW/2), pasilloW });
    muros.push_back({ -salaX, salaY-pasilloW, salaX - (bossW/2), pasilloW });
    muros.push_back({ (bossW/2), salaY-pasilloW, salaX - (bossW/2), pasilloW });

    muros.push_back({ -1480, -1400, 20, 400 });
    muros.push_back({ -1430, -1400, 20, 400 });
    muros.push_back({ -1250, -1300, 20, 300 });
    muros.push_back({ -1100, -1300, 20, 300 });
    cajas.push_back({ -1350, -1450, 80, 80 });
    cajas.push_back({ -1150, -1050, 40, 40 });
    cajas.push_back({ -1050, -1450, 40, 40 });

    muros.push_back({ 1000, -1400, 20, 150 });
    muros.push_back({ 1000, -1200, 20, 150 });
    cajas.push_back({ 1050, -1350, 100, 100 });
    cajas.push_back({ 1200, -1100, 150, 40 });
    cajas.push_back({ 1200, -1150, 150, 40 });
    muros.push_back({ 1350, -1450, 100, 20 });
    muros.push_back({ 1350, -1300, 100, 20 });
    cajas.push_back({ 1430, -1350, 40, 40 });

    muros.push_back({ -1200, 1000, 300, 20 });
    muros.push_back({ -1200, 1150, 300, 20 });
    muros.push_back({ -1200, 1300, 300, 20 });
    cajas.push_back({ -1400, 1400, 50, 50});
    cajas.push_back({ -1100, 1220, 50, 50});
    cajas.push_back({ -1450, 1050, 80, 40 });
    cajas.push_back({ -1450, 1200, 80, 40 });
    cajas.push_back({ -1450, 1350, 80, 40 });
    cajas.push_back({ -1300, 950, 40, 40 });
    cajas.push_back({ -1300, 1100, 40, 40 });
    cajas.push_back({ -1300, 1250, 40, 40 });
    muros.push_back({ -1480, 1150, 100, 20 });
    cajas.push_back({ -1300, 1050, 80, 40 });
    cajas.push_back({ -1150, 1400, 40, 80 });

    cajas.push_back({ 1000, 1000, 150, 60 });
    cajas.push_back({ 1000, 1200, 150, 60 });
    cajas.push_back({ 1200, 1000, 150, 60 });
    cajas.push_back({ 1200, 1200, 150, 60 });
    muros.push_back({ 1400, 1000, 30, 150 });
    muros.push_back({ 1440, 1000, 30, 150 });
    muros.push_back({ 1400, 1200, 30, 150 });
    muros.push_back({ 1440, 1200, 30, 150 });
    cajas.push_back({ 1000, 1400, 60, 60 });
    cajas.push_back({ 1200, 1400, 60, 60 });
    cajas.push_back({ 1150, 1100, 100, 60 });
    cajas.push_back({ 1130, 1090, 40, 40 });
    cajas.push_back({ 1270, 1110, 40, 40 });

    muros.push_back({ -850, -100, 100, 20 });
    muros.push_back({ -850, 100, 100, 20 });
    muros.push_back({ -850, -100, 20, 220 });
    cajas.push_back({ -830, -10, 60, 20 });
    muros.push_back({ 850, -75, 20, 150 });
    muros.push_back({ 830, -75, 20, 20 });
    muros.push_back({ 830, 55, 20, 20 });
    cajas.push_back({ 830, -55, 20, 110 });
    cajas.push_back({ -150, 580, 100, 20 });
    cajas.push_back({ 50, 580, 100, 20 });
    muros.push_back({ -200, -750, 400, 20 });
    cajas.push_back({ -220, -780, 40, 40 });
    cajas.push_back({ 220, -720, 40, 40 });

    muros.push_back({ -700, -1300, 20, 300 });
    muros.push_back({ -400, -1300, 20, 300 });
    muros.push_back({ -700, -1400, 320, 20 });
    muros.push_back({ -700, -1300, 100, 20 });
    muros.push_back({ -480, -1300, 80, 20 });
    cajas.push_back({ -680, -1380, 100, 40 });
    muros.push_back({ -420, -1380, 20, 60 });
    cajas.push_back({ 500, -1100, 150, 150 });
    muros.push_back({ -250, 1200, 500, 20 });
    cajas.push_back({ -200, 1160, 40, 40 });
    cajas.push_back({ 100, 1160, 40, 40 });
    cajas.push_back({ -150, 1240, 100, 40 });
    cajas.push_back({ 50, 1240, 100, 40 });
    cajas.push_back({ -50, 1300, 40, 40 });
    muros.push_back({ -1200, -300, 150, 40 });
    muros.push_back({ -1400, 100, 40, 200 });
    cajas.push_back({ -1180, -280, 40, 40 });
    cajas.push_back({ -1380, 300, 60, 60 });
    cajas.push_back({ -1100, 400, 80, 40 });
    muros.push_back({ 1100, -200, 20, 400 });
    muros.push_back({ 1300, -200, 20, 150 });
    muros.push_back({ 1300, 50, 20, 150 });
    muros.push_back({ 1100, -200, 200, 20 });
    muros.push_back({ 1100, 200, 200, 20 });
    muros.push_back({ 1120, -100, 20, 200 });
    cajas.push_back({ 1150, 0, 80, 80 });
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

    Rectangle zonaAlmacen = { -1480, -1480, 580, 580 };
    Rectangle zonaElectrica = { 920, -1480, 580, 580 };
    Rectangle zonaOficinas = { -1480, 920, 580, 580 };
    Rectangle zonaDormis = { 920, 920, 580, 580 };
    Rectangle pasilloOeste = { -800, -300, 100, 600 };
    Rectangle pasilloEste = { 700, -300, 100, 600 };
    Rectangle pasilloSur = { -300, 650, 600, 150 };
    Rectangle anilloNorte = { -300, -550, 600, 100 };
    Rectangle anilloSur = { -300, 450, 600, 100 };
    Rectangle corredorBordeN = { -900, -1500, 1800, 600 };
    Rectangle corredorBordeS = { -900, 900, 1800, 600 };
    Rectangle corredorBordeO = { -1500, -900, 600, 1800 };
    Rectangle corredorBordeE = { 900, -900, 600, 1800 };
    Rectangle habSeguridadN = { -680, -1380, 260, 80 };
    Rectangle zonaBarricadaS = { -200, 1220, 300, 100 };
    Rectangle zonaColapsoO = { -1400, -300, 200, 600 };
    Rectangle habObservacionE = { 1120, -180, 160, 360 };
    Rectangle zonaAlcoveOeste = { -830, -80, 60, 160 };
    Rectangle puestoSeguridadE = { 830, -55, 20, 110 };

    std::vector<Rectangle> zonasHabitaciones = { zonaAlmacen, zonaElectrica, zonaOficinas, zonaDormis };

    int zonaLlaveIdx = GetRandomValue(0, zonasHabitaciones.size() - 1);
    SpawnCofre spawnLlave = getSpawnCofrePegadoAPared(zonasHabitaciones[zonaLlaveIdx]);
    registrarCofreConColision(gestor, spawnLlave, 99);
    spawnsCofres.push_back(spawnLlave.pos);

    for (size_t i = 0; i < zonasHabitaciones.size(); i++)
    {
        int cofresPorHabitacion = GetRandomValue(1, 2);
        for (int j = 0; j < cofresPorHabitacion; j++)
        {
            int lootID = GetRandomValue(1, 4);
            SpawnCofre spawnCofre = getSpawnCofrePegadoAPared(zonasHabitaciones[i]);
            registrarCofreConColision(gestor, spawnCofre, lootID);
            spawnsCofres.push_back(spawnCofre.pos);
        }
    }

    SpawnCofre spawnAlcove = getSpawnCofrePegadoAPared(zonaAlcoveOeste);
    registrarCofreConColision(gestor, spawnAlcove, GetRandomValue(1, 4));
    spawnsCofres.push_back(spawnAlcove.pos);

    SpawnCofre spawnSeguridadE = getSpawnCofrePegadoAPared(puestoSeguridadE);
    registrarCofreConColision(gestor, spawnSeguridadE, GetRandomValue(1, 4));
    spawnsCofres.push_back(spawnSeguridadE.pos);

    SpawnCofre spawnCofreBN = getSpawnCofrePegadoAPared(habSeguridadN);
    registrarCofreConColision(gestor, spawnCofreBN, GetRandomValue(1, 4));
    spawnsCofres.push_back(spawnCofreBN.pos);

    SpawnCofre spawnCofreBS = getSpawnCofrePegadoAPared(zonaBarricadaS);
    registrarCofreConColision(gestor, spawnCofreBS, GetRandomValue(1, 4));
    spawnsCofres.push_back(spawnCofreBS.pos);

    SpawnCofre spawnCofreBE = getSpawnCofrePegadoAPared(habObservacionE);
    registrarCofreConColision(gestor, spawnCofreBE, GetRandomValue(1, 4));
    spawnsCofres.push_back(spawnCofreBE.pos);

    SpawnCofre spawnCofreBO = getSpawnCofrePegadoAPared(zonaColapsoO);
    registrarCofreConColision(gestor, spawnCofreBO, GetRandomValue(1, 4));
    spawnsCofres.push_back(spawnCofreBO.pos);

    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(zonaAlmacen)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(zonaAlmacen)));
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(zonaElectrica)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(zonaElectrica)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(zonaOficinas)));
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(zonaOficinas)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(zonaOficinas)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(zonaDormis)));
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(zonaDormis)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(zonaDormis)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(pasilloOeste)));
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(pasilloOeste)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(pasilloEste)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(pasilloEste)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(pasilloSur)));
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(pasilloSur)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(anilloNorte)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(anilloSur)));

    gestor.registrarEnemigo(Spawner<Fantasma>::Spawn({-9999, -9999}));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(corredorBordeN)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(habSeguridadN)));
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(habSeguridadN)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(corredorBordeE)));
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(habObservacionE)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(habObservacionE)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(corredorBordeS)));
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(zonaBarricadaS)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(zonaBarricadaS)));
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(zonaColapsoO)));
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(zonaColapsoO)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(corredorBordeO)));

    gestor.registrarConsumible(Spawner<Bateria>::Spawn(getPosicionSpawnValida(zonaElectrica)));
    gestor.registrarConsumible(Spawner<Bateria>::Spawn(getPosicionSpawnValida(zonaElectrica)));
    gestor.registrarConsumible(Spawner<CajaDeMuniciones>::Spawn(getPosicionSpawnValida(zonaAlmacen)));
    gestor.registrarConsumible(Spawner<CajaDeMuniciones>::Spawn(getPosicionSpawnValida(zonaAlmacen)));
    gestor.registrarConsumible(Spawner<Botiquin>::Spawn(getPosicionSpawnValida(zonaDormis)));
    gestor.registrarConsumible(Spawner<Botiquin>::Spawn(getPosicionSpawnValida(zonaOficinas)));
    gestor.registrarConsumible(Spawner<Armadura>::Spawn(getPosicionSpawnValida(pasilloOeste)));
    gestor.registrarConsumible(Spawner<Armadura>::Spawn(getPosicionSpawnValida(pasilloEste)));
    gestor.registrarConsumible(Spawner<Botiquin>::Spawn(getPosicionSpawnValida(anilloSur)));
    gestor.registrarConsumible(Spawner<CajaDeMuniciones>::Spawn(getPosicionSpawnValida(habSeguridadN)));
    gestor.registrarConsumible(Spawner<Armadura>::Spawn(getPosicionSpawnValida(habSeguridadN)));
    gestor.registrarConsumible(Spawner<Bateria>::Spawn(getPosicionSpawnValida(habObservacionE)));
    gestor.registrarConsumible(Spawner<Botiquin>::Spawn(getPosicionSpawnValida(zonaBarricadaS)));
    gestor.registrarConsumible(Spawner<Botiquin>::Spawn(getPosicionSpawnValida(zonaBarricadaS)));
    gestor.registrarConsumible(Spawner<Bateria>::Spawn(getPosicionSpawnValida(zonaColapsoO)));

    float posYPicaportes = puertaJefe.y + (puertaJefe.height / 2);
    Vector2 posPicaporteIzq = { puertaJefe.x + 20, posYPicaportes };
    Vector2 posPicaporteDer = { puertaJefe.x + puertaJefe.width - 20, posYPicaportes };
    gestor.registrarConsumible(Spawner<IndicadorPuerta>::Spawn(posPicaporteIzq));
    gestor.registrarConsumible(Spawner<IndicadorPuerta>::Spawn(posPicaporteDer));

    gestor.registrarConsumible(new Nota({0, 550}, 0));

    std::vector<int> idsDeNotas(10);
    std::iota(idsDeNotas.begin(), idsDeNotas.end(), 1);

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(idsDeNotas.begin(), idsDeNotas.end(), std::default_random_engine(seed));

    std::vector<Rectangle> cajasAleatorias = this->cajas;
    std::shuffle(cajasAleatorias.begin(), cajasAleatorias.end(), std::default_random_engine(seed));

    int numNotas = GetRandomValue(3, 5);

    if (numNotas > (int)cajasAleatorias.size()) {
        numNotas = cajasAleatorias.size();
    }

    for (int i = 0; i < numNotas; i++)
    {
        Rectangle cajaSpawn = cajasAleatorias[i];
        int notaID = idsDeNotas[i];
        gestor.registrarConsumible(new Nota(getPosicionSpawnNota(cajaSpawn), notaID));
    }
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
