/*

	ofApp.cpp

	ofxWinMenu advanced example

	Using ofxWinMenu addon to create a menu for a Microsoft Windows application.
	
	This advanced example includes using the addon ofxWinDialog.

	https://github.com/leadedge/ofxWinDialog
	
	Two dialogs are created :
	  - example dialog with controls
	  - About dialog with Version information

	Resources are used to embed image and icon data into the
	executable instead of requiring additional files at runtime.
	The resource also contains Version information which enables
	the version to show in Windows explorer and in the file properties.
	See resource.h and resource.rc.

	Menu and Dialog Settings are saved and restored with
	an initialization file using Save and Load functions.


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

	21.02.17 - rebuild for OF 0.9.8
	16.05.22 - Update to Openframeworks 11.2 and Visual Studio 2022
	09.01.25 - Revise using ofxWinDialog

*/
#include "ofApp.h"
#include "resource.h"


//--------------------------------------------------------------
void ofApp::setup() {

	ofSetWindowTitle("ofxWinMenu advanced example"); // Set the app name on the title bar

	// Get the window size for image load
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();

	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth()-windowWidth)/2, (ofGetScreenHeight()-windowHeight)/2);

	// Load a font
	myFont.load("Verdana.ttf", 14);

	// Load an image from resources
	LoadResourceImage(myImage, 0);

	// Initialization file path (path of executable)
	iniPath = GetExePath() + "example-advanced.ini";

	// Instance and window handles used for dialogs and the menu
	hInstance = GetModuleHandle(NULL);
	hWndApp = ofGetWin32Window();

	// Set a custom window icon
	// IDI_ICON1, and IDI_ICON2 for the dialog, are defined in 
	// resources and the icon files are not required at runtime.
	// (see resource.h and resource.rc).
	HICON hIcon = LoadIconA(GetModuleHandle(NULL), MAKEINTRESOURCEA(IDI_ICON1));
	SendMessage(hWndApp, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	SendMessage(hWndApp, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	
	//
	// Create a menu using ofxWinMenu
	//

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, hWndApp);
	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist. 
	menu->CreateMenuFunction(&ofApp::appMenuFunction);
	// Create the menu
	// Menu actions are returned in appMenuFunction
	CreateAppMenu();

	//
	// Create an Options dialog using ofxWinDialog
	//
	options = new ofxWinDialog(this, hInstance, hWndApp);
	// Register an ofApp function that is called when a dialog control is selected.
	options->AppDialogFunction(&ofApp::OptionsDialogFunction);
	// Icon for the dialog window
	hIcon = LoadIconA(GetModuleHandle(NULL), MAKEINTRESOURCEA(IDI_ICON2));
	options->SetIcon(hIcon);
	// Create the options dialog
	// Dialog controls are returned in OptionsDialogFunction
	CreateOptionsDialog();

	//
	// Create an About dialog using ofxWinDialog
	// A unique class name is required for multiple dialogs
	about = new ofxWinDialog(this, hInstance, hWndApp, "ofxAbout");
	about->SetIcon(hIcon);
	about->AppDialogFunction(&ofApp::AboutDialogFunction);
	CreateAboutDialog();

	// Read menu and dialog variables from the intialization file.
	// No action if the file does not exist yet.
	// Load options first so that original values used by Reset
	// are not affected by duplicate controls loaded by menu->Load.
	// In this example "bShowInfo".
	options->Load(iniPath, "");
	// Update ofApp with the loaded values
	options->GetControls();
	// Load the menu items
	menu->Load(iniPath);

	// The initializetion file is updated
	// when ofApp closes - see exit()

	// Open the options dialog - see MousePressed
	hwndOptions = options->Open("Options");


} // end Setup


// Create the application menu
void ofApp::CreateAppMenu()
{
	// Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	// Create a "File" popup menu
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	//
	// Add popup items to the File menu
	//

	// Options dialog
	menu->AddPopupItem(hPopup, "Options", false, false);

	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	// View popup menu
	hPopup = menu->AddPopupMenu(hMenu, "View");

	// View menu state variables
	bShowInfo = true;  // screen info display
	menu->AddPopupItem(hPopup, "Show info", true); // Checked default

	// Help popup menu
	hPopup = menu->AddPopupMenu(hMenu, "Help");

	// See resource.rc for customisation of the About dialog.
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();

}

