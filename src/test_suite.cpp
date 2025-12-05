// ==========================================================================
// ARCHIVO: src/test_suite.cpp
// DESCRIPCIÓN: Suite de tests completa.
// ==========================================================================

// 1. CONFIGURACIÓN ANTI-CONFLICTOS WINDOWS (Siempre primero)
#if defined(_WIN32)
    #define NOGDI
    #define NOUSER
    #define WIN32_LEAN_AND_MEAN
#endif

// 2. RAYLIB
#include "raylib.h"

// Limpiamos la basura que Windows puede haber metido
#if defined(_WIN32)
    #undef Rectangle
    #undef CloseWindow
    #undef ShowCursor
    #undef LoadImage
    #undef PlaySound
#endif

// 3. TAU (MODO AUTOMÁTICO)
// NO definimos TAU_NO_MAIN. Dejamos que tau genere todo.
#include <tau/tau.h>
#include <iostream>

// 4. TUS CLASES
#include "Constantes.h"
#include "Protagonista.h"
#include "Zombie.h"
#include "MonstruoObeso.h"
#include "Jefe.h"
#include "Botiquin.h"
#include "CajaDeMuniciones.h"
#include "Bateria.h"
#include "Armadura.h"
#include "Llave.h"
#include "BalaDeRifle.h"

// ==========================================================================
// INICIALIZADOR AUTOMÁTICO (Global Fixture)
// ==========================================================================
// Esta estructura se crea SOLA antes de que arranque el main de Tau.
// Aquí encendemos Raylib para que tus texturas/sonidos carguen bien.

struct ArrancadorRaylib {
    ArrancadorRaylib() {
        std::cout << "[TESTS] Iniciando contexto de Raylib..." << std::endl;
        SetTraceLogLevel(LOG_NONE);
        InitWindow(100, 100, "TEST_RUNNER");
        InitAudioDevice();
    }
    ~ArrancadorRaylib() {
        std::cout << "[TESTS] Cerrando contexto..." << std::endl;
        CloseAudioDevice();
        ::CloseWindow();
    }
};

// La variable global que activa el constructor de arriba
static ArrancadorRaylib arrancador;

// ==========================================================================
// PARCHE PARA WINDOWS (Si usas modo GUI)
// ==========================================================================
// Si tu proyecto busca WinMain, esto redirige al main de Tau.
#if defined(_WIN32)
#include <windows.h>
extern int main(int argc, char* argv[]); // El main que genera Tau
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    return main(__argc, __argv);
}
#endif

// ==========================================================================
// TUS TESTS (Rigurosos y Completos)
// ==========================================================================

// --- 1. PROTAGONISTA ---
TEST(Protagonista, NaceConStatsMaximos) {
    Protagonista p({0, 0});
    CHECK(p.getVida() == Constantes::VIDA_MAX_JUGADOR);
    CHECK(p.getMunicion() == Constantes::MUNICION_MAX);
    CHECK(p.getBateria() == Constantes::BATERIA_MAX);
    CHECK(p.estaVivo() == true);
}

TEST(Protagonista, DanioReduceVida) {
    Protagonista p({0, 0});
    int vidaInicial = p.getVida();
    p.recibirDanio(2);
    CHECK(p.getVida() == vidaInicial - 2);
    // Debe volverse inmune temporalmente
    CHECK(p.getTiempoInmune() > 0.0f);
}

TEST(Protagonista, ArmaduraEvitaDanio) {
    Protagonista p({0, 0});
    p.recibirArmadura();
    int vidaAntes = p.getVida();

    p.recibirDanio(5); // Golpe fuerte
    CHECK(p.getVida() == vidaAntes); // Vida intacta
}

TEST(Protagonista, DisparoConsumeBala) {
    Protagonista p({0, 0});
    int balasAntes = p.getMunicion();

    // true = quiere disparar
    p.intentarDisparar(true);

    CHECK(p.getMunicion() == balasAntes - 1);
}

// --- 2. ENEMIGOS ---
TEST(Zombie, ComportamientoBasico) {
    Zombie z({0,0});
    // Nace patrullando
    CHECK(z.getEstadoIA() == EstadoIA::PATRULLANDO);

    // Al recibir daño, debe perseguir (tu lógica crítica)
    z.recibirDanio(1);
    CHECK(z.getEstadoIA() == EstadoIA::PERSIGUIENDO);
}

TEST(MonstruoObeso, EsMasFuerte) {
    Zombie z({0,0});
    MonstruoObeso o({0,0});

    // Verificamos balance
    CHECK(o.getVida() > z.getVida());
    CHECK(o.getDanio() > z.getDanio());
    CHECK(o.getVelocidad() < z.getVelocidad()); // Más lento
}

// --- 3. JEFE FINAL ---
TEST(Jefe, FasesCorrectas) {
    Jefe j({0,0});
    CHECK(j.getFase() == FaseJefe::FASE_UNO);

    j.forzarFaseDos();
    // Al forzar, entra en transición (invulnerable)
    CHECK(j.esInvulnerableActualmente() == true);
}

TEST(Jefe, Muerte) {
    Jefe j({0,0});
    j.forzarMuerte();
    CHECK(j.getFase() == FaseJefe::MURIENDO);
}

// --- 4. ITEMS ---
TEST(Items, BotiquinCura) {
    Protagonista p({0,0});
    p.recibirDanio(5); // Herir primero

    Botiquin b({0,0});
    int res = b.usar(p);

    CHECK(res == 1); // Usado
    CHECK(p.getVida() == Constantes::VIDA_MAX_JUGADOR);
    CHECK(b.estaConsumido() == true);
}

TEST(Items, MunicionRecarga) {
    Protagonista p({0,0});
    p.intentarDisparar(true); // Gastar 1

    CajaDeMuniciones c({0,0});
    c.usar(p);

    CHECK(p.getMunicion() == Constantes::MUNICION_MAX);
    CHECK(c.estaConsumido() == true);
}

TEST(Items, LlaveFunciona) {
    Protagonista p({0,0});
    CHECK(p.getTieneLlave() == false);

    Llave k({0,0});
    k.usar(p);

    CHECK(p.getTieneLlave() == true);
    CHECK(k.estaConsumido() == true);
}

// --- 5. BALAS ---
TEST(Bala, RifleBasica) {
    BalaDeRifle b({0,0}, {1,0});
    CHECK(b.estaActiva() == true);
    CHECK(b.getOrigen() == OrigenBala::JUGADOR);
    // Velocidad positiva en X
    CHECK(b.getVelocidad().x > 0);
}

TEST(Bala, Desactivacion) {
    BalaDeRifle b({0,0}, {1,0});
    b.desactivar();
    CHECK(b.estaActiva() == false);
    // Para el gestor, inactiva = muerta
    CHECK(b.estaMuerto() == true);
}
