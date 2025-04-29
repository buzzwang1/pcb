

#ifndef _C_CNC_H
#define _C_CNC_H

#include "TYPEDEF.h"
#include "cRingBufT.h"
#include "cStrT.h"
#include "cPal.h"
#include "math.h"


class cCncAchseHW
{
public:
  u8 mu8Ref;

  cCncAchseHW()
  {
    mu8Ref = 0;
  }

  
  bool virtual isRefSwitch() { return False;};

  //  Der Richtungsausgang gibt je nach vorgegebener Drehrichtung des Schrittmotors
  //  ein + 5 V - Signal(Drehrichtung CCW) oder ein 0 V - Signal(Drehrichtung CW) aus.
  // positive Bewegungen finden in der Richtung vom Motor weg statt.
  // 0 = negative Drehrichtung des Schrittmotors CW; 1 = positive Drehrichtung des Schrittmotors CCW
  void virtual vSetDir(u8 lu8Dir) {};

  // 0..50000Hz
  void virtual vSetSpeed_Hz(u16 lu16Speed) {};

  void virtual vDoStep() {};
};




class cCncAchse
{
public:
  // Takt 0..50khz
  // Beschleunigung  25 Hz/ms, 50 Hz/ms, 75 Hz/ms, 100 Hz/ms
  cCncAchseHW mcHw;

  double mdSpeedAccMax;  // [Hz/ms]

  double mdSpeedAcc;     // [Hz/ms]
  double mdSpeedSoll;    // [Hz]
  double mdSpeedIst;     // [Hz]
  double mdRefSpeed;     // [Hz]

  double mdPosSoll;      // [Ticks]
  double mdPosIst;       // [Ticks]

  double mdTicksToAcc;   // [Ticks]
  double mdTicksToMove;  // [Ticks]
  double mdTicksToDeacc; // [Ticks]

  u32    mdTimeNoMove;       // [ms]
  u32    mdTimeNoMoveReload; // [ms]

  double mdDirection;

  bool   mbEnable;


  bool   mbRefEnable;
  bool   mbRefFinished;
  u32    mu32Ref1s;
  u32    mu32RefTimeout;
  u32    mu32RefTimeoutReload;
  typedef enum
  {
    enMove1sForward1 = 0,
    enMove1sForward2,
    enMoveOnRefSwitch1,
    enMoveOnRefSwitch2,
    enMoveOffRefSwitch1,
    enMoveOffRefSwitch2,
  }tenRefMode;
  tenRefMode menRefMode;

  cCncAchse(cCncAchseHW lcHw)
  {
    mcHw = lcHw;

    mdSpeedAccMax = 100.0;    // [Hz/ms]

    mdSpeedAcc    = 0.0;      // [Hz/ms]
    mdSpeedSoll   = 0.0;      // [Hz]
    mdSpeedIst    = 0.0;      // [Hz]
    mdRefSpeed    = 2000.0;   // [Hz]

    mdPosSoll     = 0.0;      // [Ticks]
    mdPosIst      = 0.0;      // [Ticks]

    mdTicksToAcc   = 0.0;     // [Ticks]
    mdTicksToMove  = 0.0;     // [Ticks]
    mdTicksToDeacc = 0.0;     // [Ticks]
    mdDirection    = 1.0;

    mdTimeNoMove = mdTimeNoMoveReload = 1 * 1000; // 1s

    mbEnable       = False;


    mbRefEnable    = False;
    mbRefFinished  = False;
    mu32Ref1s = 1 * 1000; // 1s
    mu32RefTimeout = mu32RefTimeoutReload = 1 * 1000 * 60; // 1min
    menRefMode = enMove1sForward1;
  }

  double bTimeToDeaccelerate()
  {
    double lsTickstoStop = mdSpeedIst / mdSpeedAccMax;
    double ldDist = fabs(mdPosSoll - mdPosIst);

    return (lsTickstoStop < ldDist);
  }

