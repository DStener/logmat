
class ASidePanel {

    static setHeader(text)
    {
        document.getElementById("menu-title").innerHTML = text;
    }

    static addLink(text, link, bold = false)
    {
        let panel = document.getElementById("ASidePanel");
        let a = document.createElement("a");

        a.className = "submenu-link";
        a.innerHTML = text;
        a.href = link;

        panel.appendChild(a);

        return a


        //`<a href = "{%item.href%}" class="submenu-link {%item.add_class%}" onclick = "{%item.onclick%}" >
        //    <i class="{%item.icon%}"></i>
                
        //</a>`

    }
}

window.addEventListener("load", function () {

}, false);