
// Pelota.h
#pragma once
#include "ofMain.h"
#include "MidiSender.h"
#include "ofxGui.h"

class Pelota {
public:
	void setup();
	void setup(ofRectangle marco, MidiSender* midiSender);
	void setup(ofRectangle marco, MidiSender* midiSender, int midiNote, float radio, int life);
	void setup(ofRectangle marco, MidiSender* midiSender, int midiNote, int cc);// Nueva versión
	//void update();
	void update(float factorVel = 1.0);
	void draw();
	
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	
	bool mouseClick();
	

	
	void reset(ofRectangle marco);
	//void reset2(ofRectangle marco);
	
	bool isDead();
	
	ofVec2f getPos() { return pos; }
	ofVec2f getVel() { return vel; }
	float getRadio() { return radio; }
		
	void setPos(ofVec2f nuevaPos) { pos = nuevaPos; }
	void setVel(ofVec2f nuevaVel) { vel = nuevaVel; }
	
	float lifeSpan;
	
	bool esperandoNacer = false;
	float deathTime = 0;
	float nacerDelay = 4.0; // segundos de espera
	ofVec2f pos, vel;
	
	

private:
	
	float radio;
	ofRectangle bounds;
	MidiSender* midi;
	int note = 60;
	bool noteOn = false;
	int ccOpenTime_1 = 0;
	int ccOpenTime_2 = 0;
	//------
};
