// 获取 json 对象的长度
function get_json_obj_length(obj)
{
    var number = 0;
    for (var key in obj)
    {
        number++;
    }
    return number;
}
// 获取 json 对象中的第 idx 个成员
function get_key_value_from_obj(obj, idx)
{
    var result = {}, num = 0;
    for (var key in obj)
    {
        if(++num == idx)
        {
            result['key'] = key;
            result['key_type'] = typeof key;
            result['value'] = obj[key];
            result['value_type'] = typeof obj[key];
            return result;
        }
    }
    return {};
}
// 从 json 对象中取出 key 值为 filename 的对象信息
function get_item_from_obj(obj, filename)
{
    for (var key in obj)
    {
        if(key == filename)
        {
            return obj[key];
        }
    }
    return {};
}
// 获取对象自身的属性值
function get_self_attribute_value(obj, node)
{
    for (var key in obj)
    {
        if (key == node)
        {
            return obj[key];
        }
    }
    return 'null';
}
// 获取 json 对象中的 node_name 成员 的 type 属性
function get_item_attribute_value(obj, node_name, type)
{
    for (var key in obj)
    {
        if(key == node_name)
        {
            for(var value in obj[key])
            {
                if( type == value)
                {
                    return obj[key][value];
                }
            }
        }
    }
    return 'null';
}