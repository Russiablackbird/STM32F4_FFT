#include <stdint.h>
#include "stm32f411xe.h"

#define SWAP_RE_IM(a, b) (a += b, b = a - b, a -= b)

#define Cadd(a, b, c) (c.Re = a.Re + b.Re, c.Im = a.Im + b.Im)
#define Csub(a, b, c) (c.Re = a.Re - b.Re, c.Im = a.Im - b.Im)
#define Cmul(a, b, c) (c.Re = (a.Re * b.Re) - (a.Im * b.Im), c.Im = (a.Re * b.Im) + (a.Im * b.Re))

typedef struct _сomplex
{
    float Re;
    float Im;
} Complex;

void SystemClock_Config(void);
void fft(void);
void CalcW(uint32_t k, uint32_t N);
void ReverseSample(void);
void InitArray(void);
void SystemClock_Config(void);
