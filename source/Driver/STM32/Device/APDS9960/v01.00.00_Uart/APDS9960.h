#ifndef __APDS9960_H__
#define __APDS9960_H__

#include "Typedef.h"
#include "MemTools.h"
#include "cI2C.h"

#include "cUsart.h"
#include "cStringTools.h"

/* APDS-9960 I2C address */
#define nAPDS9960_I2C_ADDR       (0x39 << 1)

/* Gesture parameters */
#define nAPDS9960_GESTURE_THRESHOLD_OUT   128

#define nAPDS9960_GESTURE_U   1
#define nAPDS9960_GESTURE_D   2
#define nAPDS9960_GESTURE_L   4
#define nAPDS9960_GESTURE_R   8


/* Error code for returned values */
#define nAPDS9960_ERROR                   0xFF

/* Acceptable device IDs */
#define nAPDS9960_ID_1           0xAB
#define nAPDS9960_ID_2           0x9C

/* Misc parameters */
#define FIFO_PAUSE_TIME         30      // Wait period (ms) between FIFO reads

/* APDS-9960 register addresses */
#define nAPDS9960_ENABLE         0x80
#define nAPDS9960_ATIME          0x81
#define nAPDS9960_WTIME          0x83
#define nAPDS9960_AILTL          0x84
#define nAPDS9960_AILTH          0x85
#define nAPDS9960_AIHTL          0x86
#define nAPDS9960_AIHTH          0x87
#define nAPDS9960_PILT           0x89
#define nAPDS9960_PIHT           0x8B
#define nAPDS9960_PERS           0x8C
#define nAPDS9960_CONFIG1        0x8D
#define nAPDS9960_PPULSE         0x8E
#define nAPDS9960_CONTROL        0x8F
#define nAPDS9960_CONFIG2        0x90
#define nAPDS9960_ID             0x92
#define nAPDS9960_STATUS         0x93
#define nAPDS9960_CDATAL         0x94
#define nAPDS9960_CDATAH         0x95
#define nAPDS9960_RDATAL         0x96
#define nAPDS9960_RDATAH         0x97
#define nAPDS9960_GDATAL         0x98
#define nAPDS9960_GDATAH         0x99
#define nAPDS9960_BDATAL         0x9A
#define nAPDS9960_BDATAH         0x9B
#define nAPDS9960_PDATA          0x9C
#define nAPDS9960_POFFSET_UR     0x9D
#define nAPDS9960_POFFSET_DL     0x9E
#define nAPDS9960_CONFIG3        0x9F
#define nAPDS9960_GPENTH         0xA0
#define nAPDS9960_GEXTH          0xA1
#define nAPDS9960_GCONF1         0xA2
#define nAPDS9960_GCONF2         0xA3
#define nAPDS9960_GOFFSET_U      0xA4
#define nAPDS9960_GOFFSET_D      0xA5
#define nAPDS9960_GOFFSET_L      0xA7
#define nAPDS9960_GOFFSET_R      0xA9
#define nAPDS9960_GPULSE         0xA6
#define nAPDS9960_GCONF3         0xAA
#define nAPDS9960_GCONF4         0xAB
#define nAPDS9960_GFLVL          0xAE
#define nAPDS9960_GSTATUS        0xAF
#define nAPDS9960_IFORCE         0xE4
#define nAPDS9960_PICLEAR        0xE5
#define nAPDS9960_CICLEAR        0xE6
#define nAPDS9960_AICLEAR        0xE7
#define nAPDS9960_GFIFO_U        0xFC
#define nAPDS9960_GFIFO_D        0xFD
#define nAPDS9960_GFIFO_L        0xFE
#define nAPDS9960_GFIFO_R        0xFF

/* Bit fields */
#define nAPDS9960_PON            0b00000001
#define nAPDS9960_AEN            0b00000010
#define nAPDS9960_PEN            0b00000100
#define nAPDS9960_WEN            0b00001000
#define nAPSD9960_AIEN           0b00010000
#define nAPDS9960_PIEN           0b00100000
#define nAPDS9960_GEN            0b01000000

#define nAPDS9960_PVALID         0b00000010
#define nAPDS9960_AVALID         0b00000001

#define nAPDS9960_GVALID         0b00000001

/* On/Off definitions */
#define nAPDS9960_OFF                     0
#define nAPDS9960_ON                      1

