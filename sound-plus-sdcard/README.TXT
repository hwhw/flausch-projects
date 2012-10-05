Sound-Ausgabe

Benutzt derzeit eine Samplerate von ca. 19531 Hz

Konversion zB:

$ sox input.wav -r19531 -c1 -b8 output-8bit-1channel-19500Hz.wav

Achtung: Zum füttern via UART sollte die Flauschlib OHNE Interrupt-unterstützten UART kompiliert sein (obwohl, vermutlich funktioniert es auch mit, ist nur ziemlich umständlich, weil das Senden eh irrelevant ist).

Zum Füttern der Daten ist im Verzeichnis "host" ein kleines Programm, das den entsprechenden Handshake versteht.
