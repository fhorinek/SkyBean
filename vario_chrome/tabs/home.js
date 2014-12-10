var pstates = {
    idle:               0,
    auto_com:           1,
    manual_com:         2,
    wait_for_device:    3,
    download_cfg:       4,
    upload_cfg:         5,
    verify_cfg:         6,
    download_done:      7,
    upload_done:        8,
    error:              9,
};

var pcmd = {
    wait_for_hello:     0,
    get_version:        1,
    write_cfg:          2,
    read_cfg:           3,
};


function PortHandler()
{
    this.state = pstates.idle;
    this.port_info = null;
    this.old_ports = false;
    this.task = false;

    chrome.serial.onReceive.addListener(function (info){
        data = new Uint8Array(info.data);
        
        for (i = 0; i < data.length; i++)
        {
            port_handler.parse(String.fromCharCode(data[i]));
        }
    });
    

    this.getStates = function()
    {
        return this.state;
    };

    this.setState = function(state)
    {
        this.state = state;
        //place for gui handlers
        update_port_state(state);
    };

    this.startWizard = function(task)
    {
        if (task!=undefined)
            this.task = task;

        console.log("Starting wizard");
        this.setState(pstates.auto_com);
        
        chrome.serial.getDevices(this.updatePortsWizard);
    };


    this.startManual = function()
    {
        // this.task = task;

        console.log("Starting manual");
        this.setState(pstates.manual_com);
        
        chrome.serial.getDevices(this.updatePortsManual);
    };

    
    this.openPort = function(port)
    {
        console.log("Opening " + port);

        this.init_parser();
        chrome.serial.connect(port, {bitrate: 9600}, function(info) {
            console.log("port opened");
            console.log(info);
            port_handler.port_info = info;
            port_handler.setState(pstates.wait_for_device);

        });

    }

    this.updatePortsWizard = function(new_ports)
    {
        if (this.state != pstates.auto_com)
            return;

        function in_array(obj, array)
        {
            for (i in array)
            {
                if (obj.displayName == array[i].displayName && obj.path == array[i].path && obj.productId == array[i].productId && obj.vendorId == array[i].vendorId)
                    return true;
            }
            return false;
        }

        console.log(new_ports);

        if (port_handler.old_ports === false)
        {
            //first iteration no old ports
            port_handler.old_ports = new_ports;
        }
        else
        {
            for (i in new_ports)
            {
                port = new_ports[i];

                //if this port was not avalible 
                if (!(in_array(port, port_handler.old_ports)))
                {
                    port_handler.openPort(port.path);
                    return;
                }
            }
            port_handler.old_ports = new_ports;
        }

        //refresh ports 
        setTimeout(function() {
            chrome.serial.getDevices(port_handler.updatePortsWizard);
        }, 1000);          
    };

    this.updatePortsManual = function(new_ports)
    {
        $("#port_selector").empty();

        console.log(new_ports);

        for (id in new_ports)
        {
            $("#port_selector").append("<option value=\"" + new_ports[id].path + "\">" + new_ports[id].path + " (" + new_ports[id].displayName + ")</option>");
        }
        
        if (new_ports.length == 0)
            $("#port_selector").append("<option disabled>No COM ports found</option>");        
    };

    this.send = function(str)
    {
        data = convertStringToArrayBuffer(str)

        chrome.serial.send(this.port_info.connectionId, data, function() {
            console.log("data send");
        });
    };

    this.init_parser = function() 
    {
        this.parser_buffer = "";
        this.parser_cmd = pcmd.wait_for_hello;
        this.parser_debug = "";
        this.vario_version = 0;
    }

    this.parse = function(c)
    {
        this.parser_debug += c;
        parser_hello_str = "skybean";

        switch (this.parser_cmd)
        {
            case(pcmd.wait_for_hello): //read hello
                if (c == parser_hello_str.charAt(this.parser_buffer.length))
                {
                    this.parser_buffer += c;
                    if (this.parser_buffer == parser_hello_str)
                    {
                        console.log("hello recieved");
                        this.parser_cmd = pcmd.get_version;
                        this.parser_buffer = "";
                    }
                }
                else
                {
                    this.parser_buffer = "";
                }
            break;   

            case(pcmd.get_version): //read hello
                this.parser_buffer += c
                if (this.parser_buffer.length == 2)
                {
                    ver = this.parser_buffer.charCodeAt(0) << 8;
                    ver += this.parser_buffer.charCodeAt(1);
                    
                    this.vario_version = ver;
                    
                    console.log("SkyBean FW build:" + ver.toString());                    
                    if (this.task == false) //just get version
                    {
                        this.closePort();
                    }

                    if (this.task == pcmd.read_cfg)
                    {
                        this.setState(pstates.download_cfg);
                        this.parser_cmd = pcmd.read_cfg;
                        this.parser_buffer = "";
                        this.send("R");
                    }

                    if (this.task == pcmd.write_cfg)
                    {
                        this.setState(pstates.upload_cfg);

                        console.log("sending cfg");

                        this.parser_cmd = pcmd.write_cfg;
                        this.parser_buffer = "";
                        this.send("W");

                        data_cfg = "";
                        data_cfg += encode_cfg(actual_cfg);
                        data_cfg += encode_prof(actual_prof[0]);
                        data_cfg += encode_prof(actual_prof[1]);
                        data_cfg += encode_prof(actual_prof[2]);        
                        
                        // this.send(data_cfg);     

                        this.tx_cnt = 0;   

                        crc = 0;

                        for (i in data_cfg)
                            crc = CalcCRC(crc, CRC_KEY, data_cfg.charCodeAt(i));

                        data_cfg += String.fromCharCode(crc);

                        debug_stream(data_cfg);

                        console.log("CALC CRC is " + crc);

                        this.send(data_cfg);  

                    }
                }
            break;               

            case(pcmd.read_cfg): //read cfg
                this.parser_buffer += c;
                if (this.parser_buffer.length == STRUCT_LEN + 1) // + CRC    
                {
               
                    crc = 0;

                    for (i = 0; i < STRUCT_LEN; i++)
                    {
                        crc = CalcCRC(crc, CRC_KEY, this.parser_buffer.charCodeAt(i));
                    }

                    rx_crc = get_uint8(this.parser_buffer, STRUCT_LEN);

                    console.log("CALC CRC is " + crc);
                    console.log("RX   CRC is " + rx_crc);
                    if (rx_crc == crc)
                    {
                        read_cfg = decode_cfg(this.parser_buffer);
                        console.log(read_cfg);
                        
                        debug_stream(this.parser_buffer);

                        read_prof = new Array(3);
                        read_prof[0] = decode_prof(this.parser_buffer, 0);
                        read_prof[1] = decode_prof(this.parser_buffer, 1);
                        read_prof[2] = decode_prof(this.parser_buffer, 2);

                        console.log(read_prof);

                        if (this.task == pcmd.read_cfg)
                        {
                            actual_prof = read_prof;
                            actual_cfg = read_cfg;

                            this.closePort();  
                            this.setState(pstates.download_done);
                        }

                        if (this.task == pcmd.write_cfg)
                        {
                            actual_prof = read_prof;
                            actual_cfg = read_cfg;

                            this.closePort();  
                            this.setState(pstates.upload_done);
                        }

                    }                    
                    else
                    {
                        this.closePort();  
                        this.setState(pstates.error);
                    }
                                 
                }
            break;    

            case(pcmd.write_cfg):
                if (c == "o")
                {
                    console.log("Transfer ok");

                    this.setState(pstates.verify_cfg);

                    this.parser_cmd = pcmd.read_cfg;
                    this.parser_buffer = "";
                    this.send("R");
                }

                if (c == '.')
                {
                    this.tx_cnt++;
                }

                if (c == 'e')
                {
                    console.log("Transfer error");

                    this.parser_buffer = "";
                    this.closePort(); 

                    this.setState(pstates.error);
                }

            break;

        }     

    };


    this.closePort = function()
    {
        chrome.serial.disconnect(this.port_info.connectionId, function(){
            console.log("Port closed");
            port_handler.setState(pstates.idle);
        });
    };
}


