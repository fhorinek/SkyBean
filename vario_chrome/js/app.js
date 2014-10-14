
tabs = [];

function add_tab(name, label, icon_name)
{
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
    $(new_menu).attr("id", "tab_" + name);
    $(new_menu).append(inner);
    $(new_menu).addClass("tab");
    $(new_menu).click(function () {
        set_page(name);
    });

    $("#menu").append(new_menu);
    
    page = document.createElement("div");
    $(page).attr("id", "page_" + name);
    $(page).load("tabs/" + name + ".html");

    $("#page").append(page);
    $(page).hide();
    
    tabs.push(name);
}

function set_page(name)
{
    for (id in tabs)
        if (tabs[id] == name)
        {
            $("#tab_" + tabs[id]).addClass("tab_active");
            $("#page_" + tabs[id]).show(0, function() {show_page(name)});
        }
        else
        {
            $("#tab_" + tabs[id]).removeClass("tab_active");    
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

function show_page(name)
{
        
    switch (name)
    {
        case("home"):
           // tab_home_init();
        break;
        case("settings"):
            tab_settings_show();
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
    add_tab("profile", "Profile", "bar-chart");
    
    set_page("home");
});
