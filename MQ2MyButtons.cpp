// MQ2MyButtons.cpp : Defines the entry point for the DLL application.
//
// From Knightly:  I don't know who originally wrote this, I stole the source
// and updated it to fix it.  But this is definitely not my code.  So, don't you
// judge me. https://www.youtube.com/watch?v=8MVpndgU1ic

#include "../MQ2Plugin.h"
#include <fstream>

// Typically in the header, but Plugins with Multiple Files get stitches.
void CreateButtonWindow();
void DestroyButtonWindow();
void ReadWindowINI(PCSIDLWND pWindow);
PLUGIN_API VOID MyButtonsCommand(PSPAWNINFO pSpawn, PCHAR szLine);

PreSetup("MQ2MyButtons");

namespace KnightlyMyButtons {
	bool boolDebug = false;
	bool boolPluginSuccess = false;
	bool boolShowWindow = true;
	std::string xmlVersion = "2018-12-23";
	// All of this should be converted into one object, but by the time I realized that it was already written.
	// And, sure arrays start at zero, but when you're talking about "Button 1" that gets confusing, so one extra won't hurt.
	char arrMyCommands[13][MAX_STRING] = { 0 };
	char arrMyLabels[13][MAX_STRING] = { 0 };
	char arrMyColors[13][3][MAX_STRING] = { 0 };

	// Log Functions we'll be using
	class Log {
		public:
			// Message is for logging a standard message.
			// All other logging calls go through this base.
			static void Message(std::string strMessage) {
				CHAR pcharMessage[MAX_STRING];
				strMessage = "\ay[\agMQ2MyButtons\ay]\aw ::: \ao" + strMessage;
				strcpy_s(pcharMessage, strMessage.c_str());
				WriteChatf(pcharMessage);
			}

			// Error is for logging errors
			static void Error(std::string strError) {
				strError = "\arERROR: " + strError;
				Message(strError);
			}

			// Debug is for logging debug messages and only
			// works if boolDebug is TRUE.
			static void Debug(std::string strDebug) {
				strDebug = "\amDEBUG: " + strDebug;
				if (boolDebug) {
					Message(strDebug);
				}
			}

			static void ShowHelp() {
				Message("\ay============== MyButtons Help ==============");
				Message("\aoTo change buttons, edit MQ2MyButtons.ini in your MQ2 directory");
				Message(" ");
				Message("\ayAvailable TLOs:");
				Message("\ay      \at${MyButtons.label[\am<Button>\at]}\ay -- String - The Name assigned to <Button>");
				Message("\ay      \at${MyButtons.cmd[\am<Button>\at]}\ay -- String - The Command assigned to <Button>");
				Message("\ayExample:");
				Message("\ay      \ao/echo ${MyButtons.label[1]}");
				Message("\ayThe above would return the label of Button 1.");
				Message(" ");
				Message("\ayUsage:");
				Message("\ay      Toggle the window on/off:");
				Message("\ay           \at/mybuttons");
				Message("\ay           \at/mybuttons on");
				Message("\ay           \at/mybuttons off");
				Message("\ay      Reload hotkeys from ini:");
				Message("\ay           \at/mybuttons reload");
				Message("\ay           \at/mybuttons reloadcolors \ar(Long - Reloads UI)");
				Message("\ay      Show buttons configuration:");
				Message("\ay           \at/mybuttons show");
				Message("\ay      Use a button:");
				Message("\ay           \at/mybuttons \am<ButtonNumber>");
				Message("\ayExample:");
				Message("\ay        \ao/mybuttons 1");
				Message("\ayThe above would be the same as clicking Button 1.");
			}

			static void ShowButtons() {
				Message("\ay========== Current Button Commands ==========");
				for (int i = 1; i <= 12; i++) {
					if (!(arrMyCommands[i][0] == 0)) {
						std::string strBuffer = "";
						Message("----------------------");
						Message("Button: \am" + std::to_string(i));
						strBuffer = arrMyLabels[i];
						Message("Label: \at" + strBuffer);
						strBuffer = arrMyCommands[i];
						Message("Command: \at" + strBuffer);
					}
				}
			}
	};

