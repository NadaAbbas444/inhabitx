/* Generated from viewer.html by gen_viewer_html.py. Do not edit. */
#pragma once
#include <pgmspace.h>
static const char VIEWER_HTML[] PROGMEM = R"HTML(<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>InhabitX main screen</title>
<style>
  :root { color-scheme: dark; }
  html, body { margin: 0; height: 100%; background: #05080f; color: #8b97ae; font: 14px system-ui, sans-serif; }
  body { display: flex; align-items: center; justify-content: center; }
  canvas { max-width: 100vw; max-height: 100vh; image-rendering: auto; touch-action: none; cursor: pointer; box-shadow: 0 0 0 1px #1e2942; }
  #status { position: fixed; left: 12px; bottom: 10px; }
</style>
</head>
<body>
<canvas id="screen" width="1024" height="600"></canvas>
<div id="status">connecting…</div>
<script>
/* Screen of the InhabitX main running on the XIAO ESP32-S3: draws what the
 * board sends and sends mouse/touch back as presses. Protocol: remote_screen.h. */
const cv = document.getElementById('screen'), ctx = cv.getContext('2d');
const status = document.getElementById('status');
let ws, down = false;

function rgb565(c, d, i) {
  d[i]     = ((c >> 11) & 31) * 255 / 31;
  d[i + 1] = ((c >> 5) & 63) * 255 / 63;
  d[i + 2] = (c & 31) * 255 / 31;
  d[i + 3] = 255;
}

function onFrame(buf) {
  const v = new DataView(buf), t = String.fromCharCode(v.getUint8(0));
  if (t === 'H') {
    cv.width = v.getUint16(1, true); cv.height = v.getUint16(3, true);
    return;
  }
  if (t !== 'R') return;
  const x = v.getUint16(1, true), y = v.getUint16(3, true), w = v.getUint16(5, true), h = v.getUint16(7, true);
  const img = ctx.createImageData(w, h), d = img.data;
  let p = 0;
  for (let o = 9; o + 4 <= buf.byteLength; o += 4) {
    const n = v.getUint16(o, true), c = v.getUint16(o + 2, true);
    for (let k = 0; k < n && p < w * h; k++, p++) rgb565(c, d, p * 4);
  }
  ctx.putImageData(img, x, y);
}

function connect() {
  const port = new URLSearchParams(location.search).get('ws') || 81;    // ?ws= for testing on a PC
  ws = new WebSocket(`ws://${location.hostname || 'inhabitx-main.local'}:${port}/`);
  ws.binaryType = 'arraybuffer';
  ws.onopen = () => { status.textContent = ''; };
  ws.onclose = () => { status.textContent = 'reconnecting…'; setTimeout(connect, 1500); };
  ws.onmessage = e => { if (e.data instanceof ArrayBuffer) onFrame(e.data); };
}

function send(kind, e) {
  if (!ws || ws.readyState !== 1) return;
  const r = cv.getBoundingClientRect();
  const x = Math.round((e.clientX - r.left) * cv.width / r.width);
  const y = Math.round((e.clientY - r.top) * cv.height / r.height);
  ws.send(`${kind} ${Math.max(0, Math.min(cv.width - 1, x))} ${Math.max(0, Math.min(cv.height - 1, y))}`);
}
cv.addEventListener('pointerdown', e => { down = true; cv.setPointerCapture(e.pointerId); send('d', e); });
cv.addEventListener('pointermove', e => { if (down) send('m', e); });
cv.addEventListener('pointerup', e => { down = false; send('u', e); });
cv.addEventListener('pointercancel', e => { down = false; send('u', e); });
connect();
</script>
</body>
</html>
)HTML";
