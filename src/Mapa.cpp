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

    // --- Muros Pasillos (Interiores) ---
    muros.push_back({ -salaX, -salaY, pasilloW, salaY - (bossH/2) });
    muros.push_back({ -salaX, (bossH/2), pasilloW, salaY - (bossH/2) });
    muros.push_back({ salaX-pasilloW, -salaY, pasilloW, salaY - (bossH/2) });
    muros.push_back({ salaX-pasilloW, (bossH/2), pasilloW, salaY - (bossH/2) });
    muros.push_back({ -salaX, -salaY, salaX - (bossW/2), pasilloW });
    muros.push_back({ (bossW/2), -salaY, salaX - (bossW/2), pasilloW });
    muros.push_back({ -salaX, salaY-pasilloW, salaX - (bossW/2), pasilloW });
    muros.push_back({ (bossW/2), salaY-pasilloW, salaX - (bossW/2), pasilloW });


    // --- Relleno de Salas Interiores (Original + Refinamiento) ---

    // --- ¡REDISEÑO! Sala NO (Almacen - "Camara del Oxido") ---
    // (Limpiado el caos y superposiciones)
    muros.push_back({ -1480, -1400, 20, 400 }); // Estantería Larga Oeste 1
    muros.push_back({ -1430, -1400, 20, 400 }); // Estantería Larga Oeste 2

    muros.push_back({ -1250, -1300, 20, 300 }); // Estantería Central 1
    muros.push_back({ -1100, -1300, 20, 300 }); // Estantería Central 2

    cajas.push_back({ -1350, -1450, 80, 80 });  // Pila de cajas grande
    cajas.push_back({ -1150, -1050, 40, 40 });  // Caja suelta
    cajas.push_back({ -1050, -1450, 40, 40 });  // Caja suelta (la de la nota)

    // --- ¡REDISEÑO! Sala NE (Electrica - "Camara de la Memoria") ---
    // (Limpiado el caos y superposiciones)
    muros.push_back({ 1000, -1400, 20, 150 }); // Panel eléctrico
    muros.push_back({ 1000, -1200, 20, 150 }); // Panel eléctrico

    cajas.push_back({ 1050, -1350, 100, 100 }); // Maquinaria central
    cajas.push_back({ 1200, -1100, 150, 40 });  // Fila de servidores (caja)
    cajas.push_back({ 1200, -1150, 150, 40 });  // Fila de servidores (caja)

    muros.push_back({ 1350, -1450, 100, 20 });  // Maquinaria (muro)
    muros.push_back({ 1350, -1300, 100, 20 });  // Maquinaria (muro)
    cajas.push_back({ 1430, -1350, 40, 40 });    // Caja suelta

    // Sala SO (Oficinas - "Camara del Remanente") - (Diseño anterior OK)
    muros.push_back({ -1200, 1000, 300, 20 }); // Cubiculo
    muros.push_back({ -1200, 1150, 300, 20 }); // Cubiculo
    muros.push_back({ -1200, 1300, 300, 20 }); // Cubiculo
    cajas.push_back({ -1400, 1400, 50, 50}); // Caja
    cajas.push_back({ -1100, 1220, 50, 50}); // Caja
    cajas.push_back({ -1450, 1050, 80, 40 }); // Escritorio
    cajas.push_back({ -1450, 1200, 80, 40 }); // Escritorio
    cajas.push_back({ -1450, 1350, 80, 40 }); // Escritorio
    cajas.push_back({ -1300, 950, 40, 40 });  // Silla/Papelera
    cajas.push_back({ -1300, 1100, 40, 40 }); // Silla/Papelera
    cajas.push_back({ -1300, 1250, 40, 40 }); // Silla/Papelera
    // Relleno extra (OK)
    muros.push_back({ -1480, 1150, 100, 20 }); // Pared cubículo rota
    cajas.push_back({ -1300, 1050, 80, 40 }); // Otro escritorio
    cajas.push_back({ -1150, 1400, 40, 80 }); // Archivador volcado

    // Sala SE (Dormis - "Camara del Canto") - (Diseño anterior OK)
    cajas.push_back({ 1000, 1000, 150, 60 }); // Cama
    cajas.push_back({ 1000, 1200, 150, 60 }); // Cama
    cajas.push_back({ 1200, 1000, 150, 60 }); // Cama
    cajas.push_back({ 1200, 1200, 150, 60 }); // Cama
    muros.push_back({ 1400, 1000, 30, 150 }); // Lockers
    muros.push_back({ 1440, 1000, 30, 150 }); // Lockers
    muros.push_back({ 1400, 1200, 30, 150 }); // Lockers
    muros.push_back({ 1440, 1200, 30, 150 }); // Lockers
    cajas.push_back({ 1000, 1400, 60, 60 });  // Mesa
    cajas.push_back({ 1200, 1400, 60, 60 });  // Mesa
    // Relleno extra (OK)
    cajas.push_back({ 1150, 1100, 100, 60 }); // Mesa central
    cajas.push_back({ 1130, 1090, 40, 40 });  // Silla volcada
    cajas.push_back({ 1270, 1110, 40, 40 });  // Silla volcada


    // --- RELLENO DE PASILLOS INTERIORES (CON PROPÓSITO) ---
    // (Limpiados de desorden aleatorio)

    // Pasillo Interior Oeste (Alcove de Descanso)
    muros.push_back({ -850, -100, 100, 20 }); // Muro superior del alcove
    muros.push_back({ -850, 100, 100, 20 });  // Muro inferior del alcove
    muros.push_back({ -850, -100, 20, 220 }); // Muro trasero del alcove
    cajas.push_back({ -830, -10, 60, 20 });   // Mesa dentro del alcove

    // Pasillo Interior Este (Puesto de Seguridad)
    muros.push_back({ 850, -75, 20, 150 });   // Pared trasera
    muros.push_back({ 830, -75, 20, 20 });    // Pared superior
    muros.push_back({ 830, 55, 20, 20 });     // Pared inferior
    cajas.push_back({ 830, -55, 20, 110 });   // Escritorio/Consola

    // Pasillo Interior Sur (Bancos)
    cajas.push_back({ -150, 580, 100, 20 });  // Banco (pegado a pared de sala jefe)
    cajas.push_back({ 50, 580, 100, 20 });    // Banco (pegado a pared de sala jefe)

    // Pasillo Interior Norte (Restos del colapso)
    muros.push_back({ -200, -750, 400, 20 }); // Muro horizontal largo (Original)
    cajas.push_back({ -220, -780, 40, 40 });  // Escombro (Original)
    cajas.push_back({ 220, -720, 40, 40 });   // Escombro (Original)


    // --- ¡¡NUEVO!! RELLENO TEMÁTICO DE ZONAS EXTERNAS (BORDES) ---

    // --- Zona Borde Norte (Caseta de Seguridad de Logística) ---
    // (X: -900 a 900, Y: -1500 a -900)
    // Habitacion (más grande y asimétrica, movida al Oeste)
    muros.push_back({ -700, -1300, 20, 300 }); // Pared Oeste (X: -700)
    muros.push_back({ -400, -1300, 20, 300 }); // Pared Este (X: -400)
    muros.push_back({ -700, -1400, 320, 20 }); // Pared Norte (Fondo)
    muros.push_back({ -700, -1300, 100, 20 }); // Pared Sur (Izquierda de la puerta)
    muros.push_back({ -480, -1300, 80, 20 });  // Pared Sur (Derecha de la puerta)
    // Contenido
    cajas.push_back({ -680, -1380, 100, 40 }); // Escritorio
    muros.push_back({ -420, -1380, 20, 60 });  // Taquilla
    cajas.push_back({ 500, -1100, 150, 150 }); // Pila de cajas (punto de interés suelto)

    // --- Zona Borde Sur (Estación de Triaje / Barricada) ---
    // (X: -900 a 900, Y: 900 a 1500)
    // Barricada (diagonal, asimétrica)
    muros.push_back({ -250, 1200, 500, 20 }); // Barricada principal
    // ¡CORREGIDO! Cajas movidas 10px arriba para no superponer
    cajas.push_back({ -200, 1160, 40, 40 });  // Cobertura (delante)
    cajas.push_back({ 100, 1160, 40, 40 });   // Cobertura (delante)
    // Zona de triaje (detrás de la barricada)
    cajas.push_back({ -150, 1240, 100, 40 }); // Cama 1
    cajas.push_back({ 50, 1240, 100, 40 });   // Cama 2
    cajas.push_back({ -50, 1300, 40, 40 });   // Suministros (detrás)

    // --- Zona Borde Oeste (Colapso Estructural) ---
    // (X: -1500 a -900, Y: -900 a 900)
    // Sin habitación, solo escombros (como pediste)
    muros.push_back({ -1200, -300, 150, 40 }); // Losa de pared caída
    muros.push_back({ -1400, 100, 40, 200 });  // Pilar caído
    cajas.push_back({ -1180, -280, 40, 40 });  // Escombro
    cajas.push_back({ -1380, 300, 60, 60 });   // Escombro grande
    cajas.push_back({ -1100, 400, 80, 40 });   // Mueble de oficina

    // --- Zona Borde Este (Laboratorio de Observación Psiónica) ---
    // (X: 900 a 1500, Y: -900 a 900)
    // Habitacion (más grande, entrada rota)
    muros.push_back({ 1100, -200, 20, 400 }); // Pared Oeste (Fondo)
    muros.push_back({ 1300, -200, 20, 150 }); // Pared Este (Norte, rota)
    muros.push_back({ 1300, 50, 20, 150 });   // Pared Este (Sur, rota)
    muros.push_back({ 1100, -200, 200, 20 }); // Pared Norte
    muros.push_back({ 1100, 200, 200, 20 });  // Pared Sur
    // Contenido (explosionado)
    muros.push_back({ 1120, -100, 20, 200 }); // Panel de servidor
    cajas.push_back({ 1150, 0, 80, 80 });     // Equipo central
    // ¡CORREGIDO! Escombros caóticos en el pasillo eliminados

    // --- FIN DE NUEVO RELLENO ---
}

