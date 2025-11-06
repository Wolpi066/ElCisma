#pragma once

namespace Constantes {

    // --- Configuracion de Pantalla ---
    const int ANCHO_PANTALLA = 1024;
    const int ALTO_PANTALLA = 768;

    // --- Parametros de Jugador/Entidad ---
    const float VELOCIDAD_JUGADOR = 2.5f; // <-- MODIFICADO (Antes 3.5f)
    const int VIDA_MAX_JUGADOR = 10;
    const int MUNICION_MAX = 20;
    const int BATERIA_MAX = 100;

    // --- Parametros de Enemigo ---
    const int VIDA_ZOMBIE = 3;
    const int DANIO_ZOMBIE = 1;
    const float VELOCIDAD_ZOMBIE = 1.0f;
    const float RADIO_ZOMBIE = 10.0f;

    const int VIDA_OBESO = 6;
    const int DANIO_OBESO = 2;
    const float VELOCIDAD_OBESO = 0.5f;
    const float RADIO_OBESO = 20.0f;

    const int VIDA_FANTASMA = 100;
    const int DANIO_FANTASMA = 10;
    const float VELOCIDAD_FANTASMA = 0.5f;
    const float RADIO_FANTASMA = 15.0f;

    // --- Parametros de IA ---
    const float RANGO_VISUAL_ZOMBIE = 250.0f;
    const float ANGULO_CONO_ZOMBIE = 90.0f;
    const float RANGO_AUDIO_ZOMBIE = 100.0f;
}
