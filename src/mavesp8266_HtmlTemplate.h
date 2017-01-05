const char PROGMEM kTEXTPLAIN[]  = "text/plain";
const char PROGMEM kTEXTHTML[]   = "text/html";
const char PROGMEM kAPPJSON[]    = "application/json";
const char PROGMEM kACCESSCTL[]  = "Access-Control-Allow-Origin";
const char PROGMEM kBADARG[]     = "BAD ARGS";


//-----------------------------------------Web Template-------------------------------------------
const char PROGMEM kTEMPLATE[]     = R"=====(
<!DOCTYPE html><html><head><meta charset="UTF-8"><title>MavLink Bridge</title><style>.basic-grey{margin-left:auto;margin-right:auto;max-width:800px;background:#f7f7f7;padding:25px 15px 25px 10px;font:12px Georgia,"Times New Roman",Times,serif;color:#888;text-shadow:1px 1px 1px #FFF;border:1px solid #e4e4e4}.basic-grey h1{font-size:25px;padding:0 0 10px 40px;display:block;border-bottom:1px solid #e4e4e4;margin:-10px -15px 30px -10px;color:#888}.basic-grey h1>span{display:block;font-size:11px}.basic-grey label{display:block;margin:0}.basic-grey label>span{float:left;width:20%;text-align:right;padding-right:10px;margin-top:10px;color:#888}.basic-grey input[type="text"],.basic-grey input[type="password"],.basic-grey textarea,.basic-grey select{border:1px solid #dadada;color:#888;height:30px;margin-bottom:16px;margin-right:6px;margin-top:2px;outline:0 none;padding:3px 3px 3px 5px;width:70%;font-size:12px;line-height:15px;box-shadow:inset 0 1px 4px #ececec;-moz-box-shadow:inset 0 1px 4px #ececec;-webkit-box-shadow:inset 0 1px 4px #ececec}.basic-grey button{background:#e27575;border:0;padding:10px 25px 10px 25px;color:#FFF;box-shadow:1px 1px 5px #b6b6b6;border-radius:3px;text-shadow:1px 1px 1px #9e3f3f;cursor:pointer}.basic-grey .buttonChange{background:#f00;border:0;padding:10px 25px 10px 25px;color:#FFF;box-shadow:1px 1px 5px #b6b6b6;border-radius:3px;text-shadow:1px 1px 1px #9e3f3f;cursor:pointer}.basic-grey .button:hover{background:#CF7A7A}</style>
</head>
<body class="basic-grey">
<h1><span><a href="/getsystemcfg">System Config</a> |  <a href="/getapcfg">AP Mode Config</a> |  <a href="/getstacfg">STA Mode Config</a> | <a href="/getstatus">Status</a> | <a href="/update">OTA</a> |  <a href="/setparameters?reboot=1">Reboot</a> </span></h1>
{body}
</body></html>
)=====";
//------------------------------------------System Config------------------------------------------
const char PROGMEM kSYSCFG[]     = R"=====(
<script>
	function $(id){return document.getElementById(id);}
	function inputblur(obj){var btn=obj.parentNode.parentNode.getElementsByTagName("button")[0];if(obj.getAttribute("oldValue")!=null){if(obj.getAttribute("oldValue")!=obj.value){btn.setAttribute("class","buttonChange")}else{btn.setAttribute("class","")}}};
	function inputfocus(obj){if(obj.getAttribute('oldValue') == null){obj.setAttribute('oldValue',obj.value)};}
	function save(obj){var input=obj.parentNode.parentNode.getElementsByTagName("input")[0];window.location.href="/setparameters?" +input.id + "=" + input.value;}
	function bindBtn(){var btns=$("configData").getElementsByTagName("button");for(var i=0;i<btns.length;i++){btns[i].onclick=function(){save(this);}}}
