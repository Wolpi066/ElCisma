#include "Mapa.h"
#include "Constantes.h"
#include "raymath.h"

// Incluimos las clases necesarias para poblar el mundo
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
#include "Nota.h" // <-- ¡NUEVO!!

Mapa::Mapa()
    : mundoRect({ -1500, -1500, 3000, 3000 }),
      puertaAbierta(false)
{
    Image img = GenImageChecked(64, 64, 32, 32, GRAY, DARKGRAY);
    pisoTexture = LoadTextureFromImage(img);
    UnloadImage(img);

    cargarMapa();
}

Mapa::~Mapa()
{
    UnloadTexture(pisoTexture);
}

void Mapa::dibujar()
{
    dibujarPiso();

    // Dibujamos los muros (grises)
    for (const auto& muro : muros) {
        DrawRectangleRec(muro, DARKGRAY);
    }

    // Dibujamos las cajas (marrones)
    for (const auto& caja : cajas) {
        DrawRectangleRec(caja, DARKBROWN); // Color diferente
    }
}

// --- ¡Getters Actualizados!! ---
const std::vector<Rectangle>& Mapa::getMuros() const {
    return muros;
}
const std::vector<Rectangle>& Mapa::getCajas() const {
    return cajas;
}
// ---------------------------------

// --- Nuevas funciones de la puerta ---
void Mapa::abrirPuerta() {
    puertaAbierta = true;
}

bool Mapa::estaPuertaAbierta() const {
    return puertaAbierta;
}

