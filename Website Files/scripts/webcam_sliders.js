/*

// Variables
var ws = undefined; // websocket instance
var logs = [];
var logsLimit = 4;
var myB = document.getElementById('btnWS');

// ----------------------------------------------------------------------//
function replaceButtonText(buttonId, text)
{
    document.getElementById(buttonId).innerHTML = text;
}

function getTimeStamp() {
    var now = new Date();
    return ((now.getMonth() + 1) + '/' + (now.getDate()) + '/' + now.getFullYear() + " " + now.getHours() + ':'
    + ((now.getMinutes() < 10) ? ("0" + now.getMinutes()) : (now.getMinutes())) + ':' + ((now.getSeconds() < 10) ? ("0" + now
            .getSeconds()) : (now.getSeconds())));
}
function setTime() {
    document.getElementById('timestamp').innerHTML = getTimeStamp();
}

// Initialize the WebSocket
function initWebSocket() {
    var ipName = window.location.hostname;

    if (ws) {
        ws.close(); // close the websocket if open.
    }
    ws = new WebSocket('ws://' + ipName + '/stream');

    ws.onopen = function () { // when handshake is complete:
        log('WebSocket open to ZentriOS device ' + ipName);

        // Change the text of the button to read "Stop Webcam" //
        // Change the title attribute of the button to display "Click to stop webcam" //
        document.getElementById('btnWS').innerHTML = "Stop Webcam";
        myB.title = "Click to stop webcam";
        document.getElementById('btnWS').disabled = false;

        // Enable the button //

    };

    ws.onclose = function () { // when socket is closed:
        log('WebSocket connection to ' + ipName + ' has been closed!');
        ws = undefined;
        // Change the text of the button to read "Start Webcam" //
        document.getElementById('btnWS').innerHTML = "Start Webcam";
        document.getElementById('btnWS').disabled = false;

        // Change the title attribute of the button to display "Click to start webcam" //
        myB.title = "Click to start webcam";

    };

    ws.onmessage = function (event) { // when client receives a WebSocket message:
        // Display a new timestamp //
        getTimeStamp();
        setTime();
        // Set the source of the image to the image on the WiFi chip //
        document.getElementById("pic").src= "./img.jpg";
    };

    ws.onerror = function () { // when an error occurs
        ws.close();
        log('Websocket error');
        // Change the text of the button to read "Start Webcam" //
        replaceButtonText(myB, 'Start Webcam');
        // Change the title attribute of the button to display "Click to start webcam" //
        myB.title = "Click to start webcam";
        ws = undefined;
    }
}

// Set up event listeners
// When the button is clicked, disable it, and depending on whether a Websocket is
// open or not, either run "initWebSocket()" or "ws.close()" //

document.getElementById('btnWS').addEventListener("click",function(){
    this.disabled = true;
    if(ws){
       ws.close();
    }else{
       initWebSocket();
    }


}, false)



// Other functions
function log(txt) {
    logs.push({
        'content': txt,
        'type': 'log'
    });
    showLog(logs, 'log', logsLimit);
}

function showLog(logArray, logId, logLimit) {
    var logContent = '';
    var logLength = logArray.length;
    var iStart = logLength - logLimit - 1;
    if (iStart < 0) {
        iStart = 0;
    }
    for (var index = iStart; index < logLength; ++index) {
        logItem = logArray[index];
        logContent += '<span class="' + logItem.type + '">' + logItem.content + '</span><br/>\n'
    }
    document.getElementById(logId).innerHTML = logContent;
}

// Define initialization function
function init() {
    initWebSocket();
}

// Open Websocket as soon as page loads
window.addEventListener("load", init, false);
*/



// Variables
var ws = undefined; // websocket instance;
var logs = [];
var logsLimit = 4;

var wsUri = "ws:";
var loc = window.location;
console.log(loc);
if (loc.protocol === "https:") { wsUri = "wss:"; }
// This needs to point to the web socket in the Node-RED flow
// ... in this case it's ws/simple
wsUri += "//" + loc.host.replace("1882","1881/ws/simple");// + loc.pathname.replace("1882/index.html","1881/ws/simple");

/*










DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER
DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER
DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER

*/

//wsUri = "ws://2147maple.site:1881/ws/imgStream";

/*

DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER
DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER
DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER DANGER










*/


var ptz = [0, 0, 0];
var pkt = ["~~", "", "", ""];

function getTimeStamp() {
    var now = new Date();
    return ((now.getMonth() + 1) + '/' + (now.getDate()) + '/' + now.getFullYear() + " " + now.getHours() + ':'
    + ((now.getMinutes() < 10) ? ("0" + now.getMinutes()) : (now.getMinutes())) + ':' + ((now.getSeconds() < 10) ? ("0" + now
            .getSeconds()) : (now.getSeconds())));
}

function setTime() {
    document.getElementById('timestamp').innerHTML = getTimeStamp();
    console.log("timestamp updated")
}

