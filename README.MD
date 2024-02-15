# Server side Exchange
This sample illustrates how to create an interactive 3D model Web viewer from uploading a CAD file to visualization.<br>
Server side loads a CAD file using HOOPS Exchange and exports SC model using libconverter of HOOPS Communicator. By using Exchange and libconverter separately instead of HC Converter, import options and advanced features of Exchange which are not supported by Converter can be used.<br>
Farthermore, by keeping imported model with Exchange, Exchange APIs can be called from client side interactively.<br>

## Dependencies
### SDKs in use (version)
* HOOPS Communicator (2024.1.0)
* HOOPS Exchange (2024.1.0)

### Third paty module
* libmicrohttpd (1.0.0)
  https://ftp.gnu.org/gnu/libmicrohttpd/

### Tested server platforms
* Ubuntu server 20.04 LTS AWE EC2 instance
* Windows 10
* MacOS 12.4

## Setup
### Demo folders
 \+ ...<br>
   \+ server_side_exchange<br>
     \+ ExServer<br>
       \+ Makefile (for Linux / MacOS)<br>
       \+ VS2019_ExServer.bat (for Windows)<br>
     \+ main.html<br>
     \+ js<br>
       \+ main.js<br>
     \+ HttpServer.js<br>
     \+ package.json<br>
   \+ src (copy src folder here from <Communicator SDK>\web_viewer)<br>

### Build ExServer
ExServer is a server side console application which is using HOOPS Exchange and HC libconverter. <br>
1. Unzip HOOPS Exchange SDK and locate a valid hoops_license.h in `includ/` 
2. Unzip HOOPS Communicator SDK and locate a valid hoops_license.h in `authoring/libconverter/include/`

Linux / MacOS<br>
3. Unzip libmicrohttpd and install with root user
    `./configure && make && make install`
4. Edit ExServer/Makefile according to your environment
5. Set library path
  Linux<br>
    `export LD_LIBRARY_PATH=/usr/local/lib:/path/to//HOOPS_Communicator/authoring/converter/bin/linux64`
  MacOS<br>
    `export DYLD_LIBRARY_PATH=/usr/local/lib:/path/to/HOOPS_Communicator/authoring/converter/bin/macos`
6. Set enviroment valiables
    `export EX_SERVER_WORKING_DIR=/path/to/temp/folder`
    `export SC_MODELS_DIR=/path/to/sc_models/folder`
7. Build `sudo make`

Windows<br>
3. Unzip libconverter SDK
4. Register the following enviroment valiables
    Name: EX_SERVER_WORKING_DIR   Value: path¥to¥temp¥folder
    Name: SC_MODELS_DIR           Value: path¥to¥sc_models¥folder
5. Edit ExServer¥VS2019_ExServer.bat accoording to your environment and open ExServer.sln using the bat file
6. Build the solution
     
## Start demo
1. Launch HTTP server with tarminal
    `cd path/to/server_side_exchange`
    `npm install`
    `npm start`
2. Start Communicator SC server (<Communicator SDK>/quick_start/start_server)
3. Start ExServer (giving a port number in command line argument)
    Windows: `ExServer 8888`
    Linux/MacOS: `./ExServer 8888`
4. Open the main.html with server's port number (using Chrome)
    `http://localhost:8000/main.html?port=8888`
    

