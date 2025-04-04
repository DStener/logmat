
async function configureGellery()
{
    try {
        const response = await fetch("/api/ref/file?tags=jpg");
        let json = await response.json();

        console.log(json);
        let gallery = document.getElementById("gallery-block");

        if (!gallery) { return; }

        json.va
        Object.keys(json).forEach((id_row) => {
            gallery.innerHTML += `
            <a data-fancybox="images" href="/api/ref/file/${json[id_row]["id"]}"  data-caption="${json[id_row]["description"]}"><img src="/api/ref/file/${json[id_row]["id"]}" width="200" height="222"/></a>
            `
        });
    } catch (e) { console.error(e); }
}


window.addEventListener("load", function () {

    


    configureGellery();

}, false);
