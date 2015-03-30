function tab_settings_init()
{
    $('#set_auto_value').spinner({
      step: 1,
      max: 60,
      min: 0
    });
    // $('#set_profile').selectmenu();
    $('.set_steps').spinner({
        step: 0.1,
        min: -10.0,
        max: 10.0,
        numberFormat: "f"
    });

    $('#set_fluid_value').spinner({
      step: 1,
      max: 255,
      min: 0
    });

    $('#set_volume1').click(function (){set_volume(1);});
    $('#set_volume2').click(function (){set_volume(2);});
    $('#set_volume3').click(function (){set_volume(3);});
    $('#set_volume4').click(function (){set_volume(4);});
    
    $('#set_auto_enable').change(function (){set_auto_poweroff();});
    $("#set_auto_value").on("spinchange", function (){set_auto_poweroff_value(this)});

    $("#set_profile").on("change", function (){set_actual_profile(this)});

    $("#set_lift_1").on("spinchange", function (){set_treshold(true, 0, this)});
    $("#set_lift_2").on("spinchange", function (){set_treshold(true, 1, this)});
    $("#set_lift_3").on("spinchange", function (){set_treshold(true, 2, this)});
    $("#set_lift_4").on("spinchange", function (){set_treshold(true, 3, this)});
    $("#set_lift_5").on("spinchange", function (){set_treshold(true, 4, this)});

    $("#set_sink_1").on("spinchange", function (){set_treshold(false, 0, this)});
    $("#set_sink_2").on("spinchange", function (){set_treshold(false, 1, this)});
    $("#set_sink_3").on("spinchange", function (){set_treshold(false, 2, this)});
    $("#set_sink_4").on("spinchange", function (){set_treshold(false, 3, this)});
    $("#set_sink_5").on("spinchange", function (){set_treshold(false, 4, this)});

    $('#set_fluid_enable').change(function (){set_fluid();});

    $('#set_supress_startup').change(function (){set_supress_startup();});
//    $("#set_serial").on("change", function (){set_serial_output(this)});
}

function set_supress_startup()
{
    if ($('#set_supress_startup').is(":checked"))
        actual_cfg.supress_startup = 1;
    else
        actual_cfg.supress_startup = 0;
}

function set_fluid()
{
    if ($('#set_fluid_enable').is(":checked"))
    {
         actual_cfg.fluid_update = 1;
    }
    else
    {
         actual_cfg.fluid_update = 0;
    }
}


function set_actual_profile(obj)
{
    actual_cfg.selected_profile = parseInt($(obj).val());
}

function set_serial_output(obj)
{
    actual_cfg.serial_output = parseInt($(obj).val());
}

function set_treshold(lift, index, obj)
{
    if (lift)
        actual_cfg.lift_steps[index] = $(obj).val() * 100;
    else
        actual_cfg.sink_steps[index] = $(obj).val() * 100;
}

function set_volume(val)
{
    actual_cfg.buzzer_volume = val;

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
        if (actual_cfg.auto_poweroff == 0)
            actual_cfg.auto_poweroff = 300;
        $('#set_auto_value').val(actual_cfg.auto_poweroff / 60);
    }
    else
    {
        $('#set_auto_value').val("disabled");
        $('#set_auto_value').spinner("disable");
        actual_cfg.auto_poweroff = 0;
    }
}

function set_auto_poweroff_value(obj)
{
    actual_cfg.auto_poweroff = $(obj).val() * 60;
    if (actual_cfg.auto_poweroff == 0)
    {
        $('#set_auto_enable').prop("checked", false);
        set_auto_poweroff();
    }
}

function tab_settings_show()
{
    set_volume(actual_cfg.buzzer_volume);
    
    $('#set_auto_enable').prop("checked", actual_cfg.auto_poweroff != 0);
    set_auto_poweroff();
    
    set_fluid();
    set_supress_startup();
    
    $("#set_lift_1").val(actual_cfg.lift_steps[0] / 100);
    $("#set_lift_2").val(actual_cfg.lift_steps[1] / 100);
    $("#set_lift_3").val(actual_cfg.lift_steps[2] / 100);
    $("#set_lift_4").val(actual_cfg.lift_steps[3] / 100);
    $("#set_lift_5").val(actual_cfg.lift_steps[4] / 100);

    $("#set_sink_1").val(actual_cfg.sink_steps[0] / 100);
    $("#set_sink_2").val(actual_cfg.sink_steps[1] / 100);
    $("#set_sink_3").val(actual_cfg.sink_steps[2] / 100);
    $("#set_sink_4").val(actual_cfg.sink_steps[3] / 100);
    $("#set_sink_5").val(actual_cfg.sink_steps[4] / 100);
    
    $("#set_profile").empty();
    for (i = 0; i < 3; i++)
    {
        if (actual_prof[i].enabled)
        {
            option = document.createElement("option");
            $(option).html((i + 1) + ": " + actual_prof[i].name);
            $(option).val(i);
            $("#set_profile").append(option);
        }
    }

    $("#set_profile").val(actual_cfg.selected_profile);
    $("#set_serial").val(actual_cfg.serial_output);
}
