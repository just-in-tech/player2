var ws = new WebSocket("wss://192.168.0.34:8181/websocket/echo");
const btnPlayPause = document.getElementById('play-pause');
const btnFullscreen = document.getElementById('fullscreen');
const btnSkipAd = document.getElementById('skip-ad');
const btnRestart = document.getElementById('restart');
const btnShutdown = document.getElementById('shutdown');
const btnStop = document.getElementById('stop');
const btnSubtitle = document.getElementById('subtitle');
const btnvoldown = document.getElementById('vol-down');
const btnvolup = document.getElementById('vol-up');
const Title = document.getElementById("title")
const PositionText = document.getElementById("position-text")
const TotalTimeText = document.getElementById("total-time-text")
const slider = document.getElementById('myRange');

ws.addEventListener("open", () => {
console.log("connected!");
});


 const messagereceived = (data => {
const mdata = JSON.parse(data.data);

console.log(mdata.position);
if(mdata.hasOwnProperty('title')){
  Title.textContent=mdata.title;
}
if(mdata.hasOwnProperty('length')){
  TotalTimeText.textContent=mdata.length;
}
if(mdata.hasOwnProperty('position')){
PositionText.textContent=mdata.position;
}
if(mdata.hasOwnProperty('sliderposition')){
  let e = slider
  e.value=mdata.sliderposition;
}
});

const PlayPause = () => {
ws.send("playpause")
};

const Fullscreen = () => {
  ws.send("fullscreen")
};

const Stop = () => {
  ws.send("stop")
};

const Shutdown = () => {
  ws.send("powerdown")
};

const Restart = () => {
  ws.send("reboot")
};

const Subtitle = () => {
  ws.send("closecapt")
};

const SkipAd = () => {
  ws.send("skipad")
};

const VolDown = () => {
  ws.send("vol-")
};

const VolUp = () => {
  ws.send("vol+")
};

const sliderChange = () => {
let sendposition="P" + slider.value
console.log(sendposition)
ws.send(sendposition)
};

document.addEventListener('visibilitychange', function (event){
    if (document.hidden) {
        console.log('not visible');
    } else {
        console.log('is visible'); 
        
        	ws=new WebSocket("wss://192.168.0.34:8181/websocket/echo");
        	ws.addEventListener("message",messagereceived)
    
}
});
ws.addEventListener("message",messagereceived)
slider.addEventListener("change", sliderChange);
btnPlayPause.addEventListener('click', PlayPause);
btnFullscreen.addEventListener('click', Fullscreen);
btnSkipAd.addEventListener('click', SkipAd);
btnStop.addEventListener('click', Stop);
btnRestart.addEventListener('click', Restart);
btnShutdown.addEventListener('click', Shutdown);
btnSubtitle.addEventListener('click', Subtitle);
btnvoldown.addEventListener('click', VolDown);
btnvolup.addEventListener('click', VolUp);

