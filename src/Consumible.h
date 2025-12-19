#pragma once
#include "raylib.h"
#include "Protagonista.h"

class Consumible
{
protected:
    Vector2 posicion;
    bool consumido;

    const char* nombreItem;
    const char* descripcionItem;

public:
    Consumible(Vector2 pos);
    virtual ~Consumible();

    virtual int usar(Protagonista& jugador) = 0;

    virtual void dibujar() = 0;
    virtual Texture2D getTextura() = 0;

    const char* getNombre() const;
    const char* getDescripcion() const;

    virtual bool estaConsumido() const;
    virtual Vector2 getPosicion() const;
    virtual Rectangle getRect() const;
    virtual bool esInteraccionPorTecla() const;

    virtual bool estaMuerto() const;

    void consumir();
};