function wsConnect() {
    console.log("connect",wsUri);
    ws = new WebSocket(wsUri);
    //var line = "";    // either uncomment this for a building list of messages
    ws.onmessage = function(msg) {
        var line = "";  // or uncomment this to overwrite the existing message
        // parse the incoming message as a JSON object
        var data = msg.data;
        console.log(data);
        // build the output from the topic and payload parts of the object
        
        line += "<p>"+data+"</p>";
        
        // replace the messages div with the new "line"
        //document.getElementById('messages').innerHTML = line;
        //ws.send(JSON.stringify({data:data}));
        
        setTime();
        document.getElementById("pic").src = "./images/img.jpg?random="+new Date().getTime();
        //document.getElementById("pic").src= "./images/img.jpg";
        //location.reload(1); // force reload from server
        
    }
    ws.onopen = function() {
        // update the status div with the connection status
        
        document.getElementById('status').innerHTML = "connected";
        //ws.send("Open for data");
        console.log("connected");
    }
    ws.onclose = function() {
        // update the status div with the connection status
        document.getElementById('status').innerHTML = "not connected";
        // in case of lost connection tries to reconnect every 3 secs
        setTimeout(wsConnect,3000);
    }
}


// for sliders
var panSlider = document.getElementById("panSlider");
var panPlus = document.getElementById("panPlus");
var panMinus = document.getElementById("panMinus");
var panVal = document.getElementById("panValue");

var tiltSlider = document.getElementById("tiltSlider");
var tiltPlus = document.getElementById("tiltPlus");
var tiltMinus = document.getElementById("tiltMinus");
var tiltVal = document.getElementById("tiltValue");


var zoomKnob = document.getElementById('zoomKnob');
var zoomVal = document.getElementById("zoomValue");

panVal.innerHTML = panSlider.value;
tiltVal.innerHTML = tiltSlider.value;
//zoomVal.innerHTML = zoomKnob.value;





function panner() {
    //pkt[0] = "[[" + String.fromCharCode(parseInt(ptz[0]) + 90 + 32);
    // 90 to make it all positive, 256 to put it in the middle of the Latin-A block of Unicode

    
    var codedPan = parseInt(ptz[0]) + 90 + 33;
    var panStr = "";
    
    if(codedPan > 123){
        panStr = panStr + String.fromCharCode((codedPan - 123)+33) + String.fromCharCode(33); 
    }
    else {
        panStr = panStr + String.fromCharCode(33) + String.fromCharCode(codedPan);
    }
    
    pkt[1] = panStr;
}       
function tilter() {    
    var codedTilt = parseInt(ptz[1]) + 90 + 33;
    var tiltStr = "";
    
    if(codedTilt > 123){
        tiltStr = tiltStr + String.fromCharCode((codedTilt - 123)+33) + String.fromCharCode(33); 
    }
    else {
        tiltStr = tiltStr + String.fromCharCode(33) + String.fromCharCode(codedTilt);
    }
    
    pkt[2] = tiltStr;
}       
function zoomer() {
    //var realZoom = ((ptz[2] / 0.125) + 3)*30;
    //var realZoom = ptz[2];
    //console.log(realZoom);
    //pkt[2] = "<<" + String.fromCharCode(realZoom + 256);
    // no 90 since it's already all positive, 256 to put it in the middle of the Latin-A block of Unicode
    
    //if(ptz[2]>0){ pkt[2] = "<<1>>"; }
    //else { pkt[2] = "<<0>>"; }
    
    var codedZoom = ptz[2] + 33;
    var zoomStr = "";
    
    if(codedZoom > 123){
        zoomStr = zoomStr + String.fromCharCode((codedZoom - 123)+33) + String.fromCharCode(33); 
    }
    else {
        zoomStr = zoomStr + String.fromCharCode(33) + String.fromCharCode(codedZoom);
    }
    
    pkt[3] = zoomStr;
}


function sendPTZ() {
    ptz[0] = panSlider.value;
    ptz[1] = tiltSlider.value;
    ptz[2] = ((zoomKnob.value / 0.125) + 3) * 30;
    
    panner();
    tilter();
    zoomer();
    
    document.getElementById('ptz').innerHTML = ptz;
    document.getElementById('pkt').innerHTML = pkt;
    console.log(ptz);
    if (ws) { ws.send(pkt); }
}


function panLabel() {
    panVal.innerHTML = panSlider.value;
    sendPTZ();
}
function tiltLabel() {
    tiltVal.innerHTML = tiltSlider.value;
    sendPTZ();
}
function zoomLabel() {
	var xZoom = (zoomKnob.value / 0.125) + 4;
    zoomVal.innerHTML = xZoom.toFixed(0);
    sendPTZ();
}

function panUpdate(p) {
    panSlider.value -= p /* using += breaks it for some reason */;
    panLabel();
}

function tiltUpdate(t) {
    tiltSlider.value -= t /* using += breaks it for some reason */;
    tiltLabel();
}


panSlider.addEventListener("input", panLabel, false);
panPlus.addEventListener("click", function() { panUpdate(-10); }, false);
panMinus.addEventListener("click", function() { panUpdate(10); }, false);

tiltSlider.addEventListener("input", tiltLabel, false);
tiltPlus.addEventListener("click", function() { tiltUpdate(-10); }, false);
tiltMinus.addEventListener("click", function() { tiltUpdate(10); }, false);

zoomKnob.addEventListener("input", zoomLabel, false);

window.addEventListener("load", wsConnect, false);