	class File {
		public:
			// Function to check for and create XML file.  By default it doesn't create the file
			// if it already exists in the right version.  This can be overridden if needed.
			static bool CheckAndCreateXMLFile(std::string strFileName, bool createFile = false) {
				// Assume something went wrong~
				bool returnResult = false;
				std::string strFilePath = "uifiles\\default\\" + strFileName;
				// Check if the file already exists
				std::ifstream readPath(strFilePath);
				if (!readPath) {
					// File doesn't exist
					createFile = true;
				}
				else {
					std::string versionLine;
					// Read the first line
					getline(readPath, versionLine);
					// Check if this is the current version
					if (versionLine == "<!-- MyButtons UI File Version:  " + xmlVersion + " -->") {
						returnResult = true;
					}
					// Otherwise replace it
					else {
						createFile = true;
					}
					// Close any open reads we have
					readPath.close();

				}

				// If we should create the file
				if (createFile) {
					// Open the file for writing
					std::ofstream writePath(strFilePath);
					// If we have a write handle (ie, we can write to it)...
					if (writePath) {
						if (strFileName == "MQUI_MyButtonsWnd.xml") {
							// Yeah, I know doing it this way is silly, versus just including it as a resource
							// but until the Online Builder lets me update my own dependency folders, this is the
							// lesser of two evils.  Or if not the lesser, certainly the more convenient.
							writePath << "<!-- MyButtons UI File Version:  " + xmlVersion + " -->";

							writePath << R"KnightlyXMLRtrn(
<?xml version="1.0" encoding="us-ascii"?>
<XML ID="EQInterfaceDefinitionLanguage">
	<Schema xmlns="EverQuestData" xmlns:dt="EverQuestDataTypes" />
	<Ui2DAnimation item="AMQMB_Button1Normal">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>0</X>
				<Y>0</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button1Pressed">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>80</X>
				<Y>0</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button1Flyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>40</X>
				<Y>0</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button1PressedFlyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>120</X>
				<Y>0</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button1Disabled">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>160</X>
				<Y>0</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button2Normal">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>0</X>
				<Y>40</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button2Pressed">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>80</X>
				<Y>40</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button2Flyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>40</X>
				<Y>40</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button2PressedFlyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>120</X>
				<Y>40</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button2Disabled">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>160</X>
				<Y>40</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button3Normal">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>0</X>
				<Y>80</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button3Pressed">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>80</X>
				<Y>80</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button3Flyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>40</X>
				<Y>80</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button3PressedFlyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>120</X>
				<Y>80</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button3Disabled">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>160</X>
				<Y>80</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button4Normal">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>0</X>
				<Y>120</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button4Pressed">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>80</X>
				<Y>120</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button4Flyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>40</X>
				<Y>120</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button4PressedFlyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>120</X>
				<Y>120</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button4Disabled">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>160</X>
				<Y>120</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button5Normal">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>0</X>
				<Y>160</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button5Pressed">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>80</X>
				<Y>160</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button5Flyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>40</X>
				<Y>160</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button5PressedFlyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>120</X>
				<Y>160</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button5Disabled">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>160</X>
				<Y>160</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button6Normal">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>0</X>
				<Y>200</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button6Pressed">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>80</X>
				<Y>200</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button6Flyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>40</X>
				<Y>200</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button6PressedFlyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>120</X>
				<Y>200</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
<!-- C++ String Limit Break -->)KnightlyXMLRtrn";

							writePath << R"KnightlyXMLRtrn(
	<Ui2DAnimation item="AMQMB_Button6Disabled">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces06.tga</Texture>
			<Location>
				<X>160</X>
				<Y>200</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button7Normal">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>0</X>
				<Y>0</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button7Pressed">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>80</X>
				<Y>0</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button7Flyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>40</X>
				<Y>0</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button7PressedFlyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>120</X>
				<Y>0</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button7Disabled">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>160</X>
				<Y>0</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button8Normal">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>0</X>
				<Y>40</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button8Pressed">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>80</X>
				<Y>40</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button8Flyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>40</X>
				<Y>40</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button8PressedFlyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>120</X>
				<Y>40</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button8Disabled">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>160</X>
				<Y>40</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button9Normal">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>0</X>
				<Y>80</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button9Pressed">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>80</X>
				<Y>80</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button9Flyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>40</X>
				<Y>80</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button9PressedFlyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>120</X>
				<Y>80</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button9Disabled">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>160</X>
				<Y>80</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button10Normal">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>0</X>
				<Y>120</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button10Pressed">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>80</X>
				<Y>120</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button10Flyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>40</X>
				<Y>120</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button10PressedFlyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>120</X>
				<Y>120</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button10Disabled">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>160</X>
				<Y>120</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button11Normal">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>0</X>
				<Y>160</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button11Pressed">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>80</X>
				<Y>160</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button11Flyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>40</X>
				<Y>160</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button11PressedFlyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>120</X>
				<Y>160</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button11Disabled">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>160</X>
				<Y>160</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button12Normal">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>0</X>
				<Y>200</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button12Pressed">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>80</X>
				<Y>200</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button12Flyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>40</X>
				<Y>200</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
<!-- C++ String Limit Break -->)KnightlyXMLRtrn";

							writePath << R"KnightlyXMLRtrn(
	<Ui2DAnimation item="AMQMB_Button12PressedFlyby">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>120</X>
				<Y>160</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
	<Ui2DAnimation item="AMQMB_Button12Disabled">
		<Cycle>true</Cycle>
		<Frames>
			<Texture>window_pieces07.tga</Texture>
			<Location>
				<X>160</X>
				<Y>160</Y>
			</Location>
			<Size>
				<CX>40</CX>
				<CY>40</CY>
			</Size>
			<Hotspot>
				<X>0</X>
				<Y>0</Y>
			</Hotspot>
			<Duration>1000</Duration>
		</Frames>
	</Ui2DAnimation>
<!-- Break for Button Inserts -->)KnightlyXMLRtrn";
							for (int j = 1; j <= 12; j++) {
								writePath << std::endl << "\t<Button item=\"MQMB_Button" + std::to_string(j) + "\">" << std::endl;
								writePath << "\t\t<ScreenID>MQMB_Button" + std::to_string(j) + "</ScreenID>";
								writePath << R"KnightlyXMLRtrn(
		<Font>1</Font>
		<RelativePosition>true</RelativePosition>
		<Size>
			<CX>37</CX>
			<CY>34</CY>
		</Size>
		<Text></Text>
		<DecalOffset>
			<X>2</X>
			<Y>2</Y>
		</DecalOffset>
		<DecalSize>
			<CX>33</CX>
			<CY>30</CY>
		</DecalSize>
		<ButtonDrawTemplate>)KnightlyXMLRtrn";
								writePath << std::endl << "\t\t\t<Normal>AMQMB_Button" + std::to_string(j) + "Normal</Normal>";
								writePath << std::endl << "\t\t\t<Pressed>AMQMB_Button" + std::to_string(j) + "Pressed</Pressed>";
								writePath << std::endl << "\t\t\t<Flyby>AMQMB_Button" + std::to_string(j) + "Flyby</Flyby>";
								writePath << std::endl << "\t\t\t<Disabled>AMQMB_Button" + std::to_string(j) + "Disabled</Disabled>";
								writePath << std::endl << "\t\t\t<PressedFlyby>AMQMB_Button" + std::to_string(j) + "PressedFlyby</PressedFlyby>" << std::endl;
								writePath << "\t\t</ButtonDrawTemplate>" << std::endl;
								writePath << "\t</Button>" << std::endl;
								writePath << "\t<Label item=\"MQMB_Label" + std::to_string(j) + "\">" << std::endl;
								writePath << "\t\t<ScreenID>MQMB_Label" + std::to_string(j) + "</ScreenID>" << std::endl;
								writePath << "\t\t<EQType>9999</EQType>" << std::endl;
								writePath << "\t\t<TooltipReference>${MyButtons.Label[" + std::to_string(j) + "]}</TooltipReference>";
								writePath << R"KnightlyXMLRtrn(
		<RelativePosition>true</RelativePosition> 
		<Location> 
			<X>0</X> 
			<Y>5</Y> 
		</Location> 
		<Size> 
			<CX>37</CX> 
			<CY>34</CY> 
		</Size> 
		<Text></Text>
		<Font>1</Font> 
		<TextColor>
<!-- Break for Color Inserts -->)KnightlyXMLRtrn";
								writePath << std::endl << "\t\t\t<R>" << KnightlyMyButtons::arrMyColors[j][0] << "</R>" << std::endl;
								writePath << "\t\t\t<G>" << KnightlyMyButtons::arrMyColors[j][1] << "</G>" << std::endl;
								writePath << "\t\t\t<B>" << KnightlyMyButtons::arrMyColors[j][2] << "</B>";
								writePath << R"KnightlyXMLRtrn(
<!-- End Break for Color Inserts --> 
		</TextColor> 
		<NoWrap>false</NoWrap> 
		<AlignCenter>true</AlignCenter> 
		<AlignRight>false</AlignRight> 
		<Style_Transparent>true</Style_Transparent>
		<Style_TransparentControl>true</Style_TransparentControl>
	</Label>)KnightlyXMLRtrn";
								writePath << std::endl << "\t<LayoutBox item=\"MQMB_LayoutB" + std::to_string(j) + "\">" << std::endl;
								writePath << "\t\t<ScreenID>MQMB_LayoutB" + std::to_string(j) + "</ScreenID>";
								writePath << R"KnightlyXMLRtrn(		
		<RelativePosition>true</RelativePosition>
		<Size>
			<CX>37</CX>
			<CY>34</CY>
		</Size>
		<Style_Transparent>true</Style_Transparent>
		<Style_TransparentControl>true</Style_TransparentControl>)KnightlyXMLRtrn";
								writePath << std::endl << "\t\t<Pieces>MQMB_Button" + std::to_string(j) + "</Pieces>" << std::endl;
								writePath << "\t\t<Pieces>MQMB_Label" + std::to_string(j) + "</Pieces>" << std::endl;
								writePath << "\t</LayoutBox>";
							}

							writePath << R"KnightlyXMLRtrn(
