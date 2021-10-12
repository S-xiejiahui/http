    <h1 style="text-align: center;">http服务器搭建</h1>
    <hr>
    <p> 搭建这个服务器的原因是兴趣所在；服务器很简陋，以后将一步步优化，加油！！</p>
    <hr>
    <p>
<pre>
    <h6>下载仓储</h6>
    git clone https://github.com/S-xiejiahui/http_v1.0.git <br>
    cd http_v1.0 <br>
    chmod 777 ./ -R 
    <h6>以下内容只需执行一次</h6>
    ssh-keygen -t rsa -C "e-mail@qq.com"                 //获取ssh-key密钥，GitHub服务器添加你的密钥，你才能有上传代码权限 <br>
    cat /home/xjh/.ssh/id_rsa.pub                       //显示ssh-key <br>
    git config --global user.name "username"            //设置你的ssh名 <br>
    git config --global user.email e-mail@qq.com        //设置你的ssh邮箱 <br>
    cat .git/config                                     //查看你的设置是否生效 <br>
</pre>
    </p>
    <hr>
<pre>
    <h6>怎么提交代码</h6>
    git fetch                                           //同步远端服务器内容到本地分支 <br>
    git rebase origin master                            //如果有打印信息，说明你本地代码落后，GitHub上的代码， <br>
                                                        //则使用这条命令，同步 <br>
    git add ...                                         //添加修改的文件 <br>
    git commit -m "..."                                 //添加修改此次文件的备注 <br>
    git log --graph                                     //查看修改历史 <br>
    git push origin master                              //将修改的文件，推送到GitHub <br>
</pre>
    <hr>
<pre>
    <h6>各目录介绍</h6>
    js：                                                //夹存放--html所需JavaScript脚本文件 <br>
    css：                                               //夹存放--html所需的css文件 <br>
    src：                                               //夹存放--c语言web服务器文件 <br>
    icon：                                              //夹存放--服务器需加载的图片文件 <br>
    Makefile：                                          //编译工具 <br>
    app.html：                                          //主页面 <br>
    http：                                              //web服务器--可执行文件 <br>
</pre>
    <hr>
    <pre>
    <h5>服务器怎么运行：</h5>
    1、make编译工程，将会生产一个http的可执行文件 <br>
    2、运行服务器：./http（默认端口号8080） <br>
        或者 ./http 8080（8080指服务器的端口号，可自行指定） <br>
    3、ifconfig查看本地ip地址 <br>
    4、修改app.js文件中的function get_file_from_server()函数 <br>
        将 url: "http://192.168.13.132:8080/app.html/allfile" 中的IP地址和端口号改成你的主机地址 <br>
            或者把ip地址改为 localhost:8080, 但这样修改，将只能在Ubuntu的浏览器中访问服务器 <br>
    5、访问服务器：浏览器输入 http://192.168.13.132:8080/app.html 即可 <br>
        或者 http://localhost:8080/app.html <br>
    6、服务器启动成功 <br>
</pre>
    <hr>
<pre>
<h6>目录结构：</h6>
    ├── Readme.md // help
    ├── http // 应用
    ├── ap.html // 页面
    ├── Makefile // 编译工具
    ├── src // 服务器C语言代码
    │ ├── cJSON.c
    │ ├── cJSON.h // 开发环境
    │ ├── main.c // 实验
    │ ├── main.h // 配置控制
    │ └── ... // 测试环境
    ├── icon //图片文件夹
    │ ├── ...
    │ └── ...
    ├── css //html所需的css文件
    │ ├── ...
    │ └── ...
    └── js //html所需的css文件
    │ ├── ...
    │ └── ...
    └── ...
</pre>