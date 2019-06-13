declare @KindID int
declare @ServerID int
declare @Charts int

set @KindID = 504
set @ServerID = 6504
set @Charts = 80

insert into AndroidUserinfo(UserID,KindID,ServerID)
select top (@Charts) UserID,@KindID,@ServerID
from QPGameUserDBLInk.QPGameUserDB.dbo.AccountsInfo
where UserID not in(
select UserID
from AndroidUserinfo
)
order by UserID desc