// src/SistemaArchivos.cpp

#include "SistemaArchivos.h"
#include <fstream>
#include <iostream>

// Inicialización del puntero estático a nullptr
SistemaArchivos* SistemaArchivos::instancia = nullptr;

SistemaArchivos::SistemaArchivos() : puntuacionMaxima(0) {
    cargarPuntuacion();
}

SistemaArchivos::~SistemaArchivos() {
    // No guarda aquí automáticamente, se hace en actualizarPuntuacionMaxima
}

SistemaArchivos& SistemaArchivos::getInstancia() {
    if (instancia == nullptr) {
        instancia = new SistemaArchivos();
    }
    return *instancia;
}

void SistemaArchivos::cargarPuntuacion() {
    // Abre el archivo en modo binario
    std::ifstream archivo(NOMBRE_ARCHIVO, std::ios::in | std::ios::binary);
    if (archivo.is_open()) {
        // Lee un entero (la puntuación)
        archivo.read(reinterpret_cast<char*>(&puntuacionMaxima), sizeof(int));
        archivo.close();
    }
    // Si no abre, puntuacionMaxima se queda en 0 (inicializado en el constructor)
}

void SistemaArchivos::guardarPuntuacion() {
    // Abre el archivo en modo binario, sobrescribiendo el contenido (trunc)
    std::ofstream archivo(NOMBRE_ARCHIVO, std::ios::out | std::ios::binary | std::ios::trunc);
    if (archivo.is_open()) {
        // Escribe el entero
        archivo.write(reinterpret_cast<const char*>(&puntuacionMaxima), sizeof(int));
        archivo.close();
    } else {
        std::cerr << "ERROR: No se pudo guardar la puntuación en " << NOMBRE_ARCHIVO << std::endl;
    }
}

void SistemaArchivos::actualizarPuntuacionMaxima(int nuevaPuntuacion) {
    if (nuevaPuntuacion > puntuacionMaxima) {
        puntuacionMaxima = nuevaPuntuacion;
        guardarPuntuacion(); // Guarda automáticamente si se establece un nuevo récord
    }
}
