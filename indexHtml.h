#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char index_html[] PROGMEM = R"rawliteral(
    <!DOCTYPE HTML>
    <html>
    
    <head>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
            body {
                font-family: Arial;
                text-align: center;
                margin: 0px auto;
                padding-top: 30px;
            }
    
            .button {
                padding: 10px 20px;
                font-size: 24px;
                text-align: center;
                outline: none;
                color: #fff;
                background-color: #2f4468;
                border: none;
                border-radius: 5px;
                box-shadow: 0 6px #999;
                cursor: pointer;
                -webkit-tap-highlight-color: rgba(0, 0, 0, 0);
            }
    
            .button:hover {
                background-color: #1f2e45
            }
    
            .button:active {
                background-color: #1f2e45;
                box-shadow: 0 4px #666;
                transform: translateY(2px);
            }
    
            .leds {
                width: 100px;
                height: 100px;
                margin: 20px;
                display: inline-block;
                border: 5px solid black;
                border-radius: 50%;
            }
    
            .container {
                text-align: center;
            }
    
            #led1 {
                background-color: white;
            }
    
            @media screen and (max-width: 480px) {
                .leds {
                    width: 50px;
                    height: 50px;
                }
    
                .button {
                    padding: 15px 100px 15px 10px;
                    font-size: 10px;
                }
    
                h1 {
                    font-size: 24px;
                    padding-top: 20px;
                }
            }
        </style>
    </head>
    
    <body>
        <h1>ESP Pushbutton Web Server</h1>
        <!-- onmousedown / onmouseup - on PC/Laptop, ontouchend / ontouchstart - on mobile -->
        <button class="button" onmousedown="algorighm1('on_alg1');" ontouchstart="algorighm1('on_alg1');"
            onmouseup="algorighm1('off_alg1');" ontouchend="algorighm1('off_alg1');">Algorithm 1</button>
    
    
        <script>
    let isHolding = false;
    
    function algorighm1(x) {
        if ((x === 'on_alg1' && isHolding) || (x === 'off_alg1' && !isHolding)) {
            return; 
        }
        
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/" + x, true);
        xhr.send();
    
        isHolding = (x === 'on_alg1');
    }
    
    document.addEventListener('DOMContentLoaded', function () {
        const button = document.querySelector('.button');
    
        button.addEventListener('mousedown', function (event) {
            event.preventDefault();
            algorighm1('on_alg1');
        });
    
        button.addEventListener('mouseup', function (event) {
            event.preventDefault();
            algorighm1('off_alg1');
        });
    
        button.addEventListener('touchstart', function (event) {
            event.preventDefault();
            algorighm1('on_alg1');
        });
    
        button.addEventListener('touchend', function (event) {
            event.preventDefault();
            algorighm1('off_alg1');
        });
    });
    
        </script>
    </body>
    
    </html>
    )rawliteral";

#endif