Rectangle Mapa::getPuertaJefe() const {
    return puertaJefe;
}
// ---------------------------------


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
    // --- ¡REFACTOR DE MUROS Y CAJAS!! ---
    muros.clear();
    cajas.clear();

    float mundoW = 1500;
    float mundoH = 1500;
    float doorW = 100;
    float halfDoor = doorW / 2;
    float pasilloW = 150;

    // --- Muros Exteriores ---
    muros.push_back({ -mundoW, -mundoH, mundoW*2, 20 });
    muros.push_back({ -mundoW, mundoH-20, mundoW*2, 20 });
    muros.push_back({ -mundoW, -mundoH, 20, mundoH*2 });
    muros.push_back({ mundoW-20, -mundoH, 20, mundoH*2 });

    // --- Muros Sala Jefe ---
    float bossW = 800;
    float bossH = 800;
    float halfBossW = bossW / 2;
    float halfBossH = bossH / 2;
    muros.push_back({ -halfBossW, -halfBossH, bossW, 20 });
    muros.push_back({ -halfBossW, -halfBossH, 20, bossH });
    muros.push_back({ halfBossW-20, -halfBossH, 20, bossH });
    muros.push_back({ -halfBossW, halfBossH-20, halfBossW - halfDoor, 20 });
    muros.push_back({ halfDoor, halfBossH-20, halfBossW - halfDoor, 20 });
    puertaJefe = { -halfDoor, halfBossH - 20, doorW, 20 };

    // --- Muros Anillo Exterior ---
    float anilloExt = 600;
    muros.push_back({ -anilloExt, -anilloExt, anilloExt - halfDoor, 20 });
    muros.push_back({ halfDoor, -anilloExt, anilloExt - halfDoor, 20 });
    muros.push_back({ -anilloExt, anilloExt-20, anilloExt - halfDoor, 20 });
    muros.push_back({ halfDoor, anilloExt-20, anilloExt - halfDoor, 20 });
    muros.push_back({ -anilloExt, -anilloExt, 20, anilloExt - halfDoor });
    muros.push_back({ -anilloExt, halfDoor, 20, anilloExt - halfDoor });
    muros.push_back({ anilloExt-20, -anilloExt, 20, anilloExt - halfDoor });
    muros.push_back({ anilloExt-20, halfDoor, 20, anilloExt - halfDoor });

    // --- Muros 4 Salas ---
    float salaW = 600;
    float salaH = 600;
    float salaX = 900;
    float salaY = 900;
    // Sala NO
    muros.push_back({ -salaX-salaW, -salaY-salaH, salaW, 20 });
    muros.push_back({ -salaX-salaW, -salaY-salaH, 20, salaH });
    muros.push_back({ -salaX-salaW, -salaY, (salaW-doorW)/2, 20 });
    muros.push_back({ -salaX-salaW + (salaW+doorW)/2, -salaY, (salaW-doorW)/2, 20 });
    muros.push_back({ -salaX, -salaY-salaH, 20, (salaH-doorW)/2 });
    muros.push_back({ -salaX, -salaY-salaH + (salaH+doorW)/2, 20, (salaH-doorW)/2 });
    // Sala NE
    muros.push_back({ salaX, -salaY-salaH, salaW, 20 });
    muros.push_back({ salaX+salaW-20, -salaY-salaH, 20, salaH });
    muros.push_back({ salaX, -salaY, (salaW-doorW)/2, 20 });
    muros.push_back({ salaX+(salaW+doorW)/2, -salaY, (salaW-doorW)/2, 20 });
    muros.push_back({ salaX, -salaY-salaH, 20, (salaH-doorW)/2 });
    muros.push_back({ salaX, -salaY-salaH+(salaH+doorW)/2, 20, (salaH-doorW)/2 });
    // Sala SO
    muros.push_back({ -salaX-salaW, salaY+salaH-20, salaW, 20 });
    muros.push_back({ -salaX-salaW, salaY, 20, salaH });
    muros.push_back({ -salaX-salaW, salaY, (salaW-doorW)/2, 20 });
    muros.push_back({ -salaX-salaW+(salaW+doorW)/2, salaY, (salaW-doorW)/2, 20 });
    muros.push_back({ -salaX, salaY, 20, (salaH-doorW)/2 });
    muros.push_back({ -salaX, salaY+(salaH+doorW)/2, 20, (salaH-doorW)/2 });
    // Sala SE
    muros.push_back({ salaX, salaY+salaH-20, salaW, 20 });
    muros.push_back({ salaX+salaW-20, salaY, 20, salaH });
    muros.push_back({ salaX, salaY, (salaW-doorW)/2, 20 });
    muros.push_back({ salaX+(salaW+doorW)/2, salaY, (salaW-doorW)/2, 20 });
    muros.push_back({ salaX, salaY, 20, (salaH-doorW)/2 });
    muros.push_back({ salaX, salaY+(salaH+doorW)/2, 20, (salaH-doorW)/2 });

    // --- Muros Pasillos ---
    muros.push_back({ -salaX, -salaY, pasilloW, salaY - (bossH/2) });
    muros.push_back({ -salaX, (bossH/2), pasilloW, salaY - (bossH/2) });
    muros.push_back({ salaX-pasilloW, -salaY, pasilloW, salaY - (bossH/2) });
    muros.push_back({ salaX-pasilloW, (bossH/2), pasilloW, salaY - (bossH/2) });
    muros.push_back({ -salaX, -salaY, salaX - (bossW/2), pasilloW });
    muros.push_back({ (bossW/2), -salaY, salaX - (bossW/2), pasilloW });
    muros.push_back({ -salaX, salaY-pasilloW, salaX - (bossW/2), pasilloW });
    muros.push_back({ (bossW/2), salaY-pasilloW, salaX - (bossW/2), pasilloW });

    // --- ¡NUEVO! Relleno de Pasillo Norte ---
    muros.push_back({ -200, -750, 400, 20 }); // Muro horizontal largo
    cajas.push_back({ -220, -780, 40, 40 });  // Escombro
    cajas.push_back({ 220, -720, 40, 40 });   // Escombro

    // --- ¡NUEVO! Relleno de Pasillo Este ---
    cajas.push_back({ 750, -50, 80, 40 }); // Maquinaria rota
    cajas.push_back({ 750, 50, 80, 40 });  // Maquinaria rota
    cajas.push_back({ 830, 0, 40, 40 });   // Escombro

    // --- ¡NUEVO! Relleno de Pasillo Oeste (Alcove) ---
    muros.push_back({ -850, -100, 100, 20 }); // Muro superior del alcove
    muros.push_back({ -850, 100, 100, 20 });  // Muro inferior del alcove
    muros.push_back({ -850, -100, 20, 220 }); // Muro trasero del alcove


    // --- Cajas (Obstaculos bajos) ---
    // Sala NO (Almacen - "Camara del Oxido")
    muros.push_back({ -1480, -1200, 380, 40 }); // Estanteria (¡CORREGIDO! (Foto 2) Acortada y pegada a la pared oeste)
    muros.push_back({ -1480, -1000, 380, 40 }); // Estanteria (¡CORREGIDO! (Foto 2) Acortada y pegada a la pared oeste)
    // ¡CORREGIDO! (Foto 2) Se eliminó el muro problemático que estaba en: { -1000, -1480, 40, 160 }
    cajas.push_back({ -1400, -1400, 40, 40}); // Caja
    cajas.push_back({ -1100, -1100, 60, 60}); // Caja grande
    // --- INICIO DE NUEVO CONTENIDO ---
    cajas.push_back({ -1450, -1400, 40, 150 }); // Estanteria vertical (¡CORREGIDO! Movida al pasillo norte)
    cajas.push_back({ -1300, -1400, 40, 150 }); // Estanteria vertical (¡CORREGIDO! Movida al pasillo norte, 110px de espacio)
    cajas.push_back({ -1150, -1450, 80, 40 });  // Cajas apiladas
    cajas.push_back({ -1150, -1400, 80, 40 });  // Cajas apiladas
    cajas.push_back({ -1050, -950, 40, 40});    // Caja suelta
    // --- FIN DE NUEVO CONTENIDO ---

    // Sala NE (Electrica - "Camara de la Memoria")
    cajas.push_back({ 1100, -1250, 100, 100 }); // Maquinaria (¡CORREGIDO! (Foto 3) Movido 50px al sur)
    cajas.push_back({ 1250, -1100, 100, 100 }); // Maquinaria (¡CORREGIDO! (Foto 4) Movido 50px al este)
    cajas.push_back({ 950, -1000, 40, 40 }); // (¡CORREGIDO! Movido 50px al norte para mejor flujo)
    cajas.push_back({ 950, -1350, 40, 40 });
    // --- INICIO DE NUEVO CONTENIDO ---
    cajas.push_back({ 1000, -1100, 20, 80 }); // Fila de servidores/paneles
    cajas.push_back({ 1030, -1100, 20, 80 }); // Fila de servidores/paneles
    cajas.push_back({ 1060, -1100, 20, 80 }); // Fila de servidores/paneles
    cajas.push_back({ 1300, -1300, 150, 40 }); // Maquinaria horizontal
    muros.push_back({ 1300, -1400, 20, 150 }); // Panel electrico solido
    // --- FIN DE NUEVO CONTENIDO ---

    // Sala SO (Oficinas - "Camara del Remanente")
    muros.push_back({ -1200, 1000, 300, 20 }); // Cubiculo
    muros.push_back({ -1200, 1150, 300, 20 }); // Cubiculo
    muros.push_back({ -1200, 1300, 300, 20 }); // Cubiculo
    cajas.push_back({ -1400, 1400, 50, 50}); // Caja
    cajas.push_back({ -1100, 1220, 50, 50}); // Caja
    // --- INICIO DE NUEVO CONTENIDO ---
    cajas.push_back({ -1450, 1050, 80, 40 }); // Escritorio
    cajas.push_back({ -1450, 1200, 80, 40 }); // Escritorio
    cajas.push_back({ -1450, 1350, 80, 40 }); // Escritorio
    cajas.push_back({ -1300, 950, 40, 40 });  // Silla/Papelera
    cajas.push_back({ -1300, 1100, 40, 40 }); // Silla/Papelera
    cajas.push_back({ -1300, 1250, 40, 40 }); // Silla/Papelera
    // --- FIN DE NUEVO CONTENIDO ---

    // Sala SE (Dormis - "Camara del Canto")
    cajas.push_back({ 1000, 1000, 150, 60 }); // Cama
    cajas.push_back({ 1000, 1200, 150, 60 }); // Cama
    cajas.push_back({ 1200, 1000, 150, 60 }); // Cama
    cajas.push_back({ 1200, 1200, 150, 60 }); // Cama
    // --- INICIO DE NUEVO CONTENIDO ---
    muros.push_back({ 1400, 1000, 30, 150 }); // Lockers
    muros.push_back({ 1440, 1000, 30, 150 }); // Lockers
    muros.push_back({ 1400, 1200, 30, 150 }); // Lockers
    muros.push_back({ 1440, 1200, 30, 150 }); // Lockers
    cajas.push_back({ 1000, 1400, 60, 60 });  // Mesa
    cajas.push_back({ 1200, 1400, 60, 60 });  // Mesa
    // --- FIN DE NUEVO CONTENIDO ---


    // Pasillos (Estos ya estaban, los respetamos)
    cajas.push_back({ -200, 450, 40, 40 });
    cajas.push_back({ 200, 450, 40, 40 });
    cajas.push_back({ 0, 700, 40, 40 });
    muros.push_back({ -150, 800, 300, 20 }); // Era caja
    muros.push_back({ -300, 700, 20, 100 }); // Era caja
    muros.push_back({ 300, 700, 20, 100 }); // Era caja
    muros.push_back({ -700, -200, 20, 150 }); // Era caja
    muros.push_back({ -700, 200, 20, 150 }); // Era caja
    cajas.push_back({ -800, 0, 40, 40 });
    muros.push_back({ -750, -300, 80, 20 }); // Era caja
    muros.push_back({ -750, 300, 80, 20 }); // Era caja
    muros.push_back({ 700, -200, 20, 150 }); // Era caja
    muros.push_back({ 700, 200, 20, 150 }); // Era caja
    cajas.push_back({ 800, 0, 40, 40 });
    muros.push_back({ 750, -300, 80, 20 }); // Era caja
    muros.push_back({ 750, 300, 80, 20 }); // Era caja
    cajas.push_back({ -20, -700, 40, 40 });
    cajas.push_back({ 20, -700, 40, 40 });
}