//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

	// File > Exit
	if(title == "Exit") {
		ofExit(); // Quit the application
	}

	// File > Options
	if (title == "Options") {
		// Open the options dialog if not already open
		if (!hwndOptions) {
			// bShowInfo is changed by both the Menu and the dialog
			// Set it here before opening the dialog
			options->SetCheckBox("Checkbox 1", bShowInfo);
			hwndOptions = options->Open("Options");
		}
	}

	// View > Show info
	if(title == "Show info") {
		// Flag is used in Draw()
		bShowInfo = bChecked;
		// Also used by the options dialog
		options->SetCheckBox("Checkbox 1", bShowInfo);
	}

	// Help > About
	if (title == "About") {
		if (!hwndAbout) hwndAbout = about->Open("About");
	}

} // end appMenuFunction


// Create the options dialog controls
void ofApp::CreateOptionsDialog()
{
	int ypos = 20;
	options->AddText("Brightness",  20, ypos, 100, 30);
	options->AddSlider("Slider 1", 105, ypos, 175, 30, 0, 255, Brightness, true, 16);
	ypos += 45;
	options->AddCheckBox("Checkbox 1", "Show info",    50, ypos,  100, 20, bShowInfo, true);
	ypos += 55;

	// OK, Cancel, Reset
	options->AddButton("Load button", "Reload",    25, ypos, 70, 30);
	options->AddButton("Reset button", "Reset",   100, ypos, 70, 30);

	options->AddButton("OK button", "OK",         175, ypos, 70, 30, BS_DEFPUSHBUTTON);
	options->AddButton("Cancel button", "Cancel", 250, ypos, 70, 30);

	//
	// Initialization file sections
	//
	// A name can be used to identify controls with a common grouping.
	// This takes effect when the controls are saved with options->Save
	// Separate sections can be specified/ for options->Load
	// In this case the dialog variables can be identified from menu items (menu->Save())
	options->SetSection("Slider 1", "Graphics");
	options->SetSection("Checkbox 1", "Graphics");

	// Centre on the main window
	options->SetPosition(0, 0, 360, 210);
}

// Options callback function for ofxWinDialog
void ofApp ::OptionsDialogFunction(std::string title, std::string text, int value)
{
	// hwndOptions is returned by options->Open
	// and re-set here when the dialog closes
	if (title == "WM_DESTROY") {
		hwndOptions = nullptr;
		return;
	}

	if (title == "Slider 1") {
		// Background darkness (see Draw)
		// value is returned *100 for a trackbar
		Brightness = value/100;
	}

	if (title == "Checkbox 1") {
		// Change depending on value 1 or 0
		bShowInfo = (value == 1);
		// Also used by the menu, set that item
		menu->SetPopupItem("Show info", bShowInfo);
	}

	// Save current control values
	if (title == "Save button") {
		// ofSystemSaveDialog has already warned for overwrite.
		// Set the overwrite flag true to avoid another warning.
		options->Save(iniPath, true);
		// Save the menu items as well in the same file
		// The items have a "Menu" grouping
		menu->Save(iniPath, true);
	}

	// Load control values from initialization file
	if (title == "Load button") {
		// Load all controls, regardless of section
		options->Load(iniPath);
		// Update ofApp
		options->GetControls();
		// Load the menu items as well
		menu->Load(iniPath);
	}

	// Reset all controls to starting values
	if (title == "Reset button") {
		// Reset controls
		options->Reset();
		// Update ofApp
		options->GetControls();
	}

	// OK
	if (title == "OK button") {
		// Get current values of all controls
		// and return values to ofApp
		options->GetControls();
		options->Close();
		// The dialog handles are set to null by WM_DESTROY
		// in the respective ofxWinDialog callback functions
	}

	// Cancel
	if (title == "Cancel button") {
		// Restore all controls with old values
		options->Restore();
		// Return values to ofApp
		options->GetControls();
		options->Close();
	}

}

