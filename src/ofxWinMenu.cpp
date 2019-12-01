/*

	ofxWinMenu

	Create a menu for a Microsoft Windows Openframeworks application.
	
	Copyright (C) 2016 Lynn Jarvis.

	https://github.com/leadedge

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

	17.11.16 - fixed submenu item count in AddPopupSeparator
			 - fixed submenu item increment in SetPopupItem
	21.02.17 - changed constructor to take the window handle
	23.12.17 - Add WM_ENTERMENULOOP and WM_EXITMENULOOP
	29.11.19 - Corrected SetClassLong > SetClassLongPtrA for 64 bits

*/
#include "ofxWinMenu.h"


static LRESULT CALLBACK ofxWinMenuWndProc(HWND, UINT, WPARAM, LPARAM); // Local window message procedure
static WNDPROC ofAppWndProc; // Openframeworks application window message procedure
static ofxWinMenu *pThis; // Pointer to access the ofxWinMenu class from the window procedure


ofxWinMenu::ofxWinMenu(ofApp *app, HWND hwnd) {

	g_hMenu = NULL; // Set by CreateMenu and returned to ofApp
	pAppMenuFunction = NULL; // Set by CreateMenuFunction to return menu state to ofApp

	// The window handle of ofApp
	g_hwnd = hwnd;

	pThis = this; // Pointer for access the ofxWinMenu class
	pApp = app; // The ofApp class pointer

	// Save the Openframeworks application window message procedure
	ofAppWndProc = (WNDPROC)GetWindowLongPtrA(g_hwnd, GWLP_WNDPROC);
	
	// Set our own window message procedure
	SetWindowLongPtrA(g_hwnd, GWLP_WNDPROC, (LONG_PTR)ofxWinMenuWndProc);

	// Set the Menu name
	SetClassLongPtrA(g_hwnd, GCLP_MENUNAME, (LONG_PTR)"ofxWinMenu");

}

ofxWinMenu::~ofxWinMenu()
{
	// Clear all vectors
	subMenus.clear();
	itemIDs.clear();
	autoCheck.clear();
	isChecked.clear();

}

//
// MENU
//

// Main menu we will create
HMENU ofxWinMenu::CreateWindowMenu()
{
	HMENU hMenu = GetMenu(g_hwnd);
	if(!hMenu) 
		g_hMenu = CreateMenu();
	return g_hMenu;
}

// Popup menu of the main menu
HMENU ofxWinMenu::AddPopupMenu(HMENU hMenu, string MenuName)
{
	if(hMenu) {
		HMENU hSubMenu = CreatePopupMenu();
		if(hSubMenu) {
			AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, MenuName.c_str());
			return hSubMenu;
		}
	}
	return NULL;

}

//
// Popup menu items
//
//     hSubMenu   - the popup menu to add an item to
//     ItemName   - the name of the menu item which will appear in the menu
//     bChecked   - initial state of the menu item, checked or not
//     bAutoCheck - Check the item on or off automatically on selection
//
bool ofxWinMenu::AddPopupItem(HMENU hSubMenu, string ItemName)
{
	return AddPopupItem(hSubMenu, ItemName, false, true);
}

bool ofxWinMenu::AddPopupItem(HMENU hSubMenu, string ItemName, bool bChecked)
{
	return AddPopupItem(hSubMenu, ItemName, bChecked, true);
}

bool ofxWinMenu::AddPopupItem(HMENU hSubMenu, string ItemName, bool bChecked, bool bAutoCheck)
{
	if(g_hMenu && hSubMenu) {
		int nItem = GetMenuItemCount(hSubMenu);
		int itemID = itemIDs.size();
		itemIDs.push_back(nItem);
		subMenus.push_back(hSubMenu);
		itemNames.push_back(ItemName);
		isChecked.push_back(bChecked);
		autoCheck.push_back(bAutoCheck);
		if(InsertMenuA(hSubMenu, nItem, MF_BYPOSITION, itemID, ItemName.c_str())) {
			if(bAutoCheck && bChecked) 
				CheckMenuItem(hSubMenu, nItem, MF_BYPOSITION | MF_CHECKED);
			return true;
		}
	}
	return false;
}

// Item separator
bool ofxWinMenu::AddPopupSeparator(HMENU hSubMenu)
{
	int nItems = 0;
	HMENU hSubSubMenu = NULL;
	char name[MAX_PATH];

	if(g_hMenu && hSubMenu) {

		GetMenuStringA(hSubMenu, 0, (LPSTR)name, MAX_PATH, MF_BYPOSITION);
		int n = GetMenuItemCount(hSubMenu);

		// Include popup submenus - allow for one level deep
		for(int i = 0; i < n; i++) {
			nItems++;
			// Is the item a submenu of the popup menu ?
			hSubSubMenu = GetSubMenu(hSubMenu, i);
			if(hSubSubMenu) {
				nItems++; // Include the submenu itself
				GetMenuStringA(hSubSubMenu, 0, (LPSTR)name, MAX_PATH, MF_BYPOSITION);
				// Add it's items to the incrementing count as we build the menu
				nItems += GetMenuItemCount(hSubMenu); 
			}
		}
		itemIDs.push_back(nItems);
		subMenus.push_back(hSubMenu);
		itemNames.push_back("");
		isChecked.push_back(false);
		autoCheck.push_back(false);
		return (bool)InsertMenuA(hSubMenu, nItems, MF_SEPARATOR, 0, NULL);
	}
	return false;
}

