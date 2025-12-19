
#include "raylib.h"
#include "Juego.h"
#include "SistemaArchivos.h"

#include "Constantes.h"

int main() {

    // Inicializar la ventana de Raylib
    InitWindow(Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, "Noche Movidita");

    InitAudioDevice();

    SetTargetFPS(60);
    HideCursor();


    Juego miJuego;

    // Ejecutar el juego
    miJuego.run();

    // Limpieza
    // Cerrar el dispositivo de audio antes de la ventana
    CloseAudioDevice();

    CloseWindow();
    return 0;
}

