// MQ2MyButtons.cpp : Defines the entry point for the DLL application.
//
// From Knightly:  I don't know who originally wrote this, I stole the source
// and updated it to fix it.  But this is definitely not my code.  So, don't you
// judge me. (This used to be a link to a My Name is Earl segment where the actors
// say that a bunch, but they took it down so you'll just have to watch the show.)

#include <mq/Plugin.h>

#include <fstream>

PreSetup("MQ2MyButtons");
PLUGIN_VERSION(2020.0604);

PLUGIN_API void MyButtonsCommand(SPAWNINFO* pSpawn, char* szLine);

namespace KnightlyMyButtons {
	bool boolDebug = false;
	int iMaxButtons = 12;
	const std::string xmlVersion = "2020-05-23";
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
				strMessage = "\ay[\agMQ2MyButtons\ay]\aw ::: \ao" + strMessage;
				WriteChatf(strMessage.c_str());
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
				for (int i = 1; i <= iMaxButtons; i++) {
					if (!(arrMyCommands[i][0] == 0)) {
						std::string strBuffer;
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
			static bool CheckAndCreateXMLFile(std::string_view strFileName, bool createFile = false) {
				// Assume something went wrong~
				bool returnResult = false;
				std::filesystem::path pathXMLFile = gPathResources / std::filesystem::path("uifiles\\default\\") / strFileName;
				// Check if the file already exists
				if (!std::filesystem::exists(pathXMLFile)) {
					// File doesn't exist
					createFile = true;
				}
				else {
					std::ifstream readPath(pathXMLFile);
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
					// If the parent folder exists or can be created
					if (std::filesystem::exists(pathXMLFile.parent_path()) || std::filesystem::create_directories(pathXMLFile.parent_path())) {
						// Open the file for writing
						std::ofstream writePath(pathXMLFile);
						// If we have a write handle (ie, we can write to it)...
						if (writePath) {
							if (strFileName == "MQUI_MyButtonsWnd.xml") {
								// TODO: Make this a resource
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
								for (int j = 1; j <= iMaxButtons; j++) {
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
									//writePath << "\t\t<EQType>9999</EQType>" << std::endl;
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
				}
				return returnResult;
			}

			static bool LoadButtonData() {
				for (int i = 1; i <= iMaxButtons; i++) {
					char szButtonSectionTmp[20] = "Button";
					strcat_s(szButtonSectionTmp, std::to_string(i).c_str());
					// Get the label into the arrMyLabels array at the position of the button number
					GetPrivateProfileString(szButtonSectionTmp, "Label", nullptr, arrMyLabels[i], MAX_STRING, INIFileName);
					// Do the same with the Command
					GetPrivateProfileString(szButtonSectionTmp, "Command", nullptr, arrMyCommands[i], MAX_STRING, INIFileName);
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
		// FIXME:  This needs to be dynamic and initialized
		CButtonWnd* MyButton[13];

		CHButWnd() : CCustomWnd("MQMBButtonWnd")
		{
			for (int i = 1; i <= KnightlyMyButtons::iMaxButtons; ++i) {
				const std::string strButton = "MQMB_Button" + std::to_string(i);
				MyButton[i] = (CButtonWnd*)GetChildItem(strButton.c_str());
			}
		}

		~CHButWnd()
		{
		}

		// FIXME:  This seems inefficient and will only get worse as the amount of buttons increases
		int WndNotification(CXWnd *pWnd, uint32_t uiMessage, void *pData) override
		{
			for (int i = 1; i <= KnightlyMyButtons::iMaxButtons; ++i) {
				if (pWnd == (CXWnd*)MyButton[i]) {
					if (uiMessage == XWM_LCLICK) {
						MyButtonsCommand(pCharSpawn, &std::to_string(i)[0]);
						break;
					}
				}
			}
			return 0;
		}

		void SetLabel(int iButton)
		{
			if (MyBtnWnd && KnightlyMyButtons::arrMyLabels[iButton][0] != '\0') {
				const std::string buttonName = "MQMB_Label" + std::to_string(iButton);
				if (CXWnd* button = MyBtnWnd->GetChildItem(buttonName.c_str()))
				{
					char buffer[MAX_STRING] = { 0 };
					strcpy_s(buffer, KnightlyMyButtons::arrMyLabels[iButton]);
					// TODO:  Add a check for the button to see if it NEEDS to be parsed and only parse buttons that need updating
					ParseMacroData(buffer, MAX_STRING);
					button->SetWindowTextA(buffer);
				}
			}
		}

		void SetButtonInfo()
		{
			for (int i = 1; i <= KnightlyMyButtons::iMaxButtons; ++i) {
				const std::string label = "MQMB_Label" + std::to_string(i);
				if (CXWnd* button = MyBtnWnd->GetChildItem(label.c_str())) {
					SetLabel(i);
					ARGBCOLOR buttonColor{};
					// Alpha matches the window containing the button so that the label fades with the button.
					buttonColor.A = MyBtnWnd->GetAlpha();
					buttonColor.R = GetIntFromString(KnightlyMyButtons::arrMyColors[i][0], 255);
					buttonColor.G = GetIntFromString(KnightlyMyButtons::arrMyColors[i][1], 255);
					buttonColor.B = GetIntFromString(KnightlyMyButtons::arrMyColors[i][2], 255);
					button->SetCRNormal(buttonColor.ARGB);
				}
				// Tooltip of the button is the command on the button
				if (MyButton[i] && KnightlyMyButtons::arrMyCommands[i]) MyButton[i]->SetTooltip(KnightlyMyButtons::arrMyCommands[i]);
			}
		};
};

PLUGIN_API void MyButtonsCommand(SPAWNINFO* pSpawn, char* szLine)
{
	CHAR szParam1[MAX_STRING] = { 0 };
	GetArg(szParam1, szLine, 1, 0);
	// If the first parameter empty then toggle the window
	if (strlen(szParam1) == 0) {
		if (MyBtnWnd) {
			MyBtnWnd->Show(!MyBtnWnd->IsVisible());
		}
	}
	// Otherwise if the first Parameter is "help" or "?"
	else if (ci_equals(szParam1, "help") || !strcmp(szParam1, "?")) {
		KnightlyMyButtons::Log::ShowHelp();
	}
	// Otherwise if the first parameter is "on"
	else if (ci_equals(szParam1, "on")) {
		if (MyBtnWnd) {
			MyBtnWnd->Show(true);
		}
	}
	// Otherwise if the first parameter is "off"
	else if (ci_equals(szParam1, "off")) {
		if (MyBtnWnd) {
			MyBtnWnd->Show(false);
		}
	}
	else if (ci_equals(szParam1, "reload")) {
		KnightlyMyButtons::Log::Message("Reloading hotkeys from INI...");
		if (KnightlyMyButtons::File::LoadButtonData()) {
			if (MyBtnWnd) {
				MyBtnWnd->SetButtonInfo();
			}
			KnightlyMyButtons::Log::Message("...Success");
		}
		else {
			KnightlyMyButtons::Log::Error("...Failed");
		}
	}
	// Otherwise if the first parameter is "show"
	else if (ci_equals(szParam1, "show")) {
		KnightlyMyButtons::Log::ShowButtons();
	}
	else {
		int i = GetIntFromString(szParam1, 0);
		// Otherwise if the first parameter is a number between 1 & iMaxButtons
		if (i > 0 && i <= KnightlyMyButtons::iMaxButtons) {
			if (KnightlyMyButtons::arrMyCommands[i] != nullptr) {
				DoCommand(pSpawn, KnightlyMyButtons::arrMyCommands[i]);
			}
			else {
				KnightlyMyButtons::Log::Error("No Command Set for Button: " + std::string(szParam1));
			}
		}
		else {
			KnightlyMyButtons::Log::Error("Invalid button command: " + std::string(szParam1));
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
			CMD
		};

		MQ2MyButtonsType() : MQ2Type("MyButtons") {
			TypeMember(Label);
			AddMember(Label, "label");

			TypeMember(CMD);
			AddMember(CMD, "cmd");
			AddMember(CMD, "Cmd");
		}

		virtual bool GetMember(MQVarPtr VarPtr, const char* Member, char* Index, MQTypeVar& Dest) override {
			_szBuffer[0] = '\0';

			auto pMember = MQ2MyButtonsType::FindMember(Member);
			if (!pMember) return false;

			// Validate the argument is between 1 and iMaxNumber
			int i = GetIntFromString(Index , 0);
			if (i > 0 && i <= KnightlyMyButtons::iMaxButtons) {
				switch ((Members)pMember->ID) {
					case Label:
						strcpy_s(_szBuffer, KnightlyMyButtons::arrMyLabels[i]);
						break;
					case CMD:
						strcpy_s(_szBuffer, KnightlyMyButtons::arrMyCommands[i]);
						break;
					default:
						return false;
				}
			}
			else {
				strcpy_s(_szBuffer, "InvalidButton");
			}

			Dest.Type = mq::datatypes::pStringType;
			Dest.Ptr = &_szBuffer[0];
			return true;
		}

		bool FromData(MQVarPtr& VarPtr, MQTypeVar& Source) { return false; }
		virtual bool FromString(MQVarPtr& VarPtr, const char* Source) override { return false; }
};

void ReadWindowINI(CSidlScreenWnd* pWindow)
{
	pWindow->SetLocation({ GetPrivateProfileInt("Location", "Left", 18, INIFileName),
	                       GetPrivateProfileInt("Location", "Top", 666, INIFileName),
	                       GetPrivateProfileInt("Location", "Right", 531, INIFileName),
	                       GetPrivateProfileInt("Location", "Bottom", 718, INIFileName) });
	pWindow->SetLocked(GetPrivateProfileBool("UISettings", "Locked", false, INIFileName));
	pWindow->SetFades(GetPrivateProfileBool("UISettings", "Fades", false, INIFileName));
	pWindow->SetFadeDelay(GetPrivateProfileInt("UISettings", "Delay", 2000, INIFileName));
	pWindow->SetFadeDuration(GetPrivateProfileInt("UISettings", "Duration", 500, INIFileName));
	pWindow->SetAlpha(GetPrivateProfileInt("UISettings", "Alpha", 255, INIFileName));
	pWindow->SetFadeToAlpha(GetPrivateProfileInt("UISettings", "FadeToAlpha", 255, INIFileName));
	pWindow->SetBGType(GetPrivateProfileInt("UISettings", "BGType", 1,INIFileName));
	ARGBCOLOR argb{};
	argb.A = GetPrivateProfileInt("UISettings","BGTint.alpha",      255,INIFileName);
	argb.R = GetPrivateProfileInt("UISettings","BGTint.red",      255,INIFileName);
	argb.G = GetPrivateProfileInt("UISettings","BGTint.green",      255,INIFileName);
	argb.B = GetPrivateProfileInt("UISettings","BGTint.blue",      255,INIFileName);
	pWindow->SetBGColor(argb.ARGB);
	pWindow->SetWindowText(&GetPrivateProfileString("UISettings","WindowTitle","MQ2 MyButton Window", INIFileName)[0]);
	pWindow->Show(GetPrivateProfileBool("UISettings", "ShowWindow", true, INIFileName));
	pWindow->UpdateLayout();
	if (pWindow->bFullyScreenClipped)
	{
		WriteChatf("Mybuttons is off screen.");
	}
	
}

void WriteWindowINI(CSidlScreenWnd* pWindow)
{
   WritePrivateProfileString("UISettings", "WindowTitle", pWindow->GetWindowText().c_str(), INIFileName);
   WritePrivateProfileString("UISettings", "Locked", std::to_string(pWindow->IsLocked()), INIFileName);
   WritePrivateProfileString("UISettings", "Fades", std::to_string(pWindow->GetFades()), INIFileName);
   WritePrivateProfileString("UISettings", "Delay", std::to_string(pWindow->GetFadeDelay()), INIFileName);
   WritePrivateProfileString("UISettings", "Duration", std::to_string(pWindow->GetFadeDuration()), INIFileName);
   WritePrivateProfileString("UISettings", "Alpha", std::to_string(pWindow->GetAlpha()), INIFileName);
   WritePrivateProfileString("UISettings", "FadeToAlpha", std::to_string(pWindow->GetFadeToAlpha()), INIFileName);
   WritePrivateProfileString("UISettings", "BGType", std::to_string(pWindow->GetBGType()), INIFileName);
   ARGBCOLOR argb{};
   argb.ARGB = pWindow->GetBGColor();
   WritePrivateProfileString("UISettings","BGTint.alpha", std::to_string(argb.A), INIFileName);
   WritePrivateProfileString("UISettings","BGTint.red", std::to_string(argb.R), INIFileName);
   WritePrivateProfileString("UISettings","BGTint.green", std::to_string(argb.G), INIFileName);
   WritePrivateProfileString("UISettings","BGTint.blue", std::to_string(argb.B), INIFileName);

   WritePrivateProfileString("UISettings","ShowWindow",   std::to_string(pWindow->IsVisible()), INIFileName);

   WritePrivateProfileString("Location", "Top", std::to_string(pWindow->GetLocation().top), INIFileName);
   WritePrivateProfileString("Location", "Bottom", std::to_string(pWindow->GetLocation().bottom), INIFileName);
   WritePrivateProfileString("Location", "Left", std::to_string(pWindow->GetLocation().left), INIFileName);
   WritePrivateProfileString("Location", "Right", std::to_string(pWindow->GetLocation().right), INIFileName);

   WritePrivateProfileString("Button1", "Label", KnightlyMyButtons::arrMyLabels[1], INIFileName);
   WritePrivateProfileString("Button1", "Command", KnightlyMyButtons::arrMyCommands[1], INIFileName);
   WritePrivateProfileString("Button1", "Red", KnightlyMyButtons::arrMyColors[1][0], INIFileName);
   WritePrivateProfileString("Button1", "Green", KnightlyMyButtons::arrMyColors[1][1], INIFileName);
   WritePrivateProfileString("Button1", "Blue", KnightlyMyButtons::arrMyColors[1][2], INIFileName);
}

void DestroyButtonWindow()
{
   DebugSpewAlways("MQ2MyButtons::DestroyButtonWindow()");
   if (MyBtnWnd)
   {
	  WriteWindowINI(MyBtnWnd);
	  delete MyBtnWnd;
	  MyBtnWnd = nullptr;
   }
}

bool MQ2MyBtnData(const char* szIndex, MQTypeVar& Dest)
{
	Dest.DWord = 1;
	Dest.Type = pMyButtonsType;
	return true;
}

PLUGIN_API void OnCleanUI()
{
	if (MyBtnWnd) {
		DestroyButtonWindow();
	}
}

PLUGIN_API void OnPulse()
{
	if (GetGameState() == GAMESTATE_INGAME) {
		if (MyBtnWnd == nullptr) {
			if (pSidlMgr->FindScreenPieceTemplate("MQMBButtonWnd")) {
				MyBtnWnd = new CHButWnd();
				if (MyBtnWnd != nullptr) {
					ReadWindowINI(MyBtnWnd);
					MyBtnWnd->SetButtonInfo();
					WriteWindowINI(MyBtnWnd);
				}
			}
		}
		else {
			// Set the labels every 30 seconds (it's a button, not a display window)
			const int SkipMS = 30 * 1000;
			static auto pulseTimer = GetTickCount64() + SkipMS;
			if (GetTickCount64() > pulseTimer) {
				for (int i = 1; i <= KnightlyMyButtons::iMaxButtons; ++i) {
					MyBtnWnd->SetLabel(i);
				}
				pulseTimer = GetTickCount64() + SkipMS;
			}
		}
	}
}


PLUGIN_API void InitializePlugin()
{
   DebugSpewAlways("Initializing MQ2MyButtons");
   if (KnightlyMyButtons::File::LoadButtonData()) {
	   if (KnightlyMyButtons::File::CheckAndCreateXMLFile("MQUI_MyButtonsWnd.xml")) {
			AddXMLFile("MQUI_MyButtonsWnd.xml");
			AddCommand("/mybuttons", MyButtonsCommand);
			pMyButtonsType = new MQ2MyButtonsType;
			AddMQ2Data("MyButtons", MQ2MyBtnData);
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
PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("Shutting down MQ2MyButtons");
	DestroyButtonWindow();
	RemoveCommand("/mybuttons");
	RemoveMQ2Data("MyButtons");
	RemoveXMLFile("MQUI_MyButtonsWnd.xml");
	delete pMyButtonsType;
}