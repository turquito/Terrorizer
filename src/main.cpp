
/*
--------------------------------------------------------------
 main.cpp
  Punto de entrada a openFrameworks.
  Configura ventana, crea la instancia de ofApp y
  lanza el loop principal de ejecución.
--------------------------------------------------------------
*/

#include "ofMain.h"
#include "ofApp.h"

//--------------------------------------------------------------
// main()
//  Configura los parámetros iniciales de la ventana
//  y ejecuta la aplicación mediante ofRunApp().
//--------------------------------------------------------------

int main( ){

	ofGLWindowSettings settings;
	// tamaño inicial de la ventana
	settings.setSize(1024, 768);
	
   // modo ventana (no fullscreen)
	settings.windowMode = OF_WINDOW;
	
	// crea ventana con la configuración anterior
	auto window = ofCreateWindow(settings);
	
	// ejecuta la aplicación
	ofRunApp(window, std::make_shared<ofApp>());
	ofRunMainLoop();

}
