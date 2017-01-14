// first part of html page
// meta-tags and stylesheet
const char* p1 = "<html>\r\n"
"<head>\r\n"
"<title>Ambient Sensor</title>\r\n"
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n"
// standard method for favicon
// http://www.w3schools.com/tags/att_link_rel.asp
"<link rel=\"icon\" type=\"image/x-icon\" href=\"[path/to/images]/cloud.ico\">\n\r"
// shortcut icon - I think is not standard. Behaviour similar to normal favicon (but you can use png?)
// https://www.w3.org/wiki/More_about_the_document_head
"<link rel=\"shortcut icon\" type=\"image/png\" href=\"[path/to/images]/cloud512.png\">\n\r"
// method used by microsoft for windows 8.1/10
// https://blogs.msdn.microsoft.com/ie/2012/06/08/high-quality-visuals-for-pinned-sites-in-windows-8/
"<meta name=\"msapplication-TileImage\" content=\"[path/to/images]/cloud270.png\">\n\r"
// method used by apple
// https://developer.apple.com/library/content/documentation/AppleApplications/Reference/SafariWebContent/ConfiguringWebApplications/ConfiguringWebApplications.html
"<link rel=\"apple-touch-icon\" href=\"[path/to/images]/cloud152.png\">\n\r"
"<style type=\"text/css\">\r\n"
".st {color:#585858; text-decoration:none; font-family:tahoma,arial; font-size:12pt; font-weight:bold; font-variant:small-caps; text-align:center; padding:8px; margin-top:10px; display:block;}\r\n"
".bo {color:white; text-decoration:none; font-family:tahoma,arial; font-size:28pt; font-weight:bold; text-align:center; padding:8px; margin-top:1px; margin-bottom:12px; display:block; border-radius:15px; outline:none;}\r\n"
".bom {color:white; text-decoration:none; font-family:tahoma,arial; font-size:22pt; font-weight:bold; text-align:center; padding:0px; margin-top:1px; margin-bottom:9px; display:inline-block; border-radius:10px; outline:none;}\r\n"
".bu {color:white; background-color:#cccccc; text-decoration:none; font-family:tahoma,arial; font-size:28pt; font-weight:bold; text-align:center; padding:8px; margin-top:1px; margin-bottom:12px; display:block; border-radius:15px; box-shadow:0 8px #666666; outline:none;}\r\n"
".bu:active {background-color:#999999; box-shadow:0 3px #333333; transform:translateY(4px);}\r\n"
"a.l:hover, a.l:link, a.l:visited {color:#0099cc; text-decoration:none; font-family:tahoma,arial; font-size:12pt; font-weight:normal; text-align:center; padding:8px; margin-top:50px; display:block;}\r\n"
"</style>\r\n";
// second part of html page
// javascript for ajax functions
const char* p2 = "<script language=\"javascript\">\n\r"
"xmlhttp=null;\n\r"
"var sensorValues = [];\n\r"
"function getValues()\n\r"
"\t{\n\r"
"\tsetTimeout('getValues()', 2000);\n\r"
"\tif (window.XMLHttpRequest)\n\r"
"\t\t{\n\r"
"\t\txmlhttp=new XMLHttpRequest();\n\r"
"\t\t}\n\r"
"\telse\n\r"
"\t\t{\n\r"
"\t\txmlhttp=new ActiveXObject('Microsoft.XMLHTTP');\n\r"
"\t\t}\n\r"
"\txmlhttp.open('GET','?getValues',false);\n\r"
"\txmlhttp.send(null);\n\r"
"\tif (xmlhttp.responseText != \"\")\n\r"
"\t\t{\n\r"
"\t\tsensorValues = xmlhttp.responseText.split(\",\");\n\r"
"\t\tif(sensorValues[0]!=\"---\"){\n\r"
"\t\t\tdocument.getElementById(\"te\").innerHTML=sensorValues[0]+\"&deg;C\";\n\r"
"\t\t\tdocument.getElementById(\"ur\").innerHTML=sensorValues[1]+\"%\";\n\r"
"\t\t\tdocument.getElementById(\"hi\").innerHTML=\"&#128102;&nbsp;\"+sensorValues[2]+\"&deg;C\";\n\r"
"\t\t\tdocument.getElementById(\"temin\").innerHTML=\"&#8681;&nbsp;\"+sensorValues[3]+\"&deg;\";\n\r"
"\t\t\tdocument.getElementById(\"temax\").innerHTML=\"&#8679;&nbsp;\"+sensorValues[4]+\"&deg;\"; }\n\r"
"\t\t\tdocument.getElementById(\"sdate\").innerHTML=sensorValues[5];\n\r"
"\t\t\tdocument.getElementById(\"stime\").innerHTML=sensorValues[6];\n\r"
"\t\t}\n\r"
"\t}\n\r"
"</script>\n\r"
"</head>\n\r";
