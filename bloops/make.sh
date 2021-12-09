#!/bin/sh
gcc -shared -o bloops.so -fPIC -I../lua -undefined dynamic_lookup -lbloopsaphone -lportaudio bloops.c
