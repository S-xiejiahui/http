#该文件夹是临时文件夹
function Get_request_file_content(node_name)
{
    $.ajax({
        url: "cgi-xjh/get_file_content?filename="+node_name,
        dataType: 'text',
        type: "GET",
        success: function (data)
        {
            // txt_data = data;
            show_word_on_page(data);
            console.log(data);
        },
        error: function ()
        {
            console.log("error");
        }
    });
}

