function formEncode(form) {
    var requestBodyArray = [];

    // Iterate through the FormData entries
    for (var pair of form.entries()) {
        if (!pair[1].length) { continue; }
        requestBodyArray.push(encodeURIComponent(pair[0]) + '=' + encodeURIComponent(pair[1]));
    }
    return requestBodyArray.join('&');
}

function clearContent() {
    document.querySelectorAll('tr.tb-line').forEach(line => {
        line.remove();
    });

    document.querySelectorAll('table.admin-table tr th').forEach(th => {
        th.remove();
    });

    document.querySelectorAll('.button-line *').forEach(button => {
        button.remove();
    });
}

function getLink(text, delete_param = false) {
    text = ((text.startsWith("http")) ? "" : document.location.origin) + text;
    let url = new URL(text);

    if (!delete_param) {
        return url.pathname + url.search;
    }
    else {
        return url.pathname;
    }
}

function hideForm() {
    console.log("clicl");
    document.getElementById("parent_popup").style.display = "none";
    document.getElementById("popup_form_form").innerHTML = "";
}

async function ShowDialog()
{
    let link = "/api/ref/file"
    let isNew = true;
    document.getElementById("parent_popup").style.display = "block";

    let row, id;

    if (isNew) {
        document.getElementById("popup_form_header").innerHTML = "Новая запись";
    }
    else {
        document.getElementById("popup_form_header").innerHTML = "Правка записи";
        document.querySelectorAll('.tb-line').forEach(r => {
            if (r.classList.contains("cropped")) { return; }
            row = r;
        });

        row.querySelectorAll(".DB-id").forEach(tr => {
            id = tr.innerHTML;
        });
    }

    console.log(id);

    document.getElementById("popup_form_form").innerHTML = "";

    try {
        const response = await fetch(`${getLink(link, true)}/form`);
        let json = await response.json();


        console.log(json);
        let field_file = "";

        Object.keys(json).forEach(cell => {

            let value = ""
            if (!isNew) {
                row.querySelectorAll(`.DB-${cell}`).forEach(tr => {
                    value = tr.innerHTML;
                });
            }
            if (json[cell]["type"] == "file") { field_file = cell; }

            document.getElementById("popup_form_form").innerHTML += `
            <label for="${cell}">${cell}</label>
            <input type="${json[cell]["type"]}" name="${cell}" id="${cell}" value="${value}" ${json[cell]["required"]}/>
            `
        });

        let div = document.createElement("div");
        div.classList.add("button-line")

        let button = document.createElement("input");
        let submit = document.createElement("input");

        button.type = "button";
        button.classList.add("previous");
        button.onclick = hideForm;
        button.value = "Отмена"

        submit.type = "submit";
        submit.classList.add("next");
        submit.value = "Отправить"
        submit.onclick = (event) => {
            event.preventDefault();

            let form = new FormData(document.getElementById("popup_form_form"));
            let form_body = formEncode(form);
            if (field_file.length == 0) {
                try {


                    if (isNew) {
                        const response = fetch(`${getLink(link, true)}/`, { method: "POST", body: form_body });
                    }
                    else {
                        const response = fetch(`${getLink(link, true)}/${id}`, { method: "PUT", body: form_body });
                        console.log(response);
                    }

                    clearContent();
                    hideForm();

                    document.querySelectorAll('a.current').forEach(a => {
                        configureContent(a.innerHTML, getLink(a.href));
                    });

                } catch (e) { console.error(e); }
            }
            else {
                let fileObj = document.getElementById(field_file).files[0]; // js get file object
                let url = getLink(link, true) + '/';
                if (!isNew) { url += `${id}`; }

                form.delete(field_file);
                form.append(field_file, fileObj);


                xhr = new XMLHttpRequest();  // XMLHttpRequest object
                xhr.open("post", url, true); //post
                xhr.onload = () => {

                    console.log(xhr.response);

                    document.getElementById("big-avatar").src = "";
                    document.getElementById("big-avatar").src = `/api/ref/file/${xhr.response}/avatar`;
                    document.getElementById("avatar-field").value = xhr.response;

                    clearContent();
                    hideForm();

                   
                };
                // xhr.onerror =  uploadFailed; 

                // xhr.upload.onprogress = progressFunction;
                // xhr.upload.onloadstart = function(){
                //     ot = new Date().getTime();
                //     oloaded = 0;
                // };

                xhr.send(form);
            }
        }
        document.getElementById("popup_form_form").appendChild(div);

        div.appendChild(button);
        div.appendChild(submit);

    } catch (e) { console.error(e); }
}

window.addEventListener("load", function () {
}, false);