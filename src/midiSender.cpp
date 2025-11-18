#include "MidiSender.h"

void MidiSender::setup(int port, int channel) {
	midiOut.listOutPorts();
	midiOut.openPort(port);
	this->channel = channel;
}

void MidiSender::sendNoteOn(int note, int velocity) {
	midiOut.sendNoteOn(channel, note, velocity);
}

void MidiSender::sendNoteOff(int note) {
	midiOut.sendNoteOff(channel, note, 0);
}

void MidiSender::sendProgramChange(int program) {
	midiOut.sendProgramChange(channel, program);
}

void MidiSender::sendControlChange(int controlador, int valor){
	midiOut.sendControlChange(channel, controlador, valor);
}

void MidiSender::exit() {
	midiOut.closePort();
}