<!-- Break for Button Inserts End -->
	<Screen item="MQMB_NoSpinnerBarTemplate">
		<ScreenID>MQMB_NoSpinnerBarTemplate</ScreenID>
		<RelativePosition>true</RelativePosition>
		<AutoStretch>true</AutoStretch>
		<TopAnchorOffset>4</TopAnchorOffset>
		<BottomAnchorOffset>4</BottomAnchorOffset>
		<LeftAnchorOffset>4</LeftAnchorOffset>
		<RightAnchorOffset>4</RightAnchorOffset>	
		<TopAnchorToTop>true</TopAnchorToTop>
		<BottomAnchorToTop>false</BottomAnchorToTop>
		<LeftAnchorToLeft>true</LeftAnchorToLeft>
		<RightAnchorToLeft>false</RightAnchorToLeft>
		<UseInLayoutVertical>false</UseInLayoutVertical>
		<UseInLayoutHorizontal>false</UseInLayoutHorizontal>
	</Screen>
	<Screen item="MQMB_HorizontalBarTemplate">
		<ScreenID>MQMB_HorizontalBarTemplate</ScreenID>
		<RelativePosition>true</RelativePosition>
		<AutoStretch>true</AutoStretch>
		<TopAnchorOffset>4</TopAnchorOffset>
		<BottomAnchorOffset>4</BottomAnchorOffset>
		<LeftAnchorOffset>4</LeftAnchorOffset>
		<RightAnchorOffset>15</RightAnchorOffset>	
		<TopAnchorToTop>true</TopAnchorToTop>
		<BottomAnchorToTop>false</BottomAnchorToTop>
		<LeftAnchorToLeft>true</LeftAnchorToLeft>
		<RightAnchorToLeft>false</RightAnchorToLeft>
		<UseInLayoutVertical>false</UseInLayoutVertical>
		<UseInLayoutHorizontal>false</UseInLayoutHorizontal>
	</Screen>
	<Screen item="MQMB_VerticalBarTemplate">
		<ScreenID>MQMB_VerticalBarTemplate</ScreenID>
		<RelativePosition>true</RelativePosition>
		<AutoStretch>true</AutoStretch>
		<TopAnchorOffset>15</TopAnchorOffset>
		<BottomAnchorOffset>4</BottomAnchorOffset>
		<LeftAnchorOffset>4</LeftAnchorOffset>
		<RightAnchorOffset>4</RightAnchorOffset>	
		<TopAnchorToTop>true</TopAnchorToTop>
		<BottomAnchorToTop>false</BottomAnchorToTop>
		<LeftAnchorToLeft>true</LeftAnchorToLeft>
		<RightAnchorToLeft>false</RightAnchorToLeft>
		<UseInLayoutVertical>false</UseInLayoutVertical>
		<UseInLayoutHorizontal>false</UseInLayoutHorizontal>
	</Screen>
	<TileLayoutBox item="MQMB_ButtonLayout">
		<ScreenID>MQMB_ButtonLayout</ScreenID>
		<RelativePosition>true</RelativePosition>
		<AutoStretch>true</AutoStretch>
		<TopAnchorOffset>4</TopAnchorOffset>
		<BottomAnchorOffset>4</BottomAnchorOffset>
		<LeftAnchorOffset>4</LeftAnchorOffset>
		<RightAnchorOffset>11</RightAnchorOffset>	
		<TopAnchorToTop>true</TopAnchorToTop>
		<BottomAnchorToTop>false</BottomAnchorToTop>
		<LeftAnchorToLeft>true</LeftAnchorToLeft>
		<RightAnchorToLeft>false</RightAnchorToLeft>
		<Style_Transparent>true</Style_Transparent>
		<Style_TransparentControl>true</Style_TransparentControl>
		<Spacing>4</Spacing>
		<SecondarySpacing>4</SecondarySpacing>
		<HorizontalFirst>true</HorizontalFirst>
		<AnchorToTop>true</AnchorToTop>
		<AnchorToLeft>true</AnchorToLeft>
		<FirstPieceTemplate>true</FirstPieceTemplate>
		<SnapToChildren>true</SnapToChildren>
		<Pieces>LayoutBox:MQMB_LayoutB1</Pieces>
		<Pieces>LayoutBox:MQMB_LayoutB2</Pieces>
		<Pieces>LayoutBox:MQMB_LayoutB3</Pieces>
		<Pieces>LayoutBox:MQMB_LayoutB4</Pieces>
		<Pieces>LayoutBox:MQMB_LayoutB5</Pieces>
		<Pieces>LayoutBox:MQMB_LayoutB6</Pieces>
		<Pieces>LayoutBox:MQMB_LayoutB7</Pieces>
		<Pieces>LayoutBox:MQMB_LayoutB8</Pieces>
		<Pieces>LayoutBox:MQMB_LayoutB9</Pieces>
		<Pieces>LayoutBox:MQMB_LayoutB10</Pieces>
		<Pieces>LayoutBox:MQMB_LayoutB11</Pieces>
		<Pieces>LayoutBox:MQMB_LayoutB12</Pieces>
	</TileLayoutBox>
	<Button item="MQMB_PageUpButton">
		<ScreenID>MQMB_PageUpButton</ScreenID>
		<RelativePosition>true</RelativePosition>
		<Size>
			<CX>11</CX>
			<CY>16</CY>
		</Size>
		<Template>BDT_VSBUp</Template>
	</Button>
	<Label item="MQMB_HorizontalCurrentPageLabel">
		<ScreenID>MQMB_HorizontalCurrentPageLabel</ScreenID>
		<Font>1</Font>
		<RelativePosition>true</RelativePosition>
		<Location>
			<X>0</X>
			<Y>16</Y>
		</Location>
		<Size>
			<CX>10</CX>
			<CY>11</CY>
		</Size>
		<Text>10</Text>
		<AlignCenter>true</AlignCenter>
	</Label>
	<Button item="MQMB_PageDownButton">
		<ScreenID>MQMB_PageDownButton</ScreenID>
		<RelativePosition>true</RelativePosition>
		<Location>
			<X>0</X>
			<Y>27</Y>
		</Location>
		<Size>
			<CX>11</CX>
			<CY>16</CY>
		</Size>
		<Template>BDT_VSBDown</Template>
	</Button>
	<Button item="MQMB_PageLeftButton">
		<ScreenID>MQMB_PageLeftButton</ScreenID>
		<RelativePosition>true</RelativePosition>
		<Size>
			<CX>16</CX>
			<CY>11</CY>
		</Size>
		<Template>BDT_HSBLeft</Template>
	</Button>
	<Label item="MQMB_VerticalCurrentPageLabel">
		<ScreenID>MQMB_VerticalCurrentPageLabel</ScreenID>
		<Font>1</Font>
		<RelativePosition>true</RelativePosition>
		<Location>
			<X>16</X>
			<Y>0</Y>
		</Location>
		<Size>
			<CX>10</CX>
			<CY>11</CY>
		</Size>
		<Text>10</Text>
		<AlignCenter>true</AlignCenter>
	</Label>
	<Button item="MQMB_PageRightButton">
		<ScreenID>MQMB_PageRightButton</ScreenID>
		<RelativePosition>true</RelativePosition>
		<Location>
			<X>27</X>
			<Y>0</Y>
		</Location>
		<Size>
			<CX>16</CX>
			<CY>11</CY>
		</Size>
		<Template>BDT_HSBRight</Template>
	</Button>
	<Screen item="MQMB_HorizontalBarPageButtons">
		<ScreenID>MQMB_HorizontalBarPageButtons</ScreenID>
		<RelativePosition>true</RelativePosition>
		<AutoStretch>true</AutoStretch>
		<TopAnchorOffset>0</TopAnchorOffset>
		<BottomAnchorOffset>43</BottomAnchorOffset>
		<LeftAnchorOffset>11</LeftAnchorOffset>
		<RightAnchorOffset>0</RightAnchorOffset>	
		<TopAnchorToTop>true</TopAnchorToTop>
		<BottomAnchorToTop>true</BottomAnchorToTop>
		<LeftAnchorToLeft>false</LeftAnchorToLeft>
		<RightAnchorToLeft>false</RightAnchorToLeft>
		<Style_Transparent>true</Style_Transparent>
		<UseInLayoutVertical>false</UseInLayoutVertical>
		<UseInLayoutHorizontal>false</UseInLayoutHorizontal>
		<Pieces>MQMB_PageUpButton</Pieces>
		<Pieces>MQMB_HorizontalCurrentPageLabel</Pieces>
		<Pieces>MQMB_PageDownButton</Pieces>
	</Screen>
	<Screen item="MQMB_VerticalBarPageButtons">
		<ScreenID>MQMB_VerticalBarPageButtons</ScreenID>
		<RelativePosition>true</RelativePosition>
		<Style_Transparent>true</Style_Transparent>
		<Location>
			<X>1</X>
			<Y>0</Y>
		</Location>
		<Size>
			<CX>42</CX>
			<CY>11</CY>
		</Size>
		<UseInLayoutVertical>false</UseInLayoutVertical>
		<UseInLayoutHorizontal>false</UseInLayoutHorizontal>
		<Pieces>MQMB_PageLeftButton</Pieces>
		<Pieces>MQMB_VerticalCurrentPageLabel</Pieces>
		<Pieces>MQMB_PageRightButton</Pieces>
	</Screen>
	<LayoutVertical item="MQMB_LayoutV">
		<ResizeVertical>true</ResizeVertical>
		<ResizeHorizontal>true</ResizeHorizontal>
	</LayoutVertical>
	<Screen item="MQMBButtonWnd">
		<ScreenID />
		<Layout>MQMB_LayoutV</Layout>
		<Font>2</Font>
		<RelativePosition>false</RelativePosition>
		<Location>
			<X>0</X>
			<Y>230</Y>
		</Location>
		<Size>
			<CX>525</CX>
			<CY>53</CY>
		</Size>
		<DrawTemplate>WDT_RoundedNoTitle</DrawTemplate>
		<Style_Qmarkbox>true</Style_Qmarkbox>
		<Style_Closebox>true</Style_Closebox>
		<Style_Border>true</Style_Border>
		<Style_Sizable>true</Style_Sizable>
		<Style_ClientMovable>true</Style_ClientMovable>
		<Escapable>false</Escapable>
		<Pieces>TileLayoutBox:MQMB_ButtonLayout</Pieces>
	</Screen>
