# EDUCAN-Mini-Software-Public
It consists of the software for EDUCAN-Mini; a CanSat training Kit that is used to gain Hands-on Experience on building and operating CanSat.A CanSat is a simulation of a real satellite, integrated within the volume and shape of a soft drink can. It is a type of Sounding Rocket Payload used to teach space technology.It is based on similar technology used in Miniaturized Satellites.The challenge is to fit all the major subsystems found in a satellite, such as power, sensors and a communication system, into this minimal volume somewhere between 300-350gm. The CanSat is then launched to an altitude of a few hundred meters by a rocket or dropped from a platform or captive balloon and its mission begins: to carry out a scientific experiment and achieve a safe landing. CanSat offer a unique opportunity to have first practical experience of a real space project. They are responsible for all aspects: designing the CanSat, selecting its mission, integrating the components, testing, preparing for launch and then analyzing the data. CanSat is for everyone interested in electronics and Space Technology. 
Our CanSat consist of a payload, ESP8266 board as the On Board Computer as well as Communication and a charging unit for the battery installed within the CanSat.
The web-Server will be used as the Ground-station displaying data,eliminating the Complexities of ground station design and Hardware.

![EDUCAN-MINI](IMAGES/CANSAT.png)

*Fig: Educan-Mini with subsystem*
## IDE Setup
Setup of Arduino IDE for ESP8266/WEMOS (ESP12S) 
1.	Download the Arduino IDE from [ click here ](https://www.arduino.cc/en/software) . Here for example procedure I have downloaded version 1.8.19.
2.	For including the WEMOS/ESP8266 board, you can include the following in preference section. Go to $ File -> Preference $. The preference setting will open up and you will see Additional Board Manager URL option. Go there and paste https://arduino.esp8266.com/stable/package_esp8266com_index.json. 
This will download the board required for ESP8266.
Ensure you will get following in tools section and make sure to have same configuration before uploading code. 

![Board setup ](IMAGES/Board_setup.png)

3.	Upload blink code which now you will get for ESP 8266 in the Examples.
$ File ->Examples -> ESP8266 ->Blink $. 

If the buildin led blinks as per delay in code , Congratulations , you’ve successfully set the board for EDUCAN-MINI. 

## Setup for Little FS
1.	Now for uploading the various file system that had been used for the extraction of sensor data , visualization of data in graphs , we have to use HTML , CSS, and Java Script. For  this to integrate within the arduino as C++ format is nearly impossible and very difficult to debug. Thus the LittleFS is being used . For more understanding about the file system and Little FS along with SPIFFS , you can always visit to site[ Read more ](https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html). For now , we will be conserning on the set up of Little FS for our example code . 

    1. Download the 2.6.0 or later version of the tool from[ click here ]( https://github.com/earlephilhower/arduino-esp8266littlefs-plugin/releases)
    2.	In your Arduino sketchbook directory, create tools directory if it doesn’t exist yet.(you can Know your sketchbook directory by pressing CTRL+k ).
    3.	Unpack the tool into tools directory (the path will look like $ <home_dir>/Arduino/tools/ESP8266FS/tool/esp8266fs.jar $) If you are upgrading, overwrite the existing JAR file with the newer version.
    4.	Restart Arduino IDE.

2.	Now here comes the most critical part , Here you should know that for uploading various file system in via Little FS , the procedure wants you to place all file inside a directory named $ data $. 

    1.	Open the example directory which has been provided to you . the direcory is already in the format that Little FS require. (If you want to create a new code , modify the chart type or any other , format the file exactly same.) There you can see inside data directory, you have a basic HTML file , basic CSS file and a highchart.js source file for graph visualization.
    2.	For uploading the directory called data , after you successfully install LittleFS , you will see following in your tools option. 

    ![ LITTLE-FS ](IMAGES/Little_fs.png)

    3.	To upload a LittleFS filesystem use Tools -> ESP8266 LittleFS Data Upload. Make sure you have selected correct port , board and also any permission (if required ). 

## Setup for arduino sketch file with the libraries and descriptions.

1.	Now we can upload the $ .ino $ File that use the filesystem for executing the tasks. 
    1.	In case of this , you have to add additional libraries for communication of website to controller, use of WiFi card , for extraction of data from payloads .
    2.	You can download library and add zip files or include from Arduino IDE via: 
    *  Sketch -> Include library -> Add .zip library …
  
    * Sketch -> Include library -> Manage library… to open library manager.

    ### Libraries

    1.	For Payloads: 
    * MPU6050 : we  have used wire library.
    * BMP: Adafruit_BMP085 [click here](https://github.com/adafruit/Adafruit-BMP085-Library.git)
    * DallasTemperature for DS18B20 library [click here](dependencies/DS18B20-1.0.0.zip).

    2. ESPAsyncTCP [click here](dependencies/ESPAsyncTCP-master.zip) and ESPAsyncWebServer [click here](dependencies/ESPAsyncWebServer.zip) has been used for communicating the ESP asynchronously with web server.
    
    3. Similarly FS and LittleFS library is used for use of LittleFS to open and execute the file previously uploaded as in procedure mentioned in setup for littleFS. 

2.	This is to clearify that any sensor can be deactivated or activated as per modification of code . by default the POCKETQUBE have 3 sensors as payload along with the EPS providing the battery data. The sensors MPU6050 and BMP is configured to use I2C protocol while DS18B20 is following one wire protocol. 
3.	If any sensor data is not ment to show make sure to modify the function in .ino file along with HTML and CSS file too. 

4.	EPS is capable of accepting the additional power supply to charge the battery. By default it is using USB mini port for charging battery.

## Setup for Host website  

1.	Connect the host to the AP of the EDUCAN-MINI. The OBC is used as the Access Point (AP). Thus no internet is required however the receiving device (host : where the data is to be obtained) is required to have connected to the AP of OBC . 
This can be modified in $ .ino $ file.
default ssid = "Dott" and password = "1234567890"
2.	By default the softAP command set the IP address to *192.168.4.1*
However,this can be set or changed. [ Read more ](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/soft-access-point-class.html)

3. The IP address is being published to serial monitor. 

### PS: during image upload via LittleFS , serial monitor should not be opened.
