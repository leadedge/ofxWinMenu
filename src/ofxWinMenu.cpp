//
//		ofxWinMenu
//
//

#include "ofApp.h"
#include "ofxWinMenu.h"


static LRESULT CALLBACK ofxWinMenuWndProc(HWND, UINT, WPARAM, LPARAM); // Local window message procedure
static WNDPROC ofAppWndProc; // Openframeworks application window message procedure
static ofxWinMenu *pThis; // Pointer to access the ofxWinMenu class from the window procedure


ofxWinMenu::ofxWinMenu(ofApp *app) {

	g_hMenu = NULL; // Set by CreateMenu and returned to ofApp
	pAppMenuFunction = NULL; // Set by CreateMenuFunction to return menu state to ofApp

	// The window handle of ofApp
	g_hwnd = GetActiveWindow();

	pThis = this; // Pointer for access the ofxWinMenu class
	pApp = app; // The ofApp class pointer

	// Save the Openframeworks application window message procedure
	ofAppWndProc = (WNDPROC)GetWindowLongPtrA(g_hwnd, GWL_WNDPROC);
	
	// Set our own window message procedure
	SetWindowLongPtrA(g_hwnd, GWL_WNDPROC, (LONG)ofxWinMenuWndProc);


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
	if(g_hMenu && hSubMenu) {
		 // Includes popup submenus
		int n = GetMenuItemCount(hSubMenu);
		 // Allow for one level deep
		for(int i = 0; i < n; i++) {
			nItems++;
			// Is the item a submenu of the popup menu ?
			hSubSubMenu = GetSubMenu(hSubMenu, i);
			if(hSubSubMenu) {
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

	// printf("SetPopupItem(%s. %d)\n", ItemName.c_str(), bChecked);
	int nItems = itemIDs.size();
	// printf("%d items\n", nItems);
	if(nItems > 0) {
		// Find the item number
		for(int i=0; i<nItems; i++) {
			// printf("%s\n", itemNames.at(i).c_str());
			if(ItemName == itemNames.at(i)) {
				// printf("Match at %d\n", i);
				// Which popup menu is the item in
				HMENU hSubMenu = subMenus.at(i);
				if(hSubMenu) {
					// How many items in the submenu
					int nPopupItems = GetMenuItemCount(hSubMenu);
					// printf("%d popup items\n", nPopupItems);
					// Loop through the popup items to find a match
					if(nPopupItems > 0) {
						char itemstring[MAX_PATH];
						for(int j=0; j<nPopupItems; j++) {
							GetMenuStringA(hSubMenu, j, (LPSTR)itemstring, MAX_PATH, MF_BYPOSITION);
							// printf("  %s\n", itemstring);
							if(ItemName == itemstring) {
								if(bChecked)
									CheckMenuItem(hSubMenu, j, MF_BYPOSITION | MF_CHECKED);
								else
									CheckMenuItem(hSubMenu, j, MF_BYPOSITION | MF_UNCHECKED);
								isChecked.at(i+j) = bChecked;
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

		case WM_COMMAND:

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

