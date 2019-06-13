简单新闻系统


1。例子说明：
=============================
a. 之所以把新闻系统作为eWebEditor功能使用的一个例子，原因在于新闻系统很具有代表性，基本上eWebEditor所具有的功能都能在此系统上给予体现。
b. 在例子文件中都有对每一步骤进行详细的注释，您可以根据实际使用的需要对某些操作步骤或功能进行增删改。
c. 要使用此例子，您必须把压缩包eWebEditor目录设为虚拟目录，且虚拟目录名必须为eWebEditor，然后访问http://localhost/ewebeditor/example/newssystem/list.asp，之所以有此要求，是因为此新闻例子默认设置的上传文件路径的要求。
d. 数据库文件db.mdb中，表的每个字段都有注释，以便于您更好的了解eWebEditor的功能及使用方法。（附：系统数据库ewebeditor.mdb表字段也是有注释的，注释是我们的规定作法，希望您能更了解我们的系统）
e. 此例子专门使用系统自带样式s_newssystem，此样式使用相对路径保存上传文件，所有上传的文件或远程获取的文件都存在文件夹下的NewsFile文件夹下，所有路径的格式都类似为"NewsFile/XXX.gif"，此样式的远程获取模式为自动打开状态，即说明当提交表单时，编辑内容中的所有远程图片将自动上传到本地服务器。


2。文件说明：
=============================
db.mdb：数据库文件
startup.asp：每页公用包含文件，包括数据库连接，公用基本函数，头尾部内容
add.asp：增加新闻表单页
addsave.asp：增加新闻保存页
modify.asp：修改新闻表单页
modifysave.asp：修改新闻保存页
list.asp：新闻列表页
delete.asp：删除新闻操作页
show.asp：显示新闻内容页



更多关于此例的说明请访问：
http://eWebEditor.webasp.net/example.asp

2004年05月20日