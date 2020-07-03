
Linux Example
=============

---
Prerequisites:
- python3
- tty0tty emulator: https://github.com/freemed/tty0tty
---

This is a basic example of an application for Linux. This example will send a
Beacon with telemetry data every 20 seconds, and will accept 4 commands
described as follow:
- Dummy: Sends a dummy message.
- Get Parameter: returns the value of a parameter in the table by the name.
- Set Parameter: set the value of a parameter by the name.
- Reboot: reboots the Computer. Requires sudo permissions. Warning will reboot
  your computer!


This application will host a parameter Table with the following parameters:
- example: An example int parameter, can be modified and is included in
  Beacon with id "A".
- time_stamp: OBC Timestamp parameter example, is included in Beacon with
  id "B".
- beacon_count: Beacon counter, is included in Beacon with id "C".
- beacon_period: Time in ms between Beacons, can be modified, not included in
  Beacon, default to 20 seconds, to modify the default value see sfsf_config.h.

When running the SFSF App following files will be created:
- beacons.txt: Store Telemetry Data.
- log.txt: Store events info.
- params.txt: Store persistent parameters.


Build the Example:
------------------
To build this example simply place the python script "build_example.py" in the
root folder of this repo, alongside waf and wscript files.

Build the source code
~~~
python build_example.py
~~~

After successfully building the example you will see two executables under the
directory 'build':
- sfsf_example: SFSF Example App
- ground_station: Ground Station Example


Run the Example:
----------------
The Linux example is composed by the SFSF App Example and a Ground Station
Example. To achieve communication between both programs in the same machine a
virtual serial port is needed, install
[tty0tty](https://github.com/freemed/tty0tty):

~~~
git clone https://github.com/freemed/tty0tty
~~~
Unzip the repo as 'tty0tty'. An run:
~~~
cd tty0tty/pts
make
~~~

The virtual serial port will be compiled into tty0tty/pts. Move it into another
directory if desired. Run the virtual port and pay attention to its output.
Don't kill the program or close the terminal!
~~~
./tty0tty &
~~~
The output should be like this:
~~~
$ (/dev/pts/2) <=> (/dev/pts/3)
~~~
This means your computer is hosting two intercommunicated serial ports:
"/dev/pts/2" and "/dev/pts/3".

Now run the SFSF App and the Ground Station assigning the virtual serial
ports just created. Run each one in a separated terminal!
~~~
./build/ground_station /dev/pts/2

# In another terminal..
./build/sfsf_example  /dev/pts/3
~~~

If everything goes well, the ground station should receive Beacons from the
SFSF App after some seconds.


Sending Commands to the APP:
----------------------------
Now you can send commands from the Ground Station to the SFSF App:

**1) Dummy command**

Write in ground station terminal:
~~~
dummy
~~~

Reply:
~~~
> Client: Sending message 0 to server...
> Client: Response from server: OK
~~~

**2) Get Parameter**

Write in ground station terminal:
~~~
get example
~~~

Reply:
~~~
> Client: Sending message 1 to server...
> Client: Response from server: example:0
~~~
In this example, we retrieve the value from the parameter with the name
"example", the value can be confirmed from the parameter table printed during
SFSF App initialization.


**3) Set Parameter**

Write in ground station terminal:
~~~
set example,1
~~~

Reply:
~~~
> Client: Sending message 2 to server...
> Client: Response from server: example:ok
~~~
In this example, we set the value from the parameter with the name "example",
the value can be confirmed by performing a "get" command for the "example"
parameter.

A more useful example is to set the value of the "beacon_period" parameter.
This will change the behaivour of the app:

Write in ground station terminal:
~~~
set beacon_period,5000
~~~

Reply:
~~~
> Client: Sending message 3 to server...
> Client: Response from server: beacon_period:ok
~~~
After this, you will note that the time between beacons will be shorted.
**This way we can simplify control the flight software by just modifying
the parameters, instead of writing commands for it.**



---------------------------------
Enjoy ;)
