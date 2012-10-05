Sound-Ausgabe

Benutzt derzeit eine Samplerate von ca. 132300 Hz

Konversion zB:

$ sox input.wav -r132300 -c1 -b8 output-8bit-1channel-132300Hz.wav

Die Daten werden von einer an UEXT angeschlossenen SD-Karte gelesen.
Der Offset der Daten, die dort am Stück liegen müssen (unfragmentiert),
ist als Konstante in der main.c definiert (als 512-Byte-Block-Count).

Abgespielt wird, bis die SD-Karte nichts mehr liefert...
