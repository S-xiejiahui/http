var father_node = [];

var obj = document.getElementById("back_home");
obj.addEventListener('click', function (event) 
{  
    if(father_node.length)
    {
        console.log("back");
        var prev_node = father_node.pop();
        clean_app_div();// 清理所有文件标签
        // console.log('prev = ' + JSON.stringify(prev_node));
        get_obj_of_file(prev_node);
        add_event_for_all_file(prev_node);
    }
}, false);

function add_event_for_all_file(root)
{
    var link = $(".app01 a");
    console.log('add '+ link.length + ' onclick event');
    for(var i = 0; i<link.length; i++)
    {
        let j = i;//块级变量
        link[i].onclick = function () // 给 .app01 里面的所有 a 添加点击事件
        {
            var node_name = link[j].id;// 获取点击标签的 id 即文件夹名字
            console.log('node_name: ' + node_name);
            var item = get_item_from_obj(root, node_name);
            if(get_self_attribute_value(item, 'type') != "DIR")
            {
                return;
            }
            father_node.push(root);
            clean_app_div();// 清理所有文件标签
            display_node_file_content(root, node_name);
            remove_event_for_all_file(link);
            add_event_for_all_file(item);  
        }
    }
    return;
}

function remove_event_for_all_file(link)
{
    console.log('Ready to delete ' + link.length + ' onclick event');
    for(var i = 0; i<link.length; i++)
    {
        let j = i;
        //console.log(link[j].id + '  [remove]');
        link[j].onclick = null;
    }
}

