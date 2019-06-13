
<%
	SqlConnectionString="Provider=Microsoft.Jet.OLEDB.4.0;Data Source = " & Server.MapPath("bbs/Data/Dvbbs8.mdb")
	Set Conn= Server.CreateObject("ADODB.Connection")
	Conn.Open SqlConnectionString
%>