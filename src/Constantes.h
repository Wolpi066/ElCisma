#pragma once

namespace Constantes {

    // --- Configuracion de Pantalla ---
    const int ANCHO_PANTALLA = 1024;
    const int ALTO_PANTALLA = 768;

    // --- ¡¡NUEVO!! Constantes de Animación del Jugador ---
    const int PLAYER_FRAME_WIDTH = 63;
    const int PLAYER_FRAME_HEIGHT = 63;
    const int PLAYER_ANIM_FRAMES = 5;       // 5 frames por spritesheet
    const float PLAYER_ANIM_SPEED = 8.0f;   // 8 frames por segundo (ajústalo a tu gusto)

    // --- Parametros de Jugador/Entidad ---
    const float VELOCIDAD_JUGADOR = 2.5f;
    const int VIDA_MAX_JUGADOR = 10;
    const int MUNICION_MAX = 20;
    const int BATERIA_MAX = 100;
    const float RADIO_SPAWN_SEGURO_JUGADOR = 300.0f;

    // --- Constantes de Jugador ---
    const float ANCHO_CONO_LINTERNA = 45.0f;
    const float ALCANCE_LINTERNA = 350.0f;
    const float TIEMPO_RECARGA_DISPARO = 0.5f;
    const float TIEMPO_INMUNIDAD_DANIO = 1.0f;
    const float BATERIA_CONSUMO_SEGUNDO = 0.5f; // <-- ¡¡ARREGLADO!! (1 punto cada 2 seg)
    const float BATERIA_FLICKER_THRESHOLD = 30.0f;
    const float VELOCIDAD_LINTERNA = 8.0f; // <-- ¡¡RESTAURADO!! (Velocidad de suavizado)
    // -----------------------------------------------------

    // --- Parametros de Enemigo ---
    const int VIDA_ZOMBIE = 3;
    const int DANIO_ZOMBIE = 1;
    const float VELOCIDAD_ZOMBIE = 1.0f;
    const float RADIO_ZOMBIE = 10.0f;

    const int VIDA_OBESO = 6;
    const int DANIO_OBESO = 2;
    const float VELOCIDAD_OBESO = 0.8f;
    const float RADIO_OBESO = 20.0f;

    const int VIDA_FANTASMA = 100;
    const int DANIO_FANTASMA = 10;
    const float VELOCIDAD_FANTASMA = 0.5f;
    const float RADIO_FANTASMA = 15.0f;

    // --- Parametros de IA ---
    const float RANGO_VISUAL_ZOMBIE = 160.0f;
    const float ANGULO_CONO_ZOMBIE = 80.0f;
    const float RANGO_AUDIO_ZOMBIE = 100.0f;
}

// --- Namespace de Jefe ---
namespace ConstantesJefe {

    // --- Stats Base ---
    const int VIDA_JEFE = 800;
    const int DANIO_CONTACTO_JEFE = 20;
    const float RADIO_JEFE = 40.0f;

    // --- Fases ---
    const float VIDA_FASE_DOS_TRIGGER = 0.3f; // 30%

    // --- Daño ---
    const float MULT_DANIO_ESPALDA = 5.0f;
    const float MULT_DANIO_FRENTE = 1.0f;

    // --- Fase 1: Embestida ---
    const float VELOCIDAD_EMBESTIDA_JEFE = 600.0f;
    const float TIEMPO_APUNTAR_EMBESTIDA = 0.8f;
    const float TIEMPO_ATURDIMIENTO_CHOQUE = 2.5f;
    const float TIEMPO_MAX_EMBESTIDA = 1.5f; // <-- ¡¡ARREGLO BUG!! (Timeout)

    // --- Fase 1: Timers Ataque ---
    const float TIEMPO_PREPARAR_ESCOPETA = 0.5f;
    const float TIEMPO_PREPARAR_PULSO = 1.0f;
    const float COOLDOWN_ESCOPETA = 1.5f;
    const float COOLDOWN_PULSO = 2.0f;
    const float COOLDOWN_ATURDIMIENTO = 1.0f;

    // --- Fase 2: Timers Ataque ---
    const float VELOCIDAD_DASH_SOMBRA = 900.0f; // <-- ¡¡NUEVO!!
    const float TIEMPO_APUNTAR_SOMBRA = 1.0f;  // <-- ¡¡NUEVO!!
    const float TIEMPO_DASH_SOMBRA = 0.5f;     // <-- ¡¡NUEVO!! (Duración del dash)
    const float TIEMPO_PREPARAR_DISPARO_INT = 0.2f;
    const float TIEMPO_PREPARAR_LLUVIA = 0.5f;
    const float COOLDOWN_DASH_SOMBRA = 1.0f;     // <-- ¡¡NUEVO!!
    const float COOLDOWN_DISPARO_INT = 0.5f;
    const float COOLDOWN_LLUVIA = 1.0f;
}
