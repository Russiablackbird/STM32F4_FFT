#include <math.h>

#include "main.h"

const float TwoPI = 6.283185307f;

uint32_t N = 8;
uint32_t shift = 32 - 3; // size(uint32_t) - (log2(N))

// Samples
Complex Samples[8];

// Twiddle factor
Complex W;

int main(void)
{
  SystemClock_Config();

  InitArray();

  ReverseSample();

  fft();

  while (1)
  {
  }
}

void SystemClock_Config(void)
{
  /* Enable HSE oscillator */
  RCC->CR |= RCC_CR_HSEON;
  while ((RCC->CR & RCC_CR_HSERDY) != RCC_CR_HSERDY)
    ;

  /* Set FLASH latency */
  FLASH->ACR |=
      FLASH_ACR_LATENCY_2WS |
      FLASH_ACR_PRFTEN |
      FLASH_ACR_ICEN |
      FLASH_ACR_DCEN;

  /* Main PLL configuration and activation */
  RCC->PLLCFGR =
      RCC_PLLCFGR_PLLSRC_HSE |
      (12 << 0) | // PLLm 12
      (96 << 6) | // PLLn 168
      (0 << 16);  // PLLp 2;
  RCC->CR |= RCC_CR_PLLON;
  while ((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY)
    ;

  /* Sysclk activation on the main PLL */
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SW_PLL) != RCC_CFGR_SW_PLL)
    ;

  /* Set APB1 & APB2 prescaler */
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; // APB1 = 50 MHz
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; // APB2 = 100 MHz

  SystemCoreClockUpdate();
  // SysTick_Config(SystemCoreClock / 1000); // 1ms
}

/* Butterfly
////////////////////////
A[a].       .(A + WB)[a]
      .   .
        х
      .   .
B[b].       .(A - WB)[b]
////////////////////////
*/

void fft(void)
{
  // Radix-2 butterfly index
  uint32_t a, b;

  // Temp butterfly data
  Complex A0, B0, BW;

  // Stage iterator
  for (uint32_t i = 2; i <= N; i <<= 1)
  {
    // W up index iterator
    for (uint32_t j = 0; j < i >> 1; j++)
    {
      CalcW(j, i);

      // Calc first butterfly index
      a = j;
      b = a + (i / 2);

      // Calc Radix-2 butterfly
      for (uint32_t k = N / i; k != 0; k--)
      {
        A0 = Samples[a];
        B0 = Samples[b];

        Cmul(B0, W, BW);
        Cadd(A0, BW, Samples[a]);
        Csub(A0, BW, Samples[b]);

        a += i;
        b = a + (i / 2);
      }
    }
  }
}

void CalcW(uint32_t k, uint32_t N)
{
  if (k % N == 0)
  {
    W.Re = 1.0f;
    W.Im = 0.0f;
    return;
  }
  float arg = -TwoPI * k / N;
  W.Re = cosf(arg);
  W.Im = sinf(arg);
}

void ReverseSample(void)
{
  uint32_t j = 0;

  for (uint32_t i = 0; i < sizeof(Samples) / sizeof(Complex); i++)
  {
    asm("rbit %0, %1"
        : "=r"(j)
        : "r"(i));

    j >>= shift;

    if (j > i)
    {
      SWAP_RE_IM(Samples[i].Re, Samples[j].Re);
      SWAP_RE_IM(Samples[i].Im, Samples[j].Im);
    }
  }
}

void InitArray(void)
{
  Samples[0].Re = 2;
  Samples[0].Im = 0;

  Samples[1].Re = 2;
  Samples[1].Im = 0;

  Samples[2].Re = 2;
  Samples[2].Im = 0;

  Samples[3].Re = 2;
  Samples[3].Im = 0;

  Samples[4].Re = 1;
  Samples[4].Im = 0;

  Samples[5].Re = 1;
  Samples[5].Im = 0;

  Samples[6].Re = 1;
  Samples[6].Im = 0;

  Samples[7].Re = 1;
  Samples[7].Im = 0;
}
