
#include "main.h"


// STM32F303
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 48KB

RCC_ClocksTypeDef RCC_Clocks;
uint32 TimingDelay = 0;

void NMI_Handler(void)
{
  while (1)
  {
  }
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}


void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}


void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}


void SVC_Handler(void)
{
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{
  while (1)
  {
  }
}


void PendSV_Handler(void)
{
  while (1)
  {
  }
}


void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


void TimingDelay_Decrement(void)
{
  if (TimingDelay > 0)
  {
    TimingDelay--;
  }
  else
  {
    TimingDelay = 0;
  }
}

void SysTick_Handler(void)
{
  TimingDelay_Decrement();
}




void TIM3_vInit();
void MAIN_vInitSystem(void)
{
  // Construct function
  SystemInit();
  /* SysTick end of count event each 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SystemCoreClockUpdate();

  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
}

class cMatrix_4x4
{
  public:

  float m[4][4];

  arm_matrix_instance_f32 mstMat1;
  arm_matrix_instance_f32 mstMat2;
  arm_matrix_instance_f32 mstRet;

  cMatrix_4x4(bool lbInit)
  {
    if (lbInit) Set_Default();

    arm_mat_init_f32(&mstMat1, 4, 4, null);
    arm_mat_init_f32(&mstMat2, 4, 4, null);
    arm_mat_init_f32(&mstRet,  4, 4, (float*)m);
  }

  void Set_Default()
  {
    float DefaultMat[4][4] =
         {{1.0f,   0.0f,   0.0f,   0.0f},
          {0.0f,   1.0f,   0.0f,   0.0f},
          {0.0f,   0.0f,   1.0f,   0.0f},
          {0.0f,   0.0f,   0.0f,   1.0f}};

    Set(DefaultMat);
  }

  void Set(float lMatrix[4][4])
  {
    unsigned char r, c;

    for (c = 0; c < 4; c++)
    {
      for (r = 0; r < 4; r++)
      {
        m[c][r] = lMatrix[c][r];
      }
    }
  }

  void Set(cMatrix_4x4 lcMatrix)
  {
    unsigned char r, c;

    for (c = 0; c < 4; c++)
    {
      for (r = 0; r < 4; r++)
      {
        m[c][r] = lcMatrix.m[c][r];
      }
    }
  }

  void Multiply(cMatrix_4x4 &mat1, cMatrix_4x4 &mat2)
  {
    cMatrix_4x4 lcRet(false);
    unsigned char r, c;
    for (c = 0; c < 4; c++)
    {
      for (r = 0; r < 4; r++)
      {
        lcRet.m[c][r] = mat1.m[r][0] * mat2.m[0][c] +
                        mat1.m[r][1] * mat2.m[1][c] +
                        mat1.m[r][2] * mat2.m[2][c] +
                        mat1.m[r][3] * mat2.m[3][c];
      }
    }
    *this = lcRet;
  }

  void MultiplyDsp(cMatrix_4x4 &mat1, cMatrix_4x4 &mat2)
  {
    mstMat1.pData = (float*)mat1.m;
    mstMat2.pData = (float*)mat2.m;
    mstRet.pData = (float*)m;
    arm_mat_mult_f32(&mstMat1, &mstMat2, &mstRet);
  }
};

class cVector4f
{
public:

  float n[4];

  arm_matrix_instance_f32 mstMat;
  arm_matrix_instance_f32 mstVec;
  arm_matrix_instance_f32 mstRetVec;

  cVector4f()
  {
    arm_mat_init_f32(&mstMat, 4, 1, null);
    arm_mat_init_f32(&mstVec, 4, 1, null);
    arm_mat_init_f32(&mstRetVec, 4, 4, (float*)n);

    n[3] = 0.0f;
  }

  void Set(float lVec[4])
  {
    unsigned char c;

    for (c = 0; c < 4; c++)
    {
      n[c] = lVec[c];
    }
  }

  void Multiply(cVector4f &v, cMatrix_4x4 &m)
  {
    n[0] = v.n[0] * m.m[0][0] + v.n[1] * m.m[1][0] + v.n[2] * m.m[2][0] + m.m[3][0];
    n[1] = v.n[0] * m.m[0][1] + v.n[1] * m.m[1][1] + v.n[2] * m.m[2][1] + m.m[3][1];
    n[2] = v.n[0] * m.m[0][2] + v.n[1] * m.m[1][2] + v.n[2] * m.m[2][2] + m.m[3][2];
    n[3] = 0;
    *this = v;
  }

  void MultiplyDsp(cVector4f &v, cMatrix_4x4 &mat)
  {
    mstMat.pData = (float*)mat.m;
    mstVec.pData = (float*)v.n;
    mstRetVec.pData = (float*)v.n;

    //arm_mat_mult_f32_4x4_mve(&mstMat1, &mstMat2, &mstRet);
    arm_mat_mult_f32(&mstMat, &mstVec, &mstRetVec);
  }
};

void Test_Mat(void)
{
#define SNR_THRESHOLD   90

  /* --------------------------------------------------------------------------------
  * Test input data(Cycles) taken from FIR Q15 module for differant cases of blockSize
  * and tapSize
  * --------------------------------------------------------------------------------- */

  const float32_t B_f32[4] =
  {
    782.0, 7577.0, 470.0, 4505.0
  };

  /* --------------------------------------------------------------------------------
  * Formula to fit is  C1 + C2 * numTaps + C3 * blockSize + C4 * numTaps * blockSize
  * -------------------------------------------------------------------------------- */

  const float32_t A_f32[16] =
  {
    /* Const,   numTaps,   blockSize,   numTaps*blockSize */
    1.0,     32.0,      4.0,     128.0,
    1.0,     32.0,     64.0,    2048.0,
    1.0,     16.0,      4.0,      64.0,
    1.0,     16.0,     64.0,    1024.0,
  };


  /* ----------------------------------------------------------------------
  * Temporary buffers  for storing intermediate values
  * ------------------------------------------------------------------- */
  /* Transpose of A Buffer */
  float32_t AT_f32[16];
  /* (Transpose of A * A) Buffer */
  float32_t ATMA_f32[16];
  /* Inverse(Transpose of A * A)  Buffer */
  float32_t ATMAI_f32[16];
  /* Test Output Buffer */
  float32_t X_f32[4];

  /* ----------------------------------------------------------------------
  * Reference ouput buffer C1, C2, C3 and C4 taken from MATLAB
  * ------------------------------------------------------------------- */
  const float32_t xRef_f32[4] = { 73.0, 8.0, 21.25, 2.875 };

  float32_t snr;


    arm_matrix_instance_f32 A;      /* Matrix A Instance */
    arm_matrix_instance_f32 AT;     /* Matrix AT(A transpose) instance */
    arm_matrix_instance_f32 ATMA;   /* Matrix ATMA( AT multiply with A) instance */
    arm_matrix_instance_f32 ATMAI;  /* Matrix ATMAI(Inverse of ATMA) instance */
    arm_matrix_instance_f32 B;      /* Matrix B instance */
    arm_matrix_instance_f32 X;      /* Matrix X(Unknown Matrix) instance */

    uint32 srcRows, srcColumns;  /* Temporary variables */
    arm_status status;

    /* Initialise A Matrix Instance with numRows, numCols and data array(A_f32) */
    srcRows = 4;
    srcColumns = 4;
    arm_mat_init_f32(&A, srcRows, srcColumns, (float32_t *)A_f32);

    /* Initialise Matrix Instance AT with numRows, numCols and data array(AT_f32) */
    srcRows = 4;
    srcColumns = 4;
    arm_mat_init_f32(&AT, srcRows, srcColumns, AT_f32);

    /* calculation of A transpose */
    status = arm_mat_trans_f32(&A, &AT);


    /* Initialise ATMA Matrix Instance with numRows, numCols and data array(ATMA_f32) */
    srcRows = 4;
    srcColumns = 4;
    arm_mat_init_f32(&ATMA, srcRows, srcColumns, ATMA_f32);

    /* calculation of AT Multiply with A */
    status = arm_mat_mult_f32(&AT, &A, &ATMA);

    /* Initialise ATMAI Matrix Instance with numRows, numCols and data array(ATMAI_f32) */
    srcRows = 4;
    srcColumns = 4;
    arm_mat_init_f32(&ATMAI, srcRows, srcColumns, ATMAI_f32);

    /* calculation of Inverse((Transpose(A) * A) */
    status = arm_mat_inverse_f32(&ATMA, &ATMAI);

    /* calculation of (Inverse((Transpose(A) * A)) *  Transpose(A)) */
    status = arm_mat_mult_f32(&ATMAI, &AT, &ATMA);

    /* Initialise B Matrix Instance with numRows, numCols and data array(B_f32) */
    srcRows = 4;
    srcColumns = 1;
    arm_mat_init_f32(&B, srcRows, srcColumns, (float32_t *)B_f32);

    /* Initialise X Matrix Instance with numRows, numCols and data array(X_f32) */
    srcRows = 4;
    srcColumns = 1;
    arm_mat_init_f32(&X, srcRows, srcColumns, X_f32);

    /* calculation ((Inverse((Transpose(A) * A)) *  Transpose(A)) * B) */
    status = arm_mat_mult_f32(&ATMA, &B, &X);
}


#define TEST_REPETITIONS 1

/* Main functions ---------------------------------------------------------*/
int main(void)
{
  typedef struct
  {
    uint32 Tim1_Nop;
    uint32 Tim2_IntMul;
    uint32 Tim3_IntDiv;
    uint32 Tim4_FloatMul;
    uint32 Tim5_FloatDiv;
    uint32 Tim6_4x4MatrixMul;
    uint32 Tim7_4x4MatrixDspMul;
    uint32 Tim8_4x4MatrixVecMul;
    uint32 Tim9_4x4MatrixDspVecMul;
  }tstMeasures;


  static volatile tstMeasures lstMeasures;
  uint32             lu32TestLoop;

  uint32                  lu32Value1 = 12345678;
  uint32                  lu32Value2 = 1234;
  static volatile uint32  lu32Value3;

  float                  lfValue1 = 1234.5678f;
  float                  lfValue2 = 12.34f;
  static volatile float  lfValue3;

  UNUSED(lstMeasures);
  UNUSED(lu32Value3);
  UNUSED(lfValue3);

  float MyMatA[4][4] = {   {1.0,     32.0,      4.0,     128.0},
                           {1.0,     32.0,     64.0,    2048.0},
                           {1.0,     16.0,      4.0,      64.0},
                           {1.0,     16.0,     64.0,    1024.0}};

  float MyMatAT[4][4] = {  { 1.0,      1.0,      1.0,       1.0},
                           {32.0,     32.0,     16.0,      16.0},
                           { 4.0,     64.0,      4.0,      64.0},
                          {128.0,   2048.0,     64.0,    1024.0}};

  float MyMatATmulA[4][4] = {  { 4.0,     96.0,    136.0,    3264.0},
                               {96.0,   2560.0,   3264.0,   87040.0},
                              {136.0,   3264.0,   8224.0,  197376.0},
                             {3264.0,  87040.0, 197376.0, 5263360.0}};

  UNUSED(MyMatATmulA);

  cMatrix_4x4           lcMat1(false);
  cMatrix_4x4           lcMat2(false);
  cMatrix_4x4           lcMat3(false);

  lcMat1.Set(MyMatAT);
  lcMat2.Set(MyMatA);

  float MyVec[4] = { 1.0f,   5.4f,   9.8f,  13.2f };

  cVector4f             lcVec1;
  cVector4f             lcVec2;

  lcVec1.Set(MyVec);
  lcVec2.Set(MyVec);

  MAIN_vInitSystem();

  cDiffTimer         lcTimer;

  Test_Mat();

  while (1)
  {
    // ----- Nop -----
    asm("nop"); // for breakpoint
    asm("nop"); // for breakpoint

    lcTimer.start();
    #define MyTest asm("nop")
    for (lu32TestLoop = 0; lu32TestLoop < TEST_REPETITIONS; lu32TestLoop++)
    {
      MyTest; MyTest; MyTest; MyTest; MyTest;
      MyTest; MyTest; MyTest; MyTest; MyTest;
    }
    #undef MyTest
    lstMeasures.Tim1_Nop = lcTimer.get();

    asm("nop"); // for breakpoint
    asm("nop"); // for breakpoint


    // ----- Integer multiplication -----
    lcTimer.start();
    #define MyTest lu32Value3 = lu32Value1 * lu32Value2
    for (lu32TestLoop = 0; lu32TestLoop < TEST_REPETITIONS; lu32TestLoop++)
    {
      MyTest; MyTest; MyTest; MyTest; MyTest;
      MyTest; MyTest; MyTest; MyTest; MyTest;
    }
    #undef MyTest
    lstMeasures.Tim2_IntMul = lcTimer.get();

    asm("nop"); // for breakpoint
    asm("nop"); // for breakpoint


    // ----- Integer division -----
    lcTimer.start();
    #define MyTest lu32Value3 = lu32Value1 * lu32Value2
    for (lu32TestLoop = 0; lu32TestLoop < TEST_REPETITIONS; lu32TestLoop++)
    {
      MyTest; MyTest; MyTest; MyTest; MyTest;
      MyTest; MyTest; MyTest; MyTest; MyTest;
    }
    #undef MyTest
    lstMeasures.Tim3_IntDiv = lcTimer.get();

    asm("nop"); // for breakpoint
    asm("nop"); // for breakpoint


    // ----- Float multiplication -----
    lcTimer.start();
    #define MyTest lfValue3 = lfValue1 * lfValue2
    for (lu32TestLoop = 0; lu32TestLoop < TEST_REPETITIONS; lu32TestLoop++)
    {
      MyTest; MyTest; MyTest; MyTest; MyTest;
      MyTest; MyTest; MyTest; MyTest; MyTest;
    }
    #undef MyTest
    lstMeasures.Tim4_FloatMul = lcTimer.get();

    asm("nop"); // for breakpoint
    asm("nop"); // for breakpoint


    // ----- Float division -----
    lcTimer.start();
    #define MyTest lfValue3 = lfValue1 / lfValue2
    for (lu32TestLoop = 0; lu32TestLoop < TEST_REPETITIONS; lu32TestLoop++)
    {
      MyTest; MyTest; MyTest; MyTest; MyTest;
      MyTest; MyTest; MyTest; MyTest; MyTest;
    }
    #undef MyTest
    lstMeasures.Tim5_FloatDiv = lcTimer.get();

    asm("nop"); // for breakpoint
    asm("nop"); // for breakpoint

    // ----- 4x4 Matrix multiplication -----
    lcTimer.start();
    #define MyTest lcMat3.Multiply(lcMat1, lcMat2);
    for (lu32TestLoop = 0; lu32TestLoop < TEST_REPETITIONS; lu32TestLoop++)
    {
      MyTest; MyTest; MyTest; MyTest; MyTest;
      MyTest; MyTest; MyTest; MyTest; MyTest;
    }
    #undef MyTest
    lstMeasures.Tim6_4x4MatrixMul = lcTimer.get();

    asm("nop"); // for breakpoint
    asm("nop"); // for breakpoint

    // ----- 4x4 Matrix multiplication using ARM Dsp lib -----
    lcTimer.start();
    #define MyTest lcMat3.MultiplyDsp(lcMat1, lcMat2);
    for (lu32TestLoop = 0; lu32TestLoop < TEST_REPETITIONS; lu32TestLoop++)
    {
      MyTest; MyTest; MyTest; MyTest; MyTest;
      MyTest; MyTest; MyTest; MyTest; MyTest;
    }
    #undef MyTest
    lstMeasures.Tim7_4x4MatrixDspMul = lcTimer.get();

    asm("nop"); // for breakpoint
    asm("nop"); // for breakpoint

    // ----- 4x4 Matrix Vector multiplication -----
    lcTimer.start();
    #define MyTest lcVec2.Multiply(lcVec1, lcMat2);
    for (lu32TestLoop = 0; lu32TestLoop < TEST_REPETITIONS; lu32TestLoop++)
    {
      MyTest; MyTest; MyTest; MyTest; MyTest;
      MyTest; MyTest; MyTest; MyTest; MyTest;
    }
    #undef MyTest
    lstMeasures.Tim8_4x4MatrixVecMul = lcTimer.get();

    asm("nop"); // for breakpoint
    asm("nop"); // for breakpoint

    // ----- 4x4 Matrix Vector multiplication using ARM Dsp lib -----
    lcTimer.start();
    #define MyTest lcVec2.MultiplyDsp(lcVec1, lcMat2);
    for (lu32TestLoop = 0; lu32TestLoop < TEST_REPETITIONS; lu32TestLoop++)
    {
      MyTest; MyTest; MyTest; MyTest; MyTest;
      MyTest; MyTest; MyTest; MyTest; MyTest;
    }
    #undef MyTest
    lstMeasures.Tim9_4x4MatrixDspVecMul = lcTimer.get();

  }
}

