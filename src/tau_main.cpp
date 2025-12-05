// ==========================================================================
// ARCHIVO: tau_main.cpp
// ==========================================================================

// 1. PREVENIR CONFLICTOS DE WINDOWS (ANTES DE CUALQUIER INCLUDE)
#if defined(_WIN32)
    #define NOGDI
    #define NOUSER
    #define WIN32_LEAN_AND_MEAN
#endif

// 2. INCLUDES
#include "raylib.h"

// Limpieza de macros de Windows
#if defined(_WIN32)
    #undef Rectangle
    #undef CloseWindow
    #undef ShowCursor
    #undef LoadImage
    #undef PlaySound
#endif

// 3. INCLUDES DE TAU
// ¡IMPORTANTE! NO definimos TAU_NO_MAIN.
// Dejamos que Tau genere el 'int main()' y sus variables internas.
#include <tau/tau.h>

#include <iostream>

// 4. INCLUDES DEL JUEGO
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
// FIXTURE GLOBAL AUTOMÁTICO
// ==========================================================================
// Esta estructura se inicializa AUTOMÁTICAMENTE antes de que arranque el main()
// de Tau. Aquí encendemos Raylib para que los tests no fallen al cargar texturas.

struct EntornoDePruebas {
    EntornoDePruebas() {
        // Se ejecuta ANTES del main
        // SetTraceLogLevel(LOG_NONE); // Descomentar si quieres menos texto en consola
        InitWindow(100, 100, "TEST_CTX"); // Ventana mínima
        InitAudioDevice();
    }

    ~EntornoDePruebas() {
        // Se ejecuta AL SALIR
        CloseAudioDevice();
        ::CloseWindow();
    }
};

// La variable estática dispara el constructor al inicio
static EntornoDePruebas entornoGlobal;

// ==========================================================================
// PUENTE WINMAIN (Para que Windows no se queje)
// ==========================================================================
#if defined(_WIN32)
#include <windows.h>

// Declaramos que existe un main (el que generó Tau)
extern int main(int argc, char* argv[]);

// Si el linker busca WinMain (modo GUI), redirigimos al main de Tau
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Argumentos dummy para que Tau no falle
    int argc = 1;
    char* argv[] = { (char*)"test_runner", NULL };
    return main(argc, argv);
}
#endif

// ==========================================================================
// TESTS UNITARIOS
// ==========================================================================

TEST(Protagonista, Inicializacion) {
    Protagonista p({0, 0});
    CHECK(p.getVida() == Constantes::VIDA_MAX_JUGADOR);
    CHECK(p.estaVivo() == true);
}

TEST(Protagonista, RecibirDanio) {
    Protagonista p({0, 0});
    int vidaAntes = p.getVida();
    p.recibirDanio(2);
    CHECK(p.getVida() == vidaAntes - 2);
}

TEST(Protagonista, Armadura) {
    Protagonista p({0, 0});
    p.recibirArmadura();
    int vidaAntes = p.getVida();

    p.recibirDanio(5);

    CHECK(p.getVida() == vidaAntes);
}

TEST(Protagonista, Disparo) {
    Protagonista p({0, 0});
    int balas = p.getMunicion();

    // Intentar disparar (true = click presionado)
    int res = p.intentarDisparar(true);

    CHECK(res == 1); // 1 = Disparo OK
    CHECK(p.getMunicion() == balas - 1);
}

TEST(Zombie, ReaccionAlDanio) {
    Zombie z({0,0});
    // Nace patrullando
    CHECK(z.getEstadoIA() == EstadoIA::PATRULLANDO);

    // Al recibir daño, persigue
    z.recibirDanio(1);
    CHECK(z.getEstadoIA() == EstadoIA::PERSIGUIENDO);
}

TEST(MonstruoObeso, Balance) {
    Zombie z({0,0});
    MonstruoObeso o({0,0});

    // El obeso debe ser más duro y lento
    CHECK(o.getVida() > z.getVida());
    CHECK(o.getDanio() > z.getDanio());
    CHECK(o.getVelocidad() < z.getVelocidad());
}

TEST(Jefe, Fases) {
    Jefe j({0,0});
    CHECK(j.getFase() == FaseJefe::FASE_UNO);

    j.forzarFaseDos();
    CHECK(j.esInvulnerableActualmente() == true);
}

TEST(Items, Botiquin) {
    Protagonista p({0,0});
    p.recibirDanio(5);

    Botiquin b({0,0});
    b.usar(p);

    CHECK(p.getVida() == Constantes::VIDA_MAX_JUGADOR);
    CHECK(b.estaConsumido() == true);
}

TEST(Items, Municion) {
    Protagonista p({0,0});
    p.intentarDisparar(true);

    CajaDeMuniciones c({0,0});
    c.usar(p);

    CHECK(p.getMunicion() == Constantes::MUNICION_MAX);
    CHECK(c.estaConsumido() == true);
}

TEST(Bala, Fisica) {
    BalaDeRifle b({0,0}, {1,0});
    CHECK(b.estaActiva() == true);
    CHECK(b.getVelocidad().x > 0);
}