  void SetAbsPos(double ldPos, double ldSpeed)
  {
    mdPosSoll   = ldPos;
    mdSpeedSoll = ldSpeed;
    
    if (mdPosSoll < mdPosIst) mdDirection = -1.0;
                         else mdDirection =  1.0;

    double ldTicksToMove = fabs(mdPosSoll - mdPosIst);
    
    double ldTicksToacc = fabs(mdSpeedSoll - mdSpeedIst) / mdSpeedAccMax;
    double ldTicksToDeacc = mdSpeedSoll / mdSpeedAccMax;

    if ((ldTicksToacc + ldTicksToDeacc) > ldTicksToMove)
    {
      mdSpeedSoll    = (ldTicksToMove * mdSpeedAccMax - mdSpeedIst) / 2;
      mdTicksToAcc   = fabs(mdSpeedSoll - mdSpeedIst) / mdSpeedAccMax;
      mdTicksToMove  = 0.0;
      mdTicksToDeacc = mdSpeedSoll / mdSpeedAccMax;
    }
    else
    {
      mdTicksToAcc   = ldTicksToacc;
      mdTicksToMove  = ldTicksToMove - (ldTicksToacc + ldTicksToDeacc);
      mdTicksToDeacc = ldTicksToDeacc;
    }
  }

  void vSetSpeed(double ldSpeed)
  {
    mdSpeedSoll = ldSpeed;
  }

  void vStop()
  {
    mdSpeedSoll = 0.0;
  }

  void vBreak()
  {
    mdSpeedSoll = 0.0;
  }

  void DoProcessSetSpeed(double ldSpeedSoll)
  {
    if (mdSpeedIst != ldSpeedSoll)
    {
      if (mdSpeedIst < ldSpeedSoll)
      {
        mdSpeedIst += mdSpeedAccMax;
        if (mdSpeedIst > ldSpeedSoll) mdSpeedIst = ldSpeedSoll;
      }
      else
      {
        if (mdSpeedIst > ldSpeedSoll)
        {
          mdSpeedIst -= mdSpeedAccMax;
          if (mdSpeedIst < ldSpeedSoll) mdSpeedIst = ldSpeedSoll;
        }
      }
      mdTimeNoMove = mdTimeNoMoveReload;
    }
  }

  void vStartRef()
  {
    mbRefEnable = False;
    mbRefFinished = False;
    mu32RefTimeout = mu32RefTimeoutReload;
    menRefMode = enMove1sForward1;
  }

  void DoProcessRef1ms()
  {
    if (mu32RefTimeout)
    {
      mu32RefTimeout--;
    }
    else
    {
      mbRefEnable = False;
      mbRefFinished = False;
    }

    switch (menRefMode)
    {
      case enMove1sForward1:
        mcHw.vSetDir(1);
        DoProcessSetSpeed(mdRefSpeed);
        if (mu32Ref1s)
        {
          mu32Ref1s--;
        }
        else
        {
          menRefMode = enMove1sForward2;
          mu32Ref1s = 1 * 1000; // 1s
        }

        break;
      case enMove1sForward2:
        mcHw.vSetDir(1);
        DoProcessSetSpeed(0);
        if (mdSpeedIst <= 0.1)
        {
          menRefMode = enMoveOnRefSwitch1;
        }
        break;
      case enMoveOnRefSwitch1:
        mcHw.vSetDir(0);
        DoProcessSetSpeed(mdRefSpeed);

        if (mcHw.isRefSwitch())
        {
          menRefMode = enMoveOffRefSwitch2;
        }

      case enMoveOnRefSwitch2:
        mcHw.vSetDir(0);
        DoProcessSetSpeed(0);
        if (mdSpeedIst <= 0.1)
        {
          menRefMode = enMoveOffRefSwitch1;
        }
        
        break;
      case enMoveOffRefSwitch1:
        mcHw.vSetDir(1);
        if (mcHw.isRefSwitch())
        {
          mcHw.vDoStep();
        }
        else
        {
          mdPosIst = 0.0;
          menRefMode = enMove1sForward1;
          mbRefEnable = False;
          mbRefFinished = True;
        }
        break;
    }
  }


  void DoProcess1ms()
  {
    if (mbEnable)
    {
      if (mbRefEnable)
      { 
        DoProcessRef1ms();
      }
      else
      {
        if (mdSpeedIst != mdSpeedSoll)
        {
          if (mdSpeedIst < mdSpeedSoll)
          {
            mdSpeedIst += mdSpeedAccMax;
            if (mdSpeedIst > mdSpeedSoll) mdSpeedIst = mdSpeedSoll;
          }
          else
          {
            if (mdSpeedIst > mdSpeedSoll)
            {
              mdSpeedIst -= mdSpeedAccMax;
              if (mdSpeedIst < mdSpeedSoll) mdSpeedIst = mdSpeedSoll;
            }
          }
          mdTimeNoMove = mdTimeNoMoveReload;
        }
        else
        {
          if (mdTimeNoMove) mdTimeNoMove--;
        }
      }
    }
  }
};


