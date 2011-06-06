#!/bin/bash

APP=VStripe

dylibbundler -od -b -x ./$APP.app/Contents/MacOS/$APP -d ./$APP.app/Contents/libs

