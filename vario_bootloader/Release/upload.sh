#!/bin/bash

#tail bootloader.hex -n +2 > upload.hex
cat bootloader.hex > upload.hex

#avrdude -p x32e5 -P usb -c avrisp2 -e -U flash:w:upload.hex:i -U fuse1:w:0x00:m -U fuse2:w:0xBF:m -U fuse4:w:0xFF:m -U fuse5:w:0xDC:m
avrdude -p x32e5 -P usb -c avrisp2 -e -U flash:w:upload.hex:i -U fuse1:w:0x00:m -U fuse2:w:0xBF:m -U fuse4:w:0xFF:m -U fuse5:w:0xF8:m

echo "done"

