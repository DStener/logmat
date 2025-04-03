
class ASidePanel {

    static setHeader(text)
    {
        document.getElementById("aside-title").innerHTML = text;
    }

    static addLink(text, link, bold = false)
    {
        let panel = document.getElementById("ASidePanel");
        let li = document.createElement("li");
        let a = document.createElement("a");

        li.className = (bold) ? "menu-title" : "menu-item";

        a.className = "submenu-link";
        a.innerHTML = text;
        a.href = link;

        panel.appendChild(li);
        li.appendChild(a);

        return a
    }
}

window.addEventListener("load", function () {

}, false);