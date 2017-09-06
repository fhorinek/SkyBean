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
    fw_prog:            10,
    fw_verify:          11,
    fw_done:            12,
    fw_get:             13,
    fw_select:          14,
    fw_download:        15,
    fw_erase:           16,
};

var pcmd = {
    wait_for_hello:     0,
    get_version:        1,
    write_cfg:          2,
    read_cfg:           3,
    prog_reset:         4,
    prog_version:       5,
    prog_erase:         6,
    prog_upload:        7,
    prog_verify:        8,
};


function chr(n)
{
    return String.fromCharCode(n);
}

function update_progressbar(val)
{
    $( "#progressbar" ).progressbar({
        value: val * 100
    });
}

var error_message = "unknown";

function update_port_state(state)
{
    $("#port_wizard_text").html("");
    $("#port_wizard_desc").html("");        
    $(".loader").hide(0);
    $("#port_manual").hide(0); 
    $("#firmware").hide(0);
    $("#popup_cancel").hide(0);
    $("#popup_close").hide(0);
    $("#yes_wizard").hide(0);
    $("#no_wizard").hide(0);
    $("#firmware_load").hide(0);
    $("#progressbar").hide(0);

    switch(state)
    {
        case(pstates.auto_com):
            $("#port_wizard_text").html("Looking for COM port");
            $("#port_wizard_desc").html("Connect USB interface board. If is the board allready connected disconnect the board and reconnect it again.");
            $(".loader").show();
            $("#no_wizard").show();
            $("#popup_cancel").show();
        break;
        case(pstates.manual_com):
            $("#port_wizard_text").html("Manual COM selection");
            $("#port_wizard_desc").html("Select USB interface COM port.");
            $("#port_manual").show(); 
            $("#yes_wizard").show();
            $("#popup_close").show();            
        break;
        case(pstates.wait_for_device):
            $("#port_wizard_text").html("Waiting for SkyBean");
            $("#port_wizard_desc").html("Connect the SkyBean to interface board and turn it on.");
            $(".loader").show();
            $("#popup_cancel").show();
        break;
        case(pstates.download_cfg):
            $("#port_wizard_text").html("Downloading configuration");
            $("#port_wizard_desc").html("Please wait until the configuration is downloaded from the SkyBean");
        break;
        case(pstates.download_done):
            $("#port_wizard_text").html("Done");
            $("#port_wizard_desc").html("Configuration was sucesfully downloaded from SkyBean.");
            $("#popup_close").show(); 
        break;
        case(pstates.error):
            $("#port_wizard_text").html("Error");
            $("#port_wizard_desc").html("There was an error during comunication with SkyBean. Please try again <br><br>Error details:<br>" + error_message);
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
            $("#popup_close").show(); 
        break;   
        case(pstates.fw_get):
            $("#port_wizard_text").html("Updating");
            $("#port_wizard_desc").html("Downloading index from update server.");
            $(".loader").show();
        break;   
        case(pstates.fw_download):
            $("#port_wizard_text").html("Downloading firmware");
            $("#port_wizard_desc").html("Downloading selected firmware from update server.");
            $(".loader").show();
            $("#popup_cancel").show();   
            $("#firmware_load").show();            
        break;   
        case(pstates.fw_list):
            $("#port_wizard_text").html("Select firmware");
            $("#port_wizard_desc").html("Choose what firmware shoud be uploaded to the SkyBean.");
            $("#firmware").show();
            $("#popup_close").show();   
            $("#firmware_load").show();
        break; 
        case(pstates.fw_erase):
            $("#port_wizard_text").html("Erasing firmware");
            $("#port_wizard_desc").html("Erasing application on SkyBean.");
            $(".loader").show();        
        break;
        case(pstates.fw_prog):
            $("#port_wizard_text").html("Programing firmware");
            $("#port_wizard_desc").html("Uploading new firmware to SkyBean.");
            $("#progressbar").show();
        break;
        case(pstates.fw_verify):
            $("#port_wizard_text").html("Verifing firmware");
            $("#port_wizard_desc").html("Verifing new firmware on SkyBean.");
            $("#progressbar").show();
        break;
        case(pstates.fw_done):
            $("#port_wizard_text").html("Sucesfully done");
            $("#port_wizard_desc").html("New firmware was sucesfully uploaded and verified.");
            $("#popup_close").show();
        break;

    }
}

