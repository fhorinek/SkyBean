var CRC_KEY = 0xAB;
var CFG_LEN = 49
var PROF_LEN = 271
var STRUCT_LEN = (CFG_LEN + PROF_LEN * 3)

function CalcCRC(crc, key, data)
{
    for (crc_i=0; crc_i<8; crc_i++)
    {
        if ((data & 0x01)^(crc & 0x01))
        {
            crc = crc >> 1;
            crc = crc ^ key;
        }
        else
            crc = crc >> 1;
        data = data >> 1;
    }

    return crc;
}

function default_prof()
{
    prof = new Object();
    
    prof.name = "Default";
    
    prof.buzzer_freq = [127, 131.5, 136, 141, 146, 152.5, 159, 167, 175, 186.5, 198, 216, 234, 258.5, 283, 313.5, 344, 379.5, 415, 489.5, 564, 632.5, 701, 744.5, 788, 817, 846, 870, 894, 910.5, 927, 941, 955, 970, 985, 996.5, 1008, 1022.5, 1037, 1053.5, 1070,];
    prof.buzzer_pause = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 540, 489, 438, 403, 368, 340, 312, 285.5, 259, 239, 219, 197.5, 176, 157, 138, 124, 110, 95.5, 81, 70.5, 60,];
    prof.buzzer_length = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 320, 281, 242, 215.5, 189, 172, 155, 144.5, 134, 124.5, 115, 105, 95, 85, 75, 65, 55, 46, 37, 33.5, 30,];
    
    prof.lift_treshold = 0;
    prof.sink_treshold = 0;
    
    prof.enabled = 1;
    
    prof.reserved = [0, 0, 0, 0, 0, 0];

    return prof;
}

function default_cfg()
{
    cfg = new Object();
    
    cfg.buzzer_volume = 4;
    cfg.supress_startup = 0;
    
    cfg.auto_poweroff = 5 * 60;
    
    cfg.serial_output = 0;
    cfg.selected_profile = 0;
    
    cfg.lift_steps = [10, 20, 30, 40, 50];
    cfg.sink_steps = [-50, -100, -150, -200, -250];
    
    cfg.kalman_q = 0.001;
    cfg.kalman_r = 6.0;
    cfg.kalman_p = 30.0;

    cfg.int_interval = 25;
    
    cfg.fluid_update = 0;
    
    cfg.reserved = [0, 0, 0, 0, 0, 0, 0, 0, 0];

    return cfg;
}

function get_uint8(data, index)
{
    return data.charCodeAt(index);
}

function set_uint8(data)
{
    return String.fromCharCode(data)
}

function get_uint16(data, index)
{
    val = data.charCodeAt(index);
    val += data.charCodeAt(index + 1) << 8;

    return val;
}

function set_uint16(data)
{
    val = "";
    val += String.fromCharCode((data & 0x00FF))
    val += String.fromCharCode((data & 0xFF00) >> 8)

    return val;
}

function get_float(data, index)
{
    buffer = new ArrayBuffer(4);
    byte = new Uint8Array(buffer);
    byte[0] = data.charCodeAt(index + 0);
    byte[1] = data.charCodeAt(index + 1);
    byte[2] = data.charCodeAt(index + 2);
    byte[3] = data.charCodeAt(index + 3);
    
    float = new Float32Array(buffer);
    
    return float[0];
}

function set_float(data)
{
    buffer = new ArrayBuffer(4);
    byte = new Uint8Array(buffer);
    float = new Float32Array(buffer);
    
    float[0] = data
    
    val = "";
    
    val += String.fromCharCode(byte[0]);
    val += String.fromCharCode(byte[1]);
    val += String.fromCharCode(byte[2]);
    val += String.fromCharCode(byte[3]);
    
    return val;
}

function get_int16(data, index)
{
    buffer = new ArrayBuffer(2);
    byte = new Uint8Array(buffer);
    byte[0] = data.charCodeAt(index + 0);
    byte[1] = data.charCodeAt(index + 1);
    
    int = new Int16Array(buffer);
    
    return int[0];
}

function set_int16(data)
{
    buffer = new ArrayBuffer(2);
    byte = new Uint8Array(buffer);
    int = new Int16Array(buffer);

    int[0] = data

    val = "";
    
    val += String.fromCharCode(byte[0])
    val += String.fromCharCode(byte[1])
    
    return val;
}

function get_int16_array(data, _index, len)
{
    val_arr = Array(len);

    for (i = 0; i < len; i++)
    {
        val_arr[i] = get_int16(data, _index + i * 2);
    }
    
    return val_arr;
}

function set_int16_array(data, len)
{
    val = "";

    for (i = 0; i < len; i++)
    {
        val += set_int16(data[i]);
    }
    
    return val;
}

function get_uint16_array(data, _index, len)
{
    val_arr = Array(len);

    for (i = 0; i < len; i++)
    {
        val_arr[i] = get_uint16(data, _index + i * 2);
    }
    
    return val_arr;
}

function set_uint16_array(data, len)
{
    val = "";

    for (i = 0; i < len; i++)
    {
        val += set_uint16(data[i]);
    }
    
    return val;
}

function get_char_array(data, index, len)
{
    val = "";

    for (i = 0; i < len; i++)
    {
        val += data[index + i];
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

    cfg.fluid_update = get_uint8(data, 39);
    
    return cfg;
}

function decode_prof(data, i)
{
    prof = new Object();
    
    offset = PROF_LEN * i + CFG_LEN;
    
    prof.name = get_char_array(data, offset + 0, 16);
    
    prof.buzzer_freq = get_uint16_array(data, offset + 16, 41);
    prof.buzzer_pause = get_uint16_array(data, offset + 98, 41);
    prof.buzzer_length = get_uint16_array(data, offset + 180, 41);

    prof.lift_treshold = get_uint8(data, offset + 262);
    prof.sink_treshold = get_uint8(data, offset + 263);
    
    prof.enabled = get_uint8(data, offset + 264);
    
    return prof;
}

function encode_cfg(data)
{
    data = "";
    
    data += set_uint8(cfg.buzzer_volume);
    data += set_uint8(cfg.supress_startup);
    
    data += set_uint16(cfg.auto_poweroff);

    data += set_uint8(cfg.serial_output);
    data += set_uint8(cfg.selected_profile);
    
    data += set_int16_array(cfg.lift_steps, 5);
    data += set_int16_array(cfg.sink_steps, 5);
    
    data += set_float(cfg.kalman_q);
    data += set_float(cfg.kalman_r);
    data += set_float(cfg.kalman_p);

    data += set_uint8(cfg.int_interval);

    data += set_uint8(cfg.fluid_update);
    
    data += "*RESERVE*" //reserved

    return data;
}

function encode_prof(prof)
{
    data = "";   

    for (i = 0; i < 16; i++)
    {
        c = prof.name[i];
        if (c == undefined)
            c = '\0';
        data += c
    }
    
    data += set_uint16_array(prof.buzzer_freq, 41);
    data += set_uint16_array(prof.buzzer_pause, 41);
    data += set_uint16_array(prof.buzzer_length, 41);

    data += set_uint8(prof.lift_treshold);
    data += set_uint8(prof.sink_treshold);
 
    data += set_uint8(prof.enabled);   

    data += "SKBEAN" //reserved
    
    return data;
}


