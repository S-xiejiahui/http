var father_node = [];
var father_name = [];

var obj = document.getElementById("back_home");
obj.addEventListener('click', function (event) 
{  
    if(father_node.length)
    {
        // console.log("back");
        var prev_node = father_node.pop();
        var path = father_name.pop();
        console.log('pop path: ' + path);
        clean_app_div();// 清理所有文件标签
        // console.log('prev = ' + JSON.stringify(prev_node));
        get_obj_of_file(prev_node);
        add_event_for_all_file(prev_node);
    }
}, false);

document.getElementById('close').onclick = function(e)
{
    var up1 = document.getElementById('up1');
    up1.style = 'z-index: 0;';
    var up2 = document.getElementById('up2');
    up2.style = 'z-index: 0;';
    up2.innerHTML = '';
}


function add_event_for_all_file(root)
{
    var link = $(".app01 a");
    // console.log('add '+ link.length + ' onclick event');
    for(var i = 0; i<link.length; i++)
    {
        let j = i;//块级变量
        link[i].onclick = function () // 给 .app01 里面的所有 a 添加点击事件
        {
            var node_name = link[j].id;// 获取点击标签的 id 即文件夹名字
            // console.log('node_name: ' + node_name);
            var item = get_item_from_obj(root, node_name);
            if(get_self_attribute_value(item, 'type') == "DIR")
            {
                father_node.push(root);
                father_name.push(node_name);
                console.log('push name: ' + node_name);

                clean_app_div();// 清理所有文件标签
                display_node_file_content(root, node_name);
                remove_event_for_all_file(link);
                add_event_for_all_file(item);
            }
            else if(get_self_attribute_value(item, 'type') == "REG")
            {
                var index = node_name.lastIndexOf(".");
                var file_type = node_name.substr(index);
                var path = './';
                for (var x = 0; x < father_name.length; x++)
                {
                    path += father_name[x] + '/';
                }
                path += node_name;
                if(file_type == '.txt' || file_type == '.c' || file_type == '.h')
                {
                    Get_request_file_content(path);
                }
                else if(file_type == '.png' || file_type == '.jpg')
                {
                    console.log(path);
                    var img = document.createElement("img");
                    img.className = "show_img";
                    img.src = path;//Get_request_file_content(path);
                    var body = document.getElementsByClassName("root");
                    body[0].appendChild(img); //通过类名添加新标签
                }
            }  
        }
    }
    return;
}

function remove_event_for_all_file(link)
{
    // console.log('Ready to delete ' + link.length + ' onclick event');
    for(var i = 0; i<link.length; i++)
    {
        let j = i;
        //console.log(link[j].id + '  [remove]');
        link[j].onclick = null;
    }
}

