// (c) Petr Kalinin, https://github.com/petr-kalinin/radarmap, GNU AGPL

var map;
var layers = {};
var timeout;
var data;
var currentFrame;
var frameHistory;
var imageCache = [];
var cacheStartSize;

function init() {
    var attribution = new ol.control.Attribution({
        collapsible: false
    });
    //var mapQuestSource = new ol.source.MapQuest({
    //        layer: 'osm',
    //    });
    //var osmLayer = new ol.layer.Tile({source: mapQuestSource});
    var osmLayer = new ol.layer.Tile({
        source: new ol.source.XYZ({
            //url: 'http://ingreelab.net/C04AF0B62BEC112E8D7242FB848631D12D252728/{z}/{x}/{y}.png',
            url: 'tiles.php?z={z}&x={x}&y={y}',
            attributions: [
                new ol.Attribution({ html: ['Данные карты &copy; участники <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>, ODbL; изображения &copy; <a href="http://чепецк.net">Чепецк.net</a>'] }),
                new ol.Attribution({
                    html: 'Данные с радаров &copy; <a href="http://meteorad.ru/">ФГБУ «ЦАО» Росгидромета, НТЦР ДМРЛ</a>'
                }),
                new ol.Attribution({
                    html: 'Сборка всего на одну карту &copy; <a href="mailto:petr@kalinin.nnov.ru">Петр Калинин</a>, <a href="http://www.gnu.org/licenses/agpl-3.0.en.html">AGPL</a>, <a href="https://github.com/petr-kalinin/radarmap">github.com/petr-kalinin/radarmap</a>'
                })
            ]
        })
    });
        
    map = new ol.Map({
        target: 'map',
        layers: [
            osmLayer,
        ],
        //renderer: 'webgl',
        controls: ol.control.defaults({ attribution: false }).extend([attribution]),
        view: new ol.View({
        center: [4895650.867513087, 7617122.422797349],
        zoom: 7
        })
    });
    
    reload();
}

function processRequest(xmlhttp) {
    if (xmlhttp.readyState == XMLHttpRequest.DONE ) {
        if(xmlhttp.status == 200){
            addLayers(JSON.parse(xmlhttp.responseText));
        }
        else {
            console.log('Error ' + xmlhttp.status)
        }
    }
}

function imageUrl(layerId, fname) {
    return "images/" + layerId + "-merc-" + fname + ".png";
}

function setLayerSource(layerId, fname) {
    var url = imageUrl(layerId, fname)
    console.log("Set layer source " + layerId + " to " + fname)
    var extent = data[layerId]["corners"];
    var source = new ol.source.ImageStatic({
                url: url,
                imageExtent: extent
            });
    if (!layers[layerId]) {
        layers[layerId] = new ol.layer.Image({
            source: source,
            opacity: 0.65,
        });
        map.addLayer(layers[layerId]);
        if (map.onNewRadarLayer)
            map.onNewRadarLayer()
    } else {
        layers[layerId].setSource(source);
    }
}

function addLayers(json) {
    data = json;
    for (var id in data) {
        if (data.hasOwnProperty(id)) { 
            setLayerSource(id, data[id]["images"][0]);
        }
    }
}

function reload() {
    var xmlhttp = new XMLHttpRequest(); 
    xmlhttp.onreadystatechange = function () { processRequest(xmlhttp); }
    xmlhttp.open("GET", "images.json", true);
    xmlhttp.send();
    timeout = setTimeout(reload, 10000);
}

function parseTimeFromNow(timeStr) {
    var y = timeStr[0] + timeStr[1] + timeStr[2] + timeStr[3];
    var m = timeStr[4] + timeStr[5] -1;
    var d = timeStr[6] + timeStr[7];
    var h = timeStr[9] + timeStr[10];
    var min = timeStr[11] + timeStr[12];
    var date = new Date(y,m,d,h,min,0,0);
    var now = new Date();
    var diff = Math.floor((now-date)/60/1000) + now.getTimezoneOffset();
    return diff;
}

function preloadImage(url) {
    var img = new Image();
    img.onload = function() {
        var index = imageCache.indexOf(this);
        if (index !== -1) {
            // remove image from the array once it's loaded
            // for memory consumption reasons
            imageCache.splice(index, 1);
        }
    }
    imageCache.push(img);
    img.src = url;
}

function prepareHistory() {
    var timeInterval = 6*60 // minutes
    var frameHistory = []
    var playSpeed = 10 // play minutes per frame
    for (var i=0; i<=Math.floor(timeInterval/playSpeed); i++) {
        frameHistory.push({})
    }
    for (var id in data) {
        if (data.hasOwnProperty(id)) { 
            for(var timeId in data[id]["images"]) {
                timeStr = data[id]["images"][timeId]
                time = timeInterval - parseTimeFromNow(timeStr) 
                if (time > 0) {
                    frame = Math.floor(time / playSpeed)
                    frameHistory[frame][id] = timeStr;
                    preloadImage(imageUrl(id, timeStr));
                }
            }
        }
    }
    cacheStartSize = imageCache.length
    return frameHistory;
}

function waitForHistory() {
    if (imageCache.length == 0) {
        stepFrame()
        return
    }
    var loaded = cacheStartSize - imageCache.length
    document.getElementById("timeFrame").innerHTML = "Загрузка " + loaded + " / " + cacheStartSize;
    setTimeout(waitForHistory, 500)
}

function playHistory() {
    clearTimeout(timeout);
    frameHistory = prepareHistory();
    currentFrame = 0
    setTimeout(waitForHistory, 500)
}

function stepFrame() {
    if (currentFrame >= frameHistory.length) {
        document.getElementById("timeFrame").innerHTML = "";
        reload();
        return;
    }
    console.log(currentFrame)
    document.getElementById("timeFrame").innerHTML = (6*60 - currentFrame * 10) + " минут назад";
    for (var id in frameHistory[currentFrame])
        if (frameHistory[currentFrame].hasOwnProperty(id)) 
            setLayerSource(id, frameHistory[currentFrame][id])
    currentFrame++
    timeout = setTimeout(stepFrame, 500);
}