/* Acceptable parameters for setMode */
#define nAPDS9960_POWER                   0
#define nAPDS9960_AMBIENT_LIGHT           1
#define nAPDS9960_PROXIMITY               2
#define nAPDS9960_WAIT                    3
#define nAPDS9960_AMBIENT_LIGHT_INT       4
#define nAPDS9960_PROXIMITY_INT           5
#define nAPDS9960_GESTURE                 6
#define nAPDS9960_ALL                     7

/* LED Drive values */
#define nAPDS9960_LED_DRIVE_100MA         0
#define nAPDS9960_LED_DRIVE_50MA          1
#define nAPDS9960_LED_DRIVE_25MA          2
#define nAPDS9960_LED_DRIVE_12_5MA        3

/* Proximity Gain (PGAIN) values */
#define nAPDS9960_PGAIN_1X                0
#define nAPDS9960_PGAIN_2X                1
#define nAPDS9960_PGAIN_4X                2
#define nAPDS9960_PGAIN_8X                3

/* ALS Gain (AGAIN) values */
#define nAPDS9960_AGAIN_1X                0
#define nAPDS9960_AGAIN_4X                1
#define nAPDS9960_AGAIN_16X               2
#define nAPDS9960_AGAIN_64X               3

/* Gesture Gain (GGAIN) values */
#define nAPDS9960_GGAIN_1X                0
#define nAPDS9960_GGAIN_2X                1
#define nAPDS9960_GGAIN_4X                2
#define nAPDS9960_GGAIN_8X                3

/* LED Boost values */
#define nAPDS9960_LED_BOOST_100           0
#define nAPDS9960_LED_BOOST_150           1
#define nAPDS9960_LED_BOOST_200           2
#define nAPDS9960_LED_BOOST_300           3

#define APDS9960_ui8ControlReg(LDRIVE, PGAIN, AGAIN) ((LDRIVE<<6) | (PGAIN<<2) | (AGAIN))

/* Gesture wait time values */
#define nAPDS9960_GWTIME_0MS              0
#define nAPDS9960_GWTIME_2_8MS            1
#define nAPDS9960_GWTIME_5_6MS            2
#define nAPDS9960_GWTIME_8_4MS            3
#define nAPDS9960_GWTIME_14_0MS           4
#define nAPDS9960_GWTIME_22_4MS           5
#define nAPDS9960_GWTIME_30_8MS           6
#define nAPDS9960_GWTIME_39_2MS           7

#define APDS9960_ui8GestureControlReg2(GGAIN, GLDRIVE, GWTIME) ((GGAIN<<5) | (GLDRIVE<<3) | (GWTIME))
#define APDS9960_ui8GestureConfigReg4(GIEN, GMODE) ((GIEN<<1) | (GMODE))

/* Default values */
#define nAPDS9960_DEFAULT_ATIME           219     // 103ms
#define nAPDS9960_DEFAULT_WTIME           246     // 27ms
#define nAPDS9960_DEFAULT_PROX_PPULSE     0x87    // 16us, 8 pulses
#define nAPDS9960_DEFAULT_GESTURE_PPULSE  0x89    // 16us, 10 pulses
#define nAPDS9960_DEFAULT_POFFSET_UR      0       // 0 offset
#define nAPDS9960_DEFAULT_POFFSET_DL      0       // 0 offset
#define nAPDS9960_DEFAULT_CONFIG1         0x60    // No 12x wait (WTIME) factor
#define nAPDS9960_DEFAULT_LDRIVE          nAPDS9960_LED_DRIVE_100MA
#define nAPDS9960_DEFAULT_PGAIN           nAPDS9960_PGAIN_4X
#define nAPDS9960_DEFAULT_AGAIN           nAPDS9960_AGAIN_4X
#define nAPDS9960_DEFAULT_PILT            0       // Low proximity threshold
#define nAPDS9960_DEFAULT_PIHT            50      // High proximity threshold
#define nAPDS9960_DEFAULT_AILT            0xFFFF  // Force interrupt for calibration
#define nAPDS9960_DEFAULT_AIHT            0
#define nAPDS9960_DEFAULT_PERS            0x11    // 2 consecutive prox or ALS for int.
#define nAPDS9960_DEFAULT_CONFIG2         0x01    // No saturation interrupts or LED boost
#define nAPDS9960_DEFAULT_CONFIG3         0       // Enable all photodiodes, no SAI
#define nAPDS9960_DEFAULT_GPENTH          20      // Threshold for entering gesture mode
#define nAPDS9960_DEFAULT_GEXTH           10      // Threshold for exiting gesture mode
#define nAPDS9960_DEFAULT_GCONF1          0x40    // 4 gesture events for int., 1 for exit
#define nAPDS9960_DEFAULT_GGAIN           nAPDS9960_GGAIN_2X
#define nAPDS9960_DEFAULT_GLDRIVE         nAPDS9960_LED_DRIVE_100MA
#define nAPDS9960_DEFAULT_GWTIME          nAPDS9960_GWTIME_0MS
#define nAPDS9960_DEFAULT_GOFFSET_U       0        // No offset scaling for gesture mode
#define nAPDS9960_DEFAULT_GOFFSET_D       0        // No offset scaling for gesture mode
#define nAPDS9960_DEFAULT_GOFFSET_L       0        // No offset scaling for gesture mode
#define nAPDS9960_DEFAULT_GOFFSET_R       0        // No offset scaling for gesture mode
#define nAPDS9960_DEFAULT_GPULSE          0xC9    // 32us, 10 pulses
#define nAPDS9960_DEFAULT_GCONF3          0       // All photodiodes active during gesture
#define nAPDS9960_DEFAULT_GIEN            0       // Disable gesture interrupts