</XML>)KnightlyXMLRtrn";

							returnResult = true;
						}
						else {
							returnResult = false;
						}
						// Either way, close the file.
						writePath.close();
					}
				}
				return returnResult;
			}

			static bool LoadButtonData() {
				for (int i = 1; i <= 12; i++) {
					char szButtonSectionTmp[20] = "Button";
					strcat_s(szButtonSectionTmp, std::to_string(i).c_str());
					// Get the label into the arrMyLabels array at the position of the button number
					GetPrivateProfileString(szButtonSectionTmp, "Label", NULL, arrMyLabels[i], MAX_STRING, INIFileName);
					// Do the same with the Command
					GetPrivateProfileString(szButtonSectionTmp, "Command", NULL, arrMyCommands[i], MAX_STRING, INIFileName);
					// If there is a command, but no label, set the label to the button number
					if (!(arrMyCommands[i][0] == 0) && (arrMyLabels[i][0] == 0))
					{
						strcpy_s(arrMyLabels[i], std::to_string(i).c_str());
					}
					// Get the colors
					GetPrivateProfileString(szButtonSectionTmp, "Red", "255", arrMyColors[i][0], MAX_STRING, INIFileName);
					GetPrivateProfileString(szButtonSectionTmp, "Green", "255", arrMyColors[i][1], MAX_STRING, INIFileName);
					GetPrivateProfileString(szButtonSectionTmp, "Blue", "255", arrMyColors[i][2], MAX_STRING, INIFileName);
					// If the first button is empty, set it to Help with the color yellow
					if ((i == 1) && (arrMyCommands[i][0] == 0)) {
						strcpy_s(arrMyLabels[i], "Help");
						strcpy_s(arrMyCommands[i], "/mybuttons help");
						strcpy_s(arrMyColors[i][0], "255");
						strcpy_s(arrMyColors[i][1], "255");
						strcpy_s(arrMyColors[i][2], "153");
					}
				}
				return true;
			}
	};
}

