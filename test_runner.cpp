// LIMPIEZA WINDOWS
#define NOGDI
#define NOUSER
#define WIN32_LEAN_AND_MEAN

// LIBRERÍAS ESTÁNDAR
#include <vector>
#include <cmath>
#include <iostream>

// FRAMEWORK TAU
#include <tau/tau.h>
#ifdef tau
#undef tau
#endif

// RAYLIB
#include "raylib.h"

#define private public
#define protected public

#include "src/Protagonista.h"
#include "src/Zombie.h"
#include "src/MonstruoObeso.h"
#include "src/Fantasma.h"
#include "src/Jefe.h"
#include "src/Cofre.h"
#include "src/Nota.h"
#include "src/Consumible.h"

// Mock simple de Mapa para evitar dependencias
class Mapa {
public:
    std::vector<Rectangle> getMuros() const { return {}; }
};

#undef private
#undef protected

#include "src/Constantes.h"

// SETUP GLOBAL
struct EntornoDePruebas {
    EntornoDePruebas() {
        SetTraceLogLevel(LOG_NONE);
        SetConfigFlags(FLAG_WINDOW_HIDDEN);
        InitWindow(800, 600, "TEST RUNNER ENV");
        InitAudioDevice();
        printf("\n=======================================================\n");
        printf("   SISTEMA DE PRUEBAS: VERIFICACION DE LOGICA     \n");
        printf("=======================================================\n\n");
    }
    ~EntornoDePruebas() {
        CloseAudioDevice();
        CloseWindow();
    }
};
EntornoDePruebas g_entorno;


// PROTAGONISTA
TEST(Protagonista, ValoresIniciales) {
    Protagonista p({0,0});
    CHECK(p.getVida() == Constantes::VIDA_MAX_JUGADOR);
    CHECK(p.getMunicion() == Constantes::MUNICION_MAX);
}

TEST(Protagonista, InmunidadYCombate) {
    Protagonista p({0,0});
    int vidaBase = p.getVida();
    p.recibirDanio(1);
    CHECK(p.getVida() == vidaBase - 1);
    CHECK(p.tiempoInmune > 0.0f);
    p.recibirDanio(1);
    CHECK(p.getVida() == vidaBase - 1);
}

TEST(Protagonista, ItemsEspeciales) {
    Protagonista p({0,0});
    CHECK(p.tieneLlave == false);
    p.recibirLlave();
    CHECK(p.tieneLlave == true);
}


// ENEMIGOS
TEST(Enemigo_Zombie, StatsBasicos) {
    Zombie z({100, 100});
    CHECK(z.vida == Constantes::VIDA_ZOMBIE);
    CHECK(z.estaMuerto() == false);
}

TEST(Enemigo_Zombie, RecibirDanioYMorir) {
    Zombie z({0,0});
    int vidaInicial = z.vida;

    // Recibe daño
    z.recibirDanio(1);
    CHECK(z.vida == vidaInicial - 1);

    // Golpe letal
    z.recibirDanio(100);

    // Verificamos muerte
    CHECK(z.vida <= 0);

    // Verificamos el estado lógico inmediato
    CHECK(z.estaMuriendo == true);
}

TEST(Enemigo_Obeso, EsMasResistente) {
    MonstruoObeso obeso({0,0});
    Zombie zombie({0,0});
    CHECK(obeso.vida > zombie.vida);
}

TEST(Enemigo_Fantasma, LogicaBasica) {
    Fantasma f({0,0});
    f.recibirDanio(10);
    CHECK(f.vida < Constantes::VIDA_FANTASMA);
}


// JEFE FINAL
TEST(Jefe, InicializacionMonstruosa) {
    Jefe boss({500, 500});

    CHECK(boss.vida == boss.vidaMaxima);
    CHECK(boss.vida == 100);
    CHECK(boss.faseActual == FaseJefe::FASE_UNO);
}

TEST(Jefe, SistemaDeDanio_Blindaje) {
    Jefe boss({0,0});
    int vidaFull = boss.vida;
    Vector2 posJugador = {0, 10};

    boss.recibirDanio(1000, posJugador);

    CHECK(boss.vida < vidaFull);

    CHECK(boss.vida > 0);
}

TEST(Jefe, TransicionDeFase_Trigger) {
    Jefe boss({0,0});

    CHECK(boss.enTransicion == false);

    boss.transicionAFaseDos();

    CHECK(boss.enTransicion == true);
    CHECK(boss.faseActual == FaseJefe::FASE_UNO); // Aún no cambia
}


// INTERACTIVOS
TEST(Interactivos_Cofre, UsarCofre) {
    Cofre c({0,0}, 1, CofreOrientacion::HORIZONTAL);
    Protagonista p({0,0});

    CHECK(c.abierto == false);
    c.usar(p);
    CHECK(c.abierto == true);
}

TEST(Interactivos_Nota, Propiedades) {
    Nota n({0,0}, 55);
    CHECK(n.notaID == 55);
    Protagonista p({0,0});
    n.usar(p);

    CHECK(n.notaID == 55);
}

TAU_MAIN()