function update_port_state(state)
{
    switch(state)
    {
        case(pstates.auto_com):
            $("#port_wizard_text").html("Looking for COM port");
            $("#port_wizard_desc").html("Connect USB interface board. If is the board allready connected disconnect the board and reconnect it again.");
            $(".loader").show();
            $("#reset_wizard").hide();            
        break;
        case(pstates.manual_com):
            $("#port_wizard_text").html("Manual COM selection");
            $("#port_wizard_desc").html("Select USB interface COM port.");
            $(".loader").hide();
        break;
        case(pstates.wait_for_device):
            $("#port_wizard_text").html("Waiting for SkyBean");
            $("#port_wizard_desc").html("Connect the SkyBean to interface board and turn it on.");
            $(".loader").show();
            $("#reset_wizard").show();    
             
            $("#port_wizard").show();
            $("#port_manual").hide(); 
        break;
        case(pstates.download_cfg):
            $("#port_wizard_text").html("Downloading configuration");
            $("#port_wizard_desc").html("Please wait until the configuration is downloaded from the SkyBean");
        break;
        case(pstates.download_done):
            $("#port_wizard_text").html("Done");
            $("#port_wizard_desc").html("Configuration was sucesfully downloaded.");
            $(".loader").hide();
        break;
        case(pstates.error):
            $("#port_wizard_text").html("Error");
            $("#port_wizard_desc").html("There was an error during transfer please try again");
            $("loader").hide();
        break;       
        case(pstates.upload_cfg):
            $("#port_wizard_text").html("Uploading configuration");
            $("#port_wizard_desc").html("Please wait until the configuration is uploaded to the SkyBean");
        break;            
        case(pstates.verify_cfg):
            $("#port_wizard_text").html("Verifing configuration");
            $("#port_wizard_desc").html("Please wait until the configuration is verified");
        break;       
        case(pstates.upload_done):
            $("#port_wizard_text").html("Done");
            $("#port_wizard_desc").html("Configuration was sucesfully uploaded.");
            $(".loader").hide();
        break;             
        
    }
}

