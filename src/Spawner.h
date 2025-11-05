#pragma once
#include "raylib.h"
#include <stdexcept> // Para std::runtime_error
#include <new>       // Para std::bad_alloc

template <typename T> // "T" es un tipo de dato genérico
class Spawner {
public:
    // Un método estático (static) se puede llamar sin crear un objeto "Spawner"
    // Ejemplo: Enemigo* z = Spawner<Zombie>::Spawn({100, 100});

    static T* Spawn(Vector2 pos) {

        // --- MANEJO DE EXCEPCIONES (try/catch/throw) ---
        try {

            // --- MEMORIA DINÁMICA (new) ---
            // Creamos una nueva instancia de "T" (sea lo que sea) en el heap
            // y guardamos su dirección en un PUNTERO (*).
            T* entidad = new T(pos);

            if (entidad == nullptr) {
                // --- 'throw' ---
                throw std::runtime_error("Fallo al asignar memoria para la entidad");
            }

            return entidad;

        }
        // --- 'catch' ---
        catch (const std::bad_alloc& e) {
            TraceLog(LOG_ERROR, "EXCEPCION: %s", e.what());
            throw std::runtime_error("Excepcion de mala asignacion (bad_alloc) en Spawner");
        }
        catch (const std::exception& e) {
            TraceLog(LOG_ERROR, "EXCEPCION: %s", e.what());
            throw; // Relanzamos la misma excepción
        }
    }
};
