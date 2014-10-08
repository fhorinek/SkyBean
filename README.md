SkyBean
=====

<b>vario</b><br>
SkyBean vario source code<br>
<b>vario_bootloader</b><br>
Bootloader inside the devices<br>
<b>vario_prog</b><br>
Script for updating firmware using bootloader<br>


Buid info
=====

Tools we are using:

Eclipse IDE for C/C++ Developers<br>
https://www.eclipse.org/downloads/packages/eclipse-ide-cc-developers/junosr2<br>
AVR Eclipse plugin<br>
http://avr-eclipse.sourceforge.net/wiki/index.php/The_AVR_Eclipse_Plugin<br>
PyDev Eclipse plugin<br>
http://pydev.org/<br>
Atmel AVG GCC Toolchain:<br>
http://www.atmel.com/tools/ATMELAVRTOOLCHAINFORLINUX.aspx<br>

Programming
=====

<ul>
<li>Connect SkyBean to the computer using USB interface board or using another 3.3V USB<->UART convertor</li>
<li>Run python script <tt>vario_prog/src/main.py <i>[COM port] [HEX file]</i></tt></li>
<li>Turn on vario using the button</li>
<li><i>OR When you bricked your vario:</i> remove the battery, short out <b>+</b> and <b>-</b> on battery holder and insert battery again</li>
</ul>


Datasheets
=====

<b>ATXmega32E5</b><br>
Part: http://www.atmel.com/Images/Atmel-8153-8-and-16-bit-AVR-Microcontroller-XMEGA-E-ATxmega8E5-ATxmega16E5-ATxmega32E5_Datasheet.pdf<br>
Family: http://www.atmel.com/Images/Atmel-42005-8-and-16-bit-AVR-Microcontrollers-XMEGA-E_Manual.pdf<br>

<b>BMP180</b><br>
http://ae-bst.resource.bosch.com/media/products/dokumente/bmp180/BST-BMP180-DS000-09.pdf<br>