void Mapa::poblarMundo(GestorEntidades& gestor)
{
    spawnsCofres.clear();
    Rectangle zonaAlmacen = { -1480, -1480, 580, 580 };
    Rectangle zonaElectrica = { 920, -1480, 580, 580 };
    Rectangle zonaOficinas = { -1480, 920, 580, 580 };
    Rectangle zonaDormis = { 920, 920, 580, 580 };
    Rectangle pasilloOeste = { -800, -300, 100, 600 };
    Rectangle pasilloEste = { 700, -300, 100, 600 };
    Rectangle pasilloSur = { -300, 650, 600, 150 };
    Rectangle pasilloNorte = { -50, -800, 100, 150 };
    Rectangle anilloNorte = { -300, -550, 600, 100 };
    Rectangle anilloSur = { -300, 450, 600, 100 };
    std::vector<Rectangle> zonasHabitaciones = { zonaAlmacen, zonaElectrica, zonaOficinas, zonaDormis };

    // --- ¡NUEVO! Definicion de Zona para el Alcove ---
    Rectangle zonaAlcoveOeste = { -830, -80, 60, 160 }; // El interior del alcove que creamos
    // ---------------------------------------------

    // --- Generacion de Enemigos ---
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(zonaAlmacen)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(zonaAlmacen)));
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(zonaElectrica)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(zonaElectrica)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(zonaOficinas)));
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(zonaOficinas)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(zonaDormis)));
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(zonaDormis)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(pasilloOeste)));
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(pasilloOeste)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(pasilloEste)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(pasilloEste)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(pasilloSur)));
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(pasilloSur)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(pasilloNorte)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(anilloNorte)));
    gestor.registrarJefe(new Jefe({0.0f, 0.0f}));
    gestor.registrarEnemigo(Spawner<Fantasma>::Spawn({-9999, -9999}));

    // --- Generacion de Consumibles Fijos (de piso) ---
    gestor.registrarConsumible(Spawner<Bateria>::Spawn(getPosicionSpawnValida(zonaElectrica)));
    gestor.registrarConsumible(Spawner<Bateria>::Spawn(getPosicionSpawnValida(zonaElectrica)));
    gestor.registrarConsumible(Spawner<CajaDeMuniciones>::Spawn(getPosicionSpawnValida(zonaAlmacen)));
    gestor.registrarConsumible(Spawner<CajaDeMuniciones>::Spawn(getPosicionSpawnValida(zonaAlmacen)));
    gestor.registrarConsumible(Spawner<Botiquin>::Spawn(getPosicionSpawnValida(zonaDormis)));
    gestor.registrarConsumible(Spawner<Botiquin>::Spawn(getPosicionSpawnValida(zonaOficinas)));
    gestor.registrarConsumible(Spawner<Armadura>::Spawn(getPosicionSpawnValida(pasilloOeste)));
    gestor.registrarConsumible(Spawner<Armadura>::Spawn(getPosicionSpawnValida(pasilloEste)));
    gestor.registrarConsumible(Spawner<Botiquin>::Spawn(getPosicionSpawnValida(anilloSur)));


    // --- Logica de Cofres y Llave ---
    int zonaLlaveIdx = GetRandomValue(0, zonasHabitaciones.size() - 1);
    SpawnCofre spawnLlave = getSpawnCofrePegadoAPared(zonasHabitaciones[zonaLlaveIdx]);
    gestor.registrarConsumible(new Cofre(spawnLlave.pos, 99, spawnLlave.orient));
    spawnsCofres.push_back(spawnLlave.pos);

    for (int i = 0; i < zonasHabitaciones.size(); i++)
    {
        int cofresPorHabitacion = GetRandomValue(1, 2);
        for (int j = 0; j < cofresPorHabitacion; j++)
        {
            int lootID = GetRandomValue(1, 4);
            SpawnCofre spawnCofre = getSpawnCofrePegadoAPared(zonasHabitaciones[i]);
            gestor.registrarConsumible(new Cofre(spawnCofre.pos, lootID, spawnCofre.orient));
            spawnsCofres.push_back(spawnCofre.pos);
        }
    }

    // --- INICIO DE NUEVO CONTENIDO ---
    // Añadimos un cofre extra en el pasillo sur, como pediste.
    SpawnCofre spawnExtra = getSpawnCofrePegadoAPared(pasilloSur);
    gestor.registrarConsumible(new Cofre(spawnExtra.pos, GetRandomValue(1, 4), spawnExtra.orient));
    spawnsCofres.push_back(spawnExtra.pos);
    // --- FIN DE NUEVO CONTENIDO ---
    SpawnCofre spawnAlcove = getSpawnCofrePegadoAPared(zonaAlcoveOeste);
    gestor.registrarConsumible(new Cofre(spawnAlcove.pos, GetRandomValue(1, 4), spawnAlcove.orient));
    spawnsCofres.push_back(spawnAlcove.pos);
    // --- FIN DE NUEVO CONTENIDO ---


    // --- Picaportes ---
    float handleSize = 10;
    float posYPicaportes = (puertaJefe.y + puertaJefe.height) + (handleSize / 2);
    Vector2 posPicaporteIzq = { puertaJefe.x + (puertaJefe.width/2) - handleSize - 5, posYPicaportes };
    Vector2 posPicaporteDer = { puertaJefe.x + (puertaJefe.width/2) + 5, posYPicaportes };
    gestor.registrarConsumible(Spawner<IndicadorPuerta>::Spawn(posPicaporteIzq));
    gestor.registrarConsumible(Spawner<IndicadorPuerta>::Spawn(posPicaporteDer));

    // --- ¡NUEVO!! Spawn de la Nota "Hola Mundo" ---
    // ¡CORREGIDO!! Buscamos una caja especifica que sabemos que existe
    // (La primera caja en Sala NO)
    Rectangle cajaParaNota = { -1400, -1400, 40, 40};
    gestor.registrarConsumible(new Nota(getPosicionSpawnNota(cajaParaNota), 1)); // ID 1 = "Hola mundo"
}

