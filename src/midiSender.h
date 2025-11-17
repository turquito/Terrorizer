// Clase Midi para hacer la tarea de comunicar OF con Ableton Live
#pragma once

#include "ofMain.h"
#include "ofxMidi.h"

/*
--------------------------------------------------------------
 midiSender.h

 Clase MidiSender

 Encapsula toda la comunicación MIDI de salida:

   - Apertura/cierre de puertos MIDI
   - Envío de notas (NoteOn / NoteOff)
   - Envío de mensajes de Control Change o Continuos Controller (CC)
   - Apagado global de notas

 Permite abstraer la complejidad de ofxMidi y mantener el
 código limpio en otras clases.
--------------------------------------------------------------
*/

class MidiSender {
public:
	// Incializa el MIDI OUT (puerto y canal)
	void setup(int port = 0, int channel = 1);
	
	// Envio nota (Note on) - mando número de nota e intensidad
	void sendNoteOn(int note, int velocity);
	
	// Apagar nota o soltar tecla (Note off)
	void sendNoteOff(int note);
	
	// Envio mensaje de Control Change
	void sendControlChange(int controlador, int valor);
	
	// Apago todas las notas MIDI
	void allNotesOff();
	
	// Cierra el puerto MIDI y lo libera para otra aplicación
	void exit();
	

private:
	
	ofxMidiOut midiOut;  // Salida MIDI
	int channel;         // Canal MIDI (1 - 16)
};