void Mapa::poblarMundo(GestorEntidades& gestor)
{
    spawnsCofres.clear();

    // --- ¡NUEVO! Helper para registrar cofres con colisión ---
    auto registrarCofreConColision = [&](GestorEntidades& gestor, const SpawnCofre& spawn, int lootID) {
        // 1. Registrar el consumible
        gestor.registrarConsumible(new Cofre(spawn.pos, lootID, spawn.orient));

        // 2. Crear su rect de colision
        Rectangle cofreRect;
        if (spawn.orient == CofreOrientacion::HORIZONTAL) {
            cofreRect = { spawn.pos.x - 12.5f, spawn.pos.y - 7.5f, 25, 15 };
        } else {
            cofreRect = { spawn.pos.x - 7.5f, spawn.pos.y - 12.5f, 15, 25 };
        }

        // 3. Añadirlo a la lista de 'cajas' para que colisione
        this->cajas.push_back(cofreRect);
    };
    // --- Fin del Helper ---

    // --- Definición de Zonas de Spawn ---
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

    // --- ¡NUEVO! Zonas de los Bordes Exteriores (Corredores) ---
    Rectangle corredorBordeN = { -900, -1500, 1800, 600 };
    Rectangle corredorBordeS = { -900, 900, 1800, 600 };
    Rectangle corredorBordeO = { -1500, -900, 600, 1800 };
    Rectangle corredorBordeE = { 900, -900, 600, 1800 };

    // --- ¡NUEVO! Zonas de las Mini-Habitaciones ---
    Rectangle habSeguridadN = { -680, -1380, 260, 80 }; // (Interior Caseta Seg.)
    Rectangle zonaBarricadaS = { -200, 1220, 300, 100 }; // (Detrás de la barricada)
    Rectangle zonaColapsoO = { -1400, -300, 200, 600 }; // (Zona de escombros)
    Rectangle habObservacionE = { 1120, -180, 160, 360 }; // (Interior Lab.)

    // --- ¡NUEVO! Zonas de Puestos Interiores ---
    Rectangle zonaAlcoveOeste = { -830, -80, 60, 160 }; // El interior del alcove
    Rectangle puestoSeguridadE = { 830, -55, 20, 110 }; // (Interior Puesto Seg.)

    std::vector<Rectangle> zonasHabitaciones = { zonaAlmacen, zonaElectrica, zonaOficinas, zonaDormis };

    // --- Generacion de Enemigos ---
    // (Total: 30 enemigos + Jefe + Fantasma)

    // Habitaciones (10)
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

    // Pasillos Interiores (6)
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(pasilloOeste)));
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(pasilloOeste)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(pasilloEste)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(pasilloEste)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(pasilloSur)));
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(pasilloSur)));

    // Anillo Interior (2)
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(anilloNorte)));
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(anilloSur)));

    // Jefe y Fantasma
    gestor.registrarJefe(new Jefe({0.0f, 0.0f}));
    gestor.registrarEnemigo(Spawner<Fantasma>::Spawn({-9999, -9999}));

    // --- ¡NUEVO! Spawns en Zonas de Bordes Exteriores (Lore-driven) ---
    // (12 Enemigos)
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(corredorBordeN))); // Patrulla
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(habSeguridadN))); // Guardia
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(habSeguridadN))); // Guardia

    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(corredorBordeE))); // Patrulla
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(habObservacionE))); // Experimento?
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(habObservacionE))); // Científico

    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(corredorBordeS))); // Vagando
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(zonaBarricadaS))); // Rompió la barricada
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(zonaBarricadaS))); // Refugiado

    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(zonaColapsoO))); // Entre escombros
    gestor.registrarEnemigo(Spawner<MonstruoObeso>::Spawn(getPosicionSpawnValida(zonaColapsoO))); // Entre escombros
    gestor.registrarEnemigo(Spawner<Zombie>::Spawn(getPosicionSpawnValida(corredorBordeO))); // Patrulla


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

    // --- ¡NUEVO! Consumibles en Zonas de Bordes Exteriores (Lore-driven) ---
    gestor.registrarConsumible(Spawner<CajaDeMuniciones>::Spawn(getPosicionSpawnValida(habSeguridadN)));
    gestor.registrarConsumible(Spawner<Armadura>::Spawn(getPosicionSpawnValida(habSeguridadN)));
    gestor.registrarConsumible(Spawner<Bateria>::Spawn(getPosicionSpawnValida(habObservacionE)));
    gestor.registrarConsumible(Spawner<Botiquin>::Spawn(getPosicionSpawnValida(zonaBarricadaS)));
    gestor.registrarConsumible(Spawner<Botiquin>::Spawn(getPosicionSpawnValida(zonaBarricadaS)));
    gestor.registrarConsumible(Spawner<Bateria>::Spawn(getPosicionSpawnValida(zonaColapsoO)));


    // --- Logica de Cofres y Llave ---
    int zonaLlaveIdx = GetRandomValue(0, zonasHabitaciones.size() - 1);
    SpawnCofre spawnLlave = getSpawnCofrePegadoAPared(zonasHabitaciones[zonaLlaveIdx]);
    // --- MODIFICADO ---
    registrarCofreConColision(gestor, spawnLlave, 99);
    spawnsCofres.push_back(spawnLlave.pos);

    for (int i = 0; i < zonasHabitaciones.size(); i++)
    {
        int cofresPorHabitacion = GetRandomValue(1, 2);
        for (int j = 0; j < cofresPorHabitacion; j++)
        {
            int lootID = GetRandomValue(1, 4);
            SpawnCofre spawnCofre = getSpawnCofrePegadoAPared(zonasHabitaciones[i]);
            // --- MODIFICADO ---
            registrarCofreConColision(gestor, spawnCofre, lootID);
            spawnsCofres.push_back(spawnCofre.pos);
        }
    }

    // --- ¡NUEVO! Cofres en Puntos de Interés Interiores ---
    SpawnCofre spawnAlcove = getSpawnCofrePegadoAPared(zonaAlcoveOeste);
    // --- MODIFICADO ---
    registrarCofreConColision(gestor, spawnAlcove, GetRandomValue(1, 4));
    spawnsCofres.push_back(spawnAlcove.pos);

    SpawnCofre spawnSeguridadE = getSpawnCofrePegadoAPared(puestoSeguridadE);
    // --- MODIFICADO ---
    registrarCofreConColision(gestor, spawnSeguridadE, GetRandomValue(1, 4));
    spawnsCofres.push_back(spawnSeguridadE.pos);

    // --- ¡NUEVO! Cofres en Mini-Habitaciones Exteriores (Lore-driven, 1 por hab) ---
    SpawnCofre spawnCofreBN = getSpawnCofrePegadoAPared(habSeguridadN);
    // --- MODIFICADO ---
    registrarCofreConColision(gestor, spawnCofreBN, GetRandomValue(1, 4));
    spawnsCofres.push_back(spawnCofreBN.pos);

    SpawnCofre spawnCofreBS = getSpawnCofrePegadoAPared(zonaBarricadaS);
    // --- MODIFICADO ---
    registrarCofreConColision(gestor, spawnCofreBS, GetRandomValue(1, 4));
    spawnsCofres.push_back(spawnCofreBS.pos);

    SpawnCofre spawnCofreBE = getSpawnCofrePegadoAPared(habObservacionE);
    // --- MODIFICADO ---
    registrarCofreConColision(gestor, spawnCofreBE, GetRandomValue(1, 4));
    spawnsCofres.push_back(spawnCofreBE.pos);

    SpawnCofre spawnCofreBO = getSpawnCofrePegadoAPared(zonaColapsoO);
    // --- MODIFICADO ---
    registrarCofreConColision(gestor, spawnCofreBO, GetRandomValue(1, 4));
    spawnsCofres.push_back(spawnCofreBO.pos);


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
    Rectangle cajaParaNota = { -1050, -1450, 40, 40}; // ¡Actualizado a la nueva pos de la caja!
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
