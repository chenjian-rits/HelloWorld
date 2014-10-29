// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "barcode.h"
#include "include/BC_QRCoderErrorCorrectionLevel.h"
#include "include/BC_QRCoderFormatInformation.h"
const FX_INT32 CBC_QRCoderFormatInformation::FORMAT_INFO_MASK_QR = 0X5412;
const FX_INT32 CBC_QRCoderFormatInformation::FORMAT_INFO_DECODE_LOOKUP[32][2] = {
    {0x5412, 0x00},
    {0x5125, 0x01},
    {0x5E7C, 0x02},
    {0x5B4B, 0x03},
    {0x45F9, 0x04},
    {0x40CE, 0x05},
    {0x4F97, 0x06},
    {0x4AA0, 0x07},
    {0x77C4, 0x08},
    {0x72F3, 0x09},
    {0x7DAA, 0x0A},
    {0x789D, 0x0B},
    {0x662F, 0x0C},
    {0x6318, 0x0D},
    {0x6C41, 0x0E},
    {0x6976, 0x0F},
    {0x1689, 0x10},
    {0x13BE, 0x11},
    {0x1CE7, 0x12},
    {0x19D0, 0x13},
    {0x0762, 0x14},
    {0x0255, 0x15},
    {0x0D0C, 0x16},
    {0x083B, 0x17},
    {0x355F, 0x18},
    {0x3068, 0x19},
    {0x3F31, 0x1A},
    {0x3A06, 0x1B},
    {0x24B4, 0x1C},
    {0x2183, 0x1D},
    {0x2EDA, 0x1E},
    {0x2BED, 0x1F},
};
const FX_INT32 CBC_QRCoderFormatInformation::BITS_SET_IN_HALF_BYTE[] =
{0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
CBC_QRCoderFormatInformation::CBC_QRCoderFormatInformation(FX_INT32 formatInfo)
{
    m_errorCorrectLevl = CBC_QRCoderErrorCorrectionLevel::ForBits((formatInfo >> 3) & 0x03);
    m_dataMask = (FX_BYTE) (formatInfo & 0x07);
}
CBC_QRCoderFormatInformation::~CBC_QRCoderFormatInformation()
{
}
FX_INT32 CBC_QRCoderFormatInformation::NumBitsDiffering(FX_INT32 a, FX_INT32 b)
{
    a ^= b;
    return BITS_SET_IN_HALF_BYTE[a & 0x0F] +
           BITS_SET_IN_HALF_BYTE[(a >> 4) & 0x0F] +
           BITS_SET_IN_HALF_BYTE[(a >> 8) & 0x0F] +
           BITS_SET_IN_HALF_BYTE[(a >> 12) & 0x0F] +
           BITS_SET_IN_HALF_BYTE[(a >> 16) & 0x0F] +
           BITS_SET_IN_HALF_BYTE[(a >> 20) & 0x0F] +
           BITS_SET_IN_HALF_BYTE[(a >> 24) & 0x0F] +
           BITS_SET_IN_HALF_BYTE[(a >> 28) & 0x0F];
}
FX_BYTE CBC_QRCoderFormatInformation::GetDataMask()
{
    return m_dataMask;
}
CBC_QRCoderErrorCorrectionLevel *CBC_QRCoderFormatInformation::GetErrorCorrectionLevel()
{
    return m_errorCorrectLevl;
}
CBC_QRCoderFormatInformation* CBC_QRCoderFormatInformation::DecodeFormatInformation(FX_INT32 maskedFormatInfo)
{
    CBC_QRCoderFormatInformation* formatInfo = DoDecodeFormatInformation(maskedFormatInfo);
    if(formatInfo != NULL) {
        return formatInfo;
    }
    return DoDecodeFormatInformation(maskedFormatInfo ^ FORMAT_INFO_MASK_QR);
}
CBC_QRCoderFormatInformation* CBC_QRCoderFormatInformation::DoDecodeFormatInformation(FX_INT32 maskedFormatInfo)
{
    FX_INT32 bestDifference = (FX_INT32)FXSYS_nan();
    FX_INT32 bestFormatInfo = 0;
    for(FX_INT32 i = 0; i < 32; i++) {
        FX_INT32 const* decodeInfo = &FORMAT_INFO_DECODE_LOOKUP[i][0];
        FX_INT32 targetInfo = decodeInfo[0];
        if(targetInfo == maskedFormatInfo) {
            return FX_NEW CBC_QRCoderFormatInformation(decodeInfo[1]);
        }
        FX_INT32 bitsDifference = NumBitsDiffering(maskedFormatInfo, targetInfo);
        if(bitsDifference < bestDifference) {
            bestFormatInfo = decodeInfo[1];
            bestDifference = bitsDifference;
        }
    }
    if(bestDifference <= 3) {
        return FX_NEW CBC_QRCoderFormatInformation(bestFormatInfo);
    }
    return NULL;
}