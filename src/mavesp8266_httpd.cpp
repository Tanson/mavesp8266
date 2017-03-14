/****************************************************************************

   Copyright (c) 2015, 2016 Gus Grubba. All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
   COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
   OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
   AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.

 ****************************************************************************/

/**
   @file mavesp8266_httpd.cpp
   ESP8266 Wifi AP, MavLink UART/UDP Bridge

   @author Gus Grubba <mavlink@grubba.com>
*/

#include "mavesp8266.h"
#include "mavesp8266_httpd.h"
#include "mavesp8266_parameters.h"
#include "mavesp8266_gcs.h"
#include "mavesp8266_vehicle.h"

#include <ESP8266WebServer.h>

const char PROGMEM kTEXTPLAIN[]  = "text/plain";
const char PROGMEM kTEXTHTML[]   = "text/html";
const char PROGMEM kACCESSCTL[]  = "Access-Control-Allow-Origin";
const char PROGMEM kUPLOADFORM[] = "<form method='POST' action='/upload' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";
const char PROGMEM kHEADER[]     = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>MavLink Bridge</title><style>.basic-grey{margin-left:auto;margin-right:auto;max-width:800px;background:#f7f7f7;padding:25px 15px 25px 10px;font:12px Georgia,'Times New Roman',Times,serif;color:#888;text-shadow:1px 1px 1px #FFF;border:1px solid #e4e4e4}.basic-grey h1{font-size:25px;padding:0 0 10px 40px;display:block;border-bottom:1px solid #e4e4e4;margin:-10px -15px 30px -10px;color:#888}.basic-grey h1>span{display:block;font-size:11px}.basic-grey label{display:block;margin:0}.basic-grey label>span{float:left;width:20%;text-align:right;padding-right:10px;margin-top:10px;color:#888}.basic-grey input[type='text'],.basic-grey input[type='password'],.basic-grey textarea,.basic-grey select{border:1px solid #dadada;color:#888;height:30px;margin-bottom:16px;margin-right:6px;margin-top:2px;outline:0 none;padding:3px 3px 3px 5px;width:70%;font-size:12px;line-height:15px;box-shadow:inset 0 1px 4px #ececec;-moz-box-shadow:inset 0 1px 4px #ececec;-webkit-box-shadow:inset 0 1px 4px #ececec}.basic-grey button{background:#e27575;border:0;padding:10px 25px 10px 25px;color:#FFF;box-shadow:1px 1px 5px #b6b6b6;border-radius:3px;text-shadow:1px 1px 1px #9e3f3f;cursor:pointer}.basic-grey .buttonChange{background:#f00;border:0;padding:10px 25px 10px 25px;color:#FFF;box-shadow:1px 1px 5px #b6b6b6;border-radius:3px;text-shadow:1px 1px 1px #9e3f3f;cursor:pointer}.basic-grey .button:hover{background:#CF7A7A}</style></head><body class='basic-grey'>";
const char PROGMEM kMENU[] = "<h1><span><a href='/getparameters'>Config</a> | <a href='/getstatus'>Status</a> | <a href='/update'>OTA</a>|  <a href='/setparameters?reboot=1'>Reboot</a> </span></h1>";
const char PROGMEM kBADARG[]     = "BAD ARGS";
const char PROGMEM kAPPJSON[]    = "application/json";

const char PROGMEM kLINK[] = "<h1>Links<span><table border='1' cellpadding='3' cellspacing='0' bordercolor='#999999'><thead><tr><td >url</td><td>description</td></tr></thead><tbody><tr><td><a href='http://192.168.4.1/getparameters'>getparameters</a></td><td>list of parameters</td></tr><tr><td><a href='http://192.168.4.1/getstatus'>getstatus</a></td><td>getstatus of the ESP8266</td></tr><tr><td><a href='http://192.168.4.1/setparameters?key=value&amp;key=value'>setparameters?key=value&amp;key=value</a></td><td>Setting parameters</td></tr><tr><td><a href='http://192.168.4.1/setparameters?baud=57600&amp;channel=9&amp;reboot=1'>setparameters?baud=57600&amp;channel=9&amp;reboot=1</a></td><td>Combination</td></tr><tr><td><a href='http://192.168.4.1/getparameters'>update</a></td><td>OTA</td></tr></tbody></table></span></h1>";
const char PROGMEM kPARA[] = "<h1>Parameter<span><table border='1' cellpadding='3' cellspacing='0' bordercolor='#999999'><thead><tr><th>Parameter</th><th>Defaults</th><th>Description</th><th>url</th></tr></thead><tbody><tr><td>baud</td><td>57600</td><td>Serial port speed</td><td><a href='/setparameters?baud=57600'>/setparameters?baud=57600</a></td></tr><tr><td>channel</td><td>11</td><td>AP mode channel</td><td><a href='/setparameters?channel=11'>/setparameters?channel=11</a></td></tr><tr><td>cport</td><td>14555</td><td>Local listening port</td><td><a href='/setparameters?cport=14555'>/setparameters?cport=14555</a></td></tr><tr><td>debug</td><td>0</td><td>Enable debug</td><td><a href='/setparameters?debug=0'>/setparameters?debug=0</a></td></tr><tr><td>hport</td><td>14550</td><td>default 14550</td><td><a href='/setparameters?hport=14550'>/setparameters?hport=14550</a></td></tr><tr><td>mode</td><td>0</td><td>AP mode = 0, and STA mode = 1</td><td><a href='/setparameters?mode=1'>/setparameters?mode=1</a></td></tr><tr><td>pwd</td><td>pixracer</td><td>AP modepassword</td><td><a href='/setparameters?pwd=pixracer'>/setparameters?pwd=pixracer</a></td></tr><tr><td>pwdsta</td><td>pixracer</td><td>STA mode password</td><td><a href='/setparameters?pwdsta=pixracer'>/setparameters?pwdsta=pixracer</a></td></tr><tr><td>reboot</td><td>0</td><td>Reboot = 1</td><td><a href='/setparameters?reboot=1'>/setparameters?reboot=1</a></td></tr><tr><td>ssid</td><td>PixRacer</td><td>AP mode ssid </td><td><a href='/setparameters?ssid=PixRacer'>/setparameters?ssid=PixRacer</a></td></tr><tr><td>ssidsta</td><td>PixRacer</td> <td>STA mode ssid </td><td><a href='/setparameters?ssidsta=PixRacer'>/setparameters?ssidsta=PixRacer</a></td></tr><tr><td>ipsta</td><td>0.0.0.0</td><td>STA mode WIFI static IP</td><td><a href='/setparameters?ipsta=192.168.4.2'>/setparameters?ipsta=192.168.4.2</a></td></tr><tr><td>gatewaysta</td><td>0.0.0.0</td><td>STA mode gateway address</td><td><a href='/setparameters?gatewaysta=192.168.4.1'>/setparameters?gatewaysta=192.168.4.1</a></td></tr><tr><td>subnetsta</td><td>0.0.0.0</td><td>STA mode subnet mask</td><td><a href='/setparameters?subnetsta=255.255.255.0'>/setparameters?subnetsta=255.255.255.0</a></td></tr> <tr><td>webaccount</td><td>PixRacer</td><td>Web authentication account</td><td><a href='/setparameters?webaccount=PixRacer'>/setparameters?webaccount=PixRacer</a></td></tr><tr><td>webpassword</td><td>pixracer</td><td>Web authentication password </td><td><a href='/setparameters?webpassword=pixracer'>/setparameters?webpassword=pixracer</a></td></tr></tbody></table></span></h1>";
const char PROGMEM kCONFIG[] = "<h1>System Config<span><table id='configData' border='1' cellpadding='3' cellspacing='0' bordercolor='#999999' width='90%'><thead><tr><th>Parameter</th><th>Defaults</th><th>Description</th><th>&nbsp;</th></tr></thead><tbody><tr><td>baud</td><td><input type='text' id='baud' name='baud'  value='{baud}' onFocus='inputfocus(this)' onBlur='inputblur(this)'/></td><td>Serial port speed</td><td><button >Save</button></td></tr><tr><td>debug</td><td><input type='text' id='debug' name='debug'  value='{debug}'  onfocus='inputfocus(this)' onblur='inputblur(this)'/></td><td><p>Enable debug</p><p>enable=1,disable=0</p></td><td><button >Save</button></td></tr><tr><td>channel</td><td><input type='text' id='channel' name='channel'  value='{channel}'  onFocus='inputfocus(this)' onBlur='inputblur(this)'/></td><td>AP mode channel</td><td><button >Save</button></td></tr><tr><td>mode</td><td><input type='text' id='mode' name='mode'  value='{mode}'  onfocus='inputfocus(this)' onblur='inputblur(this)'/></td><td>AP mode = 0, and STA mode = 1</td><td><button >Save</button></td></tr><tr><td>ssid</td><td><input type='text' id='ssid' name='ssid'  value='{ssid}'  onfocus='inputfocus(this)' onblur='inputblur(this)'/></td><td>AP mode ssid</td><td><button >Save</button></td></tr><tr><td>pwd</td><td><input type='text' id='pwd' name='pwd'  value='{pwd}' onfocus='inputfocus(this)' onblur='inputblur(this)' /></td><td>AP modepassword</td><td><button >Save</button></td></tr><tr><td>hport</td><td><input type='text' id='hport' name='hport'  value='{hport}'  onFocus='inputfocus(this)' onBlur='inputblur(this)'/></td><td>default 14550</td><td><button >Save</button></td></tr><tr><td>ssidsta</td><td><input type='text' id='ssidsta' name='ssidsta'  value='{ssidsta}'  onFocus='inputfocus(this)' onBlur='inputblur(this)'/></td><td>STA mode ssid</td><td><button >Save</button></td></tr><tr><td>pwdsta</td><td><input type='text' id='pwdsta' name='pwdsta'  value='{pwdsta}'  onfocus='inputfocus(this)' onblur='inputblur(this)'/></td><td>STA mode password</td><td><button >Save</button></td></tr><tr><td>ipsta</td><td><input type='text' id='ipsta' name='ipsta'  value='{ipsta}'  onFocus='inputfocus(this)' onBlur='inputblur(this)'/></td><td>STA mode WIFI static IP</td><td><button >Save</button></td></tr><tr><td>cport</td><td><input type='text' id='cport' name='cport'  value='{cport}' onfocus='inputfocus(this)' onblur='inputblur(this)' /></td><td>Local listening port</td><td><button >Save</button></td></tr><tr><td>gatewaysta</td><td><input type='text' id='gatewaysta' name='gatewaysta'  value='{gatewaysta}'  onFocus='inputfocus(this)' onBlur='inputblur(this)'/></td><td>STA mode gateway address</td><td><button >Save</button></td></tr><tr><td>subnetsta</td><td><input type='text' id='subnetsta' name='subnetsta'  value='{subnetsta}'  onFocus='inputfocus(this)' onBlur='inputblur(this)'/></td><td>STA mode subnet mask</td><td><button >Save</button></td></tr></tbody></table></span></h1>";

const char* kBAUD       = "baud";
const char* kPWD        = "pwd";
const char* kSSID       = "ssid";
const char* kPWDSTA     = "pwdsta";
const char* kSSIDSTA    = "ssidsta";
const char* kIPSTA      = "ipsta";
const char* kGATESTA    = "gatewaysta";
const char* kSUBSTA     = "subnetsta";
const char* kCPORT      = "cport";
const char* kHPORT      = "hport";
const char* kCHANNEL    = "channel";
const char* kDEBUG      = "debug";
const char* kREBOOT     = "reboot";
const char* kPOSITION   = "position";
const char* kMODE       = "mode";

const char* kFlashMaps[7] = {
  "512KB (256/256)",
  "256KB",
  "1MB (512/512)",
  "2MB (512/512)",
  "4MB (512/512)",
  "2MB (1024/1024)",
  "4MB (1024/1024)"
};

static uint32_t flash = 0;
static char paramCRC[12] = {""};

ESP8266WebServer    webServer(80);
MavESP8266Update*   updateCB    = NULL;
bool                started     = false;

//---------------------------------------------------------------------------------
void setNoCacheHeaders() {
  webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webServer.sendHeader("Pragma", "no-cache");
  webServer.sendHeader("Expires", "0");
}

//---------------------------------------------------------------------------------
void returnFail(String msg) {
  webServer.send(500, FPSTR(kTEXTPLAIN), msg + "\r\n");
}

//---------------------------------------------------------------------------------
void respondOK() {
  webServer.send(200, FPSTR(kTEXTPLAIN), "OK");
}

//---------------------------------------------------------------------------------
void handle_update() {
  webServer.sendHeader("Connection", "close");
  webServer.sendHeader(FPSTR(kACCESSCTL), "*");
   String message = FPSTR(kHEADER);
  message += FPSTR(kMENU);
   message += FPSTR(kUPLOADFORM);
   message += "</body></html>";
  webServer.send(200, FPSTR(kTEXTHTML), message);
}

//---------------------------------------------------------------------------------
void handle_upload() {
  webServer.sendHeader("Connection", "close");
  webServer.sendHeader(FPSTR(kACCESSCTL), "*");
  webServer.send(200, FPSTR(kTEXTPLAIN), (Update.hasError()) ? "FAIL" : "OK");
  if (updateCB) {
    updateCB->updateCompleted();
  }
  ESP.restart();
}

//---------------------------------------------------------------------------------
void handle_upload_status() {
  bool success  = true;
  if (!started) {
    started = true;
    if (updateCB) {
      updateCB->updateStarted();
    }
  }
  HTTPUpload& upload = webServer.upload();
  if (upload.status == UPLOAD_FILE_START) {
#ifdef DEBUG_SERIAL
    DEBUG_SERIAL.setDebugOutput(true);
#endif
    WiFiUDP::stopAll();
#ifdef DEBUG_SERIAL
    DEBUG_SERIAL.printf("Update: %s\n", upload.filename.c_str());
#endif
    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    if (!Update.begin(maxSketchSpace)) {
#ifdef DEBUG_SERIAL
      Update.printError(DEBUG_SERIAL);
#endif
      success = false;
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
#ifdef DEBUG_SERIAL
      Update.printError(DEBUG_SERIAL);
#endif
      success = false;
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) {
#ifdef DEBUG_SERIAL
      DEBUG_SERIAL.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
#endif
    } else {
#ifdef DEBUG_SERIAL
      Update.printError(DEBUG_SERIAL);
#endif
      success = false;
    }
#ifdef DEBUG_SERIAL
    DEBUG_SERIAL.setDebugOutput(false);
#endif
  }
  yield();
  if (!success) {
    if (updateCB) {
      updateCB->updateError();
    }
  }
}

//---------------------------------------------------------------------------------
void handle_getParameters()
{
  String message = FPSTR(kHEADER);
  message += FPSTR(kMENU);
  message += FPSTR(kCONFIG);
  message.replace("{baud}", String(getWorld()->getParameters()->getUartBaudRate()));
  message.replace("{debug}",  String(getWorld()->getParameters()->getDebugEnabled()));
  message.replace("{channel}",  String(getWorld()->getParameters()->getWifiChannel()));
  message.replace("{mode}",  String(getWorld()->getParameters()->getWifiMode()));
  message.replace("{ssid}", getWorld()->getParameters()->getWifiSsid());
  message.replace("{pwd}", getWorld()->getParameters()->getWifiPassword());
  message.replace("{hport}",  String(getWorld()->getParameters()->getWifiUdpHport()));
  message.replace("{ssidsta}", getWorld()->getParameters()->getWifiStaSsid());
  message.replace("{pwdsta}", getWorld()->getParameters()->getWifiStaPassword());
  message.replace("{ipsta}",  String(getWorld()->getParameters()->getWifiStaIP()));
  message.replace("{cport}",  String(getWorld()->getParameters()->getWifiUdpCport()));
  message.replace("{gatewaysta}",  String(getWorld()->getParameters()->getWifiStaGateway()));
  message.replace("{subnetsta}",  String(getWorld()->getParameters()->getWifiStaSubnet()));

  message += "<script>";
  message += "\r\nfunction $(id){return document.getElementById(id);}\r\n";
  message += "\r\nfunction inputblur(obj){var btn=obj.parentNode.parentNode.getElementsByTagName('button')[0];if(obj.getAttribute('oldValue')!=null){if(obj.getAttribute('oldValue')!=obj.value){btn.setAttribute('class','buttonChange')}else{btn.setAttribute('class','')}}};";
  message += "\r\nfunction inputfocus(obj){if(obj.getAttribute('oldValue') == null){obj.setAttribute('oldValue',obj.value)};}";
  message += "\r\nfunction save(obj){var input=obj.parentNode.parentNode.getElementsByTagName('input')[0];window.location.href='/setparameters?' +input.id + '=' + input.value;}";
  message += "\r\nfunction bindBtn(){var btns=$('configData').getElementsByTagName('button');for(var i=0;i<btns.length;i++){btns[i].onclick=function(){save(this);}}}";
  message += "\r\nbindBtn()";
  message += "</script>";
  message += "</body></html>";
  webServer.send(200, FPSTR(kTEXTHTML), message);
}

//---------------------------------------------------------------------------------
void handle_getStatus()
{
  if (!flash)
    flash = ESP.getFreeSketchSpace();
  if (!paramCRC[0]) {
    snprintf(paramCRC, sizeof(paramCRC), "%08X", getWorld()->getParameters()->paramHashCheck());
  }
  linkStatus* gcsStatus = getWorld()->getGCS()->getStatus();
  linkStatus* vehicleStatus = getWorld()->getVehicle()->getStatus();
  String message = FPSTR(kHEADER);
  message += FPSTR(kMENU);
  message += "<h1>System Status<span>";

  message += "<p>Comm Status</p><table><tr><td width=\"240\">Packets Received from GCS</td><td>";
  message += gcsStatus->packets_received;
  message += "</td></tr><tr><td>Packets Sent to GCS</td><td>";
  message += gcsStatus->packets_sent;
  message += "</td></tr><tr><td>GCS Packets Lost</td><td>";
  message += gcsStatus->packets_lost;
  message += "</td></tr><tr><td>Packets Received from Vehicle</td><td>";
  message += vehicleStatus->packets_received;
  message += "</td></tr><tr><td>Packets Sent to Vehicle</td><td>";
  message += vehicleStatus->packets_sent;
  message += "</td></tr><tr><td>Vehicle Packets Lost</td><td>";
  message += vehicleStatus->packets_lost;
  message += "</td></tr><tr><td>Radio Messages</td><td>";
  message += gcsStatus->radio_status_sent;
  message += "</td></tr></table>";
  message += "<p>System Status</p><table><tr><td width=\"240\">Flash Memory Left</td><td>";
  message += flash;
  message += "</td></tr><tr><td>RAM Left</td><td>";
  message += String(ESP.getFreeHeap());
  message += "</td></tr><tr><td>Parameters CRC</td><td>";
  message += paramCRC;
  message += "</td></tr></table>";
    message += "</span></h1>";
  message += "</body></html>";
  setNoCacheHeaders();
  webServer.send(200, FPSTR(kTEXTHTML), message);
}

//---------------------------------------------------------------------------------
void handle_getJLog()
{
  uint32_t position = 0, len;
  if (webServer.hasArg(kPOSITION)) {
    position = webServer.arg(kPOSITION).toInt();
  }
  String logText = getWorld()->getLogger()->getLog(&position, &len);
  char jStart[128];
  snprintf(jStart, 128, "{\"len\":%d, \"start\":%d, \"text\": \"", len, position);
  String payLoad = jStart;
  payLoad += logText;
  payLoad += "\"}";
  webServer.send(200, FPSTR(kAPPJSON), payLoad);
}

//---------------------------------------------------------------------------------
void handle_getJSysInfo()
{
  if (!flash)
    flash = ESP.getFreeSketchSpace();
  if (!paramCRC[0]) {
    snprintf(paramCRC, sizeof(paramCRC), "%08X", getWorld()->getParameters()->paramHashCheck());
  }
  uint32_t fid = spi_flash_get_id();
  char message[512];
  snprintf(message, 512,
           "{ "
           "\"size\": \"%s\", "
           "\"id\": \"0x%02lX 0x%04lX\", "
           "\"flashfree\": \"%u\", "
           "\"heapfree\": \"%u\", "
           "\"logsize\": \"%u\", "
           "\"paramcrc\": \"%s\""
           " }",
           kFlashMaps[system_get_flash_size_map()],
           fid & 0xff, (fid & 0xff00) | ((fid >> 16) & 0xff),
           flash,
           ESP.getFreeHeap(),
           getWorld()->getLogger()->getPosition(),
           paramCRC
          );
  webServer.send(200, "application/json", message);
}

//---------------------------------------------------------------------------------
void handle_getJSysStatus()
{
  bool reset = false;
  if (webServer.hasArg("r")) {
    reset = webServer.arg("r").toInt() != 0;
  }
  linkStatus* gcsStatus = getWorld()->getGCS()->getStatus();
  linkStatus* vehicleStatus = getWorld()->getVehicle()->getStatus();
  if (reset) {
    memset(gcsStatus,     0, sizeof(linkStatus));
    memset(vehicleStatus, 0, sizeof(linkStatus));
  }
  char message[512];
  snprintf(message, 512,
           "{ "
           "\"gpackets\": \"%u\", "
           "\"gsent\": \"%u\", "
           "\"glost\": \"%u\", "
           "\"vpackets\": \"%u\", "
           "\"vsent\": \"%u\", "
           "\"vlost\": \"%u\", "
           "\"radio\": \"%u\", "
           "\"buffer\": \"%u\""
           " }",
           gcsStatus->packets_received,
           gcsStatus->packets_sent,
           gcsStatus->packets_lost,
           vehicleStatus->packets_received,
           vehicleStatus->packets_sent,
           vehicleStatus->packets_lost,
           gcsStatus->radio_status_sent,
           vehicleStatus->queue_status
          );
  webServer.send(200, "application/json", message);
}

//---------------------------------------------------------------------------------
void handle_setParameters()
{
  if (webServer.args() == 0) {
    returnFail(kBADARG);
    return;
  }
  bool ok = false;
  bool reboot = false;
  if (webServer.hasArg(kBAUD)) {
    ok = true;
    getWorld()->getParameters()->setUartBaudRate(webServer.arg(kBAUD).toInt());
  }
  if (webServer.hasArg(kPWD)) {
    ok = true;
    getWorld()->getParameters()->setWifiPassword(webServer.arg(kPWD).c_str());
  }
  if (webServer.hasArg(kSSID)) {
    ok = true;
    getWorld()->getParameters()->setWifiSsid(webServer.arg(kSSID).c_str());
  }
  if (webServer.hasArg(kPWDSTA)) {
    ok = true;
    getWorld()->getParameters()->setWifiStaPassword(webServer.arg(kPWDSTA).c_str());
  }
  if (webServer.hasArg(kSSIDSTA)) {
    ok = true;
    getWorld()->getParameters()->setWifiStaSsid(webServer.arg(kSSIDSTA).c_str());
  }
  if (webServer.hasArg(kIPSTA)) {
    IPAddress ip;
    ip.fromString(webServer.arg(kIPSTA).c_str());
    getWorld()->getParameters()->setWifiStaIP(ip);
  }
  if (webServer.hasArg(kGATESTA)) {
    IPAddress ip;
    ip.fromString(webServer.arg(kGATESTA).c_str());
    getWorld()->getParameters()->setWifiStaGateway(ip);
  }
  if (webServer.hasArg(kSUBSTA)) {
    IPAddress ip;
    ip.fromString(webServer.arg(kSUBSTA).c_str());
    getWorld()->getParameters()->setWifiStaSubnet(ip);
  }
  if (webServer.hasArg(kCPORT)) {
    ok = true;
    getWorld()->getParameters()->setWifiUdpCport(webServer.arg(kCPORT).toInt());
  }
  if (webServer.hasArg(kHPORT)) {
    ok = true;
    getWorld()->getParameters()->setWifiUdpHport(webServer.arg(kHPORT).toInt());
  }
  if (webServer.hasArg(kCHANNEL)) {
    ok = true;
    getWorld()->getParameters()->setWifiChannel(webServer.arg(kCHANNEL).toInt());
  }
  if (webServer.hasArg(kDEBUG)) {
    ok = true;
    getWorld()->getParameters()->setDebugEnabled(webServer.arg(kDEBUG).toInt());
  }
  if (webServer.hasArg(kMODE)) {
    ok = true;
    getWorld()->getParameters()->setWifiMode(webServer.arg(kMODE).toInt());
  }
  if (webServer.hasArg(kREBOOT)) {
    ok = true;
    reboot = webServer.arg(kREBOOT) == "1";
  }
  if (ok) {
    getWorld()->getParameters()->saveAllToEeprom();
    //-- Send new parameters back
    handle_getParameters();
    if (reboot) {
      delay(100);
      ESP.restart();
    }
  } else
    returnFail(kBADARG);
}

//---------------------------------------------------------------------------------
//-- 404
void handle_notFound() {
  String message = FPSTR(kHEADER);
  message += FPSTR(kMENU);
   message += FPSTR(kLINK);
    message += FPSTR(kPARA);
      message += "</body></html>";
  
  webServer.send(404, FPSTR(kTEXTHTML), message);
}

//---------------------------------------------------------------------------------
MavESP8266Httpd::MavESP8266Httpd()
{

}

//---------------------------------------------------------------------------------
//-- Initialize
void
MavESP8266Httpd::begin(MavESP8266Update* updateCB_)
{
  updateCB = updateCB_;
  webServer.on("/getparameters",  handle_getParameters);
  webServer.on("/setparameters",  handle_setParameters);
  webServer.on("/getstatus",      handle_getStatus);
  webServer.on("/info.json",      handle_getJSysInfo);
  webServer.on("/status.json",    handle_getJSysStatus);
  webServer.on("/log.json",       handle_getJLog);
  webServer.on("/update",         handle_update);
  webServer.on("/upload",         HTTP_POST, handle_upload, handle_upload_status);
  webServer.onNotFound(           handle_notFound);
  webServer.begin();
}

//---------------------------------------------------------------------------------
//-- Initialize
void
MavESP8266Httpd::checkUpdates()
{
  webServer.handleClient();
}
