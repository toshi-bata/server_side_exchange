# Server side Exchange
This sample illustrates how to create an interactive 3D model Web viewer from uploading a CAD file to visualization.<br>
Server side loads a CAD file using HOOPS Exchange and exports SC model using libconverter of HOOPS Communicator. By using Exchange and libconverter separately instead of HC Converter, import options and advanced features of Exchange which are not supported by Converter can be used.<br>
Farthermore, by keeping imported model with Exchange, Exchange APIs can be called from client side interactively.<br>
This sample was developed for the following article of Tech Soft 3D Forum:<br>
https://forum.techsoft3d.com/t/how-hoops-communicator-can-use-server-side-exchange-processes/1841

## Dependencies
### SDKs in use (version)
* HOOPS Communicator (2024.4.0)
* HOOPS Exchange (2024.4.0)

### Third paty module
* libmicrohttpd (1.0.0)<br>
  https://ftp.gnu.org/gnu/libmicrohttpd/

### Tested server platforms
* Ubuntu server 24.04 LTS AWE EC2 instance
* Windows 11
* MacOS 12.4

## Setup
### Demo folders
&emsp;+ server_side_exchange<br>
&emsp;&emsp;+ ExServer<br>
&emsp;&emsp;&emsp;+ Makefile (for Linux / MacOS)<br>
&emsp;&emsp;&emsp;+ VS2019_ExServer.bat (for Windows)<br>
&emsp;&emsp;+ main.html<br>
&emsp;&emsp;+ js<br>
&emsp;&emsp;&emsp;+ main.js<br>
&emsp;&emsp;+ HttpServer.js<br>
&emsp;&emsp;+ package.json<br>
&emsp;&emsp;+ demo-app (copy demo-app folder here from Communicator_SDK/web_viewer)<br>
&emsp;&emsp;+ hoops-web-viewer.mjs (copy hoops-web-viewer.mjs file here from Communicator_SDK/web_viewer)<br>
&emsp;&emsp;+ engine.esm.js (copy engine.esm.js file here from Communicator_SDK/web_viewer)<br>
&emsp;&emsp;+ engine.esm.wasm (copy engine.esm.wasm file here from Communicator_SDK/web_viewer)<br>

### Build ExServer
ExServer is a server side console application which is using HOOPS Exchange and HC libconverter. <br>
#### Linux / MacOS
1. Unzip HOOPS Exchange SDK and locate a valid hoops_license.h in `includ/` 
2. Unzip HOOPS Communicator SDK and locate a valid hoops_license.h in `authoring/libconverter/include/`
3. Unzip libmicrohttpd and install with root user<br>
    `./configure && make && make install`
4. Edit ExServer/Makefile according to your environment
5. Set library path<br>
  Linux<br>
    `export LD_LIBRARY_PATH=/usr/local/lib:/path/to/HOOPS_Communicator/authoring/converter/bin/linux64`<br>
  MacOS<br>
    `export DYLD_LIBRARY_PATH=/usr/local/lib:/path/to/HOOPS_Communicator/authoring/converter/bin/macos`<br>
6. Build `sudo make`

#### Windows
1. Unzip HOOPS Exchange SDK and locate a valid hoops_license.h in `includ/` 
2. Unzip HOOPS Communicator SDK and locate a valid hoops_license.h in `authoring/libconverter/include/`
3. Unzip libconverter SDK
4. Register the following enviroment valiables<br>
    Name: `EX_SERVER_WORKING_DIR`&emsp;Value: `path¥to¥temp¥folder`<br>
    Name: `SC_MODELS_DIR`&emsp;Value: `path¥to¥sc_models¥folder`<br>
5. Edit ExServer¥VS2019_ExServer.bat accoording to your environment and open ExServer.sln using the bat file
6. Build the solution
     
## Start demo
1. Launch HTTP server with tarminal<br>
    `cd path/to/server_side_exchange`<br>
    `npm install`<br>
    `npm start`<br>
2. Start Communicator SC server (<Communicator SDK>/quick_start/start_server)
3. Set library path<br>
  Linux<br>
    `export LD_LIBRARY_PATH=/usr/local/lib:/path/to/HOOPS_Communicator/authoring/converter/bin/linux64`<br>
  MacOS<br>
    `export DYLD_LIBRARY_PATH=/usr/local/lib:/path/to/HOOPS_Communicator/authoring/converter/bin/macos`<br>
4. Set enviroment valiables<br>
    `export EX_SERVER_WORKING_DIR=/path/to/temp/folder`<br>
    `export SC_MODELS_DIR=/path/to/sc_models/folder`<br>
5. Start ExServer (giving a port number in command line argument)<br>
    Windows: `ExServer 8888`<br>
    Linux/MacOS: `./ExServer 8888`<br>
6. Open the main.html with server's port number (using Chrome)<br>
    `http://localhost:8000/main.html?port=8888`
    

