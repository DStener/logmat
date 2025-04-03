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

function hideFields()
{
    document.getElementById("username").type = "hidden";
    document.getElementById("username_txt").style = "display: none;";

    document.getElementById("password").type = "hidden";
    document.getElementById("password_txt").style = "display: none;";

    document.getElementById("code").type = "hidden";
    document.getElementById("code_txt").style = "display: none;";

    document.getElementById("err_mes").style = "display: none;";
}

async function login_submit(event)
{
    // Stop default call
    event.preventDefault();  

    var form = new FormData(event.target);

    if(document.getElementById("username").type == 'text')
    {
        // Hide all
        hideFields()
        document.getElementsByClassName("previous")[0].style = "";

        try {
            const response = await fetch(`/api/auth/2fa/enabled?${formEncode(form)}`, {
              method: "GET",
            });
            if(response.status != 200) { return; }
            
            var json = await response.json();

            if(json["enable"]){
                document.getElementById("code").type = "number";
                document.getElementById("code_txt").style = "";
            } else {
                document.getElementById("password").type = "password";
                document.getElementById("password_txt").style = "";
            } 
        } catch (e) {  console.error(e); }
    }
    else
    {
        try {
            const response = await fetch("/api/auth/login", {
              method: "POST",
              body: formEncode(form),
            });
            if(response.status == 200) { window.location.replace("/me.html"); }
            else{
                document.getElementById("err_mes").style = "";
                document.getElementById("err_mes").innerHTML = decodeURIComponent(await response.json());
            }
        } catch (e) {  console.error(e); }
    }
}

window.onload  = () => {
    
    document.getElementById("login_dialog").addEventListener('submit', login_submit);

    document.getElementsByClassName("previous")[0].onclick = (event) => {
        hideFields();
        document.getElementById("username").type = "text";
        document.getElementById("username_txt").style = "";
        
        document.getElementsByClassName("previous")[0].style = "display: none;";
    };
}