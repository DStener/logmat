
async function configArticle() {
    try {
        const response = await fetch("/api/ref/article");
        let json = await response.json();
        let obj = document.getElementById("div-article");

        if (!json) { return; }

        console.log(json);

        for (let i = 0; i < json.length; ++i) {
            obj.innerHTML +=
             `<a href="/article/${json[i]["id"]}" class="artcile-card">
             <div class="artcile-card box-decor">
                <img src="/api/ref/file/${json[i]["banner"]}">
                <h1>${json[i]["header"]}</h1>
             </div>
             </a>`;
        }
        
    } catch (e) { console.error(e); }
}


window.addEventListener("load", function () {
    configArticle();


    //let obj = document.getElementById("div-article");
    //for (let i = 0; i < 20; ++i) {
    //    //obj.innerHTML += `<a href="#" class="artcile-card">
    //    //        <div class="artcile-card box-decor">
    //    //            <img src="https://i.pinimg.com/originals/1d/84/60/1d846047ebb67e196629534b5e56ed02.jpg">
    //    //            <h1>Алгоритм обхода бинарного дерева</h1>
    //    //            <p>Тествцуролжапуолдапоулаполуаполупаолдупаолдупаолупаолдупаолдуцпаолдпуцаолд пу ола пуолапу олда уаол уцола упр  олда уцолджа пцуолалджуцпаголж цуралд угшлжк руцшщк</p>
    //    //        </div>
    //    //    </a>`;
    //}
}, false);