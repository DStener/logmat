


function configureAside()
{
    ASidePanel.setHeader("Панель управления");
    ASidePanel.addLink("Галерея", "#");
    ASidePanel.addLink("Файлы", "#");
    ASidePanel.addLink("Пользователи", "#");
    ASidePanel.addLink("Роли", "#");
    ASidePanel.addLink("Разрешения", "#");
    
}


window.addEventListener("load", function () {

    document.querySelectorAll('input.changed').forEach(input => {
        input.disabled = true;
    });


    configureAside()
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
}, false);
