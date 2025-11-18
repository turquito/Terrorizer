#pragma once

#include "ofMain.h"
#include "midiSender.h"
#include "pelota.h"
#include "ofxGui.h"


class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void exit();
	void resetPelotas();
	
	void keyPressed(int key);
	//static void safeAllNotesOff();
	
	float lastDeathTime = 0;      // momento en que murió la última pelota
	bool esperandoReset = false;   // indica que estamos esperando el respawn global
	float resetDelay = 2.0f;       // segundos a esperar antes de resetear todas
	int NUM_PELOTAS;
	
	bool showGUI;
	bool stop;
	
	ofFbo fbo;
	ofFbo fboLowRes;  // segundo FBO para pixelado
	ofFbo fboPixelado;
	
	// ofx controles
	ofxPanel gui;
	ofxGuiGroup creacion;
	ofxIntSlider rangoRandom;
	ofxIntSlider lifeSpan;
	ofxToggle random, record, renovar, centro;
	ofxFloatSlider factorVel;
	
	ofxGuiGroup efectos;
	ofxFloatSlider distorsion;
	ofxFloatSlider ataque;
	ofxFloatSlider release;
	ofxFloatSlider reverb;
	ofxFloatSlider tiempo;
	ofxFloatSlider delay;
	ofxFloatSlider feedback;
	ofxFloatSlider filtro;
	ofxMidiMessage cc;

	int alpha;
	
private:
	
	vector<Pelota> pelotas;
	MidiSender midi;
	void detectarColisiones();
	
};