</script>
<h1>System Config
<span>
<table id="configData" border='1' cellpadding='3' cellspacing='0' bordercolor='#999999' width="90%">
<thead><tr><th>Parameter</th><th>Defaults</th><th>Description</th><th>&nbsp;</th></tr></thead>
<tbody>
<tr><td>baud</td><td><input type="text" id="baud" name="baud"  value="{baud}" onFocus="inputfocus(this)" onBlur="inputblur(this)"/></td><td>Serial port speed</td><td><button >Save</button></td></tr><tr><td>debug</td><td><input type="text" id="debug" name="debug"  value="{debug}"  onfocus="inputfocus(this)" onblur="inputblur(this)"/></td><td><p>Enable debug</p><p>enable=1,disable=0</p></td><td><button >Save</button></td></tr><tr><td>channel</td><td><input type="text" id="channel" name="channel"  value="{channel}"  onFocus="inputfocus(this)" onBlur="inputblur(this)"/></td><td>AP mode channel</td><td><button >Save</button></td></tr><tr><td>mode</td><td><input type="text" id="mode" name="mode"  value="{mode}"  onfocus="inputfocus(this)" onblur="inputblur(this)"/></td><td>AP mode = 0, and STA mode = 1</td><td><button >Save</button></td></tr><tr><td>webaccount</td><td><input type="text" id="webaccount" name="webaccount"  value="{webaccount}"  onFocus="inputfocus(this)" onBlur="inputblur(this)"/></td><td>Web authentication account</td><td><button >Save</button></td></tr><tr><td>webpassword</td><td><input type="text" id="webpassword" name="webpassword"  value="{webpassword}"  onfocus="inputfocus(this)" onblur="inputblur(this)"/></td><td>Web authentication password</td><td><button >Save</button></td></tr>
<tr><td>ssid</td><td><input type="text" id="ssid" name="ssid"  value="{ssid}"  onfocus="inputfocus(this)" onblur="inputblur(this)"/></td><td>AP mode ssid</td><td><button >Save</button></td></tr><tr><td>pwd</td><td><input type="text" id="pwd" name="pwd"  value="{pwd}" onfocus="inputfocus(this)" onblur="inputblur(this)" /></td><td>AP modepassword</td><td><button >Save</button></td></tr><tr><td>hport</td><td><input type="text" id="hport" name="hport"  value="{hport}"  onFocus="inputfocus(this)" onBlur="inputblur(this)"/></td><td>default 14550</td><td><button >Save</button></td></tr>
<tr><td>ssidsta</td><td><input type="text" id="ssidsta" name="ssidsta"  value="{ssidsta}"  onFocus="inputfocus(this)" onBlur="inputblur(this)"/></td><td>STA mode ssid</td><td><button >Save</button></td></tr><tr><td>pwdsta</td><td><input type="text" id="pwdsta" name="pwdsta"  value="{pwdsta}"  onfocus="inputfocus(this)" onblur="inputblur(this)"/></td><td>STA mode password</td><td><button >Save</button></td></tr><tr><td>ipsta</td><td><input type="text" id="ipsta" name="ipsta"  value="{ipsta}"  onFocus="inputfocus(this)" onBlur="inputblur(this)"/></td><td>STA mode WIFI static IP</td><td><button >Save</button></td></tr><tr><td>cport</td><td><input type="text" id="cport" name="cport"  value="{cport}" onfocus="inputfocus(this)" onblur="inputblur(this)" /></td><td>Local listening port</td><td><button >Save</button></td></tr><tr><td>gatewaysta</td><td><input type="text" id="gatewaysta" name="gatewaysta"  value="{gatewaysta}"  onFocus="inputfocus(this)" onBlur="inputblur(this)"/></td><td>STA mode gateway address</td><td><button >Save</button></td></tr><tr><td>subnetsta</td><td><input type="text" id="subnetsta" name="subnetsta"  value="{subnetsta}"  onFocus="inputfocus(this)" onBlur="inputblur(this)"/></td><td>STA mode subnet mask</td><td><button >Save</button></td></tr>
</tbody></table>
</span></h1>
<script>bindBtn()</script>
)=====";

//------------------------------------------Upload------------------------------------------
const char PROGMEM kUPLOADFORM[] = R"=====(
<form method='POST' action='/upload' enctype='multipart/form-data'>
<h1>Flash
<span>upload new bin file.</span>
</h1>
<label>
<span>File :</span>
<input type='file' name='update'><button type="submit">Upload</button>
</label>
</form>
)=====";
//------------------------------------------Login------------------------------------------
const char PROGMEM kLOGINFORM[]     = R"=====(
<form action='/login' method='POST'><h1>Login<span> for Manager.</span></h1><label><span>Account :</span><input id="USERNAME" type="text" name="USERNAME" placeholder="Your Login Account" /></label><label><span>Password :</span><input id="PASSWORD" type="password" name="PASSWORD" placeholder="Your Login Password" /></label><label>{msg}</label><label><span>&nbsp;</span><button type="submit">Login</button></label></form>
)=====";

//------------------------------------------404------------------------------------------
const char PROGMEM kERRORPage[]     = "404";

