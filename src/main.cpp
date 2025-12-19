#include "raylib.h"
#include "Juego.h"
#include "SistemaArchivos.h"
#include "Constantes.h"

// Emscripten
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

Juego* juegoGlobal = nullptr;

void UpdateDrawFrame() {
    if (juegoGlobal) {
        juegoGlobal->actualizar();
        juegoGlobal->dibujar();
    }
}

int main() {

    InitWindow(Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, "Noche Movidita");

    InitAudioDevice();

    SetTargetFPS(60);
    HideCursor();

    // Creamos el juego en el Heap
    juegoGlobal = new Juego();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    juegoGlobal->run();
#endif
    delete juegoGlobal;
    juegoGlobal = nullptr;

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
