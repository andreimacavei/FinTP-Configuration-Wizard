FinTP-Configuration-Wizard
==========================

A simple interface written in Qt to allow a more user-friendly way of configuration for the open source [FinTP](http://www.allevo.ro/initiatives/fintp.aspx) project.

Requirements:
 - GCC 64/32 bit
 - Qt Creator 5.1.0

Build Instructions:
 Open src/FinTP-Configuration-Wizard.pro in Qt Creator and it will automatically import the project.
 Alternatively you can install qmake to generate the Makefile.

The XML file should always be one level above.

Usage instructions:

After building the application, you can run it from Qt Creator or from target executable "FinTP-Configuration-Wizard.exe".
This will generate a GUI based on the file FinTP-Configuration-Wizard.xml, and will allow the user to modify it, add or delete extra filter, and also save it.

The Main Window's menu has 4 entries:
 - Open - will open another xml and will generate again the GUI
 - Save - will save the GUI to the same Xml file used to generate it.
 - Save As - will save the GUI to another Xml file.
 - Exit - exits the GUI.

There are 2 buttons for adding or removing filters:
 - Add Filter - will get a list of filters available for the current Tab in GUI (mapped to a Main Filter in XML), and will let the user to choose which filter(s) to add
 - Remove Filter - will get a list of filters already added in the current Tab in GUI and will give the user the possibility to remove them.



