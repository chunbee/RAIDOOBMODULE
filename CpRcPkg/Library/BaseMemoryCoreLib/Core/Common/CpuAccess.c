/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2015 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 *
 ************************************************************************/

#include "SysFunc.h"

#ifndef IA32
#include <CpuDataStruct.h>
#endif
UINT64_STRUCT
ReadMsrLocal (
             UINT32 msrAddr
             )
/*++

  Reads 64-BIT Machine status register

  @param msrAddr - Address of MSR to read

  @retval UINT64_STRUCT - MSR value as read from CPU

--*/
{
  UINT64_STRUCT data = {0,0};

#ifdef IA32
#ifdef __GNUC__
   asm
   (
      "mov   %2, %%ecx\n\t"
      "rdmsr\n\t"
      "mov   %%eax, %0\n\t"
      "mov   %%edx, %1\n\t"
      : "=m" (data.lo), "=m" (data.hi)
      : "m" (msrAddr)
      :
   );
#else
   _asm
   {
      mov   ecx, msrAddr
      rdmsr
      mov   data.lo, eax
      mov   data.hi, edx
   }
#endif
#else // IA32
   {
      UINT64 value;
      value = AsmReadMsr64(msrAddr);
      data.lo = (UINT32)value;
      data.hi = (UINT32)(value >> 32);
   }
#endif // IA32
  return data;
}

void
WriteMsrLocal (
              UINT32        msrAddr,
              UINT64_STRUCT data
              )
/*++

  Writes 64-BIT Machine status register

  @param msrAddr - Address of MSR to write
  @param data    - 64 bit data to write to MSR

  @retval N/A

--*/
{

#ifdef IA32
#ifdef __GNUC__
   asm
   (
      "mov   %0, %%eax\n\t"
      "mov   %1, %%edx\n\t"
      "mov   %2, %%ecx\n\t"
      "wrmsr\n\t"
      :
      : "m" (data.lo), "m" (data.hi), "m" (msrAddr)
      :
   );
#else
   _asm
   {
      mov   eax, data.lo
      mov   edx, data.hi
      mov   ecx, msrAddr
      wrmsr
   }
#endif
#else // IA32
   {
      UINT64 value = 0;
      value = ((UINT64)data.hi << 32) | data.lo;
      AsmWriteMsr64(msrAddr, value);
   }
#endif // IA32
}

UINT32
ReadCpuid (
          UINT32 funcNum
          )
/*++

  Reads 32-bit CPUID from the CPU executing this code

  @param funcNum - CPUID EAX (function number) value the caller wishes to receive EAX
                   value from

  @retval UINT32 - Returns EAX value generated by CPUID call with function funcNum

--*/
{
  UINT32 eaxReg = 0;

#ifdef IA32
#ifdef __GNUC__
   asm
   (
      "mov   %1, %%eax\n\t"
      "cpuid\n\t"
      "mov   %%eax, %0\n\t"
      : "=m" (eaxReg)
      : "m" (funcNum)
      :
   );
#else
   _asm
   {
      mov   eax, funcNum
      cpuid
      mov   eaxReg, eax
   }
#endif
#else // IA32
   {
      EFI_CPUID_REGISTER cpuIdRegisters;
      AsmCpuid(1, &cpuIdRegisters.RegEax, &cpuIdRegisters.RegEbx, &cpuIdRegisters.RegEcx, &cpuIdRegisters.RegEdx);
      eaxReg = cpuIdRegisters.RegEax;
   }
#endif // IA32
  return eaxReg;
}

UINT8
CheckProcessorType (
                   PSYSHOST  host,
                   UINT8     cpuType
                   )
/*++

  Checksthe processor type that is installed on the system

  @param host      - Pointer to the system host (root) structure
  @param cpuType   - CPU type (HSX, BWX,....)

@retval Returns TRUE or FALSE

--*/
{
  if ((host->var.common.cpuType == cpuType)) {
    return 1;
  } else {
    return 0;
  }

} // CheckProcessorType


UINT8
CheckSteppingEqual (
                   PSYSHOST  host,
                   UINT8     cpuType,
                   UINT8     stepping
                   )
/*++

  Checks if the provided CPU and stepping is equal to the CPU and stepping populated

  @param host      - Pointer to the system host (root) structure
  @param cpyType   - CPU type (HSX, BWX,....)
  @param stepping  - CPU stepping

  @retval 1 if the CPU is the same and the stepping is equal to the inputs, 0 otherwise

--*/
{
  if ((host->var.common.cpuType == cpuType) && (host->var.common.stepping == stepping)) {
    return 1;
  } else {
    return 0;
  }
} // CheckSteppingEqual

UINT8
CheckSteppingLessThan (
                      PSYSHOST  host,
                      UINT8     cpuType,
                      UINT8     stepping
                      )
