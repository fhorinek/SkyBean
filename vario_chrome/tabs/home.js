//0 - auto_search
//1 - manual
//2 - defined and open
//3 - waiting for skybean
//4 - downloading
//5 - fail

port_state = 0;
port_name = "";
ports_old = false;

function update_port_state()
{
    switch(port_state)
    {
        case(0):
            $("#port_wizard_text").html("Looking for COM port");
            $("#port_wizard_desc").html("Connect USB interface board. If is the board allready connected disconnect the board and reconnect it again.");
            $(".loader").show();
            $("#reset_wizard").hide();            
        break;
        case(1):
            $("#port_wizard_text").html("Manual COM selection");
            $("#port_wizard_desc").html("Select USB interface COM port.");
            $(".loader").hide();
        break;
        case(2):
            $("#port_wizard_text").html("Waiting for SkyBean");
            $("#port_wizard_desc").html("Connect the SkyBean to interface board and turn it on.");
            $(".loader").show();
            $("#reset_wizard").show();            
        break;
        case(3):
            $("#port_wizard_text").html("Downloading configuration");
            $("#port_wizard_desc").html("Please wait until the configuration is downloaded from the SkyBean");
        break;
        case(4):
            $("#port_wizard_text").html("Done");
            $("#port_wizard_desc").html("Configuration was sucesfully downloaded.");
            $(".loader").hide();
        break;
        case(5):
            $("#port_wizard_text").html("Error");
            $("#port_wizard_desc").html("Configuration Timeout.");
            $("loader").hide();
        break;       
    }
}


var port_info = null;
var parser_length = 0;

// 0 - none
// 1 - hello
// 2 - version
// 3 - read data
var parser_cmd = 0;
var parser_hello = "skybean";
var parser_buffer = "";
var vario_version = false;

var actual_cfg;
var actual_prof;


var convertStringToArrayBuffer=function(str) 
{
    var buf=new ArrayBuffer(str.length);
    var bufView=new Uint8Array(buf);
    for (var i=0; i<str.length; i++) {
    bufView[i]=str.charCodeAt(i);
    }
    return buf;
}

function serial_parse(c)
{
    switch(parser_cmd)
    {
        case(0):
        return;
        
        case(1):
            if (c == parser_hello.charAt(parser_length))
            {
                parser_length++;
                if (parser_length == parser_hello.length)
                {
                    //get version
                    parser_cmd = 2;
                    parser_buffer = "";
                    chrome.serial.send(port_info.connectionId, convertStringToArrayBuffer('V'), function() {
                        console.log("data send");
                    });
                }
            }
            else
            {
                if (parser_length > 0)
                    parser_length = 0;
            }
        break;
        
        case(2):
            parser_buffer += c;
            if (parser_buffer.length == 2)
            {
                ver = parser_buffer.charCodeAt(0) << 8;
                ver += parser_buffer.charCodeAt(1);
                
                vario_version = ver;
                
                console.log("SkyBean FW build:" + ver.toString());
                
                //get cfg
                parser_cmd = 3;
                parser_buffer = "";
                chrome.serial.send(port_info.connectionId, convertStringToArrayBuffer('R'), function() {
                    console.log("data send");
                });           
                     
                port_state = 3;
                update_port_state();                
            }
        break;
        
        case(3):
            parser_buffer += c;
            if (parser_buffer.length == 834)            
            {
                actual_cfg = decode_cfg(parser_buffer);
                console.log(actual_cfg);
                
                actual_prof = new Array(3);
                actual_prof[0] = decode_prof(parser_buffer, 0);
                actual_prof[1] = decode_prof(parser_buffer, 1);
                actual_prof[2] = decode_prof(parser_buffer, 2);

                console.log(actual_prof);
                
                port_state = 4;
                update_port_state();                     
            }
        break;

    }
}

function serial_init()
{
    chrome.serial.onReceive.addListener(function (info){
        v = new Uint8Array(info.data);
        for (i = 0; i < v.length; i++)
            serial_parse(String.fromCharCode(v[i]));
    });
    
 /*   chrome.serial.onReceiveError.addListener(function (info){
        console.log(info);
    });*/
}

function open_port()
{
    console.log("Opening " + port_name);

    chrome.serial.connect(port_name, {bitrate: 115200}, function(info){
        console.log(info);
        
        port_info = info;
        
        port_state = 2;
        update_port_state();
        
        parser_cmd = 1;
        parser_length = 0;
    });
}

function start_wizard()
{
    port_state = 0;
    
    setTimeout(function() {
        chrome.serial.getDevices(update_ports_wizard);
    }, 1000);  
    
    update_port_state();
    ports_old = false;
}

function start_manual()
{
    port_state = 1;
    update_port_state(); 
    
    chrome.serial.getDevices(update_ports_manual);   
}

function tab_home_init()
{
    serial_init();
    start_wizard();

    $("#reset_wizard").click(function(){
        start_wizard();
    });
    
    $("#no_wizard").click(function(){
        $("#port_wizard").hide();
        $("#port_manual").show();
        
        start_manual();
    });    
    
    $("#yes_wizard").click(function(){
        $("#port_wizard").show();
        $("#port_manual").hide();
        
        start_wizard(); 
    });
    
    $("#port_refresh").click(function(){
        start_manual();
    });
    
    $("#port_open").click(function(){
        port_name = $("#port_selector").val();
        if (port_name != null)
            open_port();
    });

}

function update_ports_wizard(devices)
{
    new_ports = [];

    for (id in devices)
    {
        new_ports.push(devices[id].path);
    }
    
    if (ports_old === false)
    {
        ports_old = new_ports;
    }
    else
    {
        for (i in new_ports)
        {
            path = new_ports[i];
            
            if (ports_old.indexOf(path) == -1)
            {
                port_state = 2;
                port_name = path;
                update_port_state();
                open_port();
            }
        }
        ports_old = new_ports;
    }
    
    if (port_state == 0)    
    setTimeout(function() {
        chrome.serial.getDevices(update_ports_wizard);
    }, 1000);    
}

function update_ports_manual(devices)
{
    $("#port_selector").empty();

    for (id in devices)
    {
        $("#port_selector").append("<option>" + devices[id].path + "</option>");
    }
    
    if (devices.length == 0)
        $("#port_selector").append("<option disabled>No COM ports found</option>");

}



