function profile_get_near(findex, src)
{
    index = Math.floor(findex) + 20;
    m = findex - Math.floor(findex);
    if (index < 0)
    {
        index = 0;
        m = 0;
    }
    
    if (index > 41)
    {
        index = 41;
        m = 1;
    }
    
    start = src[index];
    diff = src[index + 1] - start;
    
    return start + diff * m;
}

var profile_id = 0;


function profile_redraw()
{
    var plot_freq = [];
    var plot_leng = [];
    var plot_paus = [];

    for (i = -20; i <= 20; i++) // step is 0.5m
    {
        plot_freq.push([i / 2, actual_prof[profile_id].buzzer_freq[i + 20]]);
        plot_leng.push([i / 2, actual_prof[profile_id].buzzer_length[i + 20]]);
        plot_paus.push([i / 2, actual_prof[profile_id].buzzer_pause[i + 20]]);
    }  

    var plot = $.plot("#plot", [
            {data: plot_freq, label: "Frequency [Hz]"},
            {data: plot_leng, label: "Length [ms]", yaxis: 2},
            {data: plot_paus, label: "Pause [ms]", yaxis: 2}
        ],
        {
	        xaxes: [ {  } ],
	        yaxes: [ 
	            { min: 0 }, 
	            {
		        // align if we are to the right
		        alignTicksWithAxis: 0.5,
		        position: "right",
	        } ],
	        legend: { position: "nw" },
            series: {
                lines: {
                    show: true
                },
                points: {
                    show: true
                }
            },
            grid: {
                hoverable: true,
                clickable: true,
                autoHighlight: false
            }
        });


    for (i in point_lock_freq)
        plot.highlight(0, point_lock_freq[i]);
    for (i in point_lock_length)
        plot.highlight(1, point_lock_length[i]);
    for (i in point_lock_pause)
        plot.highlight(2, point_lock_pause[i]);

}

function find_index(right, point_index, series_index)
{
    res = Array();

    id = point_index;

    if (series_index == 0)
        lock = point_lock_freq;
    if (series_index == 1)
        lock = point_lock_length;
    if (series_index == 2)
        lock = point_lock_pause;

    while(true)
    {
        id = id + (right ? 1 : -1);
        
        if (id < 0 || id > 40)
            break;

        res.push(id);
        
        state = lock.indexOf(id) != -1;
        if (state)
            break;
    }

    console.log(res);
    return res;
}

var point_lock_freq;
var point_lock_length;
var point_lock_pause;

function profile_init_plot()
{
	var on_point = false;
	var clicked = false;
	var series_index;
	var point_index;
	var data_x;
	var data_y;

	point_lock_freq = [5, 10, 15, 20, 25, 30, 35];    
    point_lock_length = [5, 10, 15, 20, 25, 30, 35];
    point_lock_pause = [5, 10, 15, 20, 25, 30, 35];

    $("#plot").bind("mousedown", function (event, pos, item) {
        if (on_point)
        {
            clicked = true;
        }
    });
	    
    $("#plot").bind("mouseup", function (event, pos, item) {
        clicked = false;
    });
	    
	    
    $("#plot").bind("plothover", function (event, pos, item) {
        if (clicked)
        {
            if (series_index == 0)
                pos_y = Math.round(pos.y);
            else
                pos_y = Math.round(pos.y2);
        
            if (pos_y > 2000)
                pos_y = 2000;
            if (pos_y < 0)
                pos_y = 0;
        

            if (series_index == 0)
                field = actual_prof[profile_id].buzzer_freq;
            if (series_index == 1)
                field = actual_prof[profile_id].buzzer_length;
            if (series_index == 2)
                field = actual_prof[profile_id].buzzer_pause;

            console.log("point is:" + point_index);
            left_index = find_index(false, point_index, series_index);
            right_index = find_index(true, point_index, series_index);

            field[point_index] = pos_y;

            start_i = left_index.pop();
            a = field[start_i];
            b = field[point_index];
            diff = b - a;

            for (i in left_index)
            {
                //"1" + 1 == "11"
                //"1" - 1 == 0
                m = (parseInt(i) + 1) / (left_index.length + 1)
                field[left_index[i]] = a + diff * (1-m);
            }

            start_i = right_index.pop();
            a = field[start_i];
            b = field[point_index];
            diff = b - a;

            for (i in right_index)
            {
                m = (parseInt(i) + 1) / (right_index.length + 1)
                field[right_index[i]] = a + diff * (1-m);
            }

            if (series_index == 0)
                lock = point_lock_freq;
            if (series_index == 1)
                lock = point_lock_length;
            if (series_index == 2)
                lock = point_lock_pause;

            if (lock.indexOf(point_index) == -1)
            {
                lock.push(point_index);
                console.log(lock);
            }
                
            profile_redraw(point_index);
        }
    
        if (item && !clicked)
        {
            on_point = true;
            series_index = item.seriesIndex;
            point_index = item.dataIndex;
        }
        else
        {
            on_point = false;
        }
   });
   
   profile_redraw();
}

function profile_resize()
{
    $("#plot").empty();
    profile_redraw();
}

function tab_profile_init()
{
    profile_init_plot()

    var resizeTimer;


    $("#profile_enable").click(function(){
        actual_prof[profile_id].enabled = $("#profile_enable").is(":checked");
    });

    $("#profile_name").change( function(){
        name = $("#profile_name").val()
        name.slice(0,16);
        $("#profile_name").val(name);

        actual_prof[profile_id].name = name;
    });

    $("#profile_lift").on("change", function (){
        actual_prof[profile_id].lift_treshold = $(this).val();
    });

    $("#profile_sink").on("change", function (){
        actual_prof[profile_id].sink_treshold = $(this).val();
    });

    $(window).resize(function (){
        if ($("#page_profile").is(":visible"))
        {
            clearTimeout(resizeTimer);
            resizeTimer = setTimeout(profile_resize, 100);    
        }
    });
}

function tab_profile_show(opt)
{
    profile_id = opt;

    profile_resize();

    $("#profile_name").val(actual_prof[profile_id].name);
    $('#profile_enable').prop("checked", actual_prof[profile_id].enabled);

    $("#profile_lift").empty();
    for (i = 0; i < 5; i++)
    {
        option = document.createElement("option");
        $(option).html((actual_cfg.lift_steps[i] / 100) + " m/s");
        $(option).val(i);
        $("#profile_lift").append(option);
    }

    $("#profile_lift").val(actual_prof[profile_id].lift_treshold);    

    $("#profile_sink").empty();
    for (i = 0; i < 5; i++)
    {
        option = document.createElement("option");
        $(option).html((actual_cfg.sink_steps[i] / 100) + " m/s");
        $(option).val(i);
        $("#profile_sink").append(option);
    }

    $("#profile_sink").val(actual_prof[profile_id].sink_treshold);        
}