class cCnc: public cPalCmd
{
  public:
    cCncAchse X;
    cCncAchse Y;
    cCncAchse Z;

    u8 mu8AxisCount;
    
    typedef enum
    {
      enModeSingle = 0,
      enMode2D5,
      enMode3D
    }tenMoveMode;

    tenMoveMode menMode;

    typedef enum
    {
      enCmdNop = 0,
      enCmdAchsenanzahl_setzen,
      enCmdReferenzfahrt,
      enCmdReferenzgeschwindigkeit_setzen,
      enCmdBewegung_relativ,
      enCmdBewegung_zur_Position,
      enCmdStop,
      enCmdBreak
    }tenProcessedCmd;

    bool mbAnswer;

    tenProcessedCmd menProcessedCmd;

    cRingBufT<uint8, uint16>* mcStreamOut;

    cCnc(cCncAchseHW lcX, cCncAchseHW lcY, cCncAchseHW lcZ, 
         cRingBufT<uint8, uint16>* lcStreamOut)
      : X(lcX), Y(lcY), Z(lcZ)
    {
      mu8AxisCount = 0;
      menMode      = enMode2D5;
      menProcessedCmd = enCmdNop;
      mcStreamOut     = lcStreamOut;
      mbAnswer = False;
    }


    void vStringToParam(cStr* lcStrIn, u8 lu8Seperator, i32 li32Values[], u16 lu16ValueSize)
    {
      cStr_Create(lcZwerg, 64);
      cStr_Create(lcValue, 64);

      u16 lu16t;
      for (lu16t = 0; lu16t < lu16ValueSize; lu16t++)
      {
        li32Values[lu16t] = 0;
      }

      lcZwerg = *lcStrIn;

      u16 lu16SepCnt = lcZwerg.CountOf(lu8Seperator);

      if (lu16SepCnt >= lu16ValueSize) lu16SepCnt = lu16ValueSize - 1;

      u16 lu16PosStart = 0;
      u16 lu16PosEnd   = 0;

      for (lu16t = 0; lu16t < lu16SepCnt; lu16t++)
      {
        lu16PosEnd = lcZwerg.Instr(lu16PosStart, lu8Seperator);
        lcValue = lcZwerg;
        lcValue.Mid(lu16PosStart, lu16PosEnd - lu16PosStart);
        li32Values[lu16t] = lcValue.Atoi();
        lu16PosStart = lu16PosEnd + 1;
      }

      lu16PosEnd = lcZwerg.Len();
      lcValue = lcZwerg;
      lcValue.Mid(lu16PosStart, lu16PosEnd - lu16PosStart);
      li32Values[lu16t] = lcValue.Atoi();
    }

