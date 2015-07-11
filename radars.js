var map;
var layers = {};

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
                    html: 'Сборка всего на одну карту &copy; <a href="mailto:petr@kalinin.nnov.ru">Петр Калинин</a>, <a href="http://www.gnu.org/licenses/agpl-3.0.en.html">AGPL</a>'
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

function addLayers(json) {
    for (var id in json) {
        if (json.hasOwnProperty(id)) { 
            var url = "images/" + id + "-merc-" + json[id]["images"][0] + ".png";
            var extent = json[id]["corners"];
            var source = new ol.source.ImageStatic({
                        url: url,
                        imageExtent: extent
                    });
            if (!layers[id]) {
                layers[id] = new ol.layer.Image({
                    source: source,
                    opacity: 0.65,
                });
                map.addLayer(layers[id]);
            } else {
                console.log("Set layer " + id + " to " + url);
                layers[id].setSource(source);
            }
        }
    }
}

function reload() {
    var xmlhttp = new XMLHttpRequest(); 
    xmlhttp.onreadystatechange = function () { processRequest(xmlhttp); }
    xmlhttp.open("GET", "images.json", true);
    xmlhttp.send();
    setTimeout(reload, 10000);
}