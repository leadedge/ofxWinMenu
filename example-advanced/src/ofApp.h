/*

	ofxWinMenu advanced example - opApp.h

	Copyright (C) 2016-2025 Lynn Jarvis.

	http://www.spout.zeal.co

	=========================================================================
	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
	=========================================================================
*/

#pragma once

#include "ofMain.h"
#include "ofxWinMenu.h" // Menu addon
#include "ofxWinDialog.h" // Dialog addon
#include "resource.h" // For resources
#include <shellapi.h> // For ShellExecute
#include <winver.h> // For GetFileVersionInfo and GetFileVersionInfoSize
#pragma comment(lib, "Version.lib")


class ofApp : public ofBaseApp {

	public:

		void setup();
		void update();
		void draw();
		void exit(); // exit function for initialization file save

		void keyPressed(int key); // Traps escape key if exit disabled
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		// Load image from resources
		bool LoadResourceImage(ofImage& image, int index);

		// Used by dialogs and menu
		HWND hWndApp; // Application window
		HINSTANCE hInstance; // Application instance

		// Menu
		ofxWinMenu * menu; // Menu object
		void appMenuFunction(string title, bool bChecked); // Menu return function
		void CreateAppMenu(); // Create the application menu

		// Options dialog
		ofxWinDialog* options; // Dialog object
		HWND hwndOptions = NULL; // Dialog window
		void CreateOptionsDialog(); // Create the example dialog controls
		// Callback function for ofxWinDialog to return control values
		void OptionsDialogFunction(std::string title, std::string text, int value);

		// Second dialog (About)
		ofxWinDialog* about; // Dialog object
		HWND hwndAbout = NULL; // Dialog window
		void CreateAboutDialog(); // Create about dialog
		void AboutDialogFunction(std::string title, std::string text, int value);

		// Used by example app
		ofTrueTypeFont myFont;
        ofImage myImage;
		float windowWidth, windowHeight;
		std::string iniPath; // Initialization path
		
		// Variable changed by the menu
		bool bShowInfo;

		// Options dialog
		HWND hWndDialog; // Dialog window
		int Brightness = 255; // Value changed by the trackbar
		// bShowInfo is common with the menu

};