    void vSet(double ldPosX, double ldSpeedX, 
              double ldPosY, double ldSpeedY,
              double ldPosZ, double ldSpeedZ)
    {
      double lfDistX = fabs(X.mdPosSoll - X.mdPosIst);
      double lfDistY = fabs(Y.mdPosSoll - Y.mdPosIst);
      double lfDistZ = fabs(Z.mdPosSoll - Z.mdPosIst);

      if (ldPosX < 0) ldPosX = 0;
      if (ldPosY < 0) ldPosY = 0;
      if (ldPosZ < 0) ldPosZ = 0;

      switch (menMode)
      {
        case enModeSingle:
          X.mdPosSoll = ldPosX;
          X.mdSpeedSoll = ldSpeedX;

          Y.mdPosSoll = ldPosY;
          Y.mdSpeedSoll = ldSpeedY;

          Z.mdPosSoll = ldPosZ;
          Z.mdSpeedSoll = ldSpeedZ;
          break;
        case enMode2D5:
          //  Im 2, 5D - Interpolationsbetrieb wird die
          //    Geschwindigkeitsangabe der Achse mit dem längsten
          //    Weg als Bahngeschwindigkeit übernommen, die
          //    Geschwindigkeit der anderen Achse entsprechend dem
          //    Wegeverhältnis angepasst.
          if (lfDistX > lfDistY)
          {
            X.mdPosSoll = ldPosX;
            X.mdSpeedSoll = ldSpeedX;

            Y.mdPosSoll = ldPosY;
            if (lfDistY > 0) Y.mdSpeedSoll = ldSpeedX * lfDistY / lfDistX;
          }
          else
          {
            X.mdPosSoll = ldPosX;
            if (lfDistX > 0) X.mdSpeedSoll = ldSpeedY * lfDistX / lfDistY;

            Y.mdPosSoll = ldPosY;
            Y.mdSpeedSoll = ldPosY;
          }

          Z.mdPosSoll = ldPosZ;
          Z.mdSpeedSoll = ldPosZ;
          break;
        case enMode3D:
          // Im Gegensatz dazu wird im 3D - Interpolationsbetrieb die
          //   Geschwindigkeitsangabe der X - Achse als Vorgabewert
          //   für die Bahngeschwindigkeit herangezogen.

          X.mdPosSoll = ldPosX;
          X.mdSpeedSoll = ldSpeedX;

          Y.mdPosSoll = ldPosY;
          if (lfDistY > 0) Y.mdSpeedSoll = ldSpeedX * lfDistY / lfDistX;

          Z.mdPosSoll = ldPosZ;
          if (lfDistZ > 0) Z.mdSpeedSoll = ldSpeedX * lfDistZ / lfDistX;
          break;
      }
    }


    // Takt 0..50khz
    // Beschleunigung  25 Hz/ms, 50 Hz/ms, 75 Hz/ms, 100 Hz/ms

    // Durch Übergeben der Achsenanzahl wird die Prozessorkarte
    // neu initialisiert.Dabei wird der Datenspeicher gelöscht und zur
    // Speicheroptimierung entsprechend der Anzahl der Achsen neu
    // eingeteilt.
    void vAchsenanzahl_setzen(cStr* lcStrIn, bool lbAnswer) override 
    { // @07
      UNUSED(lbAnswer);
      
      menProcessedCmd = enCmdAchsenanzahl_setzen;
      mbAnswer = lbAnswer;

      switch (lcStrIn->mpu8Data[2])
      {
        case '1': // x
          X.mbEnable = True;
          mu8AxisCount = 1;
          break;
        case '3': // xy
          X.mbEnable = True;
          Y.mbEnable = True;
          mu8AxisCount = 2;
          break;
        case '5': // xz
          X.mbEnable = True;
          Z.mbEnable = True;
          mu8AxisCount = 2;
          break;
        case '7': // xyz
          X.mbEnable = True;
          Y.mbEnable = True;
          Z.mbEnable = True;
          mu8AxisCount = 3;
          break;
      }
    }

    // Die Prozessorkarte verfährt alle angegebenen Achsen an ihre
    // Nullpunkte(Referenzpunkte).Die Referenzpunkte der Achsen
    // liegen bei isel - Systemen an der dem Motor zugewandten
    // Seite; positive Bewegungen finden in der Richtung vom Motor
    // weg statt.
    void vReferenzfahrt(cStr* lcStrIn, bool lbAnswer) override 
    {
      UNUSED(lbAnswer);

      menProcessedCmd = enCmdReferenzfahrt;
      mbAnswer = lbAnswer;

      // @0R7 oder @0r7
      switch (lcStrIn->mpu8Data[2])
      {
        case '1': // x
          X.vStartRef();
          break;
        case '2': // y
          Y.vStartRef();
          break;
        case '3': // xy
          X.vStartRef();
          Y.vStartRef();
          break;
        case '4': // z
          Z.vStartRef();
          break;
        case '5': // xz
          X.vStartRef();
          Z.vStartRef();
          break;
        case '6': // yz
          Y.vStartRef();
          Z.vStartRef();
          break;
        case '7': // xyz
          X.vStartRef();
          Y.vStartRef();
          Z.vStartRef();
          break;
      }
    }

