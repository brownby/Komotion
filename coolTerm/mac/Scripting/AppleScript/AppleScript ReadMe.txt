AppleScript Support in CoolTerm 1.8.0============================================================================Version 1.8.0 of CoolTerm includes the following AppleScript Commands:Window/App commands
-------------------
NewWindow
LoadSetting
SaveSetting
WindowCount
WindowID
WindowIDfromName
WindowName
IndexOfWindowID
CloseWindow
Quit
CoolTermVersion
ShowWindow
Print
GetFrontmostWindow
PauseDisplay

Serial Port commands
--------------------
Connect
Disconnect
IsConnected
LastErrorCode

Data exchange commands
----------------------
Write
WriteLine
WriteHex
BytesLeftToSend
Poll
Read
ReadAll
ReadHex
ReadAllHex
BytesAvailable
LookAhead
LookAheadHex
ClearBuffer

Serial commands
---------------
SendBreak
FlushPort
ResetPort
GetDTR
SetDTR
GetRTS
SetRTS
GetCTS
GetDSR
GetDCD
GetRI

Text data exchange
------------------
SendTextFile
CaptureStart
CapturePause
CaptureResume
CaptureStop

Connection Setting Commands
---------------------------
RescanSerialPorts
SerialPortCount
SerialPortName
GetCurrentSerialPort
SetCurrentSerialPort
GetParameter
SetParameter
GetAllParameters

AppleScript for CoolTerm supports multiple terminal windows, i.e. commandsthat interact with terminal windows need to be addressed to the intendedwindow by passing the ID of the window as the first command argument.The syntax, arguments, and return parameters for each command are documentedbelow.============================================================================
Window/App commands
============================================================================
----------------------------------------------------------------------------NewWindow as integer----------------------------------------------------------------------------Opens a new window and returns the ID if it was successful and -1 ifotherwise.E.g.:	set WinID to NewWindow----------------------------------------------------------------------------LoadSetting(FilePath as String) as Integer----------------------------------------------------------------------------Loads the connection settings file with the specified path. Just the filename can be passed if the settings file resides the same folder as theCoolTerm application. Otherwise, a full path must be passed to LoadSetting.Returns the ID of the window (an integer which is >= 0) if the connectionsettings file was loaded successfully and -1 if loading was not successful.E.g.:	set WinID to LoadSetting ("Macintosh HD:MySettings.stc") 	if WinID >= 0 then		display alert "The settings have been loaded successfully."	else		display alert "The settings could not be loaded."	end if----------------------------------------------------------------------------SaveSetting(WinID as integer, FilePath as String) as Boolean----------------------------------------------------------------------------Saves the connection settings at the specified path. Just the filename can be passed to save the settings to the same folder as theCoolTerm application. Otherwise, a full path must be passed to SaveSetting.Returns true if saving was successful.E.g.:	if SaveSetting {WinID, "SavedSettings.stc"} then		display alert ("Settings saved successfully")	else		display alert ("Settings could not be saved")	end if----------------------------------------------------------------------------WindowCount as Integer----------------------------------------------------------------------------Returns the number of terminal windows.E.g.:	set n to WindowCount----------------------------------------------------------------------------WindowID(Index as integer) as integer----------------------------------------------------------------------------Returns the ID of the window with the specified index.Returns -1 if the window doesn't exist.E.g.:	# Display the IDs of all terminal windows	repeat with i from 0 to n - 1		display alert (WindowID (i))	end repeatE.g.:	# Get the ID of the first terminal Window	Set WinID to WindowID (0)----------------------------------------------------------------------------WindowIDfromName(WindowName as String) as integer----------------------------------------------------------------------------Returns the ID of the window with the specified name.
Returns -1 if the window doesn't exist.E.g.:	# Get the ID of the terminal Window named "Untitled_0".	Set WinID to WindowIDfromName ("Untitled_0")----------------------------------------------------------------------------WindowName(Index as integer) as string----------------------------------------------------------------------------Returns the name of the window with the specified index.E.g.:	# Display the names of all terminal windows	repeat with i from 0 to n - 1		display alert (WindowName (i))	end repeat----------------------------------------------------------------------------IndexOfWindowID(ID as integer) as integer----------------------------------------------------------------------------Returns the index of the window with the specified IDE.g.:	i = IndexOfWindow (WinID)----------------------------------------------------------------------------CloseWindow(ID as integer)----------------------------------------------------------------------------Closes the terminal window with the specified IDE.g.:	CloseWindow (WinID)----------------------------------------------------------------------------Quit----------------------------------------------------------------------------Quits CoolTerm.E.g.:	Quit----------------------------------------------------------------------------CoolTermVersion as String----------------------------------------------------------------------------Returns the version of the currently running CoolTerm build.E.g.:	display alert (CoolTermVersion)----------------------------------------------------------------------------ShowWindow(ID as integer)----------------------------------------------------------------------------Brings the window with the specified ID to the foreground.E.g.:	ShowWindow (ID)----------------------------------------------------------------------------Print(ID as integer)----------------------------------------------------------------------------Prints the contents of window with the specified ID.E.g.:	Print (ID)----------------------------------------------------------------------------GetFrontmostWindow as integer
----------------------------------------------------------------------------Returns the ID of the frontmost terminal window. Returns -1 if there are no open or visible windows.E.g.:	ID = GetFrontmostWindow


