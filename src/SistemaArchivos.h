// src/SistemaArchivos.h

#pragma once
#include <string>

class SistemaArchivos {
private:
    // Implementación del patrón Singleton
    static SistemaArchivos* instancia;
    int puntuacionMaxima;
    const char* NOMBRE_ARCHIVO = "hiscore.dat";

    SistemaArchivos(); // Constructor privado
    void cargarPuntuacion();
    void guardarPuntuacion();

public:
    // Método para obtener la única instancia
    static SistemaArchivos& getInstancia();
    ~SistemaArchivos();

    // Deshabilitar copia y asignación (para mantener el Singleton)
    SistemaArchivos(const SistemaArchivos&) = delete;
    SistemaArchivos& operator=(const SistemaArchivos&) = delete;

    // Métodos públicos
    int getPuntuacionMaxima() const { return puntuacionMaxima; }
    // Actualiza el hi-score si la nueva puntuación es mayor y guarda
    void actualizarPuntuacionMaxima(int nuevaPuntuacion);
};