    // Der Befehl definiert - getrennt für jede Achse - die Geschwindigkeit,
    // mit der eine Referenzfahrt ausgeführt wird.Dabei handelt es sich nur
    // um die Geschwindigkeit, mit der die Achse in negativer Richtung auf
    // den Motor zuläuft; die Geschwindigkeit aus dem Schalter heraus lässt
    // sich nicht beeinflussen(vgl.Referenzfahrt).
    void vReferenzgeschwindigkeit_setzen(cStr* lcStrIn, bool lbAnswer) override 
    {
      UNUSED(lbAnswer);

      menProcessedCmd = enCmdReferenzgeschwindigkeit_setzen;
      mbAnswer = lbAnswer;

      cStr_Create(lcZwerg, 64);

      lcZwerg = *lcStrIn;

      // @0d1000, 3000, 9000
      lcZwerg.Cut(0, 3); // "@0d" entfernen

      u16 lu16Commas = lcZwerg.CountOf(',');
      u16 lu16ExpectedCommas = 0;
      if (Y.mbEnable) lu16ExpectedCommas += 1;
      if (Z.mbEnable) lu16ExpectedCommas += 1;

      i32 li32Value[3];

      vStringToParam(&lcZwerg, ',', li32Value, lu16ExpectedCommas);

      if (lu16ExpectedCommas == lu16Commas)
      {
        switch (mu8AxisCount)
        {
          case 1:
            X.mdRefSpeed = li32Value[0];
            break;
          case 2:
            X.mdRefSpeed = li32Value[0];
            if (Y.mbEnable) Y.mdRefSpeed = li32Value[1];
                       else Z.mdRefSpeed = li32Value[1];
            break;
          case 3:
            X.mdRefSpeed = li32Value[0];
            Y.mdRefSpeed = li32Value[1];
            Z.mdRefSpeed = li32Value[2];
            break;
        }
      }
    }

    // Die Prozessorkarte gibt entsprechend der übergebenen
    // Schrittanzahl und Schrittgeschwindigkeit für jede
    // Leistungsendstufe eine Impulskette aus.Die Verfahrbewegung
    // wird sofort ausgeführt oder gespeichert.

    // Im 2,5D-Interpolationsbetrieb wird die
    // Geschwindigkeitsangabe der Achse mit dem längsten
    // Weg als Bahngeschwindigkeit übernommen, die
    // Geschwindigkeit der anderen Achse entsprechend dem
    // Wegeverhältnis angepasst.
    // Im Gegensatz dazu wird im 3D - Interpolationsbetrieb die
    // Geschwindigkeitsangabe der X - Achse als Vorgabewert
    // für die Bahngeschwindigkeit herangezogen.
    void vBewegung_relativ(cStr* lcStrIn, bool lbAnswer) override 
    {
      UNUSED(lbAnswer);

      menProcessedCmd = enCmdBewegung_relativ;
      mbAnswer = lbAnswer;

      cStr_Create(lcZwerg, 64);

      lcZwerg = *lcStrIn;

      // @0A 30,800,10,900,4,90,-4,30
      lcZwerg.Cut(0, 3); // "@0a" entfernen

      u16 lu16Commas = lcZwerg.CountOf(',');
      u16 lu16ExpectedCommas = 1;
      if (Y.mbEnable) lu16ExpectedCommas += 2;
      if (Z.mbEnable) lu16ExpectedCommas += 4;

      i32 li32Value[8];

      vStringToParam(&lcZwerg, ',', li32Value, lu16ExpectedCommas);

      if (lu16Commas == lu16ExpectedCommas)
      {
        switch (mu8AxisCount)
        {
        case 1:
          vSet(X.mdPosSoll + li32Value[0], li32Value[1], 0.0, 0.0, 0.0, 0.0);
          break;
        case 2:
          if (Y.mbEnable) 
          { 
            vSet(X.mdPosSoll + li32Value[0], li32Value[1], 
                 Y.mdPosSoll + li32Value[2], li32Value[3],
                 0.0, 0.0);
          }
          else
          {
            vSet(X.mdPosSoll + li32Value[0], li32Value[1], 
                 0.0, 0.0,
                 Z.mdPosSoll + li32Value[4], li32Value[5]);
          }
          break;
        case 3:
            vSet(X.mdPosSoll + li32Value[0], li32Value[1], 
                 Y.mdPosSoll + li32Value[2], li32Value[3],
                 Z.mdPosSoll + li32Value[4], li32Value[5]);
          break;
        }
      }
    }

