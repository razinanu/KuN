#!/bin/bash

BASEURI=$1
WGET="wget -p . -nd"

function moveA {
	l=$1
	r=$2
	$WGET --delete-after "$BASEURI/drive?rofl=lol&left=$l&right=$r&xd"
	sleep 0.5
}

function moveB {
	l=$1
	r=$2
	$WGET --delete-after "$BASEURI/drive?rofl=lol&right=$r&left=$l"
	sleep 0.5
}

function image {
	$WGET "-Oimage.bmp" "$BASEURI/image?rofl=lol&a=b"
	sleep 0.5
}

if [ $# -lt 1 ] ; then
	echo "usage: $0 <base uri>"
	echo "calls robot service, such that the robot should move like he is navigating into a parking space"
	echo "example: $0 http://localhost:8080"
	exit 1
fi

image

# straight, approach barrels
moveA 1.0 1.0
moveA 1.0 1.0
moveA 1.0 1.0
moveA 0.8 0.8
moveA 0.6 0.6

# turn left
moveB 0.4 0.8
moveB 0.1 1.0
moveB 0.1 1.0
moveB 0.1 1.0
moveB 0.3 1.0

# straight, pass parking lot, hold
moveA 0.5 1.0
moveA 1.0 1.0
moveA 1.0 1.0
moveA 1.0 1.0
moveA 1.0 1.0
moveA 1.0 1.0
moveA 0.8 0.8
moveA 0.6 0.6
moveA 0.4 0.4
moveA 0.2 0.2

# reverse, right turn
moveB -0.1 -0.1
moveB -0.4 -0.2
moveB -0.6 -0.2
moveB -0.6 -0.2
moveB -0.4 -0.2

# straight reverse a bit
moveA -0.2 -0.2
moveA -0.4 -0.4
moveA -0.8 -0.8
moveA -0.8 -0.8
moveA -0.4 -0.4
moveA -0.2 -0.2

# reverse, left turn
moveB -0.1 -0.4
moveB -0.2 -0.6
moveB -0.2 -0.6

# stop gently
moveA -0.2 -0.4
moveA -0.1 -0.2
moveA -0.0 -0.1

# straight
moveA 0.2 0.2
moveA 0.4 0.4
moveA 0.6 0.6
moveA 0.4 0.4
moveA 0.2 0.2
