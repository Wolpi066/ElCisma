#pragma once
#include "raylib.h"
#include <stdexcept>
#include <new>

template <typename T>
class Spawner {
public:
    static T* Spawn(Vector2 pos) {

        try {

            T* entidad = new T(pos);

            if (entidad == nullptr) {
                throw std::runtime_error("Fallo al asignar memoria para la entidad");
            }

            return entidad;

        }
        catch (const std::bad_alloc& e) {
            TraceLog(LOG_ERROR, "EXCEPCION: %s", e.what());
            throw std::runtime_error("Excepcion de mala asignacion (bad_alloc) en Spawner");
        }
        catch (const std::exception& e) {
            TraceLog(LOG_ERROR, "EXCEPCION: %s", e.what());
            throw;
        }
    }
};
