function tab_settings_init()
{
    //$('#set_auto_enable').selectmenu();
    $('#set_auto_value').spinner({
      step: 1,
      max: 60,
      min: 0
    });
    $('#set_profile').selectmenu();
    $('.set_steps').spinner({
        step: 0.1,
        min: -10.0,
        max: 10.0,
        numberFormat: "n"
    });

    $('#set_volume1').click(function (){set_volume(1);});
    $('#set_volume2').click(function (){set_volume(2);});
    $('#set_volume3').click(function (){set_volume(3);});
    $('#set_volume4').click(function (){set_volume(4);});
    
    $('#set_auto_enable').change(function (){set_auto_poweroff();});
}

function set_volume(val)
{
    for (i = 1; i < 5; i++)
    {
        if (val >= i)
            $('#set_volume' + i).removeClass("set_value_inactive");
        else
            $('#set_volume' + i).addClass("set_value_inactive");
    }
}

function set_auto_poweroff()
{
    if ($('#set_auto_enable').is(":checked"))
    {
        $('#set_auto_value').spinner("enable");
        $('#set_auto_value').val(actual_cfg.auto_poweroff);
    }
    else
    {
        $('#set_auto_value').val("disabled");
        $('#set_auto_value').spinner("disable");
    }
}

function tab_settings_show()
{
    set_volume(actual_cfg.buzzer_volume);
    
    $('#set_auto_enable').prop("checked", actual_cfg.auto_poweroff != 0);
    set_auto_poweroff();
    
    $("#set_lift_1").val(actual_cfg.lift_steps[0]);
    $("#set_lift_2").val(actual_cfg.lift_steps[1]);
    $("#set_lift_3").val(actual_cfg.lift_steps[2]);
    $("#set_lift_4").val(actual_cfg.lift_steps[3]);
    $("#set_lift_5").val(actual_cfg.lift_steps[4]);

    $("#set_sink_1").val(actual_cfg.sink_steps[0]);
    $("#set_sink_2").val(actual_cfg.sink_steps[1]);
    $("#set_sink_3").val(actual_cfg.sink_steps[2]);
    $("#set_sink_4").val(actual_cfg.sink_steps[3]);
    $("#set_sink_5").val(actual_cfg.sink_steps[4]);

}