class CHButWnd;
CHButWnd *MyBtnWnd = 0;
class CHButWnd : public CCustomWnd 
{ 
	public: 
	   CHButWnd():CCustomWnd("MQMBButtonWnd") 
	   { 
		
		  MyButton1 = (CButtonWnd*)GetChildItem("MQMB_Button1");
		  MyButton2 = (CButtonWnd*)GetChildItem("MQMB_Button2");
		  MyButton3 = (CButtonWnd*)GetChildItem("MQMB_Button3");
		  MyButton4 = (CButtonWnd*)GetChildItem("MQMB_Button4");
		  MyButton5 = (CButtonWnd*)GetChildItem("MQMB_Button5");
		  MyButton6 = (CButtonWnd*)GetChildItem("MQMB_Button6");
		  MyButton7 = (CButtonWnd*)GetChildItem("MQMB_Button7");
		  MyButton8 = (CButtonWnd*)GetChildItem("MQMB_Button8");
		  MyButton9 = (CButtonWnd*)GetChildItem("MQMB_Button9");
		  MyButton10 = (CButtonWnd*)GetChildItem("MQMB_Button10");
		  MyButton11 = (CButtonWnd*)GetChildItem("MQMB_Button11");
		  MyButton12 = (CButtonWnd*)GetChildItem("MQMB_Button12");

		  SetWndNotification(CHButWnd); 
	   } 
	
	   ~CHButWnd() 
	   { 
	   } 

