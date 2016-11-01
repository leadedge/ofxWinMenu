
#pragma once

class ofApp; // Forward declaration

class ofxWinMenu {

	public:

		ofxWinMenu(ofApp *app);
		~ofxWinMenu();

		HWND g_hwnd; // ofApp window
		HMENU g_hMenu; // ofApp menu we are building

		// Main menu
		HMENU CreateWindowMenu();

		// Popup menu of the main menu
		HMENU AddPopupMenu(HMENU hMenu, string menuName);

		// Popup menu items
		bool AddPopupItem(HMENU hSubMenu, string ItemName);
		bool AddPopupItem(HMENU hSubMenu, string ItemName, bool bChecked);
		bool AddPopupItem(HMENU hSubMenu, string ItemName, bool bChecked, bool bAutoCheck);
		bool AddPopupSeparator(HMENU hSubMenu);

		// Set the menu to the application
		bool SetWindowMenu();

		// Remove the menu but do not destroy it
		bool RemoveWindowMenu();

		// Destroy the menu
		bool DestroyWindowMenu();

		// Set the menu checkmark of a popup item
		bool SetPopupItem(string ItemName, bool bChecked);

		// Function from ofApp for return of memu item selection
		void CreateMenuFunction(void(ofApp::*function)(string title, bool bChecked));

		// Function for ofxWinMenu to return menu item selection to ofApp
		void MenuFunction(string title, bool bChecked);

		ofApp *pApp; // Pointer to access the ofApp class
		void(ofApp::*pAppMenuFunction)(string title, bool bChecked); // The ofApp menu function

		// Menu item data
		vector<string> itemNames; // Name of the menu item
		vector<HMENU> subMenus;   // Submenu containing the menu item
		vector<int> itemIDs;      // Position of the item in the submenu
		vector<bool> autoCheck;   // Check the menu item on and off
		vector<bool> isChecked;   // Item checked flag

};