// Set the menu to the application
bool ofxWinMenu::SetWindowMenu()
{
	if(g_hwnd && g_hMenu)
		return (bool)SetMenu(g_hwnd, g_hMenu);
	else
		return false;
}

// Remove the menu from the application
bool ofxWinMenu::RemoveWindowMenu()
{
	if(g_hwnd)
		return (bool)SetMenu(g_hwnd, NULL);
	else
		return false;
}

// Destroy the menu - not normally used
bool ofxWinMenu::DestroyWindowMenu()
{
	if(g_hMenu)
		return (bool)DestroyMenu(g_hMenu);
	else
		return false;
}


// Check or uncheck a menu item
bool ofxWinMenu::SetPopupItem(string ItemName, bool bChecked)
{
	if(g_hwnd == NULL || g_hMenu == NULL || !IsMenu(g_hMenu)) return false;
	
	int nItems = itemIDs.size();
	if(nItems > 0) {
		// Find the item number
		for(int i=0; i<nItems; i++) {
			if(ItemName == itemNames.at(i)) {
				// Which popup menu is the item in
				HMENU hSubMenu = subMenus.at(i);
				if(hSubMenu) {
					// How many items in the submenu
					int nPopupItems = GetMenuItemCount(hSubMenu);
					// Loop through the popup items to find a match
					if(nPopupItems > 0) {
						char itemstring[MAX_PATH];
						for(int j=0; j<nPopupItems; j++) {
							GetMenuStringA(hSubMenu, j, (LPSTR)itemstring, MAX_PATH, MF_BYPOSITION);
							if(ItemName == itemstring) {
								if(bChecked)
									CheckMenuItem(hSubMenu, j, MF_BYPOSITION | MF_CHECKED);
								else
									CheckMenuItem(hSubMenu, j, MF_BYPOSITION | MF_UNCHECKED);
								isChecked.at(i) = bChecked;
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}


// ofApp Function for return of memu item selection
void ofxWinMenu::CreateMenuFunction(void(ofApp::*function)(string title, bool bChecked))
{
	pAppMenuFunction = function; // Return function in ofApp
}

// Pass back the menu item title and state to ofApp
// by calling the function set by "CreateMenuFunction"
void ofxWinMenu::MenuFunction(string title, bool bChecked)
{
	(pApp->*pAppMenuFunction)(title, bChecked); 
}


//
// Our local window message callback procedure
//
LRESULT CALLBACK ofxWinMenuWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HMENU hSubMenu = NULL;
	char title[MAX_PATH];
	int ID = 0;

	// Menu item ID
	int wmId = (int)LOWORD(wParam);

	switch (uMsg) {	

		// Check For Windows system messages
		case WM_SYSCOMMAND:	

			switch (wParam)	{         // Check System Calls
				case SC_SCREENSAVE:   // Screensaver Trying To Start?
				case SC_MONITORPOWER: // Monitor Trying To Enter Powersave?
				return 0;             // Prevent From Happening
			}
			break;

		case WM_ENTERMENULOOP:
			// Inform ofApp of menu entry
			pThis->MenuFunction("WM_ENTERMENULOOP", true);
			break;

		case WM_EXITMENULOOP :
			// Inform ofApp of menu exit
			pThis->MenuFunction("WM_EXITMENULOOP", true);
			break;

		case WM_COMMAND:

			// Inform ofApp which menu has been selected
			hSubMenu = pThis->subMenus.at(wmId); // Submenu that the menu item is in
			ID = pThis->itemIDs.at(wmId); // Position of the item in the submenu

			// Check the menu item if autocheck is enabled for it
			if(pThis->autoCheck.at(wmId)) {
				if(pThis->isChecked.at(wmId)) // currently checked
					CheckMenuItem (hSubMenu, ID, MF_BYPOSITION | MF_UNCHECKED); // uncheck it
				else
					CheckMenuItem (hSubMenu, ID, MF_BYPOSITION | MF_CHECKED); // otherwise check it
				pThis->isChecked.at(wmId) = !pThis->isChecked.at(wmId); // toggle the menu item state flag
			}
			
			// Get the menu item title
			GetMenuStringA(hSubMenu, wmId, (LPSTR)title, MAX_PATH, MF_BYCOMMAND);

			// Inform ofApp of the menu item title and new state
			pThis->MenuFunction(title, pThis->isChecked.at(wmId));

			break;

		case WM_CLOSE: {         // Close Message
			PostQuitMessage(0);  // Send A Quit Message
			return 0;            // Jump Back
		}
		// Openframeworks handles key and mouse and drag/drop etc.
	}

	// Pass unhandled messages on to the openframeworks application
	return(CallWindowProc(ofAppWndProc, hWnd, uMsg, wParam, lParam));

}

