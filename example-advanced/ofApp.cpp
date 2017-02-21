/*

	ofxWinMenu advanced example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.
	
	This advanced example includes an About dialog, Version information and
	a custom modeless dialog with controls.	See resource.h and resource.rc.

	These can be modified using a resource editor such as ResEdit http://www.resedit.net
	or by using the resource editor of Microsoft Visual Studio.

	The Dialog procedures AboutDlgProc and OptionsDlgProc have to be changed as well.

	Copyright (C) 2016-2017 Lynn Jarvis.

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

*/
#include "ofApp.h"
#include "resource.h"


// Message handler for About box (see resource.h and resource.rc)
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

// Custom modeless dialog (see resource.h and resource.rc)
INT_PTR CALLBACK OptionsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
// ofApp class pointer for the modeless Options dialog to access class variables
static ofApp * pThis;


//--------------------------------------------------------------
void ofApp::setup() {

	ofSetWindowTitle("ofxWinMenu advanced example"); // Set the app name on the title bar

	// Get the window size for return from full screen
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();

	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth()-windowWidth)/2, (ofGetScreenHeight()-windowHeight)/2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

	// Load an image for the example
	myImage.loadImage("lighthouse.jpg");

	// Instance handle used for dialogs
	hInstance = GetModuleHandle(NULL);

	// Window handle used for resources and topmost function
	hWnd = WindowFromDC(wglGetCurrentDC());

	// Set a custom window icon (see resource.h and resource.rc)
	SetClassLong(hWnd, GCL_HICON, (LONG)LoadIconA(GetModuleHandle(NULL), MAKEINTRESOURCEA(IDI_ICON1)));
	
	// Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
	ofSetEscapeQuitsApp(false);

	//
	// Create a menu using ofxWinMenu
	//

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, hWnd);

	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist. 
	// See the example "appMenuFunction".
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	// Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	//
	// Create a "File" popup menu
	//
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	//
	// Add popup items to the File menu
	//

	// Open an image file
	menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked
	
	// Save an image file
	menu->AddPopupItem(hPopup, "Save As", false, false); // Not checked and not auto-checked

	// Example submenu off the File menu
	HMENU hSubMenu = menu->AddPopupMenu(hPopup, "Submenu");
	menu->AddPopupItem(hSubMenu, "Item 1", true); // Checked and auto-check
	menu->AddPopupItem(hSubMenu, "Item 2"); // Not checked and auto-check (default)
	menu->AddPopupItem(hSubMenu, "Item 3", false, false); // Not checked and no autocheck

	// Options dialog
	menu->AddPopupItem(hPopup, "Options", false, false);

	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// View popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "View");

	// State variables
	bShowInfo = true;  // screen info display
	menu->AddPopupItem(hPopup, "Show info", true); // Checked default
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Show on top"); // Not checked (default)
	bFullscreen = false;
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	//
	// Help popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Help");

	// See resource.rc for customisation of the About dialog.
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();

	//
	// For a modeless Options dialog created in this example
	//
	pThis = this; // ofApp class pointer
	hWndDialog = NULL; // Dialog window handle indicates it is open
	bButton = false; // Button click flag
	bCheckBox = false; // Checkbox state flag
	TrackbarValue = 0; // For the dialog trackbar control



} // end Setup