----------------------------------------------------------------------------PauseDisplay(ID as integer, Value as boolean)
----------------------------------------------------------------------------Pauses or unpauses the display of the specified window.

E.g.:   PauseDisplay (ID, true)============================================================================
Serial Port commands
============================================================================
----------------------------------------------------------------------------Connect(ID as integer) as Boolean----------------------------------------------------------------------------Opens the serial port associated with the terminal window with the specifiedID.Returns TRUE if the port was successfully opened.E.g.:	if Connect (WinID) then		display alert "The port is open."	else		display alert "The port could not be opened."	end if----------------------------------------------------------------------------Disconnect(ID as integer)----------------------------------------------------------------------------Closes the serial port associated with the terminal window with thespecified ID.E.g.:	Disconnect (ID)----------------------------------------------------------------------------IsConnected(ID as integer) as Boolean----------------------------------------------------------------------------Checks the connection status of the serial port.Returns TRUE if the port is connected (i.e. open), returns FALSE if the portis not connected.E.g.:	if IsConnected (WinID) then		display alert "The port is open."	else		display alert "The port is not open."	end if----------------------------------------------------------------------------LastErrorCode(ID as integer) as integer----------------------------------------------------------------------------Returns the code of the last serial port error that occurred in thespecified window.E.g.:	local e	set e to LastErrorCode(WinID)============================================================================
Data exchange commands
============================================================================
----------------------------------------------------------------------------Write(ID as integer, Data as String)----------------------------------------------------------------------------Writes data to the serial port associated with the terminal window with thespecified ID.E.g.:	Write {WinID, "Hello CoolTerm"}----------------------------------------------------------------------------WriteLine(ID as integer, Data as String)----------------------------------------------------------------------------Writes data terminated by the "Enter Key Emulation" character specified inthe connection settings to the serial port associated with the terminalwindow with the specified ID.E.g.:	Write {WinID, "Writing just one line."}----------------------------------------------------------------------------WriteHex(ID as integer, HexData as String)----------------------------------------------------------------------------Writes hex data to the serial port associated with the terminal window with
the specified ID.E.g.:	WriteHex {WinID, "48 65 6C 6C 6F 20 43 6F 6F 6C 54 65 72 6D"}----------------------------------------------------------------------------BytesLeftToSend(ID as integer) as integer----------------------------------------------------------------------------Returns the number of bytes that have not yet left the transmit buffer afterstarting a transmission with Write, WriteLine, or SendTextFile.E.g.:	set b to BytesLeftToSend (WinID)----------------------------------------------------------------------------Poll(ID as integer)----------------------------------------------------------------------------Polls the serial port associated with the terminal window with the specifiedID and causes all data currently available in the serial port receivebuffer to be transferred to CoolTerm's receive buffer. It is recommended tocall this method before calling Read, ReadAll, LookAhead, and BytesAvailable.E.g.:	Poll (WinID)----------------------------------------------------------------------------Read(ID as integer, NumChars as Integer) as String----------------------------------------------------------------------------Reads and removes the specified number of characters from the receive bufferof the terminal window with the specified ID.E.g.:	local txt	set txt to Read {WinID, 5}----------------------------------------------------------------------------ReadAll(ID as integer) as String----------------------------------------------------------------------------Reads and removes all characters from the receive buffer of the terminalwindow with the specified ID.E.g.:	local txt	set txt to ReadAll (WinID)----------------------------------------------------------------------------ReadHex(ID as integer, NumChars as Integer) as String----------------------------------------------------------------------------Reads and removes the specified number of characters in Hex format from the
receive buffer of the terminal window with the specified ID.E.g.:	local txtHex	set txtHex to ReadHex {WinID, 5}----------------------------------------------------------------------------ReadAllHex(ID as integer) as String----------------------------------------------------------------------------Reads and removes all characters in Hex format from the receive buffer of the
terminal window with the specified ID.E.g.:	local txtHex	set txtHex to ReadAllHex (WinID)----------------------------------------------------------------------------BytesAvailable(ID as integer) as Integer----------------------------------------------------------------------------Returns the number of characters currently available in the receive bufferof the terminal window with the specified ID.E.g.:	local i	set i to BytesAvailable (WinID)----------------------------------------------------------------------------LookAhead(ID as integer) as String----------------------------------------------------------------------------Returns the contents of the receive buffer of the terminal window with thespecified ID without removing any data.E.g.:	local txt	set txt to LookAhead (WinID)----------------------------------------------------------------------------LookAheadHex(ID as integer) as String----------------------------------------------------------------------------Returns in Hex format the contents of the receive buffer of the terminal window 
with the specified ID without removing any data.E.g.:	local txt	set txt to LookAheadHex (WinID)----------------------------------------------------------------------------ClearBuffer(ID as integer)----------------------------------------------------------------------------Clears receive buffer of the terminal window with the specified ID.E.g.:	ClearBuffer (WinID)============================================================================
Serial Commands
============================================================================
----------------------------------------------------------------------------SendBreak(ID as integer)----------------------------------------------------------------------------Sends the break signal from the serial port in the specified window.E.g.:	Break(WinID)----------------------------------------------------------------------------FlushPort(ID as integer)----------------------------------------------------------------------------Flushes the serial port in the specified window.E.g.:	FlushPort(WinID)----------------------------------------------------------------------------ResetPort(ID as integer)----------------------------------------------------------------------------Resets the serial port in the specified window.E.g.:	ResetPort(WinID)----------------------------------------------------------------------------GetDTR(ID as integer) as boolean----------------------------------------------------------------------------Reads the value of the DTR status signal in the specified window.E.g.:	local v	set v to GetDTR(WinID)----------------------------------------------------------------------------SetDTR(ID as integer, value as boolean)----------------------------------------------------------------------------Sets the DTR status signal to the specified value.E.g.:	SetDTR(WinID, true)----------------------------------------------------------------------------GetRTS(ID as integer) as boolean----------------------------------------------------------------------------Reads the value of the RTS status signal in the specified window.E.g.:	local v	set v to GetRTS(WinID)----------------------------------------------------------------------------SetRTS(ID as integer, value as boolean)----------------------------------------------------------------------------Sets the RTS status signal to the specified value.E.g.:	SetRTS(WinID, true)----------------------------------------------------------------------------GetCTS(ID as integer) as boolean----------------------------------------------------------------------------Reads the value of the CTS status signal in the specified window.E.g.:	local v	set v to GetCTS(WinID)----------------------------------------------------------------------------GetDSR(ID as integer) as boolean----------------------------------------------------------------------------Reads the value of the DSR status signal in the specified window.E.g.:	local v	set v to GetDSR(WinID)----------------------------------------------------------------------------GetDCD(ID as integer) as boolean----------------------------------------------------------------------------Reads the value of the DCD status signal in the specified window.E.g.:	local v	set v to GetDCD(WinID)----------------------------------------------------------------------------GetRI(ID as integer) as boolean----------------------------------------------------------------------------Reads the value of the RI status signal in the specified window.E.g.:	local v	set v to GetRI(WinID)============================================================================
Text Data Exchange
============================================================================
----------------------------------------------------------------------------SendTextFile(ID as integer, FilePath as string) as boolean----------------------------------------------------------------------------Sends the specified text file via the serial port in the specified window.Returns false if the file could not be loaded.E.g.:	if SendTextFile(WinID, "Macintosh HD:MyTextFile.txt") then		display alert "Success"	else		display alert "Failed"	end if----------------------------------------------------------------------------CaptureStart(ID as integer, FilePath as string) as boolean----------------------------------------------------------------------------Starts data capture into the specified text file. Returns false if it wasnot possible to create the file in the specified location.E.g.:	if CaptureStart(WinID, "Macintosh HD:MyCaptureFile.txt") then		display alert "Success"	else		display alert "Failed"	end if----------------------------------------------------------------------------CapturePause(ID as integer)----------------------------------------------------------------------------Pauses the current capture.E.g.:	CapturePause(WinID)----------------------------------------------------------------------------CaptureResume(ID as integer)----------------------------------------------------------------------------Resumes the current capture.E.g.:	CaptureResume(WinID)----------------------------------------------------------------------------CaptureStop(ID as integer)----------------------------------------------------------------------------Stops the current capture and closes the capture file.E.g.:	CaptureStop(WinID)