// --- Metodos de Ayuda para Spawn Valido ---
bool Mapa::esAreaValida(Vector2 pos)
{
    // Chequeo simple para items de piso
    // --- ¡¡CORREGIDO!! ---
    // Usamos un área de 64x64 (radio 32) para asegurar que quepan
    // los enemigos y no solo los items.
    Rectangle areaCheck = { pos.x - 32, pos.y - 32, 64, 64 };

    for (const auto& muro : muros) { // Chequea muros
        if (CheckCollisionRecs(areaCheck, muro)) return false;
    }
    for (const auto& caja : cajas) { // Chequea cajas
        if (CheckCollisionRecs(areaCheck, caja)) return false;
    }

    if (!puertaAbierta) {
        if (CheckCollisionRecs(areaCheck, puertaJefe)) return false;
    }
    return true;
}

// --- ¡NUEVA!! Sobrecarga para Cofres ---
bool Mapa::esAreaValida(Vector2 pos, CofreOrientacion orient)
{
    Rectangle areaCheck;
    if (orient == CofreOrientacion::HORIZONTAL) {
        areaCheck = { pos.x - 12.5f, pos.y - 7.5f, 25, 15 };
    } else {
        areaCheck = { pos.x - 7.5f, pos.y - 12.5f, 15, 25 };
    }

    // Un cofre no puede estar en un muro
    for (const auto& muro : muros) {
        if (CheckCollisionRecs(areaCheck, muro)) return false;
    }

    // Un cofre no puede estar en la puerta
    if (!puertaAbierta) {
        if (CheckCollisionRecs(areaCheck, puertaJefe)) return false;
    }

    // ¡RELAJADO!! Un cofre SI PUEDE estar superpuesto con una 'caja'
    // (porque se spawnea 'pegado' a la pared, que tambien es un muro)

    return true;
}


