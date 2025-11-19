#pragma once
#include "raylib.h"
#include "Protagonista.h"

class Consumible
{
protected:
    Vector2 posicion;
    bool consumido;

    // Datos para el Pop-up
    const char* nombreItem;
    const char* descripcionItem;

public:
    Consumible(Vector2 pos);
    virtual ~Consumible();

    // Logica: Retorna int para compatibilidad con Cofre/Nota/Indicador (0 = default)
    virtual int usar(Protagonista& jugador) = 0;

    // Visuales
    virtual void dibujar() = 0;
    virtual Texture2D getTextura() = 0;

    // Getters
    const char* getNombre() const;
    const char* getDescripcion() const;

    // Metodos Virtuales (Permite que Cofre/Indicador los sobrescriban)
    virtual bool estaConsumido() const;
    virtual Vector2 getPosicion() const;
    virtual Rectangle getRect() const;
    virtual bool esInteraccionPorTecla() const;

    // Parche de compatibilidad para GestorEntidades
    virtual bool estaMuerto() const;

    void consumir();
};