	   int WndNotification(CXWnd *pWnd, unsigned int Message, void *unknown) 
	   {    
			if (pWnd==(CXWnd*)MyButton1) { 
				if (Message == XWM_LCLICK) {
					MyButtonsCommand((PSPAWNINFO)pCharSpawn, "1");
				}
				else {
				   DebugSpew("MyButton1 message %Xh / %d",Message,Message); 
				}
			} 
			else if (pWnd==(CXWnd*)MyButton2) { 
				if (Message==XWM_LCLICK) 
					MyButtonsCommand((PSPAWNINFO)pCharSpawn, "2");
				else
				   DebugSpew("MyButton2 message %Xh / %d",Message,Message); 
			} 
			else if (pWnd==(CXWnd*)MyButton3) { 
				if (Message==XWM_LCLICK) 
					MyButtonsCommand((PSPAWNINFO)pCharSpawn, "3");
				else
				   DebugSpew("MyButton3 message %Xh / %d",Message,Message); 
			} 
			else if (pWnd==(CXWnd*)MyButton4) { 
				if (Message==XWM_LCLICK) 
					MyButtonsCommand((PSPAWNINFO)pCharSpawn, "4");
				else
				   DebugSpew("MyButton4 message %Xh / %d",Message,Message); 
			} 
			else if (pWnd==(CXWnd*)MyButton5) { 
				if (Message==XWM_LCLICK)
					MyButtonsCommand((PSPAWNINFO)pCharSpawn, "5");
				else
				   DebugSpew("MyButton5 message %Xh / %d",Message,Message); 
			} 
			else if (pWnd==(CXWnd*)MyButton6) { 
				if (Message==XWM_LCLICK) 
					MyButtonsCommand((PSPAWNINFO)pCharSpawn, "6");
				else
				   DebugSpew("MyButton6 message %Xh / %d",Message,Message); 
			} 
			else if (pWnd==(CXWnd*)MyButton7) { 
				if (Message==XWM_LCLICK) 
					MyButtonsCommand((PSPAWNINFO)pCharSpawn, "7");
				else
				   DebugSpew("MyButton7 message %Xh / %d",Message,Message); 
			}
			else if (pWnd == (CXWnd*)MyButton8) {
				if (Message == XWM_LCLICK)
					MyButtonsCommand((PSPAWNINFO)pCharSpawn, "8");
				else
					DebugSpew("MyButton8 message %Xh / %d", Message, Message);
			}
			else if (pWnd==(CXWnd*)MyButton9) { 
				if (Message==XWM_LCLICK) 
					MyButtonsCommand((PSPAWNINFO)pCharSpawn, "9");
				else
				   DebugSpew("MyButton9 message %Xh / %d",Message,Message); 
			} 
			else if (pWnd==(CXWnd*)MyButton10) { 
				if (Message==XWM_LCLICK) 
					MyButtonsCommand((PSPAWNINFO)pCharSpawn, "10");
				else
				   DebugSpew("MyButton10 message %Xh / %d",Message,Message); 
			}
			else if (pWnd == (CXWnd*)MyButton11) {
			  if (Message == XWM_LCLICK)
				  MyButtonsCommand((PSPAWNINFO)pCharSpawn, "11");
			  else
				  DebugSpew("MyButton11 message %Xh / %d", Message, Message);
			}
			else if (pWnd == (CXWnd*)MyButton12) {
			  if (Message == XWM_LCLICK)
				  MyButtonsCommand((PSPAWNINFO)pCharSpawn, "12");
			  else
				  DebugSpew("MyButton12 message %Xh / %d", Message, Message);
			}
			return 0;
	   }; 
	CButtonWnd *MyButton1; 
	CButtonWnd *MyButton2; 
	CButtonWnd *MyButton3; 
	CButtonWnd *MyButton4; 
	CButtonWnd *MyButton5; 
	CButtonWnd *MyButton6; 
	CButtonWnd *MyButton7; 
	CButtonWnd *MyButton8; 
	CButtonWnd *MyButton9; 
	CButtonWnd *MyButton10; 
	CButtonWnd *MyButton11;
	CButtonWnd *MyButton12;
};

PLUGIN_API VOID MyButtonsCommand(PSPAWNINFO pSpawn, PCHAR szLine)
{
	bool WindowToggle = false;
	CHAR szParam1[MAX_STRING] = { 0 };
	GetArg(szParam1, szLine, 1, 0);
	// If the first parameter empty then toggle the window
	if (!szParam1 || strlen(szParam1) == 0) {
		WindowToggle = true;
	}
	// Otherwise if the first Parameter is "help" or "?"
	else if (szParam1 && (!strcmp(szParam1, "help") || !strcmp(szParam1, "?"))) {
		KnightlyMyButtons::Log::ShowHelp();
	}
	// Otherwise if the first parameter is "on"
	else if (szParam1 && !strcmp(szParam1, "on")) {
		WindowToggle = (KnightlyMyButtons::boolShowWindow ? false : true);
	}
	// Otherwise if the first parameter is "off"
	else if (szParam1 && !strcmp(szParam1, "off")) {
		WindowToggle = (KnightlyMyButtons::boolShowWindow ? true : false);
	}
	// Otherwise if the first parameter is "reloadcolors"
	else if (szParam1 && !strcmp(szParam1, "reloadcolors")) {
		KnightlyMyButtons::Log::Message("Reloading hotkeys from INI...");
		if (KnightlyMyButtons::File::LoadButtonData()) {
			KnightlyMyButtons::Log::Message("Resetting button colors...");
			if (KnightlyMyButtons::File::CheckAndCreateXMLFile("MQUI_MyButtonsWnd.xml", true)) {
				KnightlyMyButtons::Log::Message("Reloading user interface...");
				DoCommand((PSPAWNINFO)pCharSpawn, "/loadskin ${EverQuest.CurrentUI}");
				KnightlyMyButtons::Log::Message("...Done");
			}
			else {
				KnightlyMyButtons::Log::Error("...Failed");
			}
		}
		else {
			KnightlyMyButtons::Log::Error("...Failed");
		}
	}
	// Otherwise if the first parameter is "reload"
	else if (szParam1 && !strcmp(szParam1, "reload")) {
		KnightlyMyButtons::Log::Message("Reloading hotkeys from INI (does not include colors)...");
		if (KnightlyMyButtons::File::LoadButtonData()) {
			KnightlyMyButtons::Log::Message("...Success");
		}
		else {
			KnightlyMyButtons::Log::Error("...Failed");
		}
	}
	// Otherwise if the first parameter is "show"
	else if (szParam1 && !strcmp(szParam1, "show")) {
		KnightlyMyButtons::Log::ShowButtons();
	}
	// Otherwise if the first paramater is a number between 1 & 12
	else if (szParam1 && (atoi(szParam1) >= 1) && (atoi(szParam1) <= 12)) {
		if (KnightlyMyButtons::arrMyCommands[atoi(szParam1)] != NULL) {
			DoCommand((PSPAWNINFO)pCharSpawn, KnightlyMyButtons::arrMyCommands[atoi(szParam1)]);
		}
		else {
			KnightlyMyButtons::Log::Error("No Command Set for Button: " + std::string(szParam1));
		}
	}
	else {
		KnightlyMyButtons::Log::Error("Invalid button: " + std::string(szParam1));
	}

	if (WindowToggle) {
		if (KnightlyMyButtons::boolShowWindow == true) {
			KnightlyMyButtons::boolShowWindow = false;
			CreateButtonWindow();
			((CXWnd*)MyBtnWnd)->Show(0, 0);
		}
		else {
			KnightlyMyButtons::boolShowWindow = true;
			CreateButtonWindow();
			((CXWnd*)MyBtnWnd)->Show(1, 1);
		}
	}
}


class MQ2MyButtonsType *pMyButtonsType = nullptr;
class MQ2MyButtonsType : public MQ2Type {
	private:
		CHAR _szBuffer[MAX_STRING] = { 0 };
	public:
		enum Members {
			Label,
			label,
			CMD,
			Cmd,
			cmd
		};

