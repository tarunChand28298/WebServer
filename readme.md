# Web Server:
As the title suggests, this is a web server that serves up static web pages and other resources to web browsers or other clients.

## Motivation
This is a hobby project we did as a part of learning networking and socket programming in C++ and WIN32 api for windows.

## Screenshots:
placeholder

## Technologies used:
* C++ (MSVC)
* Win32 API
    * sockets
    * threads
* C++ STL

## Features:
* Serve up static web pages
* serve up media (audio, video, images)
* Serve nested pages in I-Frame
* Checks for unavailable resources
* easy to get up and running with for simple uses
* easy to customize and build up on

# Installation:
1. Clone this repository
2. open the .sln file in visual studio 2019
3. in `main.cpp`, change the` #define CONTENT_BASE_DIR` to the folder which you would like to serve
4. specify the `#define FILE_NOT_FOUND_PATH` file path to show when an unavailable resource is requested
5. Build the solution
6. the built binary is inside the `bin` folder of the project directory

