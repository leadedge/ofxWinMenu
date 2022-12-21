/*

	ofxWinMenu

	Create a menu for a Microsoft Windows Openframeworks application.
	
	Copyright (C) 2016-2023 Lynn Jarvis.

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

*/
#pragma once

#include <Windows.h>
#include <string>
#include <vector>

class ofApp; // Forward declaration

class ofxWinMenu {

	public:

		ofxWinMenu(ofApp *app, HWND hwnd);
		~ofxWinMenu();

		HWND g_hwnd; // ofApp window
		HMENU g_hMenu; // ofApp menu we are building

		// Main menu
		HMENU CreateWindowMenu();

		// Popup menu of the main menu
		HMENU AddPopupMenu(HMENU hMenu, std::string menuName);

		// Popup menu items
		bool AddPopupItem(HMENU hSubMenu, std::string ItemName);
		bool AddPopupItem(HMENU hSubMenu, std::string ItemName, bool bChecked);
		bool AddPopupItem(HMENU hSubMenu, std::string ItemName, bool bChecked, bool bAutoCheck);
		bool AddPopupSeparator(HMENU hSubMenu);

		// Set the menu to the application
		bool SetWindowMenu();

		// Remove the menu but do not destroy it
		bool RemoveWindowMenu();

		// Destroy the menu
		bool DestroyWindowMenu();

		// Set the menu checkmark of a popup item
		bool SetPopupItem(std::string ItemName, bool bChecked);

		// Return 0 (not checked), 1 (checked), -1 (does not exist)
		int GetPopupItem(std::string ItemName);

		// Change the name of a popup item
		bool SetPopupItemName(std::string ItemName, std::string NewName);

		// Enable or disable (grey out) a popup item
		bool EnablePopupItem(std::string ItemName, bool bEnabled);

		// Function from ofApp for return of memu item selection
		void CreateMenuFunction(void(ofApp::*function)(std::string title, bool bChecked));

		// Function for ofxWinMenu to return menu item selection to ofApp
		void MenuFunction(std::string title, bool bChecked);

		ofApp *pApp; // Pointer to access the ofApp class
		void(ofApp::*pAppMenuFunction)(std::string title, bool bChecked); // The ofApp menu function

		// Menu item data
		std::vector<std::string> itemNames; // Name of the menu item
		std::vector<HMENU> subMenus;   // Submenu containing the menu item
		std::vector<int> itemIDs;      // Position of the item in the submenu
		std::vector<bool> autoCheck;   // Check the menu item on and off
		std::vector<bool> isChecked;   // Item checked flag

};
