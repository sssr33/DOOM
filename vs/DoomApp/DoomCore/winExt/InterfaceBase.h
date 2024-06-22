#pragma once

// WARNING DO NOT USE windows headers because it will conflict with boolean type which must be int

#include <stdint.h>

typedef struct IGUID {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t  Data4[8];
} IGUID;

typedef int32_t IRESULT;

#define IRESULT_OK ((IRESULT)0)
#define IRESULT_ERROR ((IRESULT)-1)

#define IRESULT_FAILED(expression) (expression < IRESULT_OK)
