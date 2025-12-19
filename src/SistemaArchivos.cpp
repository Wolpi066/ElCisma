#include "SistemaArchivos.h"
#include <fstream>
#include <iostream>

SistemaArchivos* SistemaArchivos::instancia = nullptr;

SistemaArchivos::SistemaArchivos() : puntuacionMaxima(0) {
    cargarPuntuacion();
}

SistemaArchivos::~SistemaArchivos() {
}

SistemaArchivos& SistemaArchivos::getInstancia() {
    if (instancia == nullptr) {
        instancia = new SistemaArchivos();
    }
    return *instancia;
}

void SistemaArchivos::cargarPuntuacion() {
    std::ifstream archivo(NOMBRE_ARCHIVO, std::ios::in | std::ios::binary);
    if (archivo.is_open()) {
        archivo.read(reinterpret_cast<char*>(&puntuacionMaxima), sizeof(int));
        archivo.close();
    }
}

void SistemaArchivos::guardarPuntuacion() {
    std::ofstream archivo(NOMBRE_ARCHIVO, std::ios::out | std::ios::binary | std::ios::trunc);
    if (archivo.is_open()) {
        archivo.write(reinterpret_cast<const char*>(&puntuacionMaxima), sizeof(int));
        archivo.close();
    } else {
        std::cerr << "ERROR: No se pudo guardar la puntuación en " << NOMBRE_ARCHIVO << std::endl;
    }
}

void SistemaArchivos::actualizarPuntuacionMaxima(int nuevaPuntuacion) {
    if (nuevaPuntuacion > puntuacionMaxima) {
        puntuacionMaxima = nuevaPuntuacion;
        guardarPuntuacion();
    }
}
