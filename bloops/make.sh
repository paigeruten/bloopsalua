#!/bin/sh
gcc -shared -o lbloops.so -fPIC -I../lua -undefined dynamic_lookup -lbloopsaphone -lportaudio lbloops.c
