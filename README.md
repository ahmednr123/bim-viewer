### Prerequisites
- emscripten (https://emscripten.org/docs/getting_started/downloads.html)
- NodeJs (https://nodejs.org/en)

### Building the wasm file
- Move to directory with the code: `cd ./public`
- Build the required files: `emcc shader.cpp camera.cpp drawable.cpp main.cpp -o index.html -s "EXPORTED_FUNCTIONS=['_lock','_free_lock','_update_camera_state','_main']" -s USE_WEBGL2=1 -s USE_GLFW=3 -s WASM=1 -std=c++1z`
- Make the below changes to the index.html (Copy from index.backup.html) (Sorry for this, didn't have enough time)
  <details>
    <summary>Changes</summary>
    
    ```
    --- /home/ahmed/Documents/bim-viewer/public/index.html
    +++ /home/ahmed/Documents/bim-viewer/public/index.backup.html
    @@ -14,7 +14,7 @@
     div.emscripten { text-align: center; }
     div.emscripten_border { border: 1px solid black; }
     /* the canvas *must not* have any border or padding, or mouse coords will be wrong */
    -canvas.emscripten { border: 0px none; background-color: black; }
    +canvas.emscripten { border: 0px none; background-color: black; position: fixed; left: 0; top: 0; width: 100%; height: 100%; }
     
     #emscripten_logo {
       display: inline-block;
    @@ -100,7 +100,102 @@
       font-family: 'Lucida Console', Monaco, monospace;
       outline: none;
     }
    +#lock_border_left {
    +    position: fixed;
    +    width: 10px;
    +    height: 100%;
    +    top: 0;
    +    left: 0;
    +    display: none;
    +}
    +#lock_border_top {
    +    position: fixed;
    +    width: 100%;
    +    height: 10px;
    +    top: 0;
    +    left: 0;
    +    display: none;
    +}
    +#lock_border_right {
    +    position: fixed;
    +    width: 10px;
    +    height: 100%;
    +    top: 0;
    +    right: 0;
    +    display: none;
    +}
    +#lock_border_bottom {
    +    position: fixed;
    +    width: 100%;
    +    height: 10px;
    +    bottom: 0;
    +    left: 0;
    +    display: none;
    +}
    +#sockets_div {
    +    position: fixed;
    +    top: 10px;
    +    left: 10px;
    +    padding: 10px;
    +}
    +.socket-div {
    +    display: flex;
    +    flex-direction: row;
    +    align-items: center;
    +    padding: 5px;
    +    border: 1px solid lightgray;
    +    border-radius: 3px;
    +    padding-left: 8px;
    +    margin-bottom: 5px;
    +}
    +.socket-color {
    +    margin-right: 5px;
    +    border-radius: 50%;
    +    width: 10px;
    +    height: 10px;
    +}
    +.socket-id {
    +    text-overflow: ellipsis;
    +    overflow: hidden;
    +    width: 100px;
    +    height: 1.2em;
    +    white-space: nowrap;
    +    font-size: 14px;
    +}
    +.color-red { background-color: red; }
    +.color-green { background-color: green; }
    +.color-blue { background-color: blue; }
    +.color-yellow { background-color: yellow; }
    +.color-orange { background-color: orange; }
    +.loading-screen {
    +    position: fixed;
    +    z-index: 10;
    +    width: 100%;
    +    height: 100%;
    +    top: 0; left: 0;
    +    background: white;
    +    padding-top: calc(50vh - 40px);
    +    padding-left: calc(50% - 25px);
    +}
    +/* HTML: <div class="loader"></div> */
    +.loader {
    +  width: 50px;
    +  padding: 8px;
    +  aspect-ratio: 1;
    +  border-radius: 50%;
    +  background: #25b09b;
    +  --_m:
    +    conic-gradient(#0000 10%,#000),
    +    linear-gradient(#000 0 0) content-box;
    +  -webkit-mask: var(--_m);
    +          mask: var(--_m);
    +  -webkit-mask-composite: source-out;
    +          mask-composite: subtract;
    +  animation: l3 1s infinite linear;
    +}
    +@keyframes l3 {to{transform: rotate(1turn)}}
     </style>
    +<script src="./socket.io.min.js"></script>
       </head>
       <body>
         <a href="http://emscripten.org"><img id="emscripten_logo" src="data:image/png;base64,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"></a>
    @@ -111,7 +206,7 @@
         <span id='controls'>
           <span><input type="checkbox" id="resize">Resize canvas</span>
           <span><input type="checkbox" id="pointerLock" checked>Lock/hide mouse pointer &nbsp;&nbsp;&nbsp;</span>
    -      <span><input type="button" value="Fullscreen" onclick="Module.requestFullscreen(document.getElementById('pointerLock').checked, 
    +      <span><input type="button" value="Fullscreen" onclick="Module.requestFullscreen(document.getElementById('pointerLock').checked,
                                                                                     document.getElementById('resize').checked)">
           </span>
         </span>
    @@ -124,6 +219,15 @@
           <canvas class="emscripten" id="canvas" oncontextmenu="event.preventDefault()" tabindex=-1></canvas>
         </div>
         <textarea id="output" rows="8"></textarea>
    +    <div id="lock_border_left"></div>
    +    <div id="lock_border_top"></div>
    +    <div id="lock_border_right"></div>
    +    <div id="lock_border_bottom"></div>
    +    <div id="sockets_div"></div>
    +
    +    <div id="loading-screen" class="loading-screen">
    +        <div class="loader"></div>
    +    </div>
     
         <script type='text/javascript'>
           var statusElement = document.getElementById('status');
    @@ -134,8 +238,8 @@
             print: (function() {
               var element = document.getElementById('output');
               if (element) element.value = ''; // clear browser cache
    -          return (...args) => {
    -            var text = args.join(' ');
    +          return function(text) {
    +            if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
                 // These replacements are necessary if you render to raw HTML
                 //text = text.replace(/&/g, "&amp;");
                 //text = text.replace(/</g, "&lt;");
    @@ -150,7 +254,8 @@
             })(),
             canvas: (() => {
               var canvas = document.getElementById('canvas');
    -
    +          canvas.width = window.innerWidth;
    +          canvas.height = window.innerHeight;
               // As a default initial behavior, pop up an alert when webgl context is lost. To make your
               // application robust, you may want to override this behavior before shipping!
               // See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
    @@ -159,7 +264,7 @@
               return canvas;
             })(),
             setStatus: (text) => {
    -          Module.setStatus.last ??= { time: Date.now(), text: '' };
    +          if (!Module.setStatus.last) Module.setStatus.last = { time: Date.now(), text: '' };
               if (text === Module.setStatus.last.text) return;
               var m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
               var now = Date.now();
    @@ -197,5 +302,6 @@
           };
         </script>
         <script async type="text/javascript" src="index.js"></script>
    +    <script src="./bim_app.js"></script>
       </body>
     </html>
    ```
  </details>
- Install npm packages: `npm install`
- Run server: `node app.js`

### Preview:

https://github.com/user-attachments/assets/c79000ba-2e5a-4354-b7df-51e642ded4a7

