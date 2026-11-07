function $(id) {
  return document.getElementById(id);
}

function rotors() {
  return ["I", "II", "III", "IV", "V", "VI", "VII", "VIII"];
}

function fillSel(id, v) {
  var s = $(id);
  s.innerHTML = "";
  rotors().forEach(function (r) {
    var o = document.createElement("option");
    o.value = r;
    o.textContent = r;
    s.appendChild(o);
  });
  if (v) {
    s.value = v;
  }
}

function payloadEnigma() {
  return {
    rotorLeft: $("rl").value,
    rotorMiddle: $("rm").value,
    rotorRight: $("rr").value,
    reflector: $("ref").value,
    rings: $("rings").value,
    positions: $("pos").value,
    plugboard: $("plugs").value,
    text: $("text").value
  };
}

function initEnigma() {
  fillSel("rl", "I");
  fillSel("rm", "II");
  fillSel("rr", "III");
  $("go").onclick = function () {
    fetch("/api/enigma", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(payloadEnigma())
    }).then(function (r) { return r.json(); }).then(function (j) {
      if (!j.ok) {
        $("out").textContent = j.error;
        $("meta").textContent = "";
        return;
      }
      $("out").textContent = j.result;
      $("meta").textContent = "Posiciones finales " + j.positions + " | ignorados " + j.ignored;
    });
  };
  $("rst").onclick = function () {
    fetch("/api/enigma/reset", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(payloadEnigma())
    }).then(function () {
      $("out").textContent = "-";
      $("meta").textContent = "Maquina restablecida";
    });
  };
}

function setLed(id, on, cls) {
  $(id).className = "led " + cls + (on ? " on" : "");
}

function initBombe() {
  fillSel("rl", "I");
  fillSel("rm", "II");
  fillSel("rr", "III");
  function body() {
    return {
      rotorLeft: $("rl").value,
      rotorMiddle: $("rm").value,
      rotorRight: $("rr").value,
      reflector: $("ref").value,
      rings: $("rings").value,
      plugboard: $("plugs").value,
      ciphertext: $("cipher").value,
      crib: $("crib").value,
      alignment: parseInt($("align").value || "0", 10)
    };
  }
  function refreshC() {
    fetch("/api/bombe/candidates").then(function (r) { return r.json(); }).then(function (j) {
      var tb = $("cands");
      tb.innerHTML = "";
      (j.items || []).forEach(function (c) {
        var tr = document.createElement("tr");
        tr.innerHTML = "<td>" + c.pos + "</td><td>" + (c.verified ? "si" : "no") + "</td><td>" + c.recovered + "</td><td>" + c.stecker + "</td>";
        tb.appendChild(tr);
      });
    });
  }
  function poll() {
    fetch("/api/bombe/status").then(function (r) { return r.json(); }).then(function (j) {
      $("status").textContent = "Estado: " + j.state + " | evaluadas " + j.evaluated + " / " + j.total;
      $("detail").textContent = "Descartadas " + j.discarded + " | candidatas " + j.candidates + " | verificadas " + j.verified + " | actual " + j.current + " | ultimo descarte " + j.lastDiscard + " | " + j.lastReason;
      setLed("ly", j.leds.yellow, "yellow");
      setLed("lr", j.leds.red, "red");
      setLed("lg", j.leds.green, "green");
      if (j.state === "running") {
        setTimeout(poll, 400);
      } else {
        refreshC();
      }
    });
  }
  $("start").onclick = function () {
    fetch("/api/bombe/start", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(body())
    }).then(function (r) { return r.json(); }).then(function (j) {
      if (!j.ok) {
        $("status").textContent = j.error;
        return;
      }
      poll();
    });
  };
  $("cancel").onclick = function () {
    fetch("/api/bombe/cancel", { method: "POST" }).then(poll);
  };
  $("reset").onclick = function () {
    fetch("/api/bombe/reset", { method: "POST" }).then(function () {
      $("cands").innerHTML = "";
      poll();
    });
  };
}
