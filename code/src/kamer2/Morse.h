int fase = 1;

void kort() {
  huidigeTijd = millis();
  if (huidigeTijd - startTijd >= periodeKort) {
    startTijd = huidigeTijd;
    if (digitalRead(blauw)) {
      digitalWrite(blauw, LOW);
      fase++;
    } else {
      digitalWrite(blauw, HIGH);
    }
  }
}

void lang() {
  huidigeTijd = millis();
  if (huidigeTijd - startTijd >= periodeLang) {
    startTijd = huidigeTijd;
    if (digitalRead(blauw)) {
      digitalWrite(blauw, LOW);
      fase++;
    } else {
      digitalWrite(blauw, HIGH);
    }
  }
}

void wachten() {
  digitalWrite(blauw, HIGH);
  huidigeTijd = millis();
  if (huidigeTijd - startTijd >= 1000) {
    startTijd = huidigeTijd;
    if (wacht) {
      wacht = false; fase = 0;
    }
    else {
      wacht = true;
    }
  }
}

//....................................................................
//Alphabet
void a() {
  kort();
  lang();
} void b() {
  lang();
  kort(); kort(); kort();
} void c() {
  lang(); kort(); lang(); kort();
} void d() {
  lang(); kort(); kort();
} void e() {
  kort();
} void f() {
  kort(); kort(); lang(); kort();
} void g() {
  lang(); lang(); kort();
} void h() {
  kort(); kort(); kort(); kort();
} void i() {
  kort(); kort();
} void j() {
  kort(); lang(); lang(); lang();
} void k() {
  lang(); kort(); lang();
} void l() {
  kort(); lang(); kort(); kort();
} void m() {
  lang(); lang();
} void n() {
  lang(); kort();
} void o() {
  lang(); lang(); lang(); !wacht;
} void p() {
  kort(); lang(); lang(); kort();
} void q() {
  lang(); lang(); kort(); lang();
} void r() {
  kort(); lang(); kort();
} void s() {
  kort(); kort(); kort(); !wacht;
} void t() {
  lang();
} void u() {
  kort(); kort(); lang();
} void v() {
  kort(); kort(); kort(); lang();
} void w() {
  kort(); lang(); lang();
} void x() {
  lang(); kort(); kort(); lang();
} void y() {
  lang(); kort(); lang(); lang();
} void z() {
  lang(); lang(); kort(); kort();
}