
function formEncode(form)
{
    var requestBodyArray = [];

    // Iterate through the FormData entries
    for (var pair of form.entries()) {
        if(!pair[1].length) { continue; }
        requestBodyArray.push(encodeURIComponent(pair[0]) + '=' + encodeURIComponent(pair[1]));
    }
    return requestBodyArray.join('&');
}

function clearContent()
{
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

function createButton(text)
{
    let button = document.createElement("input");
    document.querySelectorAll('.button-line').forEach(line => {
        button.type = "button";
        button.value = text;

        line.appendChild(button);
        return;
    });
    return button;
}


async function configureLine()
{
    document.querySelectorAll('.tb-line').forEach(row => {
        row.addEventListener('click', () => {

            document.querySelectorAll('input.changed').forEach(input => {
                input.disabled = true;
            });

            document.querySelectorAll('tr:not(.cropped)').forEach(notcrop => {
                if (row == notcrop) { return; }
                notcrop.classList.add("cropped");
            });

            if (row.classList.contains("cropped")) {
                document.querySelectorAll('input.changed').forEach(input => {
                    input.disabled = false;
                });
                row.classList.remove("cropped");
            }
            else 
            {
                row.classList.add("cropped");
            }
        });
    });
}

async function restoreLog()
{
    let id = "";

    document.querySelectorAll('.tb-line').forEach(row => {
        if (row.classList.contains("cropped")) { return; }

        for (const child of row.children) {
            if (!child.classList.contains("DB-id")) { continue; }
            id = child.innerHTML;   
        }
    });

    try {
        const response = await fetch(`/api/log/${id}/restore`, {method: "POST"});
        
        clearContent();
        document.querySelectorAll('a.current').forEach(a => {
            let url = new URL(a.href);
            configureContent(a.innerHTML, url.pathname);
        });
        

    } catch (e) {  console.error(e); }

}

async function showForm(link, isNew = true)
{
    document.getElementById("parent_popup").style.display = "block";

    let row, id;

    if(isNew)
    {
        document.getElementById("popup_form_header").innerHTML = "Новая запись";
    }
    else 
    {
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
        const response = await fetch(`${link}/form`);
        let json = await response.json();
        
        
        console.log(json);
        let field_file = "";

        Object.keys(json).forEach(cell => {

            let value = ""
            if(!isNew)
            {
                row.querySelectorAll(`.DB-${cell}`).forEach(tr => {
                    value = tr.innerHTML;
                });
            }
            if(json[cell]["type"] == "file") { field_file = cell;}

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
            if(field_file.length == 0)
            {
                try {
                    
                    
                    if (isNew)
                    {
                        const response = fetch(`${link}/`, {method: "POST", body: form_body});
                    }
                    else
                    {
                        const response = fetch(`${link}/${id}`, {method: "PUT", body: form_body});
                        console.log(response);
                    }
                    
                    clearContent();
                    hideForm();

                    document.querySelectorAll('a.current').forEach(a => {
                        let url = new URL(a.href);
                        configureContent(a.innerHTML, url.pathname);
                    });

                } catch (e) {  console.error(e); }
            }
            else
            {
                let fileObj = document.getElementById(field_file).files[0]; // js get file object
                let url =  `${link}/`; 
                if (!isNew) { url += `${id}`; }

                form.delete(field_file);
                form.append(field_file, fileObj);


                xhr = new XMLHttpRequest();  // XMLHttpRequest object
                xhr.open("post", url, true); //post
                xhr.onload = () => {
                    clearContent();
                    hideForm();

                    document.querySelectorAll('a.current').forEach(a => {
                        let url = new URL(a.href);
                        configureContent(a.innerHTML, url.pathname);
                    });
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

    } catch (e) {  console.error(e); }

}

function hideForm()
{
    console.log("clicl");
    document.getElementById("parent_popup").style.display = "none"; 
    document.getElementById("popup_form_form").innerHTML = "";
}

async function DeleteCell(link, soft = false)
{
    let id = "";

    document.querySelectorAll('.tb-line').forEach(row => {
        if (row.classList.contains("cropped")) { return; }

        for (const child of row.children) {
            if (!child.classList.contains("DB-id")) { continue; }
            id = child.innerHTML;   
        }
    });

    try {
        if(!soft)
        {
            const response = await fetch(`${link}/${id}`, {method: "DELETE"});
        }
        else
        {
            const response = await fetch(`${link}/${id}/soft`, {method: "DELETE"});
        }
        
        clearContent();

        document.querySelectorAll('a.current').forEach(a => {
            let url = new URL(a.href);
            configureContent(a.innerHTML, url.pathname);
        });
        

    } catch (e) {  console.error(e); }

}

async function configureContent(name, link)
{
    document.getElementsByClassName("admin-title")[0].innerHTML = name;

    try {
        const response = await fetch(link);
        var json = await response.json();

        if(response.status != 200) { 
            alert(json); 
            return;
        }


        if(link == "/api/log")
        {
            let btn = createButton("Восстановить");
            btn.classList.add("previous");
            btn.classList.add("new");
            btn.classList.add("changed");
            btn.disabled = true;
            btn.addEventListener("click", restoreLog)
        } 
        else if(link != "/api/ref/user")
        {
            let btn = createButton("Новая запись");
            btn.classList.add("previous");
            btn.classList.add("new");
            btn.addEventListener("click", ()=>{showForm(link)});

            let edit = createButton("Редактировать");
            edit.classList.add("changed");
            edit.classList.add("next");
            edit.disabled = true;
            edit.addEventListener("click", ()=>{showForm(link, false)});

            let soft = createButton("Мягкое удаление");
            soft.classList.add("changed");
            soft.classList.add("soft");
            soft.disabled = true;
            soft.addEventListener("click", ()=>{DeleteCell(link, true)});

            let del = createButton("Жёсткое удаление");
            del.classList.add("changed");
            del.classList.add("delete");
            del.disabled = true;
            del.addEventListener("click", ()=>{DeleteCell(link)});
        }

        

        if(typeof json == "string")
        {
            document.querySelectorAll('table.admin-table thead tr').forEach(tr => {
                tr.innerHTML += `<th>${json}</th>`      
            });
            return;
        }

        document.querySelectorAll('table.admin-table thead tr').forEach(tr => {
            Object.keys(json["0"]).forEach(str => {
                tr.innerHTML += `<th>${str}</th>` 
            }); 
        });

        json.forEach(row => {
            document.querySelectorAll('table.admin-table tbody').forEach(table => {
                let tr = document.createElement("tr");
                tr.classList.add("tb-line");
                tr.classList.add("cropped");
                
                Object.keys(row).forEach(key => {
                    let td = document.createElement("td");
                    td.classList.add(`DB-${key}`);
                    td.innerHTML = row[key];
                    tr.appendChild(td);
                }); 
                table.appendChild(tr);    
            });        
        });

        configureLine();

    } catch (e) {  console.error(e); }
}

function configureAside()
{
    ASidePanel.setHeader("Панель управления");

    [["Галерея", "/api/ref/file"], 
     ["Файлы", "/api/ref/file"],
     ["Пользователи", "/api/ref/user"],
     ["Роли", "/api/ref/policy/role"],
     ["Разрешения", "/api/ref/policy/permission"],
     ["Logs", "/api/log"]
    ].forEach(row =>{
        let a = ASidePanel.addLink(row[0], row[1]);

        a.addEventListener('click', (event) => {

            event.preventDefault();  
            
            document.querySelectorAll('a.current').forEach(b => {
                b.classList.remove("current");
            });

            a.classList.add("current");
            clearContent();
            configureContent(row[0], row[1]);
        })

    });
    
}


window.addEventListener("load", function () {

    document.querySelectorAll('input.changed').forEach(input => {
        input.disabled = true;
    });


    configureAside()
    
}, false);
