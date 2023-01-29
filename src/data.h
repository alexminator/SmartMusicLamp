
String index_html = R"====(<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>SML</title>
      <style>
      body {
        background-color: #424242;
      }

      h1, h2, h3 {
        background-color: rgb(47, 47, 47);
        color: white;
        padding: 10px;
      }
      
      #rangevalue{
        font-size:5vw;
        color: white;
      }

      .button {
        position: relative;
        display: inline-block;
        width: 60px;
        height: 34px;
        background-color: #ccc;
        border-radius: 34px;
      }

      .on .button {
        background-color: #2196f3;
      }

      .button:hover {
        background-color: #072a46;
      }

      .slide {
        position: absolute;
        content: "";
        height: 26px;
        width: 26px;
        left: 4px;
        bottom: 4px;
        border-radius: 34px;
        background-color: white;
      }

      .on .slide {
        left: 28px;
      }

      input {
        margin: 0.4rem;
      }

      input[type="range"] {
        -webkit-appearance: none;
        width: 200px;
        height: 10px;
        border-radius: 5px;
        background-image: linear-gradient(#71b9f4, #2196f3);
        background-repeat: no-repeat;
      }
    </style>
  </head>

  <body>
    <center>
      <h1>Smart Music Lamp</h1>
      <h3>Power</h3>
      <div id="stripButton" class="on">
        <span class="button">
          <span class="slide"></span>
        </span>
      </div>
      
      <h3>Brightness</h3>
      <output id="rangevalue">50</output><br>
      <input type="range" value="50" min="0" max="255" id="rangeLvl" oninput="rangevalue.value=value">
     
      <h3>Color</h3>
      <input type="color"  id="colorHTML" name="colorHTML" value="#ff0000" > </br>
      
      <h3>Rainbow</h3>
      <div id="Rainbowbutton" class="on">
        <span class="button">
          <span class="slide"></span>
        </span>
      </div>
    </center>

    <script>

      const StateButton = document.getElementById("stripButton");
      const SliderBright = document.getElementById("rangeLvl");
      const ColorPicker = document.getElementById("colorHTML");
      const RainbowState = document.getElementById("Rainbowbutton");
      
      StateButton.addEventListener("click", changeState);
      SliderBright.addEventListener("click", changeBright);
      ColorPicker.addEventListener("change", changeColor);
      RainbowState.addEventListener("click", changeRainbow);

      function changeState(){
        console.log("%ip");
        const StateButton = document.getElementById("stripButton");

        if(StateButton.className == "on"){
          StateButton.className = "off";
        } else{
          StateButton.className = "on";
        }
        console.log("Cambiando boton de la tira led a " + StateButton.className)
        sendGET("http://%ip/" + StateButton.className);
      };

      function changeBright(){
        const Brightlvl = document.getElementById("rangeLvl");
        console.log("Cambiando nivel a " + Brightlvl.value);
        sendGET("http://%ip/bright?valor=" +  Brightlvl.value);
      }

      function changeColor(){
        console.log("Cambiando color");
        console.log(colorHTML.value);
        colorNow = hexa_a_rgb(colorHTML.value);
        sendGET("http://%ip/color?r=" + colorNow.r +"&g=" + colorNow.g + "&b=" + colorNow.b);
      };

      function hexa_a_rgb(hex) {
        var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
        return result
          ? {
              r: parseInt(result[1], 16),
              g: parseInt(result[2], 16),
              b: parseInt(result[3], 16),
            }
          : null;
      }

      function changeRainbow(){
        const StateButton = document.getElementById("Rainbowbutton");

        if(StateButton.className == "on"){
          StateButton.className = "off";
        } else{
          StateButton.className = "on";
        }
        console.log("Cambiando Arcoiris a " + StateButton.className)
        sendGET("http://%ip/rainbow?ButtonState=" + StateButton.className);
      }
      
      function sendGET(consult){
        const Http = new XMLHttpRequest();
        console.log(`Consultando  ${consult}`)
        Http.open("GET", consult);
        Http.send();

        Http.onreadystatechange = (e) => {
          console.log(Http.status );
          // console.log(Http.responseText);
        };
      };
    </script>
  </body>
</html>
)====";
