# Terrorizer

## MANUAL TÉCNICO DE "Terrorizer"

### Introducción
Este documento describe la arquitectura, funcionamiento y diseño del artefacto audiovisual "Terrorizer", desarrollado en openFrameworks y pensado para ser usado en cualquier dispositivo que reciba mensajes MIDI. En este caso particular la interacción está hecha en Ableton Live 
El sistema genera pelotas autónomas que interactúan visualmente mientras envían notas y efectos MIDI que son recibidos por el DAW, generando sonidos fácilmente variables.

### Arquitectura General
El sistema se compone de varios módulos:
- ofApp: núcleo de la aplicación.
- Pelota: clase que modela pelotas con movimiento, vida, rebotes y notas MIDI.
- MidiSender: clase para enviar mensajes MIDI.
- controlGui: interfaz gráfica para modificar parámetros en tiempo real y darle performatividad al artefacto.

### Flujo General
- El usuario interactúa mediante GUI y teclas o mouse.
- Se generan pelotas con propiedades musicales y visuales.
- Las pelotas se mueven dentro de un rectángulo, rebotan y producen notas MIDI.
- Se aplican efectos como distorsión, reverb (desde Ableton) y pixelado.

### MIDI
El módulo MidiSender envía:
- NoteOn / NoteOff para los rebotes.
- Control Change según sliders de GUI (ataque, reverb, delay, etc.).

### Ciclo de Vida de Pelotas
- Nacen: se crean con posición, nota y radio.
- Viven: se mueven y rebotan.
- Mueren: se agota su tiempoVital.
- Renacen: después de un delay, recuperan valores nuevos.


### Pixelado y efectos visuales
Según la cantidad de distorsión y/o reverb, el FBO principal es redibujado en baja resolución y reescalado para simular pixelado. 

La distorsión produce un pixelado mas duro, de bordes nítidos y va tiñendo el fondo de la pantalla de rojo.
 
La reverb produce una pixelado mas “blureado”, de bordes suaves y una suerte de aura alrededor de las pelotas aunque llevado al extremo y con la Distorsión = 0, desaparece. el contorno de cada pelota.


### GUI
El panel permite ajustar condiciones y parámetros que van a modificar el comportamiento de los objetos y del sonido producido. Se puede acceder a los controles con mouse o con teclado para tener mayor performatividad. 

La lógica en la organización para el control por teclado es la siguiente: para parámetros asignados a una misma tecla-letra, si es minúscula aumenta el valor, si es mayúscula lo disminuye. En caso de asignar una función a letras diferentes o teclas especiales, la que se ubica a la derecha aumenta, la que está a la izquierda disminuye. Las teclas están indicadas en el panel entre paréntesis.

Setup pelotero nos permite fijar las condiciones físicas iniciales del sistema: velocidad, posición, tiempo de vida, cantidad de pelotas, y también escalas musicales.

Todos los parámetros aplican al nacer las pelotas, luego permanecen invariables hasta un nuevo nacimiento excepto la velocidad que se aplica en tiempo real.

- Nro. de pelotas: número de pelotas que van a nacer, pueden de 1 a 12. Teclas ‘q’ y ‘w’.

- Elección aleatoria: número de pelotas por nacer aleatorio. Tecla ‘k’ .

- Auto-Regeneración: si está activo, el ciclo muerte y nacimiento es automático, solo hay que provocar el primer nacimiento. Caso contrario hay que disparar cada nuevo nacimiento. Tecla ‘k’.
 La ‘Barra espaciadora’ es la encargada de hacer nacer.
 
- Sumar pelotas: se van sumando nuevos nacimientos y conviven generaciones de pelotas. Tecla ‘s’.
Obs: en esta configuración el sistema se puede comportar de forma extraña.

- Tiempo de vida: cuanto va vivir cada generación de pelotas antes de desaparecer. Teclas ‘v’ y ‘V’.

- Velocidad: varía la velocidad de cada pelota, multiplicando cada vector por un mismo factor. Varía en tiempo real. Teclas ‘left’ y ‘right’  .

- Centro/Mouse: ubica el origen de una nueva tanda de pelotas, si Centro está activado, nacen en el centro de la pantalla, si no donde esté posicionado el mouse. Tecla ‘c’.

- aCordes: ubica el origen en el rincón superior derecho de la pantalla haciendo que las pelotas choquen con las paredes en forma simultánea, generando acordes. Para poder seleccionarlo desde el mouse, debe estar “desclickeado” Centro/Mouse. Tecla ‘C’.

- Tipo de escala: Toma un valor entre 0 y 4 y lo asigna a la escala musical a la que van a pertenecer las notas de cada nuevo nacimiento. Tecla ‘e’ y ‘E’.

Los valores están asignados así: ‘0’ para escala cromática,  el ‘1’ para escala diatónica,  el ‘2’ Escala menor melódica, el ‘3’ para escala menor armónica y el ‘4’ para escala mayor armónica.

Efectos: la sección efectos controla parámetros relacionados exclusivamente con el audio. 

Ataque (‘a’ y ‘A’)  y Release (‘l’ y ‘L’)  modifican la envolvente de la amplitud, Distorsión (‘d’ y ‘D’)  agrega ganancia. Filtro (‘p’ y ‘P’)es un filtro pasa bajos, y Delay – wet (‘d’ y ‘D’) , Tiempo (‘t’ y ‘T’) y Feedback (‘f’ y ‘F’) controlan parámetros de un delay, y Reverb (‘up’ y ‘down’) , la cantidad de efecto aplicada.

- Grabar: Nos permite hacer una grabación en Ableton Live de la performance en curso, tanto MIDI como audio. Live debe estar en Modo Arrangement, y ada nueva grabación empieza desde cero y si no tomamos la precaución de guardar el audio, sobregrabará. Tecla ‘g’.

Hay otras funciones y teclas de control que no están específicas en el GUI:

 Barra espaciadora: generar pelotas          -     Tecla ‘n’ o ‘N’: Matar pelotas 
 
Tecla ‘x’: Guardar preset GUI                   -     Tecla ‘b’: Cargar preset GUI  
   
Tecla ‘z’: Mostra o esconder panel GUI    -     Tecla  ‘i’ Mostrar texto informativo

Nota: la elección de los nombres para los controles fue pensada en relación a las teclas disponibles  en la computadora, en lo posible guardando relación con alguna característica del efecto.

### Conclusión

"Terrorizer" combina visuales generativas, interacción en tiempo real y comunicación MIDI logrando un sistema audiovisual coherente y expandible.

En principio fue pensado como un secuenciador generativo completamente autónomo pero a medida que se lo fue desarrollando apareció la necesidad de controlar parámetros, y se fue convirtiendo en algo parecido a un instrumento musical híbrido autónomo/controlable. 

También sería deseable darle un carácter aleatorio a cada parámetro, como la elección de las escalas, el tiempo de vida y toda la sección de efectos.

Un posible uso es poder ejecutarlo o dejarlo que se “toque solo” en un entorno de instalación audiovisual comandando instrumentos virtuales o hardware. O como introducción o previa en proyectos musical que usen sintetizadores antes de la llegada de los músicos al escenario.

