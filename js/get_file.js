var obj_allfile = {};

function GET_request_for_allfile()
{
    $.ajax({
        url: "cgi-xjh/get_detailed_info",
        dataType: 'json',
        type: "GET",
        success: function (data)
        {
            console.log(data);
            obj_allfile = data;
            get_obj_of_file(obj_allfile);
            add_event_for_all_file(obj_allfile);
        },
        error: function ()
        {
            console.log("error");
        }
    });
}

function clean_app_div()
{
    var app01 = document.querySelector(".app01");// 获取 父标签
    var pObjs = app01.childNodes;// 获取 父标签下的所有子节点
    for (var i = pObjs.length - 1; i >= 0; i--)
    {
        app01.removeChild(pObjs[i]); // 一定要倒序，正序是删不干净的，可自行尝试
    }
}

function add_item_into_div(filename, img_path, idx)
{   
    var a = document.createElement("a");
    a.href = "#";
    a.id = filename;
    a.className = "mya";

    var img = document.createElement("img");
    img.className = "myimg";

    var p = document.createElement("p");
    p.className = "filename";
    p.innerHTML = filename;

    img.src = img_path;

    var app01 = document.getElementsByClassName("app01");
    app01[0].appendChild(a); //通过类名添加新标签
    a.appendChild(img);
    a.appendChild(p);

    if (idx % 5 == 0)
    {
        // 清除浮动
        var div = document.createElement("div");
        div.style = "clear: both;";
        app01[0].appendChild(div);
    }
}

function display_node_file_content(root, node_name)
{
    var idx = 1;
    var item = get_item_from_obj(root, node_name);// 获取 点击文件的 子文件
    console.log(item);
    for( var key in item)
    {
        if(key == 'type')// 跳过文件夹自身的属性值
        {
            continue;
        }
        var type = get_item_attribute_value(item, key, 'type');// 获取子文件夹的 type 值
        if(type == "REG")
        {
            add_item_into_div(key, "./icon/img/lianjiewenjian.png", idx++);
        }
        else if(type == "DIR")
        {
            add_item_into_div(key, "./icon/icon/wenjianjia.png", idx++);
        }
    }
    return item;
}

function get_obj_of_file(root)
{
    var idx = 1;
    clean_app_div();
    for (var name in root)
    {
        var type = get_item_attribute_value(root, name, 'type');
        if(type == "REG")
        {
            add_item_into_div(name, "./icon/img/lianjiewenjian.png", idx++);
        }
        else if(type == "DIR")
        {
            add_item_into_div(name, "./icon/icon/wenjianjia.png", idx++);
        }
    }
}