function PortHandler()
{
    this.state = pstates.idle;
    this.port_info = null;
    this.task = false;
    this.prog_size = 128;
    this.old_port = false;

    chrome.serial.onReceive.addListener(function (info)
    {
        data = new Uint8Array(info.data);
        console.log("data_in");
        
        for (i = 0; i < data.length; i++)
        {
            port_handler.parse(String.fromCharCode(data[i]));
        }
    });
    
    chrome.serial.onReceiveError.addListener(function watch_for_on_receive_errors(info) {
        console.log(info);

        switch (info.error)
        {
            case("device_lost"):
                error_message = "Device was disconnected.";
                port_handler.cancel();
                update_port_state(pstates.error);
            break;
            case("system_error"):
                console.log("trying to recover..");
                if (port_handler.old_port)
                {
                    chrome.serial.disconnect(port_handler.port_info.connectionId, function(){
                        console.log("closing fast");
                        chrome.serial.connect(port_handler.old_port, {bitrate: 115200}, function(info){
                            console.log("port reopened");
                            console.log(info);
                            port_handler.port_info = info;

                        });
                    });
                }

            break;
        }
    });


    this.getStates = function()
    {
        return this.state;
    };

    this.setState = function(state)
    {
        this.state = state;
        console.log("gui state is " + this.state);
        //place for gui handlers
        update_port_state(state);
    };

    this.startWizard = function(task)
    {
        if (task != undefined)
            this.task = task;

        console.log("Starting wizard");
        this.setState(pstates.auto_com);
        this.old_ports = false;

        chrome.serial.getDevices(this.updatePortsWizard);
    };


    this.startManual = function()
    {
        console.log("Starting manual");
        this.setState(pstates.manual_com);
        
        chrome.serial.getDevices(this.updatePortsManual);
    };

    
    this.openPort = function(port)
    {
        console.log("Opening " + port);

        this.old_port = port;

        chrome.serial.connect(port, {bitrate: 115200}, function(info) {
            console.log("port opened");
            console.log(info);
            port_handler.port_info = info;
            port_handler.setState(pstates.wait_for_device);

            if (port_handler.task == pcmd.prog_upload)
                port_handler.startProg();
            else
                port_handler.init_parser();



        });

    };

    this.updatePortsWizard = function(new_ports)
    {
        if (port_handler.state != pstates.auto_com)
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
            name = new_ports[id].displayName;
            if (name == "FT231X_USB_UART")
                name = "SkyBean USB interface";

            $("#port_selector").append("<option value=\"" + new_ports[id].path + "\">" + new_ports[id].path + " (" + name + ")</option>");
        }
        
        if (new_ports.length == 0)
            $("#port_selector").append("<option disabled>No COM ports found</option>");        
    };

    this.send = function(str, callback)
    {
        data = convertStringToArrayBuffer(str)

        if (callback === undefined)
        {
            callback = function() {
                console.log("data send");
            }

        }

        chrome.serial.send(this.port_info.connectionId, data, callback);
    };

    this.init_parser = function() 
    {
        console.log("Parser init");
        this.parser_buffer = "";
        this.parser_cmd = pcmd.wait_for_hello;
        this.parser_debug = "";
        this.vario_version = 0;
    };

    this.parse = function(c)
    {
        this.parser_debug += c;
        parser_hello_str = "skybean";
        parser_btl_str = "bootloader";

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
                        
                        console.log(data_cfg.length);
                        // this.send(data_cfg);     

                        this.tx_cnt = 0;   

                        crc = 0;

                        for (i in data_cfg)
                            crc = CalcCRC(crc, CRC_KEY, data_cfg.charCodeAt(i));

                        data_cfg += String.fromCharCode(crc);

                        debug_stream(data_cfg);

                        console.log("CALC CRC is " + crc);

                        this.data_to_send = data_cfg;
                        this.send(this.data_to_send[0]);  
                        this.data_to_send = this.data_to_send.slice(1);
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
                    if (this.data_to_send.length > 0)
                    {
                        this.send(this.data_to_send[0]); 
                        this.data_to_send = this.data_to_send.slice(1);
                    }
                }

                if (c == 'e')
                {
                    console.log("Transfer error");

                    this.parser_buffer = "";
                    this.closePort(); 

                    this.setState(pstates.idle);
                }

            break;

            case(pcmd.prog_reset):
                if (c == parser_btl_str.charAt(this.parser_buffer.length))
                {
                    this.parser_buffer += c;
                    if (this.parser_buffer == parser_btl_str)
                    {
                        console.log("bootloader entered");
                        this.parser_cmd = pcmd.prog_version;
                        this.parser_buffer = "";
                    }
                }
                else
                {
                    this.parser_buffer = "";
                }
            break;  

            case(pcmd.prog_version):
                this.parser_buffer += c;
                if (this.parser_buffer.length == 2)
                {
                    ver = this.parser_buffer.charCodeAt(0) << 8;
                    ver += this.parser_buffer.charCodeAt(1);

                    console.log("Erasing app");
                    this.parser_cmd = pcmd.prog_erase;
                    this.parser_buffer = "";          
                    this.send("e");          
                    this.setState(pstates.fw_erase);
                }
            break;

            case(pcmd.prog_erase):
                if (c == 'd')
                {
                    console.log("Erasing done");

                    this.prog_pos = 0;
                    this.parser_cmd = pcmd.prog_upload;

                    //start program upload
                    this.progNextBlock();
                    this.setState(pstates.fw_prog);
                }
            break;


            case(pcmd.prog_upload):
                if (c == 'd')
                {
                    console.log("Block " + this.prog_pos + " done");

                    update_progressbar(this.prog_pos / fw_bin.length);

                    if (this.prog_pos >= fw_bin.length)
                    {
                        //start verification
                        console.log("Verification start");
                        this.prog_pos = 0;
                        this.parser_cmd = pcmd.prog_verify;
                        this.parser_buffer = "";
                        this.verifyNextBlock();
                        this.setState(pstates.fw_verify);                 
                    }
                    else
                        this.progNextBlock();
                }
            break;

            case(pcmd.prog_verify):
                this.parser_buffer += c;
                //console.log("got " + this.parser_buffer.length);
                if (this.parser_buffer.length == this.prog_size)
                {
                    fail = false;

                    update_progressbar(this.prog_pos / fw_bin.length);

                    for (i = 0; i < this.prog_size; i++)
                    {
                        if (this.prog_pos + i >= fw_bin.length)
                        {
                            if (this.parser_buffer[i] != chr(0xFF))
                            {
                                console.log("Error at " + (this.prog_pos + i) + " got [" + this.parser_buffer.charCodeAt(i) + "] but 0xFF expected!");
                                fail = true;
                                break;
                            }                                

                        }
                        else
                        {
                            if (this.parser_buffer[i] != fw_bin[this.prog_pos + i])
                            {
                                console.log("Error at " + (this.prog_pos + i) + " got [" + this.parser_buffer.charCodeAt(i) + "] but " + fw_bin.charCodeAt(this.prog_pos + i) + " expected!");
                                fail = true;
                                break;
                            }
                        }

                    }

                    if (fail)
                    {
                        console.log("Verification error");
                        this.closePort();   
                        break;
                    }

                    //continue verification
                    this.prog_pos += this.prog_size;
                    if (this.prog_pos > fw_bin.length)
                    {
                        console.log("Verification done");
                        this.setState(pstates.fw_done);      
                        this.send("b", function(){
                            this.closePort();    
                        });
                    }
                    else
                    {
                        console.log("block " + this.prog_pos + " ok");
                        this.parser_buffer = "";   
                        this.verifyNextBlock();                                  
                    }

                }
            break

        }     

    };


    this.progNextBlock = function()
    {
        text = "p";
        adr = this.prog_pos

        text += chr((adr & 0x0000FF) >> 0);
        text += chr((adr & 0x00FF00) >> 8);
        text += chr((adr & 0xFF0000) >> 16);

        max_size = this.prog_size;

        size = fw_bin.length - this.prog_pos;
        if (size > max_size)
            size = max_size


        wsize = size / 2;
        text += chr((wsize & 0x00FF) >> 0);
        text += chr((wsize & 0xFF00) >> 8);

        for (i = 0; i < size; i++)
        {
            text += fw_bin[this.prog_pos + i];
        }

        this.send(text);

        // debug_stream(text);

        this.prog_pos += size;
    };

    this.verifyNextBlock = function()
    {
        text = "r";
        adr = this.prog_pos

        text += chr((adr & 0x0000FF) >> 0);
        text += chr((adr & 0x00FF00) >> 8);
        text += chr((adr & 0xFF0000) >> 16);

        size = this.prog_size;
        text += chr((size & 0x00FF) >> 0);
        text += chr((size & 0xFF00) >> 8);

        this.send(text);
        // debug_stream(text);
    };

    this.reset = function()
    {
        if (this.parser_cmd != pcmd.prog_reset)
            return;

        this.send("ebl");

        setTimeout(function() {
            port_handler.reset();
        }, 100);      
    };

    this.startProg = function()
    {
        this.parser_cmd = pcmd.prog_reset;

        this.parser_buffer = "";
        this.parser_debug = "";

        this.reset();
    };

    this.closePort = function()
    {
        if (this.port_info != null)
        {
            chrome.serial.disconnect(this.port_info.connectionId, function(){
                console.log("Port closed");
                port_handler.port_info = null;
                port_handler.old_port = false;
            });
        }
    };

    this.cancel = function()
    {
        this.state = pstates.idle;
        this.parser_cmd = pcmd.wait_for_hello;
    };
}

