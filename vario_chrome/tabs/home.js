function tab_home_init()
{
    console.log("home_starting");
    
    ports = [];
    setTimeout(function() {
        chrome.serial.getDevices(update_ports);
    }, 3000);
        
    console.log($("#no_wizard").click(function(){
        console.log("???");
        $("#port_wizard").hide();
        $("#port_manual").show();
    }));
}

function update_ports(devices)
{
    new_ports = [];

    for (id in devices)
    {
        new_ports.push(devices[id])
    }
    
    setTimeout(function() {
        chrome.serial.getDevices(update_ports);
    }, 3000);    
}
