
async function configVersion() {
    try {
        const response = await fetch("/info/version");
        let json = await response.json();

        let obj = document.getElementById("footer-text")

        if (json["version"] != "") {
            obj.innerHTML += `Версия: ${json["version"]}`
        }
    } catch (e) { console.error(e); }
}

async function configureUser() {
    try {
        const response = await fetch("/api/auth/me");

        let json = await response.json();

        if (typeof json != "string") {

            let obj = document.getElementById("user-name")
            if (obj) {
                let avatar = "https://avatars.mds.yandex.net/i?id=9028b32c2dbca7fbe65309f1fb7141cf_l-4957727-images-thumbs&amp;n=13";
                if (json["avatar"] != 0) { avatar = `/api/ref/file/${json["avatar"]}/avatar`; }

                obj.innerHTML = `
                    <img src="${avatar}" alt="Аватар пользователя" class="user-avatar">
                    <p>${json["username"]}</p>`;
            }

            obj = document.getElementById("user-menu-list")
            if (obj) {
                obj.innerHTML = `
                    <li><a href="/me" class="user-menu-link">Профиль</a></li>
                    <li>
                        <form action="/api/auth/out?redirect=/" method="POST">
                            <input type="submit" class="user-menu-link" value="Выход" />
                        </form>
                    </li>`;
            }

        }
    } catch (e) { console.error(e); }

}


window.addEventListener("load", function () {
    configureUser()
    configVersion()
}, false);