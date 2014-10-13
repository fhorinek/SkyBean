function create_profile()
{
    prof = new Object();
    
    prof.name = "";
    
    prof.buzzer_freq = new Array(41);
    prof.buzzer_pause = new Array(41);
    prof.buzzer_length = new Array(41);
    
    prof.lift_treshold = 0;
    prof.sink_treshold = 0;
    
    prof.enabled = 0;
    
    return prof;
}

function create_configuration()
{
    cfg = new Object();
    
    cfg.buzzer_volume = 0;
    cfg.supress_startup = 0;
    
    cfg.auto_poweroff = 0;
    
    cfg.serial_output = 0;
    cfg.selected_profile = 0;
    
    cfg.lift_steps = new Array(5);
    cfg.sink_steps = new Array(5);
    
    cfg.kalman_q = 0;
    cfg.kalman_r = 0;
    cfg.kalman_p = 0;

    cfg.int_interval = 0;
}
