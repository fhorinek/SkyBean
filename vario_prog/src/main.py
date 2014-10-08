#!/usr/bin/python

import sys
import serial
from intelhex import IntelHex
import time


def add8(a, b):
    return (a + b & 0xFF)

page_size = 255

class StaxProg():
    def __init__(self, port, speed):
        port = port
        speed = speed
        self.hex = IntelHex()
        self.port = port
        self.speed = speed

        
    def load(self, filename):
        print "Loading application from hex"
        self.hex.loadfile(filename, "hex")
        
        size = self.hex.maxaddr() - self.hex.minaddr()
        print " size: %0.2f KiB (%d B)" % (size/1024, size) 

    def read(self):
        c = self.handle.read()
        return c 

    def open(self):
        print "Programer ready connect SkyBean...", 
        sys.stdout.flush()
        
        self.handle = serial.Serial(self.port, self.speed, timeout=.2)
        
        done = False
        i = 0
        first = True
        while (not done):
            i += 1
            
            time.sleep(0.1)


            self.handle.write("ebl")
            str = self.handle.read(10)
                
            if (str == 'bootloader'):
                done = True
                verh = ord(self.handle.read(1))
                verl = ord(self.handle.read(1))
                ver = (verh << 8) | verl 
            else:
                if first:
#                     print "Unable to reset automatically. Press reset now."
                    sys.stdout.flush()
                    first = False
                
                self.handle.flushInput();    
                time.sleep(.1)
                
            if (i > 1000):
                raise Exception("Unable to acquire bootloader control")

        print "done"
        print "Bootloader version %d" % ver
        self.handle.setTimeout(2)
        
    def erase(self):
        print "Erasing application...",
        sys.stdout.flush()
        self.handle.write('e')
        
        c = self.read()

        if c == 'd':
            print "done"
        else:
            raise Exception("Unexpected character (%c = %d)" % (c, ord(c)))

    def boot(self):
        print "Booting application..."
        self.handle.write('b')
    
    def prog(self):
        done = False
        adr = self.hex.minaddr()
        
#         self.handle.write('s')
#         adrh = (self.hex.maxaddr() & 0xFF0000) >> 16
#         adrm = (self.hex.maxaddr() & 0x00FF00) >> 8
#         adrl = (self.hex.maxaddr() & 0x0000FF) >> 0
#         
#         self.handle.write(chr(adrl))
#         self.handle.write(chr(adrm))
#         self.handle.write(chr(adrh))        
        
        print "Programing application..."
        while(not done):
            self.handle.write('p')

            adrh = (adr & 0xFF0000) >> 16
            adrm = (adr & 0x00FF00) >> 8
            adrl = (adr & 0x0000FF) >> 0
            
            self.handle.write(chr(adrl))
            self.handle.write(chr(adrm))
            self.handle.write(chr(adrh))
            
            max_size = 64
            
            size = self.hex.maxaddr() - adr
            if (size > max_size):
                size = max_size
                
            sizel = chr(size & 0x00FF)
            sizeh = chr((size & 0xFF00) >> 8)
                
            self.handle.write(sizel)
            self.handle.write(sizeh)

            for i in range(size):
                low = self.hex[adr + i*2]
                high = self.hex[adr + i*2 + 1]
                
                self.handle.write(chr(low))
                self.handle.write(chr(high))

            adr += size << 1
            
            if adr >= self.hex.maxaddr():
                adr = self.hex.maxaddr()
                done = True

            print " adr 0x%04X  size %03X  (%3.0f%%)" % (adr, size, (float(adr)/float(self.hex.maxaddr()))*100)
            sys.stdout.flush()
                       
            c = self.read()
            
            if c != 'd':
                a = self.read()               
                raise Exception("Unexpected character (%c = %d)" % (a, ord(a)))
        print "Done"
        
    def verify(self):
        print "Verifying application..."
        
        #atxmega128a3 app section size
#        max_adr = 0x20000


        self.handle.write('s')
        adrh = (self.hex.maxaddr() & 0xFF0000) >> 16
        adrm = (self.hex.maxaddr() & 0x00FF00) >> 8
        adrl = (self.hex.maxaddr() & 0x0000FF) >> 0
        
        self.handle.write(chr(adrl))
        self.handle.write(chr(adrm))
        self.handle.write(chr(adrh))    

        max_adr = self.hex.maxaddr()
         
        size = 512
        done = False
        adr = 0

        read_data = []
                
        while (not done):
            self.handle.write('r')


            adrh = (adr & 0xFF0000) >> 16
            adrm = (adr & 0x00FF00) >> 8
            adrl = (adr & 0x0000FF) >> 0
            
            self.handle.write(chr(adrl))
            self.handle.write(chr(adrm))
            self.handle.write(chr(adrh))
            
            if (size > max_adr - adr):
                size = (max_adr - adr) * 2

            sizel = chr(size & 0x00FF)
            sizeh = chr((size & 0xFF00) >> 8)
                
            self.handle.write(sizel)
            self.handle.write(sizeh)
            
            for i in range(size):
                cadr = adr + i
                data = ord(self.handle.read())
                if (cadr >= self.hex.minaddr() and cadr <= self.hex.maxaddr()):
                    read_data.append(data)
                else:
                    if (data is not 0xFF):
                        print "FF expected on %06X" % cadr

            adr += size
            if (adr >= max_adr):
                adr = max_adr
                done = True
 
            print " adr 0x%04X  size %03X  (%3.0f%%)" % (adr, size/2, (float(adr)/float(self.hex.maxaddr()))*100)
            sys.stdout.flush()

            
        if (self.hex.tobinarray() == read_data):
            print "Verification OK"
        else:
            print "Verification FAILED"
        
            print self.hex.tobinarray()
            print read_data
        
            wrong = 0    
        
            for i in range(self.hex.maxaddr() - self.hex.minaddr()):
                cadr = i + self.hex.minaddr()
                if (self.hex._buf[cadr] is not read_data[cadr]):
                    wrong += 1
            
            print "Wrong bytes %d/%d (%d %%)" % (wrong, self.hex.maxaddr(), (wrong*100)/self.hex.maxaddr())
                
            
    def batch(self, filename):
        start = time.clock()
        
        self.load(filename)
        self.open()
        self.erase()
        self.prog()
        #self.verify()
        self.boot()
        
        end = time.clock()
        print 
        print "That's all folks! (%.2f seconds)" % (end - start)
            

if (len(sys.argv) is not 3):
    print "Usage %s <port> <hex>" % __file__
    sys.exit(-1)
    
port = sys.argv[1]
hex = sys.argv[2]    
    
#a = StaxProg("/dev/ttyACM0", 115200)
#a.batch("/home/horinek/data/workspace/RGBtest/Debug/RGBtest.hex")

a = StaxProg(port, 115200)
a.batch(hex)

