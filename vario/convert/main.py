#!/usr/bin/python

import sys
import serial
from intelhex import IntelHex
import time
import datetime
import base64


def add8(a, b):
    return (a + b & 0xFF)

page_size = 255

class Hex2BinConv():
    def __init__(self, out):
        self.hex = IntelHex()
        self.out = out
 
        
    def load(self, filename):
        print
        print "Loading application from hex"
        self.hex.loadfile(filename, "hex")
        
        size = self.hex.maxaddr() - self.hex.minaddr()
        print " size: %0.2f KiB (%d B)" % (size/1024, size) 

    
    def conv(self, label):
        done = False
        adr = self.hex.minaddr()
        max_adr = self.hex.maxaddr()

        tmp_file = open("tmp.bin", "wb")
        out_file = open(self.out, "w")

        print "Converting HEX 2 BIN ...", 

        while(adr <= max_adr):
            tmp_file.write(chr(self.hex[adr]))
            adr += 1

        tmp_file.close()
        
        tmp_file = open("tmp.bin", "r")
        
        base64.encode(tmp_file, out_file)
        
        out_file.close()

        print "Done"
        
            
    def batch(self, filename, label):
        start = time.clock()
        
        self.load(filename)
        self.conv(label)
        
        end = time.clock()
        print 
        print "That's all folks! (%.2f seconds)" % (end - start)
            

if (len(sys.argv) < 3 or len(sys.argv) > 4):
    print "Usage %s hex_file output_file [label]" % __file__
    sys.exit(-1)
    
hex = sys.argv[1]
label = ""  
if (len(sys.argv) == 4):
    label = sys.argv[3]
    
if (label == "" or label == "auto"):
    label = datetime.datetime.now().strftime("%Y%m%d%H%M")

f = open("../src/build_number.txt", "r")
if (f):
    number = f.readline() + "_"
    f.close()
else:
    number = ""

out = sys.argv[2] + "skybean_" + number + label + ".ebin"
    

a = Hex2BinConv(out)
a.batch(hex, label)

