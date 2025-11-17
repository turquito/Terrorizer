#include "MidiSender.h"

/*
--------------------------------------------------------------
 midiSender.cpp

 Implementación de las funciones de MidiSender
 Encargado de enviar mensajes MIDI a aplicaciones externas.
 Ahora trabajamos con Ableton Live, pero puede ser usada en cualquier hardware que reciba MIDI.
--------------------------------------------------------------
*/

void MidiSender::setup(int port, int midiCh) {
	
	midiOut.listOutPorts();  // Imprime en consola los puertos disponibles
	
	midiOut.openPort(port);  // Abre o conecta el puerto dado
	channel = midiCh;        // Fija el canal MIDI activo
}

// Función para mandar notas. El rango es de 0 a 127 pero voy a usar notas entre 24 y 96
void MidiSender::sendNoteOn(int note, int velocity) {
	midiOut.sendNoteOn(channel, note, velocity);
}

// Note off, velocity 0 equivale a no tocar
void MidiSender::sendNoteOff(int note) {
	midiOut.sendNoteOff(channel, note, 0);
}

// Mensaje CC - numero de CC y valor mandando
void MidiSender::sendControlChange(int controlador, int valor){
	midiOut.sendControlChange(channel, controlador, valor);
}

// Apagar las notas del canal
void MidiSender::allNotesOff() {
	for (int n = 0; n < 128; ++n)
		sendNoteOff(n);
}

// Cerrar puerto MIDI
void MidiSender::exit() {
	midiOut.closePort(); 
}