		MQ2MyButtonsType() : MQ2Type("MyButtons") {
			TypeMember(Label);
			TypeMember(label);
			TypeMember(CMD);
			TypeMember(Cmd);
			TypeMember(cmd);
		}

		bool GetMember(MQ2VARPTR VarPtr, char* Member, char* Index, MQ2TYPEVAR &Dest) {
			_szBuffer[0] = '\0';
			// The Parameter holds the button
			CHAR szResultParam1[MAX_STRING] = { 0 };

			PMQ2TYPEMEMBER pMember = MQ2MyButtonsType::FindMember(Member);
			if (!pMember) return FALSE;

			switch ((Members)pMember->ID) {
				case Label:
				case label:
					Dest.Type = pStringType;
					// Validate the argument is between 1 and 12
					if (atoi(Index) > 0 && atoi(Index) < 13) {
						strcpy_s(_szBuffer, KnightlyMyButtons::arrMyLabels[atoi(Index)]);
					}
					else {
						// Return Invalid Button
						strcpy_s(_szBuffer, "InvalidButton");
					}
					break;
				case CMD:
				case Cmd:
				case cmd:
					Dest.Type = pStringType;
					// Validate the argument is between 1 and 12
					if (atoi(Index) > 0 && atoi(Index) < 13) {
						strcpy_s(_szBuffer, KnightlyMyButtons::arrMyCommands[atoi(Index)]);
					}
					else {
						// Return Invalid Button
						strcpy_s(_szBuffer, "InvalidButton");
					}
					break;
				default:
					return FALSE;
					break;
			}
			Dest.Ptr = &_szBuffer[0];
			return TRUE;
		}

		bool FromData(MQ2VARPTR &VarPtr, MQ2TYPEVAR &Source) { return FALSE; }
		bool FromString(MQ2VARPTR &VarPtr, char* Source) { return FALSE; }
};

BOOL MQ2MyBtnData(PCHAR szIndex, MQ2TYPEVAR &Dest)
{
	Dest.DWord = 1;
	Dest.Type = pMyButtonsType;
	return TRUE;
}

PLUGIN_API VOID OnCleanUI(VOID) 
{
	if (KnightlyMyButtons::boolPluginSuccess) {
		DestroyButtonWindow();
	}
} 

PLUGIN_API VOID SetGameState(DWORD GameState) 
{ 
	if (KnightlyMyButtons::boolPluginSuccess) {
		if (GameState == GAMESTATE_INGAME && !MyBtnWnd)
		{
			if (pSidlMgr->FindScreenPieceTemplate("MQMBButtonWnd"))
				CreateButtonWindow();
		}
	}
} 

PLUGIN_API VOID OnReloadUI() 
{ 
	if (KnightlyMyButtons::boolPluginSuccess && gGameState == GAMESTATE_INGAME && pCharSpawn) {
		CreateButtonWindow();
	}
} 

PLUGIN_API VOID OnPulse(VOID) 
{ 
	if (KnightlyMyButtons::boolPluginSuccess){
		if (gGameState==GAMESTATE_INGAME && KnightlyMyButtons::boolShowWindow && ( !MyBtnWnd || (MyBtnWnd && !(MyBtnWnd->dShow)))) 
		{    
			CreateButtonWindow(); 
			((CXWnd*)MyBtnWnd)->Show(1,1); 
		} 
	   
		if ( gGameState==GAMESTATE_INGAME && !KnightlyMyButtons::boolShowWindow ) 
		{ 
			CreateButtonWindow(); 
			((CXWnd*)MyBtnWnd)->Show(0,0); 
		} 
	}
} 


PLUGIN_API VOID InitializePlugin(VOID) 
{ 
   DebugSpewAlways("Initializing MQ2MyButtons");
   if (KnightlyMyButtons::File::LoadButtonData()) {
	   if (KnightlyMyButtons::File::CheckAndCreateXMLFile("MQUI_MyButtonsWnd.xml")) {
			AddXMLFile("MQUI_MyButtonsWnd.xml");
			AddCommand("/mybuttons", MyButtonsCommand);
			pMyButtonsType = new MQ2MyButtonsType;
			AddMQ2Data("MyButtons", MQ2MyBtnData);
			KnightlyMyButtons::boolPluginSuccess = true;
	   }
	   else {
		   KnightlyMyButtons::Log::Error("Could not open MQUI_MyButtonsWnd.xml. Plugin should be unloaded.");
	   }
   }
   else {
	   KnightlyMyButtons::Log::Error("Could not load INI. Plugin should be unloaded.");
   }
} 

// Called once, when the plugin is to shutdown 
PLUGIN_API VOID ShutdownPlugin(VOID) 
{ 
	DebugSpewAlways("Shutting down MQ2MyButtons"); 
	if (KnightlyMyButtons::boolPluginSuccess) {
		DestroyButtonWindow();
		RemoveCommand("/mybuttons");
		RemoveMQ2Data("MyButtons");
		RemoveXMLFile("MQUI_MyButtonsWnd.xml");
		delete pMyButtonsType;
	}
} 