var file_base = "http://glados.horinek.sk/home/~horinek/skybean/";
var xhr = new XMLHttpRequest();

var fw_list = {};

var fw_bin = null;
var fw_build = 0;
var fw_md5 = "";
var fw_file = "";

function get_list()
{   
    update_port_state(pstates.fw_get);
    xhr.open("GET", file_base, true);

    xhr.onreadystatechange = function() 
    {
        if (xhr.readyState == 4) 
        {
            fw_list = JSON.parse(xhr.responseText);
            console.log(fw_list);
            console.log(xhr.responseText);

            select = $("#fw_select");
            select.empty();

            for (key in fw_list)
            {
                option = document.createElement("option");
                $(option).val(key);
                $(option).html(fw_list[key].file);
                select.append(option);
            }

            update_port_state(pstates.fw_list);
        }
    }
    xhr.send();
}


function get_fw(id)
{   
    fw_md5 = fw_list[id].md5;
    fw_file = fw_list[id].file;
    fw_build = id;
    fw_bin = null;

    update_port_state(pstates.fw_download);

    xhr.open("GET", file_base + "?file=" + fw_file, true);

    xhr.onreadystatechange = function() {
      if (xhr.readyState == 4) {
        tmp = xhr.responseText;
        calc = md5(tmp);

        if (calc == fw_md5)
        {
            fw_bin = atob(tmp);
            console.log("md5 ok");
            start_fw_update();
        }
        else
        {
            console.log("md5 check fail");
            update_port_state(pstates.error);
        }

      }
    }
    xhr.send();
}