//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if(title == "Open") {
		result = ofSystemLoadDialog("Select an image file", false);
		if(result.bSuccess) {
			filePath = result.getPath();
			printf("Open\n");
			pos = filePath.find_last_of(".");
			if(pos != string::npos && pos != 0 && filePath.substr(pos+1) == "jpg") {
				// found the right extension
				printf("  Loading [%s]\n", result.getName().c_str());
				myImage.loadImage(filePath);
				// Adjust window height to match image aspect ratio
				windowHeight = ofGetWidth()*myImage.getHeight()/myImage.getWidth();
				ofSetWindowShape(windowWidth, windowHeight);
			}
			else
				printf("  Needs a '.jpg' extension\n");
		}
	}
	if(title == "Save As") {
		result = ofSystemSaveDialog(filePath, "Save an image file");
		if(result.bSuccess) {
			printf("Save As\n");
			filePath = result.getPath();
			pos = filePath.find_last_of(".");
			if(pos != string::npos && pos != 0 && filePath.substr(pos+1) == "jpg") {
				printf("  Saving [%s]\n", result.getName().c_str());
				myImage.saveImage(filePath, OF_IMAGE_QUALITY_BEST);
			}
			else
				printf("  Needs a '.jpg' extension\n");
		}
	}
	if(title == "Alert") {
		ofSystemAlertDialog("ofxWinMenu");
	}
	if(title == "Exit") {
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//
	if(title == "Show info") {
		bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
	}

	if(title == "Show on top") {
		doTopmost(bChecked); // Use the checked value directly
	}

	if(title == "Full screen") {
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//
	if(title == "About") {
		// About dialog (see recource.h and resource.rc)
		DialogBoxA(hInstance, MAKEINTRESOURCEA(IDD_ABOUTBOX), hWnd, About);
	}

	//
	// Options dialog
	//
	if(title == "Options") { 
		if(!hWndDialog) // Open the dialog if not already
			hWndDialog = CreateDialogA(hInstance, MAKEINTRESOURCEA(IDD_OPTIONSBOX), hWnd, OptionsDlgProc);
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {

}


//--------------------------------------------------------------
void ofApp::draw() {

	char str[256];
	ofBackground(0, 0, 0, 255);

	// Check Options dialog button state and do something with it
	// Can go in Update - causes a flicker here so you can see it.
	if(bButton) {
		printf("Button pressed\n");
		bButton = false; // wait for the next button press
		return;
	}
	
	// Darken background according to the Options dialog trackbar value
	int level = 255-(int)(TrackbarValue*255); 
	ofSetColor(level);

	myImage.draw(0, 0, ofGetWidth(), ofGetHeight());

	if(bShowInfo) {
		if(bCheckBox) // Changed by Options dialog checkbox
			ofSetColor(255, 255, 0); // Yellow
		else
			ofSetColor(255, 255, 255);
		// Show keyboard duplicates of menu functions
		sprintf(str, " ' ' show/hide info : 'f' toggle full screen : RH click for options");
		myFont.drawString(str, 15, ofGetHeight()-20);
	}


} // end Draw


void ofApp::doFullScreen(bool bFull)
{
	// Enter full screen
	if(bFull) {
		// Close the modeless Options dialog if open
		if(hWndDialog) SendMessage(hWndDialog, WM_CLOSE, 0, 0);
		// Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		// hide the cursor
		ofHideCursor();
		// Set full screen
		ofSetFullscreen(true);
	}
	else { 
		// return from full screen
		ofSetFullscreen(false);
		// Restore the menu
		menu->SetWindowMenu();
		// Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU)); 
		// Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth()-ofGetWidth())/2, (ofGetScreenHeight()-ofGetHeight())/2);
		ofShowCursor();
		// Restore topmost state
		if(bTopmost) doTopmost(true);
	}

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
	if(bTop) {
		// get the current top window for return
		hWndForeground = GetForegroundWindow();
		// Set this window topmost
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
		ShowWindow(hWnd, SW_SHOW);
	}
	else {
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
		// Reset the window that was topmost before
		if(GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
		else
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
	}
} // end doTopmost


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	
	// Escape key exit has been disabled so it can be checked here
	if(key == VK_ESCAPE) {
		// Exit if fullscreen set, otherwise exit
		if(bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	// Remove or show screen info
	if(key == ' ') {
		bShowInfo = !bShowInfo;
		// Update the menu check mark because the item state has been changed
		menu->SetPopupItem("Show info", bShowInfo);
	}

	if(key == 'f') {
		bFullscreen = !bFullscreen;	
		doFullScreen(bFullscreen);
		// Do not check this menu item
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
	if(button == 2) { // rh button
		// Open the the Options dialog window if closed
		if(!hWndDialog) {
			hWndDialog = CreateDialogA(hInstance, MAKEINTRESOURCEA(IDD_OPTIONSBOX), hWnd, OptionsDlgProc);
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


// Message handler for About box
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	char temp[512];
	char about[1024];
	DWORD dummy, dwSize;
	LPDRAWITEMSTRUCT lpdis;
	HWND hwnd = NULL;
	HCURSOR cursorHand = NULL;
	HINSTANCE hInstance = GetModuleHandle(NULL);

	switch (message) {

		case WM_INITDIALOG:

			// About box text - 6 lines, 35 characters each line, centered
			sprintf_s(about, 1024, "ofxWinMenu\nadvanced example\n");

			//
			// Get product version number from resources ("ProductVersion")
			//
			// Other strings available in the same way :
			// "CompanyName", "FileDescription", "FileVersion","InternalName",
			// "LegalCopyright", "OriginalFilename", "ProductName"
			//
			if(GetModuleFileNameA(hInstance, temp, 256)) {
				dwSize = GetFileVersionInfoSizeA(temp, &dummy);
				if(dwSize > 0) {
					vector<BYTE> data(dwSize);
					if(GetFileVersionInfoA(temp, NULL, dwSize, &data[0])) {
						LPVOID pvProductVersion = NULL;
						unsigned int iProductVersionLen = 0;
						if(VerQueryValueA(&data[0], ("\\StringFileInfo\\080904E4\\ProductVersion"), &pvProductVersion, &iProductVersionLen)) {
							sprintf_s(temp, 256, "  Version %s\n", (char *)pvProductVersion);
							strcat_s(about, 1024, temp);
						}
					}
				}
			}
			SetDlgItemTextA(hDlg, IDC_ABOUT_TEXT, (LPCSTR)about);

			// Hyperlink hand cursor
			cursorHand=LoadCursor(NULL, IDC_HAND);
			hwnd = GetDlgItem(hDlg, IDC_ABOUT_URL);
			SetClassLongA(hwnd, GCL_HCURSOR, (long)cursorHand);

			return (INT_PTR)TRUE;

		case WM_DRAWITEM:
			// The blue hyperlink
            lpdis = (LPDRAWITEMSTRUCT)lParam;
			if (lpdis->itemID == -1) break; 
            SetTextColor(lpdis->hDC, RGB(6, 69, 173));
			DrawTextA(lpdis->hDC, "http://spout.zeal.co", -1, &lpdis->rcItem, DT_CENTER);
			break;

		case WM_COMMAND:

			if (LOWORD(wParam) == IDC_ABOUT_URL) {
				sprintf(temp, "http://spout.zeal.co");
				ShellExecuteA(hDlg, "open", temp, NULL, NULL, SW_SHOWNORMAL); 
				EndDialog(hDlg, 0);
				return (INT_PTR)TRUE;
			}

			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
	}
	return (INT_PTR)FALSE;

} // end About dialog callback


// Message handler for modeless Options dialog
INT_PTR CALLBACK OptionsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam); // suppress warning
	char str1[MAX_PATH];
	HWND hBar = NULL;
	static int iPos, dn;

	switch (message) {

		case WM_INITDIALOG:

			// Keep the dialog topmost
			SetWindowPos(hDlg, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 

			// Trackbar
			hBar = GetDlgItem(hDlg, IDC_TRACKBAR);
			// Set the trackbar limits - 100 units wide 
			SendMessage(hBar, TBM_SETRANGE,  TRUE, MAKELONG(0, 100));
			// Set the start position
			iPos = (int)(pThis->TrackbarValue*100.0); // value is from 0 to 1
			SendMessage(hBar, TBM_SETPOS, TRUE, (LPARAM)iPos);
			// Set scrollbar text
			sprintf_s(str1, 16, "%.2f", (float)pThis->TrackbarValue);
			SetDlgItemTextA(hDlg, IDC_TRACKBARVALUE, (LPCSTR)str1);
			// Set a backup value in case of cancel
			pThis->OldTrackbarValue = pThis->TrackbarValue;

			// Initialise checkbox
			if(pThis->bCheckBox) CheckDlgButton(hDlg, IDC_CHECKBOX, BST_CHECKED);
			else     CheckDlgButton(hDlg, IDC_CHECKBOX, BST_UNCHECKED);
			// Set a backup value in case of cancel
			pThis->bOldCheckBox = pThis->bCheckBox;

			return (INT_PTR)TRUE;

		// Trackbar messages
		case WM_HSCROLL:
			hBar = (HWND)lParam;
			iPos = (int)SendMessageA(hBar, TBM_GETPOS, 0, 0); // Trackbar

            // Calculate new horizontal scroll position
			// Mouse move as well as < > and PgUp/PgDn keys
            switch (LOWORD(wParam)) {
                case SB_LINEDOWN: // Down arrow key
                        dn =  1;
                        break;
                case SB_LINEUP:
                        dn = -1;
                        break;
                case SB_PAGEDOWN: // PgDn key
                        dn =  10;
                        break;
                case SB_PAGEUP:
                        dn = -10;
                        break;
                case SB_THUMBTRACK: // Mouse move of slider
                case SB_THUMBPOSITION:
                        dn = HIWORD(wParam) - iPos;
                        break;
                default:
                    dn = 0;
            }
            iPos = iPos+dn;
			if(iPos < 0) iPos = 0;
			if(iPos > 100) iPos = 100;
			
			// Trackbar
			if(hBar == GetDlgItem(hDlg, IDC_TRACKBAR)) {
				// printf("Trackbar position = %d\n", iPos);
				pThis->TrackbarValue = (float)iPos/100; // 0 - 1
				sprintf_s(str1, 16, "%.2f", pThis->TrackbarValue);
				SetDlgItemTextA(hDlg, IDC_TRACKBARVALUE, (LPCSTR)str1);
			}
			break;

		case WM_CLOSE:
			DestroyWindow(hDlg);
			pThis->hWndDialog = NULL;
			return (INT_PTR)TRUE;

		case WM_COMMAND:

			switch(LOWORD(wParam)) {

				// Button press - activated on Button Up
				// The application handles the action and sets to false again
				case IDC_BUTTON:
					pThis->bButton = true;
					break;

				//
				// Checkbox
				//
				case IDC_CHECKBOX:
					if(IsDlgButtonChecked(hDlg, IDC_CHECKBOX) == BST_CHECKED)
						pThis->bCheckBox = true;
					else
						pThis->bCheckBox = false;
					break;

				case IDOK :
					// Destroy window and it's handle
					DestroyWindow(hDlg);
					pThis->hWndDialog = NULL;
					pThis->bButton = false; // In case the application did not handle the state change
					return (INT_PTR)TRUE;

				case IDCANCEL :
                    // User pressed cancel
					
					// Restore intial values
					pThis->TrackbarValue = pThis->OldTrackbarValue;
					pThis->bCheckBox = pThis->bOldCheckBox;
					pThis->bButton = false;
					// Take down dialog box.
					DestroyWindow(hDlg);
					pThis->hWndDialog = NULL;
					return (INT_PTR)TRUE;

				default:
					break;

			}

			break;
	}
	return (INT_PTR)FALSE;
}