// Create the About dialog
void ofApp::CreateAboutDialog()
{
	int ypos = 20;
	// A hyperlink is static text modified to be detected by the event handler
	// The dialog is closed when the hyperlink is selected
	about->AddHyperlink("ofxWinMenu", "https://github.com/leadedge/ofxWinMenu", 20, ypos, 300, 20, SS_CENTER);
	ypos += 20;

	// Get version number from resources
	char temp[256]{};
	if (GetModuleFileNameA(hInstance, temp, 256)) {
		DWORD dummy = 0;
		DWORD dwSize = GetFileVersionInfoSizeA(temp, &dummy);
		if (dwSize > 0) {
			std::vector<BYTE> data(dwSize);
			if (GetFileVersionInfoA(temp, NULL, dwSize, &data[0])) {
				LPVOID pvProductVersion = NULL;
				unsigned int iProductVersionLen = 0;
				if (VerQueryValueA(&data[0], ("\\StringFileInfo\\080904E4\\ProductVersion"), &pvProductVersion, &iProductVersionLen)) {
					sprintf_s(temp, 256, "Version %s\n", (char*)pvProductVersion);
				}
			}
		}
	}
	about->AddText(temp, 20, ypos, 300, 20, SS_CENTER);
	ypos += 30;

	std::string str ="Advanced example using";
	about->AddText(str, 20, ypos, 300, 20, SS_CENTER);
	ypos += 20;

	// A hyperlink is static text modified to be detected by the event handler
	// The dialog is closed when the hyperlink is selected
	about->AddHyperlink("ofxWinDialog", "https://github.com/leadedge/ofxWinDialog", 20, ypos, 300, 20, SS_CENTER);
	ypos += 30;

	str = "Resources are used for ";
	str += "icons, images and version. ";
	str += "Settings are saved and restored ";
	str += "with an initialization file.";
	about->AddText(str, 10, ypos, 320, 80, SS_CENTER);
	ypos += 80;

	about->AddButton("OK button", "OK", 120, ypos, 95, 30, BS_DEFPUSHBUTTON);

	// Centre on the main window
	about->SetPosition(0, 0, 360, 300);
}

// About callback function
void ofApp::AboutDialogFunction(std::string title, std::string text, int value)
{
	if (title == "WM_DESTROY") {
		hwndAbout = nullptr;
		return;
	}

	if (title == "OK button") {
		about->Close();
	}
}



//--------------------------------------------------------------
void ofApp::update() {

}


//--------------------------------------------------------------
void ofApp::draw() {

	// Darken background to the Options dialog trackbar value
	ofSetColor(Brightness);

	myImage.draw(0, 0, ofGetWidth(), ofGetHeight());

	// Show on-screen info - activated by
	// Menu item and Options dialog checkbox
	if(bShowInfo) {
		std::string str = " ' ' show/hide info : Right mouse click for options dialog";
		myFont.drawString(str, 40, ofGetHeight()-20);
	}


} // end Draw

//--------------------------------------------------------------
void ofApp::exit() {
	// Save menu and dialog variables
	menu->Save(iniPath, true);
	options->Save(iniPath, true);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	
	// Escape key exit has been disabled so check for it here
	if(key == VK_ESCAPE) {
		ofExit();
	}

	// Remove or show on-screen info
	if(key == ' ') {
		bShowInfo = !bShowInfo;
		// Update the menu check mark because the item state has been changed
		menu->SetPopupItem("Show info", bShowInfo);
		// And also the dialog checkbox if open
		options->SetCheckBox("Checkbox 1", bShowInfo);
	}

} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

	// Right button
	if(button == 2) {
		// Open/Close the Options dialog window
		if (!hwndOptions) {
			// bShowInfo is changed by both the Menu and the dialog
			// Set it here before opening the dialog
			options->SetCheckBox("Checkbox 1", bShowInfo);
			hwndOptions = options->Open("Options");
		}
		else {
			options->Close();
		}
	}

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
// Load an image from resources
bool ofApp::LoadResourceImage(ofImage& image, int index)
{
	HMODULE hModule = NULL;
	DWORD dwSize = 0;
	HRSRC hResInfo = NULL;
	HGLOBAL rcData = NULL;
	const char* data = NULL;

	hModule = GetCurrentModule();
	if (hModule) {
		hResInfo = FindResource(hModule, MAKEINTRESOURCE(IDB_IMAGEFILE0+index), RT_RCDATA);
		if (hResInfo) {
			dwSize = SizeofResource(hModule, hResInfo);
			if (dwSize > 0) {
				rcData = LoadResource(hModule, hResInfo);
				if (rcData) {
					data = static_cast<const char*>(LockResource(rcData));
					ofBuffer buffer;
					buffer.set(data, dwSize);
					ofPixels pixels;
					if (ofLoadImage(pixels, buffer)) {
						image.setFromPixels(pixels);
						UnlockResource(rcData);
						FreeResource(rcData);
						return true;
					}
					UnlockResource(rcData);
				}
				FreeResource(rcData);
			}
		}
	}
	return false;
}
