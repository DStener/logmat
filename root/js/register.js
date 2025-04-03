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

async function login(form)
{
    try {
        const response = await fetch("/api/auth/login", {
          method: "POST",
          body: formEncode(form),
        });
        if(response.status == 200) { window.location.replace("/me"); }
        else{ window.location.replace("/register.html"); }
    } catch (e) {  console.error(e); }
}

async function login_submit(event)
{
    // Stop default call
    event.preventDefault();  

    var form = new FormData(event.target);

   

    try {
        const response = await fetch("/api/auth/register", {
            method: "POST",
            body: formEncode(form),
        });
        if(response.status == 201) { login(form) }
        else {
            document.getElementById("err_mes").style = "";
            document.getElementById("err_mes").innerHTML = decodeURIComponent(await response.json());
        }
    } catch (e) {  console.error(e); }
}

async function Captcha()
{
    try {
        const response = await fetch("/api/auth/captcha", {
            method: "GET",
        });

        if (response.status != 200) { return; }
        document.getElementById("captcha").innerHTML = await response.json();
    } catch (e) { console.error(e); }
}


window.onload  = () => {
    document.getElementById("register_dialog").addEventListener('submit', login_submit);
    Captcha();
    
}