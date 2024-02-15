# Server side Exchange
This sample illustrates how to create an interactive 3D model Web viewer from uploading a CAD file to visualization.<br>
Server side loads a CAD file using HOOPS Exchange and exports SC model using libconverter of HOOPS Communicator. By using Exchange and libconverter separately instead of HC Converter, import options and advanced features of Exchange which are not supported by Converter can be used.<br>
Farthermore, by keeping imported model with Exchange, Exchange APIs can be called from client side interactively.<br>

## Dependencies
### SDKs in use (version)
* HOOPS Communicator (2024.1.0)
* HOOPS Exchange (2024.1.0)

### Third paty module
* libmicrohttpd (1.0.0)<br>
  https://ftp.gnu.org/gnu/libmicrohttpd/

### Tested server platforms
* Ubuntu server 20.04 LTS AWE EC2 instance
* Windows 10
* MacOS 12.4

## Setup
### Demo folders
&emsp;\+ ...<br>
&emsp;&emsp;\+ server_side_exchange<br>
&emsp;&emsp;&emsp;\+ ExServer<br>
&emsp;&emsp;&emsp;&emsp;\+ Makefile (for Linux / MacOS)<br>
&emsp;&emsp;&emsp;&emsp;\+ VS2019_ExServer.bat (for Windows)<br>
&emsp;&emsp;&emsp;\+ main.html<br>
&emsp;&emsp;&emsp;\+ js<br>
&emsp;&emsp;&emsp;&emsp;\+ main.js<br>
&emsp;&emsp;&emsp;\+ HttpServer.js<br>
&emsp;&emsp;&emsp;\+ package.json<br>
&emsp;&emsp;\+ src (copy src folder here from <Communicator SDK>\web_viewer)<br>

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
    `export LD_LIBRARY_PATH=/usr/local/lib:/path/to//HOOPS_Communicator/authoring/converter/bin/linux64`<br>
  MacOS<br>
    `export DYLD_LIBRARY_PATH=/usr/local/lib:/path/to/HOOPS_Communicator/authoring/converter/bin/macos`<br>
6. Set enviroment valiables
    `export EX_SERVER_WORKING_DIR=/path/to/temp/folder`
    `export SC_MODELS_DIR=/path/to/sc_models/folder`
7. Build `sudo make`

#### Windows
1. Unzip HOOPS Exchange SDK and locate a valid hoops_license.h in `includ/` 
2. Unzip HOOPS Communicator SDK and locate a valid hoops_license.h in `authoring/libconverter/include/`
3. Unzip libconverter SDK
4. Register the following enviroment valiables<br>
    Name: EX_SERVER_WORKING_DIR   Value: path¥to¥temp¥folder<br>
    Name: SC_MODELS_DIR           Value: path¥to¥sc_models¥folder<br>
5. Edit ExServer¥VS2019_ExServer.bat accoording to your environment and open ExServer.sln using the bat file
6. Build the solution
     
## Start demo
1. Launch HTTP server with tarminal<br>
    `cd path/to/server_side_exchange`<br>
    `npm install`<br>
    `npm start`<br>
2. Start Communicator SC server (<Communicator SDK>/quick_start/start_server)
3. Start ExServer (giving a port number in command line argument)<br>
    Windows: `ExServer 8888`<br>
    Linux/MacOS: `./ExServer 8888`<br>
4. Open the main.html with server's port number (using Chrome)<br>
    `http://localhost:8000/main.html?port=8888`
    

