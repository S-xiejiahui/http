/***********************************************************************************
                            全局变量
************************************************************************************/
var x = 1, idx = 1, max = 24, number = 66;
var _IP_ = "192.168.3.13";
var _URL_ = "http://" + _IP_ + ":8080/app.html/allfile"
/***********************************************************************************
                                页面打开即允许192.168.1.1
************************************************************************************/
window.onload = function ()
{
    get_file_from_server();

    setInterval(function ()
    {
        var d = new Date();
        var year = d.getFullYear();
        var month = d.getMonth() + 1 > 9 ? d.getMonth() + 1 : '0' + d.getMonth() + 1;
        var data = d.getDate() > 9 ? d.getDate() : '0' + d.getDate();
        var hours = d.getHours() > 9 ? d.getHours() : '0' + d.getHours();
        var minutes = d.getMinutes() > 9 ? d.getMinutes() : '0' + d.getMinutes();
        var seconds = d.getSeconds() > 9 ? d.getSeconds() : '0' + d.getSeconds();

        var time = document.getElementById("time");
        time.innerHTML = year + '-' + month + '-' + data + ' ' + hours + ':' + minutes + ':' + seconds;
    }, 1000);
}

/***********************************************************************************
                            GET、POST请求
************************************************************************************/
function get_file_from_server()
{
    $.ajax({
        url: _URL_,
        dataType: 'json',
        type: "GET",
        success: function (data)
        {
            console.log(data);
            load_all_file(data);
        },
        error: function ()
        {
            console.log("error");
        }
    });
}
/***********************************************************************************
                            加载文件
************************************************************************************/
function load_all_file(data) 
{
    var file_count = 0;
    $.each(data, function (obj_name, obj_key)
    {
        // console.log('00000');
        // console.log('type=' + obj_name + '-----');
        $.each(obj_key, function (name, key)
        {
            // console.log('name=' + name + ';' + 'key=' + key);
            var first = key.indexOf("=");
            var centre = key.indexOf(";");
            var end = key.lastIndexOf("=");
            var type = key.slice(first + 1, centre);
            var size = key.slice(end + 1);
            console.log('filename == ' + name + ' , ' + 'type == ' + type + ', ' + 'size == ' + size);

            var a = document.createElement("a");
            a.href = "#";
            a.className = "mya";

            var img = document.createElement("img");
            img.className = "myimg";

            var p = document.createElement("p");
            p.className = "filename";
            p.innerHTML = name;

            if (type == 'dir') 
            {
                if (name == 'Makefile')
                {
                    img.src = "icon/icon/Makefile.png";
                }
                else
                    img.src = "icon/img/File.png";
            }
            else if (type == '.html') 
            {
                img.src = "icon/img/lianjiewenjian.png";
            }
            else
            {
                img.src = "icon/img/weizhiwenjian.png";
            }
            var app01 = document.getElementsByClassName("app01");
            app01[0].appendChild(a); //通过类名添加新标签
            a.appendChild(img);
            a.appendChild(p);

            if (++file_count % 5 == 0)
            {
                // 清除浮动
                var div = document.createElement("div");
                div.style = "clear: both;";
                app01[0].appendChild(div);
            }

        })
    })

    // for (x = idx; x <= number; x++)
    // {
    //     var a = document.createElement("a");
    //     a.href = "#";
    //     a.className = "mya";

    //     var img = document.createElement("img");
    //     img.src = "icon/img/txt.png";
    //     img.className = "myimg";

    //     var p = document.createElement("p");
    //     p.innerHTML = "文件夹" + x;
    //     p.className = "filename";

    //     var app01 = document.getElementsByClassName("app01");
    //     app01[0].appendChild(a);//通过类名添加新标签
    //     a.appendChild(img);
    //     a.appendChild(p);

    //     if (x % 6 == 0 && x != 0)
    //     {
    //         // 清除浮动
    //         var div = document.createElement("div");
    //         div.style = "clear: both;";
    //         app01[0].appendChild(div);
    //     }
    //     if (x % max == 0)
    //     {
    //         break;
    //     }
    // }
}
/***********************************************************************************
                            next按钮、prev按钮
************************************************************************************/
// var next = document.querySelector(".next");
// next.onclick = function ()
// {
//     if (x >= number)
//     {
//         x = number;
//         idx = idx;
//     }
//     else
//     {
//         var app01 = document.querySelector(".app01");// 获取 父标签
//         var pObjs = app01.childNodes;// 获取 父标签下的所有子节点
//         for (var i = pObjs.length - 1; i >= 0; i--)
//         {
//             app01.removeChild(pObjs[i]); // 一定要倒序，正序是删不干净的，可自行尝试
//         }
//         idx = x + 1;
//         load_all_file(0);
//     }
// }

// var prev = document.querySelector(".prev");
// prev.onclick = function ()
// {
//     if (x != 24)
//     {
//         var app01 = document.querySelector(".app01");// 获取 父标签
//         var pObjs = app01.childNodes;// 获取 父标签下的所有子节点
//         for (var i = pObjs.length - 1; i >= 0; i--)
//         {
//             app01.removeChild(pObjs[i]);
//         }
//         idx = idx - max;
//         load_all_file(0);
//     }
// }
/***********************************************************************************
                                菜单栏标签
************************************************************************************/

var oli = document.getElementsByTagName("li");
for (var i = 0; i < oli.length; i++)
{
    (function (j)
    {
        oli[j].onclick = function ()
        {
            oli[j].className = 'active';
            for (var k = 0; k < oli.length; k++)
            {
                if (k != j)
                {
                    console.log(k);
                    oli[k].className = '';
                }
            }
        };
    })(i)
}
/***********************************************************************************

************************************************************************************/
