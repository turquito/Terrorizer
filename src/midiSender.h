
#pragma once

#include "ofMain.h"
#include "ofxMidi.h"


class MidiSender {
public:
	void setup(int port = 0, int channel = 1);
	void sendNoteOn(int note, int velocity);
	void sendNoteOff(int note);
	void sendProgramChange(int program);
	void sendControlChange(int controlador, int valor);
	void exit();
	

private:
	ofxMidiOut midiOut;
	int channel;
};
