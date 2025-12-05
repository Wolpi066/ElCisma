# ElCisma



\# 🌑 El Cisma



\*\*El Cisma\*\* es una experiencia de \*Survival Horror\* inmersiva desarrollada en C++ sobre el motor \[Raylib](https://www.raylib.com/). Diseñado con un enfoque en la atmósfera opresiva y la tensión psicológica, el juego desafía al usuario a sobrevivir en un entorno hostil donde la gestión de recursos es la única clave para la supervivencia.



---



\## 🎮 Visión General del Producto



En \*\*El Cisma\*\*, el jugador se adentra en una instalación sumida en la oscuridad. La narrativa se desarrolla de forma ambiental, obligando al usuario a avanzar con cautela. La jugabilidad combina mecánicas de \*shooter\* cenital con elementos de terror psicológico y fases de combate de alta intensidad (\*Bullet Hell\*).



\### Características Principales

\* \*\*Motor de Iluminación Dinámica:\*\* La visión del jugador está restringida por el cono de luz de su linterna. La batería se agota progresivamente, reduciendo el campo visual y aumentando la vulnerabilidad.

\* \*\*Sistema de Combate Táctico:\*\* La munición es extremadamente limitada. Cada disparo cuenta y el jugador debe decidir cuándo combatir y cuándo conservar recursos.

\* \*\*Inteligencia Artificial Reactiva:\*\* Diversas entidades hostiles con comportamientos únicos que requieren estrategias de evasión y ataque diferenciadas.

\* \*\*Enfrentamientos Escalonados:\*\* Batallas contra jefes diseñadas con múltiples fases y patrones complejos que ponen a prueba los reflejos y la planificación del jugador.



\### Controles

La interfaz de control ha sido diseñada para ser minimalista e intuitiva, maximizando la inmersión:

\* \*\*WASD / Flechas:\*\* Desplazamiento del personaje.

\* \*\*Mouse:\*\* Apuntar dirección de la vista/linterna.

\* \*\*Click Izquierdo:\*\* Disparar arma.



---



\## 🧪 Ingeniería de Calidad: Suite de Pruebas (TDD)



Para garantizar la estabilidad y la integridad lógica de \*\*El Cisma\*\*, nuestro equipo de desarrollo ha implementado una arquitectura de pruebas unitarias automatizadas utilizando el framework \*\*Tau\*\*. Este sistema permite la verificación continua de las mecánicas críticas del juego sin intervención manual.



\### Metodología de Testing "Invasivo"

Utilizamos técnicas avanzadas de C++ (Access Hack Pattern) en nuestro entorno de pruebas (`test\_runner.cpp`). Esto nos permite inspeccionar y validar el estado interno de objetos encapsulados (variables `private`) sin comprometer la seguridad ni la estructura del código fuente de producción.



\### Cobertura del Sistema

Nuestra suite de pruebas valida automáticamente:

1\.  \*\*Lógica del Jugador:\*\* Integridad de salud, cálculos de invulnerabilidad y consumo de recursos.

2\.  \*\*Comportamiento de la IA:\*\* Estados de transición, detección de daño y lógica de eliminación.

3\.  \*\*Mecánicas de Jefes:\*\* Transiciones de fase, sistemas de blindaje direccional y lógica de inicialización.

4\.  \*\*Sistemas de Inventario:\*\* Interacción segura con objetos clave y consumibles.



\### 🚀 Ejecución de Pruebas (Entorno Code::Blocks)



El proyecto incluye una configuración de compilación dedicada para el aseguramiento de la calidad (QA).



1\.  Abra el proyecto `NocheMovidita.cbp` en Code::Blocks.

2\.  En la barra de herramientas, cambie el \*\*Build Target\*\* de `Debug`/`Release` a \*\*`Tests`\*\*.

3\.  Ejecute la compilación (\*\*Build and Run\*\* o `F9`).



El sistema desplegará una consola de diagnóstico limpia, reportando el estado de cada módulo lógico:



```text

=======================================================

&nbsp;  SISTEMA DE PRUEBAS: VERIFICACION DE LOGICA REAL     

=======================================================

\[ RUN      ] Protagonista.InmunidadYCombate

\[       OK ] Protagonista.InmunidadYCombate

...

\[ PASSED   ] All test suites.

