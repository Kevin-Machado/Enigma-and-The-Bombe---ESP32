#include "pages.h"

namespace web {

static const char INDEX_HTML[] =
"<!DOCTYPE html><html lang='es'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>"
"<title>Enigma y The Bombe</title><link rel='stylesheet' href='/style.css'></head><body>"
"<header><h1>Maqueta Turing</h1><nav><a href='/'>Inicio</a><a href='/enigma'>Enigma</a><a href='/bombe'>The Bombe</a></nav></header>"
"<main><div class='card'><p>Simulacion educativa de la maquina Enigma y de una Bombe simplificada, ejecutadas en un ESP32.</p>"
"<p class='muted'>Conectado al punto de acceso local. No se usa Internet ni servicios externos.</p></div>"
"<div class='home-grid'><a class='card' href='/enigma'><h2>Maquina Enigma</h2><p>Cifrado y descifrado de tres rotores, plugboard, anillos y doble paso.</p></a>"
"<a class='card' href='/bombe'><h2>The Bombe</h2><p>Busqueda educativa de posiciones iniciales con crib, contradicciones y verificacion.</p></a></div>"
"<p class='muted'>Exposicion de Historia &mdash; 18 de noviembre de 2026.</p></main></body></html>";

static const char ENIGMA_HTML[] =
"<!DOCTYPE html><html lang='es'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>"
"<title>Enigma</title><link rel='stylesheet' href='/style.css'></head><body>"
"<header><h1>Maquina Enigma</h1><nav><a href='/'>Inicio</a><a href='/enigma'>Enigma</a><a href='/bombe'>The Bombe</a></nav></header>"
"<main><div class='card'><div class='grid'>"
"<div><label>Rotor izquierdo</label><select id='rl'></select></div>"
"<div><label>Rotor medio</label><select id='rm'></select></div>"
"<div><label>Rotor derecho</label><select id='rr'></select></div>"
"<div><label>Reflector</label><select id='ref'><option>B</option><option>C</option><option>A</option></select></div>"
"<div><label>Anillos (LMR)</label><input id='rings' value='AAA' maxlength='3'></div>"
"<div><label>Posiciones (LMR)</label><input id='pos' value='AAA' maxlength='3'></div>"
"</div><label>Plugboard (pares, ej. AB CD EF)</label><input id='plugs' value=''>"
"<label>Texto</label><textarea id='text'>HELLOWORLD</textarea>"
"<div class='row' style='margin-top:12px'><button id='go'>Cifrar / Descifrar</button><button class='secondary' id='rst'>Restablecer</button></div>"
"<p class='muted'>Enigma cifra y descifra con el mismo mecanismo. Restaure las posiciones iniciales para descifrar.</p></div>"
"<div class='card'><label>Resultado</label><div class='out' id='out'>-</div><p class='muted' id='meta'></p></div>"
"</main><script src='/app.js'></script><script>initEnigma()</script></body></html>";

static const char BOMBE_HTML[] =
"<!DOCTYPE html><html lang='es'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>"
"<title>The Bombe</title><link rel='stylesheet' href='/style.css'></head><body>"
"<header><h1>The Bombe</h1><nav><a href='/'>Inicio</a><a href='/enigma'>Enigma</a><a href='/bombe'>The Bombe</a></nav></header>"
"<main><div class='card'><p class='muted'>Version educativa. No es una reproduccion electromecanica historica. Una configuracion sin contradicciones es solo candidata.</p>"
"<div class='grid'>"
"<div><label>Rotor izquierdo</label><select id='rl'></select></div>"
"<div><label>Rotor medio</label><select id='rm'></select></div>"
"<div><label>Rotor derecho</label><select id='rr'></select></div>"
"<div><label>Reflector</label><select id='ref'><option>B</option><option>C</option><option>A</option></select></div>"
"<div><label>Anillos (LMR)</label><input id='rings' value='AAA' maxlength='3'></div>"
"<div><label>Alineacion del crib</label><input id='align' value='0'></div>"
"</div><label>Plugboard conocido (vacio = identidad)</label><input id='plugs' value=''>"
"<label>Texto cifrado</label><textarea id='cipher'>KLHFRT</textarea>"
"<label>Crib</label><input id='crib' value='WETTER'>"
"<div class='row' style='margin-top:12px'><button id='start'>Iniciar busqueda</button>"
"<button class='danger' id='cancel'>Cancelar</button><button class='secondary' id='reset'>Reiniciar</button></div></div>"
"<div class='card'><p><span id='ly' class='led yellow'></span>Amarillo busqueda "
"<span id='lr' class='led red'></span>Rojo contradiccion "
"<span id='lg' class='led green'></span>Verde verificado</p>"
"<p id='status'>Estado: idle</p><p class='muted' id='detail'></p></div>"
"<div class='card'><h3>Candidatos</h3><table><thead><tr><th>Posicion</th><th>Verificado</th><th>Recuperado</th><th>Stecker inferido</th></tr></thead>"
"<tbody id='cands'></tbody></table></div>"
"</main><script src='/app.js'></script><script>initBombe()</script></body></html>";

static const char CSS[] =
":root{--bg:#0e1116;--panel:#171c24;--ink:#e8dcc4;--muted:#9a8f78;--line:#2c3340;--gold:#c9a227;--red:#b33a3a;--green:#3d8b5a}"
"*{box-sizing:border-box}html,body{margin:0;background:var(--bg);color:var(--ink);font-family:Georgia,serif}"
"a{color:var(--gold);text-decoration:none}header{border-bottom:1px solid var(--line);padding:18px 24px;display:flex;justify-content:space-between;align-items:baseline}"
"header h1{margin:0;font-size:22px;letter-spacing:.08em;text-transform:uppercase}header nav a{margin-left:16px}"
"main{max-width:980px;margin:0 auto;padding:24px}.card{background:var(--panel);border:1px solid var(--line);padding:18px 20px;margin:0 0 18px}"
".grid{display:grid;grid-template-columns:1fr 1fr;gap:14px}label{display:block;font-size:12px;color:var(--muted);margin:8px 0 4px;text-transform:uppercase}"
"input,select,textarea,button{font:inherit;background:#10141b;color:var(--ink);border:1px solid var(--line);padding:8px 10px;width:100%}"
"textarea{min-height:90px;letter-spacing:.12em}button{cursor:pointer;background:var(--gold);color:#111;border:0;font-weight:700;text-transform:uppercase}"
"button.secondary{background:transparent;color:var(--ink);border:1px solid var(--line)}button.danger{background:var(--red);color:#fff}"
".row{display:flex;gap:10px}.row>*{flex:1}.out{font-family:monospace;letter-spacing:.18em;white-space:pre-wrap}"
".muted{color:var(--muted);font-size:13px}.led{display:inline-block;width:14px;height:14px;border-radius:50%;background:#333;margin-right:6px}"
".led.on.yellow{background:#c9a227}.led.on.red{background:var(--red)}.led.on.green{background:var(--green)}"
"table{width:100%;border-collapse:collapse}th,td{border-bottom:1px solid var(--line);padding:8px;text-align:left}"
".home-grid{display:grid;grid-template-columns:1fr 1fr;gap:18px}.home-grid a.card{display:block;min-height:140px}"
"@media(max-width:720px){.grid,.home-grid{grid-template-columns:1fr}}";

static const char JS[] =
"function $(id){return document.getElementById(id)}"
"function rotors(){return['I','II','III','IV','V','VI','VII','VIII']}"
"function fillSel(id,v){var s=$(id);s.innerHTML='';rotors().forEach(function(r){var o=document.createElement('option');o.value=r;o.textContent=r;s.appendChild(o)});if(v)s.value=v}"
"function payloadEnigma(){return{rotorLeft:$('rl').value,rotorMiddle:$('rm').value,rotorRight:$('rr').value,reflector:$('ref').value,rings:$('rings').value,positions:$('pos').value,plugboard:$('plugs').value,text:$('text').value}}"
"function initEnigma(){fillSel('rl','I');fillSel('rm','II');fillSel('rr','III');$('go').onclick=function(){fetch('/api/enigma',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(payloadEnigma())}).then(function(r){return r.json()}).then(function(j){if(!j.ok){$('out').textContent=j.error;$('meta').textContent='';return}$('out').textContent=j.result;$('meta').textContent='Posiciones finales '+j.positions+' | ignorados '+j.ignored})} ;$('rst').onclick=function(){fetch('/api/enigma/reset',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(payloadEnigma())}).then(function(){$('out').textContent='-';$('meta').textContent='Maquina restablecida'})}}"
"function setLed(id,on,cls){var e=$(id);e.className='led '+cls+(on?' on':'')}"
"function initBombe(){fillSel('rl','I');fillSel('rm','II');fillSel('rr','III');var t=null;"
"function body(){return{rotorLeft:$('rl').value,rotorMiddle:$('rm').value,rotorRight:$('rr').value,reflector:$('ref').value,rings:$('rings').value,plugboard:$('plugs').value,ciphertext:$('cipher').value,crib:$('crib').value,alignment:parseInt($('align').value||'0',10)}}"
"function poll(){fetch('/api/bombe/status').then(function(r){return r.json()}).then(function(j){$('status').textContent='Estado: '+j.state+' | evaluadas '+j.evaluated+' / '+j.total;$('detail').textContent='Descartadas '+j.discarded+' | candidatas '+j.candidates+' | verificadas '+j.verified+' | actual '+j.current+' | ultimo descarte '+j.lastDiscard+' | '+j.lastReason;setLed('ly',j.leds.yellow,'yellow');setLed('lr',j.leds.red,'red');setLed('lg',j.leds.green,'green');if(j.state==='running'){t=setTimeout(poll,400)}else{refreshC()}})}"
"function refreshC(){fetch('/api/bombe/candidates').then(function(r){return r.json()}).then(function(j){var tb=$('cands');tb.innerHTML='';(j.items||[]).forEach(function(c){var tr=document.createElement('tr');tr.innerHTML='<td>'+c.pos+'</td><td>'+(c.verified?'si':'no')+'</td><td>'+c.recovered+'</td><td>'+c.stecker+'</td>';tb.appendChild(tr)})})}"
"$('start').onclick=function(){fetch('/api/bombe/start',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(body())}).then(function(r){return r.json()}).then(function(j){if(!j.ok){$('status').textContent=j.error;return}poll()})};"
"$('cancel').onclick=function(){fetch('/api/bombe/cancel',{method:'POST'}).then(poll)};"
"$('reset').onclick=function(){fetch('/api/bombe/reset',{method:'POST'}).then(function(){$('cands').innerHTML='';poll()})}}";

const char *pageIndex() { return INDEX_HTML; }
const char *pageEnigma() { return ENIGMA_HTML; }
const char *pageBombe() { return BOMBE_HTML; }
const char *pageCss() { return CSS; }
const char *pageJs() { return JS; }

}  // namespace web
