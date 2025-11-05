#pragma once
#include "Consumible.h"

// --- ¡NUEVO! Enum para la orientacion ---
enum class CofreOrientacion {
    HORIZONTAL, // Apoyado en pared Norte o Sur
    VERTICAL    // Apoyado en pared Este u Oeste
};

// Cofre que se abre con 'E' y suelta un item.
class Cofre : public Consumible {
private:
    int tipoDeLoot; // 1=Botiquin, 2=Bateria, 3=Municion, 4=Armadura, 99=Llave
    Rectangle rect; // Rectangulo del cofre (mas grande)
    CofreOrientacion orientacion;

public:
    Cofre(Vector2 pos, int lootID, CofreOrientacion orient);

    // Devuelve el 'tipoDeLoot' cuando se usa
    virtual int usar(Protagonista& jugador) override;
    virtual void dibujar() override;

    // Requiere la tecla 'E'
    virtual bool esInteraccionPorTecla() const override;

    // Anulamos getRect para usar nuestro rect mas grande
    virtual Rectangle getRect() const override;
};