Vector2 Mapa::getPosicionSpawnValida(Rectangle zona)
{
    Vector2 pos;
    int intentos = 0;
    do {
        pos.x = (float)GetRandomValue(zona.x, zona.x + zona.width);
        pos.y = (float)GetRandomValue(zona.y, zona.y + zona.height);
        intentos++;
        if (intentos > 100) {
            TraceLog(LOG_WARNING, "No se pudo encontrar un spawn valido en la zona, spawneando en la esquina.");
            return {zona.x, zona.y};
        }
    } while (!esAreaValida(pos)); // Usa la sobrecarga simple

    return pos;
}

// --- ¡NUEVA FUNCION!! ---
SpawnCofre Mapa::getSpawnCofrePegadoAPared(Rectangle zona)
{
    SpawnCofre spawn;
    int intentos = 0;
    float paddingH = 15.0f / 2.0f + 2.0f; // Mitad alto (vertical) + 2px
    float paddingL = 25.0f / 2.0f + 2.0f; // Mitad largo (horizontal) + 2px

    do {
        int pared = GetRandomValue(0, 3); // 0=N, 1=S, 2=O, 3=E

        switch (pared) {
            case 0: // Pared Norte
                spawn.orient = CofreOrientacion::HORIZONTAL;
                spawn.pos.x = (float)GetRandomValue(zona.x + paddingL, zona.x + zona.width - paddingL);
                spawn.pos.y = zona.y + paddingH;
                break;
            case 1: // Pared Sur
                spawn.orient = CofreOrientacion::HORIZONTAL;
                spawn.pos.x = (float)GetRandomValue(zona.x + paddingL, zona.x + zona.width - paddingL);
                spawn.pos.y = zona.y + zona.height - paddingH;
                break;
            case 2: // Pared Oeste
                spawn.orient = CofreOrientacion::VERTICAL;
                spawn.pos.x = zona.x + paddingH;
                spawn.pos.y = (float)GetRandomValue(zona.y + paddingL, zona.y + zona.height - paddingL);
                break;
            case 3: // Pared Este
                spawn.orient = CofreOrientacion::VERTICAL;
                spawn.pos.x = zona.x + zona.width - paddingH;
                spawn.pos.y = (float)GetRandomValue(zona.y + paddingL, zona.y + zona.height - paddingL);
                break;
        }

        intentos++;
        if (intentos > 100) {
            TraceLog(LOG_WARNING, "No se pudo encontrar un spawn en pared, usando spawn aleatorio.");
            spawn.pos = getPosicionSpawnValida(zona); // Fallback
            spawn.orient = CofreOrientacion::HORIZONTAL;
            spawnsCofres.push_back(spawn.pos); // Guardar el fallback
            return spawn;
        }

        // --- ¡NUEVO CHEQUEO DE DISTANCIA!! ---
        bool estaMuyCerca = false;
        for (const auto& p : spawnsCofres) {
            if (Vector2Distance(spawn.pos, p) < 150.0f) { // 150px de separacion minima
                estaMuyCerca = true;
                break;
            }
        }

        if (estaMuyCerca) continue;

    } while (!esAreaValida(spawn.pos, spawn.orient)); // Usa la sobrecarga de cofre

    return spawn;
}

// --- ¡NUEVA FUNCION!! ---
Vector2 Mapa::getPosicionSpawnNota(const Rectangle& caja)
{
    // ¡CORREGIDO!! Devuelve el centro de la caja.
    // La nota se dibujara "encima" (visualmente)
    return { caja.x + caja.width / 2, caja.y + caja.height / 2 };
}
