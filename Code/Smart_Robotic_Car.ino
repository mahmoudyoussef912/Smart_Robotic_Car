#include <WiFi.h>
#include <WebServer.h>

// ===== WIFI AP CONFIG =====
const char* ssid = "Smart Robotic Car";
const char* password = "12345678";

WebServer server(80);

// ===== MOTOR PINS =====
// Left motor
const int enA = 5;
const int IN1 = 18;
const int IN2 = 19;
// Right motor
const int IN3 = 21;
const int IN4 = 22;
const int enB = 23;

// ===== ACCESSORIES PINS 
const int redLed = 33;       // Brake & Reverse
const int blueLed = 25;      // Headlights
const int turnLeftLed = 26;  // Left Blinker
const int turnRightLed = 32; // Right Blinker
const int buzzer = 27;       // Horn & Reverse Beep

// ===== PWM CONFIG =====
const int freq = 5000;
const int resolution = 8;
const int maxPwm = 255;

// ===== STATE VARIABLES =====
int baseSpeed = 200;
bool headlightsOn = false;

// Blinkers State
bool blinkLeftActive = false;
bool blinkRightActive = false;
unsigned long prevBlinkTime = 0;
const int blinkDelay = 400; // ms
bool blinkerState = LOW;

// Buzzer State
bool hornActive = false;
bool reverseBuzzerActive = false;
unsigned long prevBuzzerTime = 0;
const int buzzerDelay = 250; // ms
bool buzzerState = LOW;

// ===== MOTOR CONTROL =====
void setSpeed(int a, int b) {
  ledcWrite(enA, a);
  ledcWrite(enB, b);
}

void clearBlinkersAndReverse() {
  blinkLeftActive = false;
  blinkRightActive = false;
  reverseBuzzerActive = false;
  digitalWrite(turnLeftLed, LOW);
  digitalWrite(turnRightLed, LOW);
}

void stopAll() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  setSpeed(0, 0);
  
  digitalWrite(redLed, HIGH); // Brake light ON
  clearBlinkersAndReverse();
}

void moveStart() {
  digitalWrite(redLed, LOW); // Brake OFF
  clearBlinkersAndReverse();
}

void forward() {
  moveStart();
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  setSpeed(baseSpeed, baseSpeed);
}

void backward() {
  moveStart();
  digitalWrite(redLed, HIGH); // Reverse Light
  reverseBuzzerActive = true; // Reverse Beep
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  setSpeed(baseSpeed, baseSpeed);
}

void turnLeft() {
  moveStart(); blinkLeftActive = true;
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  setSpeed(baseSpeed, baseSpeed);
}

void turnRight() {
  moveStart(); blinkRightActive = true;
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  setSpeed(baseSpeed, baseSpeed);
}

void forwardLeft() {
  moveStart(); blinkLeftActive = true;
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  setSpeed(baseSpeed / 2, baseSpeed); // Left motor slower
}

void forwardRight() {
  moveStart(); blinkRightActive = true;
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  setSpeed(baseSpeed, baseSpeed / 2); // Right motor slower
}

void backwardLeft() {
  moveStart();
  digitalWrite(redLed, HIGH); // Reverse Light
  reverseBuzzerActive = true; // Reverse Beep
  blinkLeftActive = true;
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  setSpeed(baseSpeed / 2, baseSpeed);
}

void backwardRight() {
  moveStart();
  digitalWrite(redLed, HIGH); // Reverse Light
  reverseBuzzerActive = true; // Reverse Beep
  blinkRightActive = true;
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  setSpeed(baseSpeed, baseSpeed / 2);
}

void updateHeadlights() {
  digitalWrite(blueLed, headlightsOn ? HIGH : LOW);
}

