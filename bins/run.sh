#!/bin/bash

MACHINE=$(dirname $1)
BIN=$(basename $1)
QEMU=qemu-${MACHINE%%-*}
$QEMU -L /usr/$MACHINE $1