function store_fw()
{
    if (fw_bin == null)
        return;
}

function start_fw_update()
{
    console.log("Start fw update");
    update_port_state(pstates.idle);
    port_handler.startWizard(pcmd.prog_upload);
}




var actual_cfg = false;
var actual_prof = false;

function set_default_configuration()
{
    actual_cfg = default_cfg();
    actual_prof = [default_prof(), default_prof(), default_prof()];
    
//     actual_prof[1].enabled = 0;
//     actual_prof[2].enabled = 0;    
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
//        arr.push(stream.charCodeAt(i));
        arr.push(stream[i]);
    }
    console.log(arr);
}

var port_handler = new PortHandler();

function popup_open()
{
    $("#home_popup").fadeIn(function(){});   
    $("#home_popup_lock").show(0);   
}

function popup_close()
{
    $("#home_popup").fadeOut(function(){
        $("#home_popup_lock").hide();       
    });   
}

function tab_home_init()
{
    set_default_configuration();
    update_port_state(pstates.idle);

    $("#home_button_load_from_skybean").click(function(){
        port_handler.startWizard(pcmd.read_cfg);
        popup_open();
    });
    
    $("#home_button_save_to_skybean").click(function(){
        port_handler.startWizard(pcmd.write_cfg);
        popup_open();
    });    

    $("#home_button_load_from_file").click(function(){
        load_from_file();
    });

    $("#home_button_save_to_file").click(function(){
        save_to_file();
    });

    $("#home_button_reset").click(function(){
        set_default_configuration();
    });

    $("#home_button_program").click(function(){
        get_list();
        popup_open();
    });

    $("#fw_refresh").click(function(){
        get_list();
    });    

    $("#popup_cancel").click(function(){
        port_handler.closePort();
        port_handler.cancel();
        popup_close();
    });

    $("#popup_close").click(function(){
        port_handler.closePort();
        port_handler.cancel();
        popup_close();
    });    

    $("#fw_start").click(function(){
        get_fw($("#fw_select").val());
    });

    $("#firmware_load").click(function(){
        load_fw_file();
    });
    
    $("#no_wizard").click(function(){
        port_handler.startManual();
    });    
    
    $("#yes_wizard").click(function(){
        port_handler.startWizard()
    });
    
    $("#port_refresh").click(function(){
        port_handler.startManual();
    });
    
    $("#port_open").click(function(){
        var port_name = $("#port_selector").val();
        if (port_name != null)
            port_handler.openPort(port_name);
    });

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

function load_fw_file()
{
var config = {type: 'openFile', suggestedName: "*.ebin"};
 chrome.fileSystem.chooseEntry(config, function(chosenEntry) {
  chosenEntry.file(function(file) {
    xhr.abort();
    readAsText(chosenEntry, function(result) {
        fw_bin = atob(result);
        start_fw_update();
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