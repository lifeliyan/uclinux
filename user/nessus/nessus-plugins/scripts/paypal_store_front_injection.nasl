#
# This script is (C) Tenable Network Security
#




if(description)
{
 script_id(11873);
 script_bugtraq_id(8791);
 script_version ("$Revision$");

 name["english"] = "PayPal Store Front code injection";

 script_name(english:name["english"]);
 
 desc["english"] = "
It is possible to make the remote host include php files hosted on a third 
party server using the PayPal Store Front CGI suite which is installed.

An attacker may use this flaw to inject arbitrary code in the remote
host and gain a shell with the privileges of the web server.

Solution : Upgrade to the latest version
Risk factor : Serious";




 script_description(english:desc["english"]);
 
 summary["english"] = "Checks for the presence of index.php";
 
 script_summary(english:summary["english"]);
 
 script_category(ACT_ATTACK);
 
 
 script_copyright(english:"This script is Copyright (C) 2003 Tenable Network Security",
		francais:"Ce script est Copyright (C) 2003 Tenable Network Security");
 family["english"] = "CGI abuses";
 family["francais"] = "Abus de CGI";
 script_family(english:family["english"], francais:family["francais"]);
 script_dependencie("find_service.nes", "no404.nasl");
 script_require_ports("Services/www", 80);
 exit(0);
}

#
# The script code starts here
#


include("http_func.inc");
include("http_keepalive.inc");

port = get_kb_item("Services/www");
if(!port) port = 80;
if(!get_port_state(port))exit(0);



function check(loc)
{
 req = http_get(item:string(loc, "/index.php?do=ext&page=http://xxxxxxxx/file"),
 		port:port);			
 r = http_keepalive_send_recv(port:port, data:req);
 if( r == NULL )exit(0);
 if(egrep(pattern:".*http://xxxxxxxx/file\.php", string:r))
 {
 	security_hole(port);
	exit(0);
 }
}



dirs = make_list("", cgi_dirs());


foreach dir (dirs)
{
 check(loc:dir);
}
