var vario_freq = [127, 131.5, 136, 141, 146, 152.5, 159, 167, 175, 186.5, 198, 216, 234, 258.5, 283, 313.5, 344, 379.5, 415, 489.5, 564, 632.5, 701, 744.5, 788, 817, 846, 870, 894, 910.5, 927, 941, 955, 970, 985, 996.5, 1008, 1022.5, 1037, 1053.5, 1070,];
var vario_leng = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 540, 489, 438, 403, 368, 340, 312, 285.5, 259, 239, 219, 197.5, 176, 157, 138, 124, 110, 95.5, 81, 70.5, 60,];
var vario_paus = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 320, 281, 242, 215.5, 189, 172, 155, 144.5, 134, 124.5, 115, 105, 95, 85, 75, 65, 55, 46, 37, 33.5, 30,];

function profile_get_near(findex, src)
{
    index = Math.floor(findex) + 12;
    m = findex - Math.floor(findex);
    if (index < 0)
    {
        index = 0;
        m = 0;
    }
    
    if (index > 23)
    {
        index = 23;
        m = 1;
    }
    
    start = src[index];
    diff = src[index + 1] - start;
    
    return start + diff * m;
}



function profile_redraw()
{
    var plot_freq = [];
    var plot_leng = [];
    var plot_paus = [];

    for (i = -20; i <= 20; i++) // step is 0.5m
    {
        plot_freq.push([i / 2, vario_freq[i + 20]]);
        plot_leng.push([i / 2, vario_leng[i + 20]]);
        plot_paus.push([i / 2, vario_paus[i + 20]]);
    }  

    $.plot("#plot", [
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
                clickable: true
            }
        });

}

function tab_profile_init()
{
    profile_redraw();
        
	var on_point = false;
	var clicked = false;
	var series_index;
	var point_index;
	var data_x;
	var data_y;
	    
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
                vario_freq[point_index] = pos_y;
            if (series_index == 1)
                vario_leng[point_index] = pos_y;
            if (series_index == 2)
                vario_paus[point_index] = pos_y;
                
            profile_redraw(point_index);
        }
    
        if (item && !clicked)
        {
            on_point = true;
            series_index = item.seriesIndex;
            point_index = item.dataIndex;

        }
        else
            on_point = false;
   });

    row = document.createElement("tr");
    for (i = 0; i < 41; i++)
    {
        td = document.createElement("td");
        button = document.createElement("input");
        $(button).attr("type", "button");
        $(button).attr("value", "+");      
        $(td).append(button);  
        
        $(row).append(td);
    }
    
    $("#plot_table").append(row);
    
}

