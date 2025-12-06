
#include "raylib.h"
#include "Juego.h"
#include "SistemaArchivos.h"

#include "Constantes.h"

int main() {

    // 1. Inicializar la ventana de Raylib
    InitWindow(Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, "Noche Movidita");

    // Esto es obligatorio para que funcionen LoadSound/LoadMusicStream
    InitAudioDevice();
    // -------------------------------------------------------------

    SetTargetFPS(60);
    HideCursor();


    Juego miJuego;

    // 3. Ejecutar el juego
    miJuego.run();

    // 4. Limpieza
    // Cerrar el dispositivo de audio antes de la ventana
    CloseAudioDevice();

    CloseWindow();
    return 0;
}

