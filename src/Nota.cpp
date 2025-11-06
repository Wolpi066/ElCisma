#include "Nota.h"
#include "raymath.h" // <-- ¡¡ERROR CORREGIDO!! Faltaba esta inclusion para sin()

Nota::Nota(Vector2 pos, int id)
    : Consumible(pos), notaID(id), leida(false) // Inicializar leida
{
}

void Nota::dibujar() {
    // Dibujamos un papelito blanco
    Rectangle rect = getRect();
    rect.width = 12;
    rect.height = 12;
    rect.x -= 1;
    rect.y -= 1;

    // La nota inicial (ID 0) tiene un brillo especial
    if (notaID == 0) {
        float alpha = (sin(GetTime() * 5.0f) + 1.0f) / 2.0f; // 0.0 a 1.0
        DrawCircleV(posicion, 10.0f, Fade((Color){0, 150, 255, 255}, alpha * 0.3f));
        DrawRectangleRec(rect, (Color){220, 240, 255, 255}); // Blanco-azulado
    } else {
        DrawRectangleRec(rect, WHITE);
    }

    DrawRectangleLinesEx(rect, 1.0f, BLACK);
}

int Nota::usar(Protagonista& jugador) {
    // ¡¡MODIFICADO!!
    this->leida = true; // Marcar como leida
    return notaID;
}

bool Nota::esInteraccionPorTecla() const {
    return true; // Se usa con 'E'
}

bool Nota::estaConsumido() const {
    // ¡¡MODIFICADO!!
    // Solo se consume (desaparece) la nota ID 0 y solo si ya fue leida.
    return (notaID == 0 && leida);
}