/* Container for gesture data */
typedef struct {
    uint8 U;
    uint8 D;
    uint8 L;
    uint8 R;
} APDS9960_tstGestureValues;


/* Container for gesture data */
typedef struct {
    APDS9960_tstGestureValues stValues[32];
    uint8 index;
    uint8 total_gestures;
} APDS9960_tstGestureData;

/* Container for gesture data */
typedef struct {
    APDS9960_tstGestureValues stValues[100];
    uint8 index;
} APDS9960_tstGestureDataTest;


/* Gesture wait time values */
#define nAPDS9960_R              1 // Read
#define nAPDS9960_W              2 // Write
#define nAPDS9960_S              4 // Special
#define nAPDS9960_I              8 // Init with default value
#define nAPDS9960_WS             (nAPDS9960_W | nAPDS9960_S)
#define nAPDS9960_RW             (nAPDS9960_R | nAPDS9960_W)
#define nAPDS9960_RWI            (nAPDS9960_R | nAPDS9960_W | nAPDS9960_I)

const uint8 APDS9960_mpaui8RegSetDefault[] =
{
  0x00, nAPDS9960_RW,  0x00, //RAM
  0x80, nAPDS9960_RWI, 0x00, //ENABLE
  0x81, nAPDS9960_RWI, nAPDS9960_DEFAULT_ATIME, //ATIME
  0x83, nAPDS9960_RWI, nAPDS9960_DEFAULT_WTIME, //WTIME
  0x84, nAPDS9960_RWI, (nAPDS9960_DEFAULT_AILT  & 0xFF), //AILTL
  0x85, nAPDS9960_RWI, (nAPDS9960_DEFAULT_AILT >> 8), //AILTH
  0x86, nAPDS9960_RWI, (nAPDS9960_DEFAULT_AIHT  & 0xFF), //AIHTL
  0x87, nAPDS9960_RWI, (nAPDS9960_DEFAULT_AIHT >> 8), //AIHTH
  0x89, nAPDS9960_RWI, nAPDS9960_DEFAULT_PILT, //PILT
  0x8B, nAPDS9960_RWI, nAPDS9960_DEFAULT_PIHT, //PIHT
  0x8C, nAPDS9960_RWI, nAPDS9960_DEFAULT_PERS, //PERS
  0x8D, nAPDS9960_RWI, nAPDS9960_DEFAULT_CONFIG1, //CONFIG1
  0x8E, nAPDS9960_RWI, nAPDS9960_DEFAULT_PROX_PPULSE, //PPULSE
  0x8F, nAPDS9960_RWI, APDS9960_ui8ControlReg(nAPDS9960_DEFAULT_LDRIVE, nAPDS9960_DEFAULT_PGAIN, nAPDS9960_DEFAULT_AGAIN), //CONTROL
  0x90, nAPDS9960_RWI, nAPDS9960_DEFAULT_CONFIG2, //CONFIG2
  0x92, nAPDS9960_R,  0x00, //ID
  0x93, nAPDS9960_R,  0x00, //STATUS
  0x94, nAPDS9960_R,  0x00, //CDATAL
  0x95, nAPDS9960_R,  0x00, //CDATAH
  0x96, nAPDS9960_R,  0x00, //RDATAL
  0x97, nAPDS9960_R,  0x00, //RDATAH
  0x98, nAPDS9960_R,  0x00, //GDATAL
  0x99, nAPDS9960_R,  0x00, //GDATAH
  0x9A, nAPDS9960_R,  0x00, //BDATAL
  0x9B, nAPDS9960_R,  0x00, //BDATAH
  0x9C, nAPDS9960_R,  0x00, //PDATA
  0x9D, nAPDS9960_RWI, nAPDS9960_DEFAULT_POFFSET_UR, //POFFSET_UR
  0x9E, nAPDS9960_RWI, nAPDS9960_DEFAULT_POFFSET_DL, //POFFSET_DL
  0x9F, nAPDS9960_RWI, nAPDS9960_DEFAULT_CONFIG3, //CONFIG3
  0xA0, nAPDS9960_RWI, nAPDS9960_DEFAULT_GPENTH, //GPENTH
  0xA1, nAPDS9960_RWI, nAPDS9960_DEFAULT_GEXTH, //GEXTH
  0xA2, nAPDS9960_RWI, nAPDS9960_DEFAULT_GCONF1, //GCONF1
  0xA3, nAPDS9960_RWI, APDS9960_ui8GestureControlReg2(nAPDS9960_DEFAULT_GGAIN, nAPDS9960_DEFAULT_GLDRIVE, nAPDS9960_DEFAULT_GWTIME), //GCONF2
  0xA4, nAPDS9960_RWI, nAPDS9960_DEFAULT_GOFFSET_U, //GOFFSET_U
  0xA5, nAPDS9960_RWI, nAPDS9960_DEFAULT_GOFFSET_D, //GOFFSET_D
  0xA7, nAPDS9960_RWI, nAPDS9960_DEFAULT_GOFFSET_L, //GOFFSET_L
  0xA9, nAPDS9960_RWI, nAPDS9960_DEFAULT_GOFFSET_R, //GOFFSET_R
  0xA6, nAPDS9960_RWI, nAPDS9960_DEFAULT_GPULSE, //GPULSE
  0xAA, nAPDS9960_RWI, nAPDS9960_DEFAULT_GCONF3, //GCONF3
  0xAB, nAPDS9960_RWI, APDS9960_ui8GestureConfigReg4(nAPDS9960_DEFAULT_GIEN, 0), //GCONF4
  0xAE, nAPDS9960_R,  0x00, //GFLVL
  0xAF, nAPDS9960_R,  0x00, //GSTATUS
  0xE4, nAPDS9960_WS, 0x00, //IFORCE
  0xE5, nAPDS9960_WS, 0x00, //PICLEAR
  0xE6, nAPDS9960_WS, 0x00, //CICLEAR
  0xE7, nAPDS9960_WS, 0x00, //AICLEAR
  0xFC, nAPDS9960_R,  0x00, //GFIFO_U
  0xFD, nAPDS9960_R,  0x00, //GFIFO_D
  0xFE, nAPDS9960_R,  0x00, //GFIFO_L
  0xFF, nAPDS9960_R,  0x00, //GFIFO_R
};