var actual_cfg = false;
var actual_prof = false;

function set_defult_configuration()
{
    actual_cfg = default_cfg();
    actual_prof = [default_prof(), default_prof(), default_prof()];
    
    actual_prof[1].enabled = 0;
    actual_prof[2].enabled = 0;    
}


function convertStringToArrayBuffer(str) 
{
    buf = new ArrayBuffer(str.length);
    bufView = new Uint8Array(buf);
    
    for (i=0; i<str.length; i++) 
    {
        bufView[i]=str.charCodeAt(i);
    }
    return buf;
}

function debug_stream(stream)
{
    arr = [];    
    for (i = 0; i < stream.length; i++)
    {
        arr.push(stream.charCodeAt(i));
    }
    console.log(arr);
}

var port_handler = new PortHandler();

function tab_home_init()
{
    set_defult_configuration();


    $("#home_button_load_from_skybean").click(function(){
        port_handler.startWizard(pcmd.read_cfg);
        $("#home_popup_load_from_skybean").fadeIn(function(){
            
        });
    });
    
    $("#home_button_save_to_skybean").click(function(){
        port_handler.startWizard(pcmd.write_cfg);
        $("#home_popup_load_from_skybean").fadeIn(function(){
            
        });
    });    

    $("#reset_wizard").click(function(){
        port_handler.startWizard()
    });
    
    $("#no_wizard").click(function(){
        $("#port_wizard").hide();
        $("#port_manual").show();
        
        port_handler.startManual(pcmd.read_cfg);
    });    
    
    $("#yes_wizard").click(function(){
        $("#port_wizard").show();
        $("#port_manual").hide();
        
        port_handler.startWizard()
    });
    
    $("#port_refresh").click(function(){
        port_handler.startManual(pcmd.read_cfg);
    });
    
    $("#port_open").click(function(){
        port_name = $("#port_selector").val();
        if (port_name != null)
            port_handler.openPort(port_name);
    });

    $("#home_load_from_skybean_wizard_cancel").click(function(){
        // stop_wizard();
        $("#home_popup_load_from_skybean").fadeOut();
    });

    $("#home_load_from_skybean_manual_cancel").click(function(){
        // stop_wizard();
        $("#home_popup_load_from_skybean").fadeOut();
    });

    $("#home_button_save_to_file").click(function(){
        save_to_file();

    });

    $("#home_button_load_from_file").click(function(){
        load_from_file();
    });
}


function update_ports_manual(devices)
{


}

function writeFileEntry(writableEntry, opt_blob, callback) {
  writableEntry.createWriter(function(writer) {

    writer.onerror = errorHandler;
    writer.onwriteend = callback;

      writer.truncate(opt_blob.size);
      waitForIO(writer, function() {
        writer.seek(0);
        writer.write(opt_blob);
      });


  }, errorHandler);
}

function get_cfg()
{
    block = {};
    block["cfg"] = actual_cfg;
    block["profile"] = actual_prof;

    return JSON.stringify(block);
}

function save_to_file()
{
var config = {type: 'saveFile', suggestedName: "config.sbc"};
  chrome.fileSystem.chooseEntry(config, function(writableEntry) {
    var blob = new Blob([get_cfg()], {type: 'text/plain'});
    writeFileEntry(writableEntry, blob, function(e) {
      console.log("Write complete!");
    });
  });
}

function load_from_file()
{
var config = {type: 'openFile', suggestedName: "config.sbc"};
 chrome.fileSystem.chooseEntry(config, function(chosenEntry) {
  chosenEntry.file(function(file) {
    readAsText(chosenEntry, function(result) {
      block = JSON.parse(result);
      console.log(block);
      actual_cfg = block["cfg"];
      actual_prof = block["profile"];
    });
  });  
});
}

function errorHandler(e) {
  console.error(e);
}

function waitForIO(writer, callback) {
  // set a watchdog to avoid eventual locking:
  var start = Date.now();
  // wait for a few seconds
  var reentrant = function() {
    if (writer.readyState===writer.WRITING && Date.now()-start<4000) {
      setTimeout(reentrant, 100);
      return;
    }
    if (writer.readyState===writer.WRITING) {
      console.error("Write operation taking too long, aborting!"+
        " (current writer readyState is "+writer.readyState+")");
      writer.abort();
    } 
    else {
      callback();
    }
  };
  setTimeout(reentrant, 100);
}

function readAsText(fileEntry, callback) {
  fileEntry.file(function(file) {
    var reader = new FileReader();

    reader.onerror = errorHandler;
    reader.onload = function(e) {
      callback(e.target.result);
    };

    reader.readAsText(file);
  });
}