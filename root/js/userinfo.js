

async function getUserInfo() {
}


async function getMeInfo()
{
    try {
        const response = await fetch("/api/auth/me");

        return response.json();
    } catch (e) { console.error(e); }
}

async function ConfigureHeaderUserInfo()
{
    let elements = document.getElementsByClassName("user-info")

    console.log(elements.length);

    for (let i = 0; i < elements.length; ++i)
    {
        elements[i].innerHTML = "";

        let signin = document.createElement("a");
        let separator = document.createElement("p");
        let signup = document.createElement("a");

        signin.innerHTML = "Вход";
        signin.className = "submenu-link";
        signin.href = "login.html";

        separator.innerHTML = "/";

        signup.innerHTML = "Регистрация";
        signup.className = "submenu-link";
        signup.href = "register.html";

        elements[i].appendChild(signin);
        elements[i].appendChild(separator);
        elements[i].appendChild(signup);
    }
    getMeInfo().then(result => {

        
        if (typeof result != "string")
        {   

            for (let i = 0; i < elements.length; ++i)
            {
                if(result["username"] != "admin")
                {
                    elements[i].innerHTML = `
                    <img src = ${"https://avatars.mds.yandex.net/i?id=9028b32c2dbca7fbe65309f1fb7141cf_l-4957727-images-thumbs&n=13"} alt = "Аватар пользователя" class="user-avatar" />
                    <details class="user-menu">
                        <summary class="user-name">${result["username"]}</summary>
                        <ul class="user-menu-list">
                            <li><a href="/me" class="user-menu-link">Профиль</a></li>
                            <li><a onclick = "LogoutMe()" class="user-menu-link">Выход</a></li>
                        </ul>
                    </details>`;
                }
                else 
                {
                    elements[i].innerHTML = `
                    <img src = ${"https://avatars.mds.yandex.net/i?id=9028b32c2dbca7fbe65309f1fb7141cf_l-4957727-images-thumbs&n=13"} alt = "Аватар пользователя" class="user-avatar" />
                    <details class="user-menu">
                        <summary class="user-name">${result["username"]}</summary>
                        <ul class="user-menu-list">
                            <li><a href="/me" class="user-menu-link">Профиль</a></li>
                            <li><a href="/admin.html" class="user-menu-link">Панель упарвления</a></li>
                            <li><a onclick = "LogoutMe()" class="user-menu-link">Выход</a></li>
                        </ul>
                    </details>`;
                }
            }
        }
    });

}

function printTest()
{
    alert("PRINT");
}

async function LogoutMe()
{
    try {
        const response = await fetch("/api/auth/out", {
          method: "POST",
        });
        window.location.replace("/");
    } catch (e) {  console.error(e); }
    
}


async function ConfigureName()
{
    getMeInfo().then(result => {

        
        if (typeof result != "string")
        {   
            let obj = document.getElementById("profile-name")
            if(obj) {
                obj.innerHTML = result["username"];
            }
        }
    });
}

window.addEventListener("load", function () {
    ConfigureHeaderUserInfo();
    ConfigureName();
}, false);