/* Register Set */
typedef struct
{
  uint8 aui8RAM[128];
  uint8 ui8ENABLE;      // R/W, Enable states and interrupts
  uint8 ui8ATIME;       // R/W, ADC integration time
  uint8 ui8WTIME;       // R/W, Wait time (non-gesture)
  uint8 ui8AILTL;       // R/W, ALS interrupt low threshold low byte
  uint8 ui8AILTH;       // R/W, ALS interrupt low threshold high byte
  uint8 ui8AIHTL;       // R/W, ALS interrupt high threshold low byte
  uint8 ui8AIHTH;       // R/W, ALS interrupt high threshold high byte
  uint8 ui8PILT;        // R/W, Proximity interrupt low threshold
  uint8 ui8PIHT;        // R/W, Proximity interrupt high threshold
  uint8 ui8PERS;        // R/W, Interrupt persistence filters (non-gesture)
  uint8 ui8CONFIG1;     // R/W, Configuration register one
  uint8 ui8PPULSE;      // R/W, Proximity pulse count and length
  uint8 ui8CONTROL;     // R/W, Gain control
  uint8 ui8CONFIG2;     // R/W, Configuration register two
  uint8 ui8ID;          // R,   Device IDID
  uint8 ui8STATUS;      // R,   Device status
  uint8 ui8CDATAL;      // R,   Low byte of clear channel data
  uint8 ui8CDATAH;      // R,   High byte of clear channel data
  uint8 ui8RDATAL;      // R,   Low byte of red channel data
  uint8 ui8RDATAH;      // R,   High byte of red channel data
  uint8 ui8GDATAL;      // R,   Low byte of green channel data
  uint8 ui8GDATAH;      // R,   High byte of green channel data
  uint8 ui8BDATAL;      // R,   Low byte of blue channel data
  uint8 ui8BDATAH;      // R,   High byte of blue channel data
  uint8 ui8PDATA;       // R,   Proximity data
  uint8 ui8POFFSET_UR;  // R/W, Proximity offset for UP and RIGHT photodiodes
  uint8 ui8POFFSET_DL;  // R/W, Proximity offset for DOWN and LEFT photodiodes
  uint8 ui8CONFIG3;     // R/W, Configuration register three
  uint8 ui8GPENTH;      // R/W, Gesture proximity enter threshold
  uint8 ui8GEXTH;       // R/W, Gesture exit threshold
  uint8 ui8GCONF1;      // R/W, Gesture configuration one
  uint8 ui8GCONF2;      // R/W, Gesture configuration two
  uint8 ui8GOFFSET_U;   // R/W, Gesture UP offset register
  uint8 ui8GOFFSET_D;   // R/W, Gesture DOWN offset register
  uint8 ui8GOFFSET_L;   // R/W, Gesture LEFT offset register
  uint8 ui8GOFFSET_R;   // R/W, Gesture RIGHT offset register
  uint8 ui8GPULSE;      // R/W, Gesture pulse count and length
  uint8 ui8GCONF3;      // R/W, Gesture configuration three
  uint8 ui8GCONF4;      // R/W, Gesture configuration four
  uint8 ui8GFLVL;       // R,   Gesture FIFO level
  uint8 ui8GSTATUS;     // R,   Gesture status
  uint8 ui8IFORCE;      // W,   Force interrupt
  uint8 ui8PICLEAR;     // W,   Proximity interrupt clear
  uint8 ui8CICLEAR;     // W,   ALS clear channel interrupt clear
  uint8 ui8AICLEAR;     // W,   All non-gesture interrupts clear
  uint8 ui8GFIFO_U;     // R,   Gesture FIFO UP value
  uint8 ui8GFIFO_D;     // R,   Gesture FIFO DOWN value
  uint8 ui8GFIFO_L;     // R,   Gesture FIFO LEFT value
  uint8 ui8GFIFO_R;     // R,   Gesture FIFO RIGHT value
} APDS9960_tstRegSet;