// ===== WEB PAGE =====
String getWebPage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>Smart Robotic Car</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; -webkit-tap-highlight-color: transparent; }

    html, body {
      width: 100vw; height: 100vh; overflow: hidden;
      font-family: 'Segoe UI', Tahoma, sans-serif;
      background: #0b1120; /* Deep blue dark mode */
      color: #f8fafc; touch-action: none; user-select: none;
    }

    /* Animated background glow */
    body::before {
      content: ''; position: fixed; inset: 0;
      background: radial-gradient(circle at 15% 20%, rgba(56,189,248,0.15) 0%, transparent 40%),
                  radial-gradient(circle at 85% 80%, rgba(244,63,94,0.1) 0%, transparent 40%);
      pointer-events: none; z-index: 0;
    }

    .page { position: relative; z-index: 1; width: 100%; height: 100%; display: flex; flex-direction: column; }

    /* Top Bar */
    .top-bar { 
      display: flex; flex-direction: column; gap: 15px; padding: 20px 25px; 
      background: linear-gradient(180deg, rgba(15,23,42,0.9), rgba(15,23,42,0.2));
      border-bottom: 1px solid rgba(255,255,255,0.03); 
    }
    
    .header-row { display: flex; justify-content: space-between; align-items: center; width: 100%; }
    
    .top-bar h1 { 
      font-size: 1.6rem; font-weight: 800; 
      background: linear-gradient(to right, #38bdf8, #818cf8); 
      -webkit-background-clip: text; -webkit-text-fill-color: transparent;
      letter-spacing: 0.5px;
    }

    /* 3-Level Speed Segmented Control */
    .speed-control {
      display: flex; width: 100%; max-width: 350px; margin: 0 auto;
      background: #1e293b; border-radius: 14px; padding: 5px; gap: 5px;
      box-shadow: inset 0 2px 4px rgba(0,0,0,0.4);
    }
    .spd-btn {
      flex: 1; padding: 10px 0; font-size: 0.9rem; font-weight: 700;
      border-radius: 10px; border: none; background: transparent; color: #64748b;
      cursor: pointer; transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
    }
    .spd-btn.active.slow { background: #3b82f6; color: #fff; box-shadow: 0 2px 8px rgba(59,130,246,0.4); }
    .spd-btn.active.normal { background: #10b981; color: #fff; box-shadow: 0 2px 8px rgba(16,185,129,0.4); }
    .spd-btn.active.turbo { background: #f43f5e; color: #fff; box-shadow: 0 2px 8px rgba(244,63,94,0.4); }

    /* Main Layout */
    .main-controls { flex: 1; display: flex; padding: 10px 20px 25px 20px; gap: 10px; }
    .col { display: flex; flex: 1; }
    
    /* Left Column (Fwd/Rev) */
    .left-col { flex-direction: column; gap: 20px; align-items: center; justify-content: center; }
    .left-col .ctrl-btn { width: 90%; height: 35%; border-radius: 24px; }

    /* Center Column (Bulb/Horn) */
    .center-col { flex-direction: column; gap: 35px; align-items: center; justify-content: center; flex: 0.6; }

    /* Right Column (Left/Right) */
    .right-col { flex-direction: row; gap: 15px; align-items: center; justify-content: center; }
    .right-col .ctrl-btn { flex: 1; height: 35%; max-height: 250px; border-radius: 24px; }

    /* Tactile Neumorphic Buttons */
    .ctrl-btn {
      background: linear-gradient(145deg, #1e293b, #0f172a);
      border: 1px solid #334155; color: #94a3b8; font-size: 3rem;
      cursor: pointer; transition: all 0.1s;
      display: flex; align-items: center; justify-content: center;
      box-shadow: 8px 8px 16px #070a13, -8px -8px 16px #172441; /* 3D pop effect */
    }
    .ctrl-btn.active { 
      background: linear-gradient(145deg, #0ea5e9, #0284c7); border-color: #38bdf8; color: #fff;
      box-shadow: inset 4px 4px 10px #0369a1, inset -4px -4px 10px #38bdf8, 0 0 20px rgba(56,189,248,0.5); 
      transform: scale(0.96); 
    }

    /* Special Gadget Buttons */
    .bulb-btn, .horn-btn {
      width: 75px; height: 75px; border-radius: 50%;
      background: linear-gradient(145deg, #1e293b, #0f172a); border: 1px solid #334155;
      font-size: 2.2rem; cursor: pointer; transition: all 0.2s;
      display: flex; align-items: center; justify-content: center;
      box-shadow: 5px 5px 10px #070a13, -5px -5px 10px #172441;
    }
    .bulb-btn.active { 
      background: linear-gradient(145deg, #eab308, #ca8a04); border-color: #fef08a; 
      box-shadow: inset 2px 2px 5px #a16207, inset -2px -2px 5px #fef08a, 0 0 35px rgba(234,179,8,0.7); 
      transform: scale(0.96);
    }
    .horn-btn.active { 
      background: linear-gradient(145deg, #f43f5e, #e11d48); border-color: #fecdd3; 
      box-shadow: inset 2px 2px 5px #be123c, inset -2px -2px 5px #fecdd3, 0 0 35px rgba(244,63,94,0.7); 
      transform: scale(0.93);
    }

  </style>
</head>
<body>
  <div class="page">
    <div class="top-bar">
      <div class="header-row">
        <h1>Smart Robotic Car</h1>
      </div>
      
      <!-- 3 Level Speed Control -->
      <div class="speed-control" id="speedControl">
        <button class="spd-btn slow" onclick="setSpeed(170, 'slow', this)">Slow</button>
        <button class="spd-btn normal active" onclick="setSpeed(200, 'normal', this)">Normal</button>
        <button class="spd-btn turbo" onclick="setSpeed(255, 'turbo', this)">Turbo</button>
      </div>
    </div>

    <div class="main-controls">
      <!-- Left Column -->
      <div class="col left-col">
        <button class="ctrl-btn" data-key="up">⬆</button>
        <button class="ctrl-btn" data-key="down">⬇</button>
      </div>

      <!-- Center Column -->
      <div class="col center-col">
        <button class="bulb-btn" id="lightBtn" onclick="toggleLights()">💡</button>
        <button class="horn-btn" id="hornBtn">📢</button>
      </div>

      <!-- Right Column -->
      <div class="col right-col">
        <button class="ctrl-btn" data-key="left">⬅</button>
        <button class="ctrl-btn" data-key="right">➡</button>
      </div>
    </div>
  </div>

  <script>
    let lastCmd = 'S';
    function send(cmd) { 
      if(cmd !== lastCmd) { lastCmd = cmd; fetch('/cmd?c=' + cmd).catch(()=>{}); }
    }

    // Modern Speed Selector
    function setSpeed(spdVul, type, btn) {
      document.querySelectorAll('.spd-btn').forEach(b => {
        b.classList.remove('active', 'slow', 'normal', 'turbo');
      });
      btn.classList.add('active', type);
      fetch('/cmd?c=P' + spdVul); // Sends P170, P200, P255
    }

    // Drive Controller
    let state = { up: false, down: false, left: false, right: false };

    function calculateCommand() {
      if (state.up && state.right) return 'I';
      if (state.up && state.left) return 'G';
      if (state.down && state.right) return 'J';
      if (state.down && state.left) return 'H';
      
      if (state.up) return 'F';
      if (state.down) return 'B';
      if (state.left) return 'L';
      if (state.right) return 'R';
      
      return 'S';
    }

    function updateDriveState() {
      send(calculateCommand());
    }

    document.querySelectorAll('.ctrl-btn').forEach(btn => {
      const key = btn.dataset.key;
      const press = (e) => { if(e) e.preventDefault(); btn.classList.add('active'); state[key] = true; updateDriveState(); };
      const release = (e) => { if(e) e.preventDefault(); btn.classList.remove('active'); state[key] = false; updateDriveState(); };

      btn.addEventListener('pointerdown', press);
      btn.addEventListener('pointerup', release);
      btn.addEventListener('pointercancel', release);
      btn.addEventListener('pointerleave', release);
    });

    // Accessories
    const horn = document.getElementById('hornBtn');
    let hornIsOn = false;
    const hornStart = (e) => { if(e) e.preventDefault(); horn.classList.add('active'); if(!hornIsOn) { fetch('/cmd?c=Z'); hornIsOn = true; } };
    const hornStop = (e) => { if(e) e.preventDefault(); horn.classList.remove('active'); if(hornIsOn) { fetch('/cmd?c=X'); hornIsOn = false; } };
    horn.addEventListener('pointerdown', hornStart);
    horn.addEventListener('pointerup', hornStop);
    horn.addEventListener('pointerleave', hornStop);
    horn.addEventListener('pointercancel', hornStop);

    let lightsOn = false;
    function toggleLights() {
      lightsOn = !lightsOn;
      const btn = document.getElementById('lightBtn');
      if (lightsOn) { btn.classList.add('active'); fetch('/cmd?c=O'); } 
      else { btn.classList.remove('active'); fetch('/cmd?c=C'); }
    }
  </script>
</body>
</html>
)rawliteral";
  return html;
}

// ===== COMMAND HANDLER =====
void applyCommand(char c) {
  switch (c) {
    case 'F': forward();   break;
    case 'B': backward();  break;
    case 'L': turnLeft();  break;
    case 'R': turnRight(); break;
    case 'G': forwardLeft(); break;
    case 'I': forwardRight(); break;
    case 'H': backwardLeft(); break;
    case 'J': backwardRight(); break;
    case 'S': stopAll();   break;
    case 'Z': hornActive = true; break;  // Horn ON
    case 'X': hornActive = false; break; // Horn OFF
    case 'O': headlightsOn = true; updateHeadlights(); break; // Lights ON
    case 'C': headlightsOn = false; updateHeadlights(); break; // Lights OFF
    default: break;
  }
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);

  // Motor Pins
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  
  // Accessories Pins
  pinMode(redLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(turnLeftLed, OUTPUT);
  pinMode(turnRightLed, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // Setup PWM for Motors (ESP32 core handles channel automatically if you use ledcAttach, ledcWrite)
  ledcAttach(enA, freq, resolution);
  ledcAttach(enB, freq, resolution);

  // Initial State
  updateHeadlights();
  stopAll(); 

  // WiFi Access Point
  WiFi.softAP(ssid, password);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  // Web routes
  server.on("/", []() {
    server.send(200, "text/html", getWebPage());
  });

  server.on("/cmd", []() {
    if (server.hasArg("c")) {
      String cmd = server.arg("c");
      if (cmd.startsWith("P")) {
        baseSpeed = cmd.substring(1).toInt();
        baseSpeed = constrain(baseSpeed, 0, 255);
      } else if (cmd.length() == 1) {
        applyCommand(cmd.charAt(0));
      }
    }
    server.send(200, "text/plain", "OK");
  });

  server.begin();
  Serial.println("Server started!");
}

// ===== LOOP =====
void loop() {
  server.handleClient();
  unsigned long currentMillis = millis();

  // Handling Blinkers dynamically without delay
  if (blinkLeftActive || blinkRightActive) {
    if (currentMillis - prevBlinkTime >= blinkDelay) {
      prevBlinkTime = currentMillis;
      blinkerState = !blinkerState; // Toggle state

      if (blinkLeftActive) digitalWrite(turnLeftLed, blinkerState);
      if (blinkRightActive) digitalWrite(turnRightLed, blinkerState);
    }
  } else {
    digitalWrite(turnLeftLed, LOW);
    digitalWrite(turnRightLed, LOW);
    blinkerState = LOW;
  }

  // Handling Buzzer dynamically (Horn vs Reverse Beep)
  if (hornActive) {
    digitalWrite(buzzer, HIGH);
  } else if (reverseBuzzerActive) {
    if (currentMillis - prevBuzzerTime >= buzzerDelay) {
      prevBuzzerTime = currentMillis;
      buzzerState = !buzzerState;
      digitalWrite(buzzer, buzzerState);
    }
  } else {
    digitalWrite(buzzer, LOW);
    buzzerState = LOW;
  }
}