/*++

  Checks if the provided CPU and stepping is less than the CPU and stepping populated

  @param host      - Pointer to the system host (root) structure
  @param cpyType   - CPU type (HSX, BWX,....)
  @param stepping  - CPU stepping

  @retval 1 if the CPU is the same and the stepping is less than the inputs, 0 otherwise

--*/
{
  if ((host->var.common.cpuType == cpuType) && (host->var.common.stepping < stepping)) {
    return 1;
  } else {
    return 0;
  }
} // CheckSteppingLessThan

UINT8
CheckSteppingGreaterThan (
                         PSYSHOST  host,
                         UINT8     cpuType,
                         UINT8     stepping
                         )
/*++

  Checks if the provided CPU and stepping is greater than the CPU and stepping populated

  @param host      - Pointer to the system host (root) structure
  @param cpyType   - CPU type (HSX, BWX,....)
  @param stepping  - CPU stepping

  @retval 1 if the CPU is the same and the stepping is greater than the inputs, 0 otherwise

--*/
{
  if ((host->var.common.cpuType == cpuType) && (host->var.common.stepping > stepping)) {
    return 1;
  } else {
    return 0;
  }
} // CheckSteppingGreaterThan


UINT8
GetProcApicId (
  )
/*++

  Reads 8-bit APIC ID from the CPU executing this code

  @retval UINT32 - Returns APIC ID

--*/
{
  UINT32 apicId = 0;

#ifdef IA32
#ifdef __GNUC__
  asm
  (
    "mov   $1, %%eax\n\t"
    "cpuid\n\t"
    "mov   %%ebx, %0\n\t"
    : "=m" (apicId)
    :
    :
  );
#else
  _asm
  {
    mov   eax, 1
    cpuid
    mov   apicId, ebx
  }
#endif
#else // IA32
   {
    EFI_CPUID_REGISTER cpuIdRegisters;
    AsmCpuid(1, &cpuIdRegisters.RegEax, &cpuIdRegisters.RegEbx, &cpuIdRegisters.RegEcx, &cpuIdRegisters.RegEdx);
    apicId = cpuIdRegisters.RegEbx;
  }
#endif // IA32
  return (UINT8)((apicId >> 24) & 0xFF);
}

VOID
GetCpuID (
     UINT32*   RegEax,
     UINT32*   RegEbx,
     UINT32*   RegEcx,
     UINT32*   RegEdx 
     )
/*++

    C-wrapper of CPUID instruction

    @param RegEax - ptr to EAX register
    @param RegEbx - ptr to EBX register
    @param RegEcx - ptr to ECX register
    @param RegEdx - ptr to EDX register

    @retval N/A: EAX, EBX, ECX, EDX contains return values of CPUID instruction

--*/
{
#ifdef IA32
#ifdef __GNUC__
  asm (
     "pushal\n\t"

     "movl  %0, %%esi\n\t"
     "movl  (%%esi), %%eax\n\t"
     "movl  %2, %%esi\n\t"
     "movl  (%%esi), %%ecx\n\t"

     "cpuid\n\t"

     "movl  %0, %%esi\n\t"
     "movl  %%eax, (%%esi)\n\t"
     "movl  %1, %%esi\n\t"
     "movl  %%ebx, (%%esi)\n\t"
     "movl  %2, %%esi\n\t"
     "movl  %%ecx, (%%esi)\n\t"
     "movl  %3, %%esi\n\t"
     "movl  %%edx, (%%esi)\n\t"

     "popal\n\t"
     :
     : "m" (RegEax), "m" (RegEbx), "m" (RegEcx), "m" (RegEdx)
     :
  );
#else
  _asm {
     pushad

     mov   esi, RegEax
     mov   eax, dword ptr [esi]
     mov   esi, RegEcx
     mov   ecx, dword ptr [esi]

     cpuid

     mov   esi, RegEax
     mov   dword ptr [esi], eax
     mov   esi, RegEbx
     mov   dword ptr [esi], ebx
     mov   esi, RegEcx
     mov   dword ptr [esi], ecx
     mov   esi, RegEdx
     mov   dword ptr [esi], edx

     popad
  }
#endif
#else
  EFI_CPUID_REGISTER cpuIdRegisters;
  AsmCpuid(*RegEax, &cpuIdRegisters.RegEax, &cpuIdRegisters.RegEbx, &cpuIdRegisters.RegEcx, &cpuIdRegisters.RegEdx);
  *RegEax = cpuIdRegisters.RegEax;
  *RegEbx = cpuIdRegisters.RegEbx;
  *RegEcx = cpuIdRegisters.RegEcx;
  *RegEdx = cpuIdRegisters.RegEdx;
#endif // IA32
  return;
}