    // Die Prozessorkarte fährt mit den angegebenen
    // Geschwindigkeiten an die angegebene Position.Die
    // Verfahrbewegung wird sofort ausgeführt.
    void vBewegung_zur_Position(cStr* lcStrIn, bool lbAnswer) override
    {
      UNUSED(lbAnswer);

      menProcessedCmd = enCmdBewegung_zur_Position;
      mbAnswer = lbAnswer;

      cStr_Create(lcZwerg, 64);

      lcZwerg = *lcStrIn;

      // @0A 30,800,10,900,4,90,-4,30
      lcZwerg.Cut(0, 3); // "@0a" entfernen

      u16 lu16Commas = lcZwerg.CountOf(',');
      u16 lu16ExpectedCommas = 1;
      if (Y.mbEnable) lu16ExpectedCommas += 2;
      if (Z.mbEnable) lu16ExpectedCommas += 4;

      i32 li32Value[8];

      vStringToParam(&lcZwerg, ',', li32Value, lu16ExpectedCommas);

      if (lu16Commas == lu16ExpectedCommas)
      {
        switch (mu8AxisCount)
        {
        case 1:
          vSet(li32Value[0], li32Value[1], 0.0, 0.0, 0.0, 0.0);
          break;
        case 2:
          if (Y.mbEnable) 
          { 
            vSet(li32Value[0], li32Value[1], 
                 li32Value[2], li32Value[3],
                 0.0, 0.0);
          }
          else
          {
            vSet(li32Value[0], li32Value[1], 
                 0.0, 0.0,
                 li32Value[4], li32Value[5]);
          }
          break;
        case 3:
            vSet(li32Value[0], li32Value[1], 
                 li32Value[2], li32Value[3],
                 li32Value[4], li32Value[5]);
          break;
        }
      }
    }

    // Die Prozessorkarte bricht eine momentane Verfahrbewegung ab.
    void vStop(cStr* lcStrIn, bool lbAnswer) override
    {
      UNUSED(lbAnswer);    
      menProcessedCmd = enCmdStop;
      mbAnswer = lbAnswer;
    }

    // Der Break - Befehl dient zum Abbruch des Aktuellen Positioniervorganges.
    void vBreak(cStr* lcStrIn, bool lbAnswer) override 
    {
      UNUSED(lbAnswer);    
      menProcessedCmd = enCmdBreak;
      mbAnswer = lbAnswer;
    }

    void DoProcess1ms()
    {
      X.DoProcess1ms();
      Y.DoProcess1ms();
      Z.DoProcess1ms();

      if ((X.mdTimeNoMove == 0) &&
          (Y.mdTimeNoMove == 0) &&
          (Z.mdTimeNoMove == 0))
      {
        // Alle Achsen wurden seit 1s nicht mehr bewegt.
        // Stromabsenkung
      }


      switch (menProcessedCmd)
      {
        case enCmdNop:
          menProcessedCmd = enCmdNop;
          break;
        case enCmdAchsenanzahl_setzen:
          menProcessedCmd = enCmdNop;
          break;
        case enCmdReferenzfahrt:
          if (!((X.mbRefEnable) ||
                (Y.mbRefEnable) ||
                (Z.mbRefEnable)))
          {
            menProcessedCmd = enCmdNop;
            if (mbAnswer)
            {
              mbAnswer = False;
              mcStreamOut->put('0');
            }
          }
          break;
        case enCmdReferenzgeschwindigkeit_setzen:
          menProcessedCmd = enCmdNop;
          break;
        case enCmdBewegung_relativ:
          if ((X.mdPosSoll == X.mdPosIst) &&
              (Y.mdPosSoll == Y.mdPosIst) &&
              (Z.mdPosSoll == Z.mdPosIst))
          {
            menProcessedCmd = enCmdNop;
            if (mbAnswer)
            {
              mbAnswer = False;
              mcStreamOut->put('0');
            }
          }
          break;
        case enCmdBewegung_zur_Position:
          if ((X.mdPosSoll == X.mdPosIst) &&
              (Y.mdPosSoll == Y.mdPosIst) &&
              (Z.mdPosSoll == Z.mdPosIst))
          {
            menProcessedCmd = enCmdNop;
            if (mbAnswer)
            {
              mbAnswer = False;
              mcStreamOut->put('0');
            }
          }
          break;
        case enCmdStop:
          menProcessedCmd = enCmdNop;
          break;
        case enCmdBreak:
          menProcessedCmd = enCmdNop;
          break;
      }
    }
};

#endif /* _C_CNC_H */
