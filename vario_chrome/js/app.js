
tabs = [];

function add_tab(name, label_arr, icon_name)
{
    if (typeof label_arr == "string")
        label_arr = [label_arr]

    for (label_arr_key in label_arr)
    {
        label = label_arr[label_arr_key];

        icon = document.createElement("i");
        $(icon).addClass("fa");
        $(icon).addClass("fa-2x");    
        $(icon).addClass("fa-" + icon_name);    

        inner = document.createElement("div");
        $(inner).append(icon);
        $(inner).append(document.createElement("br"));
        $(inner).append(label);
        $(inner).addClass("valign");

        new_menu = document.createElement("div");
        $(new_menu).attr("id", "tab_" + name + label_arr_key);
        $(new_menu).append(inner);
        $(new_menu).addClass("tab");

        $(new_menu).click({key: label_arr_key, name: name}, function (e) {
            set_page(e.data.name, e.data.key);
        });

        $("#menu").append(new_menu);
    }
    
    page = document.createElement("div");
    $(page).attr("id", "page_" + name);
    $(page).load("tabs/" + name + ".html");

    $("#page").append(page);
    $(page).hide();
    
    tabs.push(name);
}



function set_page(name, key)
{
    $("[id^=tab]").removeClass("tab_active");    
    $("#tab_" + name + key).addClass("tab_active");

    for (id in tabs)
        if (tabs[id] == name)
        {
            
            $("#page_" + tabs[id]).show(0, function() {show_page(name, key)});
        }
        else
        {
            $("#page_" + tabs[id]).hide(0);
        }
}


function init_page(name)
{
    switch (name)
    {
        case("home"):
            tab_home_init();
        break;
        case("profile"):
            tab_profile_init();
        break;
        case("settings"):
            tab_settings_init();
        break;    
    }    

}

function show_page(name, opt)
{
        
    switch (name)
    {
        case("home"):
           // tab_home_init();
        break;
        case("settings"):
            tab_settings_show();
        break;
        case("profile"):
            tab_profile_show(opt);
        break;
    }        
}


$(document).ready(function(){
    $(document).ajaxComplete(function(event, xhr, settings){
        name = settings.url.replace("tabs/", "").replace(".html", "");
        init_page(name);
    });

    add_tab("home", "Home", "home");
    add_tab("settings", "Settings", "gear");
    add_tab("profile", ["Profile 1", "Profile 2", "Profile 3"], "bar-chart");
    
    set_page("home", 0);
});
