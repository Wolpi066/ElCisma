#include "raylib.h"
#include "Juego.h"
#include "SistemaArchivos.h"
// ¡Necesitamos incluir "Constantes.h" para inicializar la ventana!
#include "Constantes.h"

int main() {

    // 1. Inicializar la ventana de Raylib
    InitWindow(Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, "Noche Movidita");

    // --- ¡¡CAMBIO CRÍTICO!! Inicializar el dispositivo de Audio ---
    // Esto es obligatorio para que funcionen LoadSound/LoadMusicStream
    InitAudioDevice();
    // -------------------------------------------------------------

    SetTargetFPS(60); // Buena práctica
    HideCursor();     // Oculta el cursor del sistema.

    // 2. Crear la instancia del juego
    // Esto llamará al constructor de Juego, que prepara el mapa, gestor y CARGA LOS SONIDOS.
    // Por eso InitAudioDevice() tiene que estar ANTES de esta línea.
    Juego miJuego;

    // 3. Ejecutar el juego
    miJuego.run();

    // 4. Limpieza
    // Cerrar el dispositivo de audio antes de la ventana
    CloseAudioDevice(); // <--- ¡AÑADIDO!

    CloseWindow();
    return 0;
}
