#pragma once
#include <string>

class SistemaArchivos {
private:
    static SistemaArchivos* instancia;
    int puntuacionMaxima;
    const char* NOMBRE_ARCHIVO = "hiscore.dat";

    SistemaArchivos();
    void cargarPuntuacion();
    void guardarPuntuacion();

public:
    static SistemaArchivos& getInstancia();
    ~SistemaArchivos();

    SistemaArchivos(const SistemaArchivos&) = delete;
    SistemaArchivos& operator=(const SistemaArchivos&) = delete;

    int getPuntuacionMaxima() const { return puntuacionMaxima; }
    void actualizarPuntuacionMaxima(int nuevaPuntuacion);
};
