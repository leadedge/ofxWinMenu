# ofxWinMenu

Create a menu for a Microsoft Windows Openframeworks application.

### Setup

1. Add "ofxWinMenu.h" and "ofxWinMenu.cpp" to your Visual Studio project.

In the Visual Studio project properties :

- Add "ofxWinMenu/src" to additional "C/C++/General/Additional Include Directories"
- Add "#include ofxWinMenu.h" to the ofApp header file

### Example basic

- Copy the source files from "ofxWinMenu/example-basic/src" to the application "src" folder.
- Copy the image and font files from "ofxWinMenu/example-basic/bin/data" to the application "bin/data" folder.

### Example advanced

- Copy the source files from "ofxWinMenu/example-advanced/src" to the application "src" folder.
- Copy the image and font files from "ofxWinMenu/example-advanced/bin/data" to the application "bin/data" folder.
- Add resource.h and resource.rc to your project.

### Using ofxWinMenu

In the ofApp header file declare a menu object

    ofxWinMenu * menu;
  
Also declare a function in ofApp that will receive messages back from ofxWinMenu. For example :

    void appMenuFunction(string title, bool bChecked);
  
The title string and bChecked flag are returned whenever a menu selection is made. The compare the string returned with the menu items that have been added to take action as required.

In ofApp create a new menu object and pass the application class pointer to it.

    menu = new ofxWinMenu(this);
  
Create the return function

    void CreateMenuFunction(void(ofApp::*function*)(string title, bool bChecked));
  
This establishes a function within ofApp for return of menu item selection. The function can be any name and must exist. It is used as follows :

    menu->CreateMenuFunction(&ofApp::appMenuFunction);
  
and the function is simply :

    void ofApp::appMenuFunction(string title, bool bChecked)
    {
        // Check menu items here
    }

### ofxWinMenu functions

    HMENU CreateWindowMenu();

Creates the menu that will be filled with menu items and returns a handle to it.

    HMENU AddPopupMenu(HMENU hMenu, string menuName);

Adds a popup menu which will apear on the menu bar and returns it's handle.

    bool AddPopupItem(HMENU hSubMenu, string ItemName);

Adds a menu item to the popup menu. By default the item is checked on or off by selection and a checkmark appears in the menu.

    bool AddPopupItem(HMENU hSubMenu, string ItemName, bool bChecked);

Allows the item added to start with checked or "on" status and with a menu checkmark. This is necessary where the action to be toggled on or off has to start "on", such as the "Show info" option in the examples.

    bool AddPopupItem(HMENU hSubMenu, string ItemName, bool bChecked, bool bAutoCheck);

Allows the automatic checking to be disabled. This might be necessary where the item performs an action, such as full screen or the About box.

    bool AddPopupSeparator(HMENU hSubMenu);

Adds a separator between popup menu items. This cannot be selected and has no effect on the menu function other than providing spacing from other items, such as the Exit option.

    bool SetWindowMenu();

When all popup menus and items have been added, this finalises the menu and sets it to the application window. It is also used if RemoveWindowMenu has been used.

    bool RemoveWindowMenu();

Removes the menu but does not destroy it, as in the full screen example.

    bool DestroyWindowMenu();

Destroys the menu if this is necessary while the application is running, for example if a different menu has to be created.

### Using resources

The advanced example includes an About dialog, Version information and a custom modeless dialog with controls. See resource.h and resource.rc. 

Examine resource.h to modify version information.

resource.rc can be modified using a resource editor such as [ResEdit](http://www.resedit.net) or by Microsoft Visual Studio. If the edition of Visual Studio does not include resource editing, RH click on resource.rc, "Open with" and "C++ source code editor".
The Dialog procedures AboutDlgProc and OptionsDlgProc have to be changed as well to handle any different controls.

Resedit may include "richedit.h" to resource.rc. This can be removed if it causes warning RC4011.

Resedit will add additional control IDs to resource.h starting at 40000 and can include duplicate ID values. Check and edit resource.h as necessary.

### Binaries

Binaries are included to illustrate the function of ofxWinMenu before building the project.

### Copyright

ofxWinMenu - Copyright (C) 2016 Lynn Jarvis [http://spout.zeal.co/](http://spout.zeal.co/)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser  General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this program.  If not, see [http://www.gnu.org/licenses/](http://www.gnu.org/licenses).
