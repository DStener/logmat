function formEncode(form) {
    var requestBodyArray = [];

    // Iterate through the FormData entries
    for (var pair of form.entries()) {
        if (!pair[1].length) { continue; }
        requestBodyArray.push(encodeURIComponent(pair[0]) + '=' + encodeURIComponent(pair[1]));
    }
    return requestBodyArray.join('&');
}


async function WebHookStart(event)
{
    event.preventDefault();

    var form = new FormData(event.target);
    let params = new URLSearchParams(form);

    console.log(event.target);
    console.log(params.toString());

    try {
        const response = await fetch(event.target.action, {
            method: event.target.method,
            body: params.toString(),
        });
        var json = await response.json();
        

        if (response.status == 200) {
            document.getElementById("shell").innerHTML = `<p style="color: green;">${json["message"]}</p>`;
        } else {
            document.getElementById("shell").innerHTML = `<p style="color: red;">${json["message"]}</p>`;
            return;
        }
        let socket = new WebSocket(`ws://${window.location.host}${json["websocet_url"]}`);

        socket.onopen = function (e) {
            socket.send("TEST");
        };

        socket.onmessage = function (event) {
            document.getElementById("shell").innerHTML += `<p style="white-space: pre-line;">${atob(event.data)}</p>`;
        };

        socket.onclose = function (event) {
            if (event.wasClean) {
                concole.log(`[close] Соединение закрыто чисто, код=${event.code} причина=${event.reason}`);
            } else {
                // например, сервер убил процесс или сеть недоступна
                // обычно в этом случае event.code 1006
                console.log('[close] Соединение прервано');
            }
        };

        socket.onerror = function (error) {
            console.log(`[error]`);
        };

    } catch (e) { console.error(e); }
}



window.addEventListener("load", function () {

    document.getElementById("shell-cmd").addEventListener('submit', WebHookStart);
    alert("BEST OF THE BESTs");
}, false);
