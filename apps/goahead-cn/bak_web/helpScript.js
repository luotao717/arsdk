function popHelp(thepage)
{
win1=window.open(thepage,"Help",'width=600,height=450,left=50,top=50,toolbar=no,status=no,scrollbars=yes,resizable=yes,menubar=no');
}
function popupHelp(thePage)
{
win1=window.open(thePage,"Help",'width=600,height=450,left=50,top=50,toolbar=no,status=no,scrollbars=yes,resizable=yes,menubar=no');
}
function GURL(x){location=x}
function makesure(p,l){if (confirm(p)) GURL(l);}
function wizquit()
{
if(window.confirm("The Wizard has not yet finished setting up your OfficeConnect Wireless Cable/DSL Gateway.\n\nAre you sure you want to exit?")==true)
{
GURL('QS_ethernet_dynamic.htm');
}
}
function closePopup()
{   top.close()
}