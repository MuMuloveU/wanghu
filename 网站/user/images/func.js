// JScript 文件
<!--
function showMsg(msg)
{
    window.alert(msg);
}


function equalsValue(objId1,objId2)
{
    var obj1=window.document.getElementById(objId1);
    var obj2=window.document.getElementById(objId2);
    try
    {
        if (obj1.value==obj2.value)
            return true;
        else
        {
            
            obj2.select();
            return false;
        }
    }
    catch(ex){}
}

function reflash()
{
    window.location.href=self.location;
}


function validateByDate(objId)
{
    var obj=window.document.getElementById(objId);
    var re=new RegExp("^\\s*(19|20)\\d{2}\\-([0]\\d|[1][012])-([012]\\d|3[01])\\s*$");
    if(re.test(obj.value))
        return true;
    else
    {
        obj.focus();
        obj.select();
        showMsg('日期格式输入错误,请输入正确日期。例如:1990-05-01');
        return false;
    }
}

function validateByEmail(objId)
{
     var obj=window.document.getElementById(objId);
     var re=new RegExp("\\w+([\\-\\+\\.']\\w+)*@\\w+([\\-\\.]\\w+)*\\.\\w+([\\-\\.]\\w+)*");
     if(re.test(obj.value))
        return true;
    else
    {
        obj.focus();
        obj.select();
        showMsg('E-mail格式输入不正确');
    }
}


function validateMinLength(objId,min)
{

    var obj=window.document.getElementById(objId);
    var input=obj.value;
    
    if(input.length<min)
    {
        obj.focus();
        obj.select();
        showMsg('请输入大于'+min+'位的字符');
        return false;
    }
    else
    {
        return true;
    }
    
}

function validateNumeric(objId)
{
    var obj=window.document.getElementById(objId);
    var input=obj.value;
    var re=new RegExp("^\\-{0,1}[\\d]+$");
    if(re.test(obj.value))
        return true;
    else
    {
        obj.focus();
        obj.select();
        showMsg('请输入数字');
    }
}

function validatefloat(objId)
{
    var obj=window.document.getElementById(objId);
    var input=obj.value;
    var re=new RegExp("^\\-{0,1}[\\d]+\\.{0,1}[\\d]*$");
    if(re.test(obj.value))
        return true;
    else
    {
        obj.focus();
        obj.select();
        showMsg('请输入数字');
    }
}

function validatePositive(objId)
{
    var obj=window.document.getElementById(objId);
    var input=obj.value;
    if(input>=0)
        return true;
    else
    {
        return false;
    }
}

//-->