class cAPDS9960: public cComNode
{
  public:

  /* Direction definitions */
  typedef enum {
    nAPDS9960_DIR_NONE,
    nAPDS9960_DIR_LEFT,
    nAPDS9960_DIR_RIGHT,
    nAPDS9960_DIR_UP,
    nAPDS9960_DIR_DOWN,
    nAPDS9960_DIR_NEAR,
    nAPDS9960_DIR_FAR,
    nAPDS9960_DIR_ALL
  }tenAPDS9960_Direction;

  typedef enum
  {
    enCmdIdle  = 0,
    enCmdInit,
    enCmdInit2,
    enCmdInit3,

    enCmdSetMode,
    enCmdSetMode2,
    enCmdSetControl,
    enCmdSetControl2,

    enCmdReadAmbientLight,
    enCmdReadAmbientLight2,

    enCmdReadProximitySensor,
    enCmdReadProximitySensor2,
    enCmdReadProximitySensor3,

    enCmdSetupGestureSensor,
    enCmdSetupGestureSensor2,
    enCmdSetupGestureSensor3,
    enCmdSetupGestureSensor4,
    enCmdSetupGestureSensor5,

    enCmdReadGestureSensor,
    enCmdReadGestureSensor2,
    enCmdReadGestureSensor3,
    enCmdReadGestureSensor4,
    enCmdReadGestureSensor5

  }tenAPDS9960_Cmd;


