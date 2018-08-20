
# About

This tools runs and sniff's dbus messages which are sent from media players to the gnome shell desktop audio controls. From this is can extract the artist, song names. This means it can rip music from most audio players as well as prevent recording adverts from such media players since this information if often left blank.

It works for what I need it to do. Please raise issues / send patch's if you have problems. But for now I won't be supporting an enviroment other than gnome

# Compiling

This should compile on ubuntu 18.04 and probably many others.

Just run the following

./autogen.sh && make

# Usage

It supports --help

Usage: ./src/srip <options>

 -h --help           Print this help
 -d --debug          Switch on debug level logging
 -V --version        Print version and exit

    --output-wav         Output to wav file
    --output-lame        Output to mp3 file using lame encoder
    --output-scopebasic  Output to a basic scope (graphics)

One of more output options can be used at the same time. Outputs that produce files will store the output files in the current working directory.

At some point you will also need to open pauvcontrol and change the pulse audio recording configuration (under the record tab) so that it can monitor the audio output audio. This step should be automated by the program but I have not implemented that yet

Note: Nothing will be output until it detects the start of a new songs.