//------------------------------------------Helper------------------------------------------
const char PROGMEM kHELPHTML[]     = R"=====(
<h1>Links<span><table border='1' cellpadding='3' cellspacing='0' bordercolor='#999999'><thead><tr><td >url</td><td>description</td></tr></thead><tbody><tr><td><a href='http://192.168.4.1/getparameters'>getparameters</a></td><td>list of parameters</td></tr><tr><td><a href='http://192.168.4.1/getstatus'>getstatus</a></td><td>getstatus of the ESP8266</td></tr><tr><td><a href='http://192.168.4.1/setparameters?key=value&amp;key=value'>setparameters?key=value&amp;key=value</a></td><td>Setting parameters</td></tr><tr><td><a href='http://192.168.4.1/setparameters?baud=57600&amp;channel=9&amp;reboot=1'>setparameters?baud=57600&amp;channel=9&amp;reboot=1</a></td><td>Combination</td></tr><tr><td><a href='http://192.168.4.1/getparameters'>update</a></td><td>OTA</td></tr></tbody></table></span></h1><h1>Parameter<span><table border='1' cellpadding='3' cellspacing='0' bordercolor='#999999'><thead><tr><th>Parameter</th><th>Defaults</th><th>Description</th><th>url</th></tr></thead><tbody><tr><td>baud</td><td>57600</td><td>Serial port speed</td><td><a href='http://192.168.4.1/setparameters?baud=57600'>http://192.168.4.1/setparameters?baud=57600</a></td></tr><tr><td>channel</td><td>11</td><td>AP mode channel</td><td><a href='http://192.168.4.1/setparameters?channel=11'>http://192.168.4.1/setparameters?channel=11</a></td></tr><tr><td>cport</td><td>14555</td><td>Local listening port</td><td><a href='http://192.168.4.1/setparameters?cport=14555'>http://192.168.4.1/setparameters?cport=14555</a></td></tr><tr><td>debug</td><td>0</td><td>Enable debug</td><td><a href='http://192.168.4.1/setparameters?debug=0'>http://192.168.4.1/setparameters?debug=0</a></td></tr><tr><td>hport</td><td>14550</td><td>default 14550</td><td><a href='http://192.168.4.1/setparameters?hport=14550'>http://192.168.4.1/setparameters?hport=14550</a></td></tr><tr><td>mode</td><td>0</td><td>AP mode = 0, and STA mode = 1</td><td><a href='http://192.168.4.1/setparameters?mode=1'>http://192.168.4.1/setparameters?mode=1</a></td></tr><tr><td>pwd</td><td>pixracer</td><td>AP modepassword</td><td><a href='http://192.168.4.1/setparameters?pwd=pixracer'>http://192.168.4.1/setparameters?pwd=pixracer</a></td></tr><tr><td>pwdsta</td><td>pixracer</td><td>STA mode password</td><td><a href='http://192.168.4.1/setparameters?pwdsta=pixracer'>http://192.168.4.1/setparameters?pwdsta=pixracer</a></td></tr><tr><td>reboot</td><td>0</td><td>Reboot = 1</td><td><a href='http://192.168.4.1/setparameters?reboot=1'>http://192.168.4.1/setparameters?reboot=1</a></td></tr><tr><td>ssid</td><td>PixRacer</td><td>AP mode ssid</td><td><a href='http://192.168.4.1/setparameters?ssid=PixRacer'>http://192.168.4.1/setparameters?ssid=PixRacer</a></td></tr><tr><td>ssidsta</td><td>PixRacer</td><td>STA mode ssid</td><td><a href='http://192.168.4.1/setparameters?ssidsta=PixRacer'>http://192.168.4.1/setparameters?ssidsta=PixRacer</a></td></tr><tr><td>ipsta</td><td>0.0.0.0</td><td>STA mode WIFI static IP</td><td><a href='http://192.168.4.1/setparameters?ipsta=192.168.4.2'>http://192.168.4.1/setparameters?ipsta=192.168.4.2</a></td></tr><tr><td>gatewaysta</td><td>0.0.0.0</td><td>STA mode gateway address</td><td><a href='http://192.168.4.1/setparameters?gatewaysta=192.168.4.1'>http://192.168.4.1/setparameters?gatewaysta=192.168.4.1</a></td></tr><tr><td>subnetsta</td><td>0.0.0.0</td><td>STA mode subnet mask</td><td><a href='http://192.168.4.1/setparameters?subnetsta=255.255.255.0'>http://192.168.4.1/setparameters?subnetsta=255.255.255.0</a></td></tr><tr><td>webaccount</td><td>PixRacer</td><td>Web authentication account</td><td><a href='http://192.168.4.1/setparameters?webaccount=PixRacer'>http://192.168.4.1/setparameters?webaccount=PixRacer</a></td></tr><tr><td>webpassword</td><td>pixracer</td><td>Web authentication password</td><td><a href='http://192.168.4.1/setparameters?webpassword=pixracer'>http://192.168.4.1/setparameters?webpassword=pixracer</a></td></tr></tbody></table></span></h1>
)=====";