  typedef enum
  {
    enSmStIdle  = 0,
    enSmStWait,
    enSmStWait2,



    enSmStRw, // Rightwards
    enSmStRw1,
    enSmStRw2,
    enSmStRw3,
    enSmStRw4,
    enSmStRw5,
    enSmStRw6,
    enSmStRw7,
    enSmStRw8,
    enSmStRw9,
    enSmStRw10,
    enSmStRw11,
    enSmStRw12,
    enSmStRw13,
    enSmStRw14,
    enSmStRw15,

    enSmStLw, // Leftwards
    enSmStLw1,
    enSmStLw2,
    enSmStLw3,
    enSmStLw4,
    enSmStLw5,
    enSmStLw6,
    enSmStLw7,
    enSmStLw8,
    enSmStLw9,
    enSmStLw10,
    enSmStLw11,
    enSmStLw12,
    enSmStLw13,
    enSmStLw14,
    enSmStLw15,

    enSmStUw, // Upwards
    enSmStUw1,
    enSmStUw2,
    enSmStUw3,
    enSmStUw4,
    enSmStUw5,
    enSmStUw6,
    enSmStUw7,
    enSmStUw8,
    enSmStUw9,
    enSmStUw10,
    enSmStUw11,
    enSmStUw12,
    enSmStUw13,
    enSmStUw14,
    enSmStUw15,


    enSmStDw, // Downwards
    enSmStDw1,
    enSmStDw2,
    enSmStDw3,
    enSmStDw4,
    enSmStDw5,
    enSmStDw6,
    enSmStDw7,
    enSmStDw8,
    enSmStDw9,
    enSmStDw10,
    enSmStDw11,
    enSmStDw12,
    enSmStDw13,
    enSmStDw14,
    enSmStDw15,

  }tenAPDS9960_GestureSMStates;


  APDS9960_tstGestureData     mstGestureData;
  APDS9960_tstGestureDataTest mstGestureDataTest;
  int16 mi16Gesture_ud_delta;
  int16 mi16Gesture_lr_delta;
  int16 mi16Gesture_ud_count;
  int16 mi16Gesture_lr_count;
  int16 mi16Gesture_near_count;
  int16 mi16Gesture_far_count;
  int16 mi16Gesture_state;
  int16 mi16Gesture_motion;

  cI2cMaster*              mI2C;

  cComDatMsg mpcMsgWrite;
  cComDatMsg mpcMsgRead;
  cComDatMsg mpcMsgReadData;


  tenAPDS9960_Cmd    menCmd;
  uint8              mIdx;

  APDS9960_tstRegSet stRegSet;

  uint8 menGestureSM;
  uint8 menDirection;

  cUart* mcUart;

  bool mbInit;
  bool mbSetMode;
  bool mbReadAmbientLight;

  bool mbSetControl;
  bool mbReadProximitySensor;

  bool mbSetupGestureSensor;
  bool mbReadGestureSensor;


  cAPDS9960(cI2cMaster* lpcI2C, uint8 lui8Adr, cUart* lcUart);
  ~cAPDS9960();

  int8  i8Setup();

  void vComStart(cComNode::tenEvent lenEvent) override;
  void vComDone() override;
  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override;


  int8  isEnabledAmbientLigth();
  int8  i8EnableAmbientLight();
  int8  i8ReadAmbientLight();
  bool  boGetAmbientLight_Valid();
  uint16 ui16GetAmbientLight_Clear();
  uint16 ui16GetAmbientLight_Red();
  uint16 ui16GetAmbientLight_Green();
  uint16 ui16GetAmbientLight_Blue();


  int8  isEnabledProximitySensor();
  int8  i8EnableProximitySensor();
  int8  i8ReadProximitySensor();
  bool  boGetProximitySensor_Valid();
  uint8 ui8GetProximitySensor();

  int8 isEnabledGestureSensor();
  int8 i8EnableGestureSensor();
  void vResetGestureParameters();
  int8 i8ReadGestureSensor();
  bool bProcessGestureData();
  bool bDecodeGestureData();
};

#endif // __INA291_H__
