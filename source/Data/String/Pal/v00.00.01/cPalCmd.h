

#ifndef _C_PALCMD_H
#define _C_PALCMD_H

#include "TYPEDEF.h"
#include "cRingBufT.h"
#include "cStrT.h"


class cPalCmd
{
public:

  // Durch Übergeben der Achsenanzahl wird die Prozessorkarte
  // neu initialisiert.Dabei wird der Datenspeicher gelöscht und zur
  // Speicheroptimierung entsprechend der Anzahl der Achsen neu
  // eingeteilt.
  void virtual vAchsenanzahl_setzen(cStr* lcStrIn, bool lbAnswer) = 0;

  // Die Prozessorkarte verfährt alle angegebenen Achsen an ihre
  // Nullpunkte(Referenzpunkte).Die Referenzpunkte der Achsen
  // liegen bei isel - Systemen an der dem Motor zugewandten
  // Seite; positive Bewegungen finden in der Richtung vom Motor
  // weg statt.
  void virtual vReferenzfahrt(cStr* lcStrIn, bool lbAnswer) = 0;

  // Der Befehl definiert - getrennt für jede Achse - die Geschwindigkeit,
  // mit der eine Referenzfahrt ausgeführt wird.Dabei handelt es sich nur
  // um die Geschwindigkeit, mit der die Achse in negativer Richtung auf
  // den Motor zuläuft; die Geschwindigkeit aus dem Schalter heraus lässt
  // sich nicht beeinflussen(vgl.Referenzfahrt).
  void virtual vReferenzgeschwindigkeit_setzen(cStr* lcStrIn, bool lbAnswer) = 0;

  // Die Prozessorkarte gibt entsprechend der übergebenen
  // Schrittanzahl und Schrittgeschwindigkeit für jede
  // Leistungsendstufe eine Impulskette aus.Die Verfahrbewegung
  // wird sofort ausgeführt oder gespeichert.
  void virtual vBewegung_relativ(cStr* lcStrIn, bool lbAnswer) = 0;

  // Die Prozessorkarte fährt mit den angegebenen
  // Geschwindigkeiten an die angegebene Position.Die
  // Verfahrbewegung wird sofort ausgeführt.
  void virtual vBewegung_zur_Position(cStr* lcStrIn, bool lbAnswer) = 0;

  // Die Prozessorkarte bricht eine momentane Verfahrbewegung ab.
  void virtual vStop(cStr* lcStrIn, bool lbAnswer) = 0;

  // Der Break - Befehl dient zum Abbruch des Aktuellen Positioniervorganges.
  void virtual vBreak(cStr* lcStrIn, bool lbAnswer) = 0;
};

#endif /* _C_PALCMD_H */
