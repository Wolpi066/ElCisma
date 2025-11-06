#include "raylib.h"
#include "Juego.h"
#include "SistemaArchivos.h"
// ¡Necesitamos incluir "Constantes.h" para inicializar la ventana!
#include "Constantes.h"

int main() {

    // 1. Inicializar la ventana de Raylib (¡Esto faltaba!)
    // Asumo que ANCHO_PANTALLA y ALTO_PANTALLA están en Constantes.h
    // Si no, reemplázalos por números (ej. 1280, 720)

    // CORRECCIÓN: Usamos el scope 'Constantes::' para acceder a las variables
    InitWindow(Constantes::ANCHO_PANTALLA, Constantes::ALTO_PANTALLA, "Noche Movidita");
    SetTargetFPS(60); // Buena práctica
    HideCursor();     // <-- ¡AÑADIDO! Oculta el cursor del sistema.

    // 2. Crear la instancia del juego
    // Esto llamará al constructor de Juego, que prepara el mapa, gestor, etc.
    Juego miJuego;

    // 3. Ejecutar el juego
    // La función 'run()' ES el bucle principal.
    // Llamará internamente a actualizar() y dibujar() en cada fotograma.
    miJuego.run();

    // 4. El bucle "while" que tenías aquí se ha eliminado.
    // Era redundante y causaba el error al intentar llamar
    // a funciones privadas.

    // 5. Cerrar la ventana
    // Esta línea solo se ejecutará cuando el bucle dentro de miJuego.run() termine
    // (es decir, cuando el usuario cierre la ventana).
    CloseWindow();
    return 0;
}
