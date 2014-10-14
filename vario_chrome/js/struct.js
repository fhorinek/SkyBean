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

function get_uint8(data, index)
{
    return data.charCodeAt(index);
}

function get_uint16(data, index)
{
    val = data.charCodeAt(index) << 8;
    val += data.charCodeAt(index + 1);

    return val;
}

function get_float(data, index)
{
    buffer = new ArrayBuffer(4);
    byte = new Uint8Array(buffer);
    byte[3] = data.charCodeAt(index + 0);
    byte[2] = data.charCodeAt(index + 1);
    byte[1] = data.charCodeAt(index + 2);
    byte[0] = data.charCodeAt(index + 3);
    
    console.log(byte);
    
    float = new Float32Array(buffer);
    
    console.log(float);
    
    return float[0];
}

function get_int16(data, index)
{
    buffer = new ArrayBuffer(2);
    byte = new Uint8Array(buffer);
    byte[1] = data.charCodeAt(index + 0);
    byte[0] = data.charCodeAt(index + 1);
    
    int = new Int16Array(buffer);
    
    return int[0];
}

function get_int16_array(data, index, len)
{
    val = Array(len);

    for (i = 0; i < len; i++)
    {
        val[i] = get_int16(data, index + i * 2);
    }
    
    return val;
}

function get_char_array(data, index, len)
{
    val = "";

    for (i = 0; i < len; i++)
    {
        val += data[i]
    }
    
    return val;
}

function decode_cfg(data)
{
    cfg = new Object();
    
    cfg.buzzer_volume = get_uint8(data, 0);
    cfg.supress_startup = get_uint8(data, 1);
    
    cfg.auto_poweroff = get_uint16(data, 2);
    
    cfg.serial_output = get_uint8(data, 4);
    cfg.selected_profile = get_uint8(data, 5);
    
    cfg.lift_steps = get_int16_array(data, 6, 5);
    cfg.sink_steps = get_int16_array(data, 16, 5);
    
    cfg.kalman_q = get_float(data, 26);
    cfg.kalman_r = get_float(data, 30);
    cfg.kalman_p = get_float(data, 34);

    cfg.int_interval = get_uint8(data, 38);
    
    return cfg;
}

function decode_prof(data, i)
{
    prof = new Object();
    
    offset = 265 * i;
    
    prof.name = get_char_array(data, offset + 0, 16);
    
    prof.buzzer_freq = get_int16_array(data, offset + 16, 41);
    prof.buzzer_pause = get_int16_array(data, offset + 98, 41);
    prof.buzzer_length = get_int16_array(data, offset + 180, 41);

    prof.lift_treshold = get_uint8(data, offset + 262);
    prof.sink_treshold = get_uint8(data, offset + 263);
    
    prof.enabled = get_uint8(data, offset + 264);
    
    return prof;
}
