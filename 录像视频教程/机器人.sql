declare @KindID int
declare @ServerID int
declare @Charts int

set @KindID = 522
set @ServerID = 6522
set @Charts = 100

insert into AndroidUserinfo(UserID,KindID,ServerID)
select top (@Charts) UserID,KindID,ServerID
from QPGameUserDBLInk.QPGameUserDB.dbo.AccountsInfo
where UserID not in(
select UserID
from AndroidUserinfo
)
order by UserID desc