﻿function lightTheme() {
    document.documentElement.style.setProperty("--is-light-theme", "1");
    document.documentElement.style.setProperty("--main-background-color", " #f3f4f6");
    document.documentElement.style.setProperty("--block-background-color", "#fff");
    document.documentElement.style.setProperty("--additional-background-color", "#e5e7eb");
    document.documentElement.style.setProperty("--main-font-color", "#000");
    document.documentElement.style.setProperty("--additional-font-color", "#374151"); 
}

function darkTheme() {
    document.documentElement.style.setProperty("--is-light-theme", "0");
    document.documentElement.style.setProperty("--main-background-color", "#232325");
    document.documentElement.style.setProperty("--block-background-color", "#121212");
    document.documentElement.style.setProperty("--additional-background-color", "#2c2d31");
    document.documentElement.style.setProperty("--main-font-color", "#fff");
    document.documentElement.style.setProperty("--additional-font-color", "#c6ced9");
}
function changeColorTheme() {
    let theme = document.getElementById("color-switcher").getElementsByTagName("input")[0].checked;
    localStorage.setItem("isDarkTheme", theme)

    if (theme) { darkTheme(); }
    else { lightTheme(); }
}


function openMainsidemenu() {
    let obj = document.getElementById("mainsidemenu-detail")
    if (!obj.hasAttribute("open") && window.innerWidth < 1280) {
        console.log("TUT");
        document.body.style.position = 'fixed';
        document.body.style.overflowY = 'hidden';
    } else {
        document.body.style.position = '';
        document.body.style.overflowY = '';
    }
}




function preLoadThemeConfig() {
    let isDarkTheme = (window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches);

    if (localStorage.getItem("isDarkTheme") != null) {
        isDarkTheme = localStorage.getItem("isDarkTheme") == "true";
    }

    if (isDarkTheme) {
        darkTheme();
    } else {
        lightTheme();
    }
}
preLoadThemeConfig()


document.addEventListener('DOMContentLoaded', () => {
    let switcher = document.getElementById("color-switcher").getElementsByTagName("input")[0];
    let isDarkTheme = (window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches);

    if (localStorage.getItem("isDarkTheme") != null) {
        isDarkTheme = localStorage.getItem("isDarkTheme") == "true";
    }

    if (isDarkTheme) {
        switcher.checked = true;
        darkTheme();
    } else {
        switcher.checked = false;
        lightTheme();
    }
});