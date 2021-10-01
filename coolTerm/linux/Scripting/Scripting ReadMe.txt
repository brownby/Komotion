================================================================================
COOLTERM SCRIPTING - READ ME
================================================================================

Applies to CoolTerm v1.7.0 and later.

CoolTerm supports remote control and scripting via the following means:
- CoolTerm Remote Control Socket Protocol
- HTTP Server
- AppleScript (macOS only)


CoolTerm Remote Control Socket Protocol
=======================================
The CoolTerm Remote Control Socket Protocol, based on TCP/IP, allows most aspects of CoolTerm, normally performed via the CoolTerm GUI, to be automated by a separate piece of software (e.g. scripting software). A listening TCP socket embedded in CoolTerm (Remote Control Socket), which is enabled via the Preferences in CoolTerm, can accept connections (default port: 51413) from the same computer on which CoolTerm is running as well as other computers that can make a TCP/IP connection to the computer on which CoolTerm is running. Another application that is connected to the Remote Control Socket can send commands to initiate actions normally performed via the GUI (e.g. open/closing the serial port, reading/writing data, etc.).

Refer the to the included "Remote Control Socket Protocol.pdf" document for detailed specifications.

CoolTerm scripting via Python
-----------------------------
The included Python Module "CoolTerm.py" fully implements the CoolTerm Remote Control Socket Protocol and offers useful Python functions to automate CoolTerm. Refer to the description included in CoolTerm.py as well as the included examples. The CoolTerm.py module can either be installed in the "site-packages" location of your Python installation, or it can be located in the same place as your own python scripts using the module.


HTTP Server
===========
CoolTerm has simple, built-in, HTTP server (default port: 8080) that can be enabled via the Preferences in CoolTerm. Simply direct a web browser to http://127.0.0.1:8080 to be presented with a form that lists buttons for all available commands. The ID (ID as integer) of the CoolTerm window being addressed needs to be specified in the "Window ID" field. All other arguments are entered into the "Data" field in the form. For commands such as "SetParameter" where more than one argument (in addition to the ID) are needed, the arguments (e.g. ParameterName and Value) must be separated by a single space character. Commands are executed by pressing the specific push button. CoolTerm will respond with a new page to show the response.

Instead of using the built-in form, commands can also be sent to CoolTerm from custom HTML pages, using POST. 

Example:  POST /?method=post&WINID=0&DATA=&CMD=Connect


AppleScript
===========
CoolTerm for Mac has supported AppleScript since v1.4.0. Refer to the included "AppleScript ReadMe.txt" document for more information.
Note that CoolTerm v.1.7.0 now offers an option to disable AppleScript via the Preferences dialog.