============================================================================
Connection Settings Commands
============================================================================
----------------------------------------------------------------------------RescanSerialPorts----------------------------------------------------------------------------Rescans the available serial port. Useful for when serial ports are added or
removed while CoolTerm is runningE.g.:	RescanSerialPorts


----------------------------------------------------------------------------SerialPortCount----------------------------------------------------------------------------Returns the number of serial ports available on the current system.

E.g.:	set n to SerialPortCount

----------------------------------------------------------------------------SerialPortName(SerialPortIndex as integer) as string----------------------------------------------------------------------------Returns the name of the serial port with the specified index.
Returns an empty string if SerialPortIndex is invalid.

E.g.:	set PortName to SerialPortName(0)	display alert ("Current Port Name: " & (PortName))


----------------------------------------------------------------------------GetCurrentSerialPort(ID) as integer----------------------------------------------------------------------------Returns the index of currently selected serial port for the window with the
specified ID. Returns -1 if getting current serial port index is not successful.

E.g.:	set CurPort to GetCurrentSerialPort (ID)	display alert ("Current Port Index: " & (CurPort))----------------------------------------------------------------------------SetCurrentSerialPort(ID as integer, SerialPortIndex as integer) as boolean----------------------------------------------------------------------------Sets the serial port for the window with the specified ID to the port with
the specified SerialPortIndex. Returns true if setting the port was successful.
Note: the serial port of the window must be closed.

E.g.:	set CurPort to GetCurrentSerialPort (ID)	display alert ("Current Port ID: " & (CurPort))----------------------------------------------------------------------------GetParameter(ID as integer, ParameterName as String) as String----------------------------------------------------------------------------Returns the value of the settings parameter specified by ParameterName, for
the window with the specified ID.
Note: Use GetAllParemeters to get a list of all parameter names and values.

E.g.:	set BaudRate to GetParameter {ID, "BaudRate"}----------------------------------------------------------------------------SetParameter(ID as integer, ParameterName as String, Value as string) as Boolean----------------------------------------------------------------------------Sets the value of the settings parameter specified by ParameterName, for
the window with the specified ID. Returns true when setting was successful.
Note: Use GetAllParemeters to get a list of all parameter names and values.

E.g.:	if not (SetParameter {WinID, "BaudRate", 115200}) then		display alert ("BaudRate could not be set")	end if----------------------------------------------------------------------------GetAllParameters(ID as integer) as String----------------------------------------------------------------------------Returns a list of all parameters (one per line) for the window with the
specified ID

E.g.:	set Params to (GetAllParameters (WinID))