void ReadWindowINI(PCSIDLWND pWindow) 
{ 
   CHAR Buffer[MAX_STRING] = {0};
   pWindow->Location.top      = GetPrivateProfileInt("Location", "Top", 666, INIFileName);
   pWindow->Location.bottom   = GetPrivateProfileInt("Location", "Bottom", 718, INIFileName);
   pWindow->Location.left     = GetPrivateProfileInt("Location", "Left", 18, INIFileName);
   pWindow->Location.right    = GetPrivateProfileInt("Location", "Right", 531, INIFileName);
   pWindow->Locked            = GetPrivateProfileInt("UISettings","Locked",         0,INIFileName);
   pWindow->Fades             = GetPrivateProfileInt("UISettings","Fades",         0,INIFileName); 
   pWindow->FadeDelay		  = GetPrivateProfileInt("UISettings","Delay",         2000,INIFileName); 
   pWindow->FadeDuration      = GetPrivateProfileInt("UISettings","Duration",      500,INIFileName); 
   pWindow->Alpha             = GetPrivateProfileInt("UISettings","Alpha",         255,INIFileName); 
   pWindow->FadeToAlpha       = GetPrivateProfileInt("UISettings","FadeToAlpha",   255,INIFileName); 
   pWindow->BGType            = GetPrivateProfileInt("UISettings","BGType",         1,INIFileName); 
   ARGBCOLOR argb;
   argb.A					  = GetPrivateProfileInt("UISettings","BGTint.alpha",      255,INIFileName); 
   argb.R					  = GetPrivateProfileInt("UISettings","BGTint.red",      255,INIFileName); 
   argb.G					  = GetPrivateProfileInt("UISettings","BGTint.green",      255,INIFileName); 
   argb.B					  = GetPrivateProfileInt("UISettings","BGTint.blue",      255,INIFileName); 
   pWindow->BGColor			  = argb.ARGB;

   GetPrivateProfileString("UISettings","WindowTitle","MQ2 MyButton Window",Buffer,MAX_STRING,INIFileName); 
   KnightlyMyButtons::boolShowWindow               = 0x00000001 & GetPrivateProfileInt("Settings","ShowWindow",   1,INIFileName);
   SetCXStr(&pWindow->WindowText,Buffer); 
   // I am absolutely sure there's a better way to do this, but I'm tired of working on this.
   // and the window won't frickin follow the frame.  I hate XML, I hate UI/UX and I'm done!
   // So...here we're just going to toggle it twice which will move the child window for us 
   // and leave it off or on as the user had it :p
   MyButtonsCommand((PSPAWNINFO)pCharSpawn, "");
   MyButtonsCommand((PSPAWNINFO)pCharSpawn, "");
} 
template <unsigned int _Size>LPSTR SafeItoa(int _Value,char(&_Buffer)[_Size], int _Radix)
{
	errno_t err = _itoa_s(_Value, _Buffer, _Radix);
	if (!err) {
		return _Buffer;
	}
	return "";
}
void WriteWindowINI(PCSIDLWND pWindow) 
{ 
   CHAR szTemp[MAX_STRING] = {0}; 
   GetCXStr(pWindow->WindowText, szTemp, MAX_STRING);
   WritePrivateProfileString("UISettings", "WindowTitle", szTemp, INIFileName);
   WritePrivateProfileString("UISettings","Locked",      SafeItoa(pWindow->Locked,         szTemp,10),INIFileName); 
   WritePrivateProfileString("UISettings","Fades",      SafeItoa(pWindow->Fades,        szTemp,10),INIFileName); 
   WritePrivateProfileString("UISettings","Delay",      SafeItoa(pWindow->MouseOver,    szTemp,10),INIFileName); 
   WritePrivateProfileString("UISettings","Duration",      SafeItoa(pWindow->FadeDuration, szTemp,10),INIFileName); 
   WritePrivateProfileString("UISettings","Alpha",      SafeItoa(pWindow->Alpha,        szTemp,10),INIFileName); 
   WritePrivateProfileString("UISettings","FadeToAlpha",  SafeItoa(pWindow->FadeToAlpha,  szTemp,10),INIFileName); 
   WritePrivateProfileString("UISettings","BGType",      SafeItoa(pWindow->BGType,       szTemp,10),INIFileName); 
   ARGBCOLOR argb;
   argb.ARGB = pWindow->BGColor;
   WritePrivateProfileString("UISettings","BGTint.alpha",   SafeItoa(argb.A,    szTemp,10),INIFileName); 
   WritePrivateProfileString("UISettings","BGTint.red",   SafeItoa(argb.R,    szTemp,10),INIFileName); 
   WritePrivateProfileString("UISettings","BGTint.green", SafeItoa(argb.G,    szTemp,10),INIFileName); 
   WritePrivateProfileString("UISettings","BGTint.blue",  SafeItoa(argb.B,    szTemp,10),INIFileName); 

   WritePrivateProfileString("UISettings","ShowWindow",   SafeItoa((int)KnightlyMyButtons::boolShowWindow, szTemp,10),INIFileName);

   WritePrivateProfileString("Location", "Top", SafeItoa(pWindow->Location.top, szTemp, 10), INIFileName);
   WritePrivateProfileString("Location", "Bottom", SafeItoa(pWindow->Location.bottom, szTemp, 10), INIFileName);
   WritePrivateProfileString("Location", "Left", SafeItoa(pWindow->Location.left, szTemp, 10), INIFileName);
   WritePrivateProfileString("Location", "Right", SafeItoa(pWindow->Location.right, szTemp, 10), INIFileName);

   WritePrivateProfileString("Button1", "Label", KnightlyMyButtons::arrMyLabels[1], INIFileName);
   WritePrivateProfileString("Button1", "Command", KnightlyMyButtons::arrMyCommands[1], INIFileName);
   WritePrivateProfileString("Button1", "Red", KnightlyMyButtons::arrMyColors[1][0], INIFileName);
   WritePrivateProfileString("Button1", "Green", KnightlyMyButtons::arrMyColors[1][1], INIFileName);
   WritePrivateProfileString("Button1", "Blue", KnightlyMyButtons::arrMyColors[1][2], INIFileName);
}


void CreateButtonWindow() 
{ 
	DebugSpew("MQ2MyButtons::CreateButtonWindow()");
	if (MyBtnWnd)
	{
		return;
	}
	if (pSidlMgr->FindScreenPieceTemplate("MQMBButtonWnd")) {
		MyBtnWnd = new CHButWnd();
		if (MyBtnWnd)
		{
			ReadWindowINI((PCSIDLWND)MyBtnWnd);
			WriteWindowINI((PCSIDLWND)MyBtnWnd);
		}
	}
} 


void DestroyButtonWindow() 
{ 
   DebugSpewAlways("MQ2MyButtons::DestroyButtonWindow()"); 
   if (MyBtnWnd) 
   {
	  WriteWindowINI((PCSIDLWND)MyBtnWnd); 
	  delete MyBtnWnd; 
	  MyBtnWnd=0; 
   }
} 