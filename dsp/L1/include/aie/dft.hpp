/*
 * Copyright (C) 2019-2022, Xilinx, Inc.
 * Copyright (C) 2022-2023, Advanced Micro Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _DSPLIB_DFT_HPP_
#define _DSPLIB_DFT_HPP_

/*
DFT
This file exists to capture the definition of the single channel DFT/iDFT filter kernel class.
The class definition holds defensive checks on parameter range and other legality.
The constructor definition is held in this class because this class must be accessible to graph
level aie compilation.
The main runtime function is captured elsewhere as it contains aie intrinsics which are not
included in aie graph level compilation.
*/

/* Coding conventions
   TT_      template type suffix
   TP_      template parameter suffix
*/

/* Design Notes
*/

#include <adf.h>
#include <vector>

#include "dft_traits.hpp"
#include "fir_utils.hpp"

#ifndef _DSPLIB_DFT_HPP_DEBUG_
// #define _DSPLIB_DFT_HPP_DEBUG_
#endif //_DSPLIB_DFT_HPP_DEBUG_

namespace xf {
namespace dsp {
namespace aie {
namespace fft {
namespace dft {

struct no_port {};
template <typename T_A, typename T_B>
struct accType {
    using type = cacc48;
};

template <>
struct accType<cint16, cint16> {
    using type = cacc48;
};
template <>
struct accType<cint32, cint16> {
    using type = cacc80;
};
template <>
struct accType<cfloat, cfloat> {
    using type = caccfloat;
}; // cfloat?

template <typename T_D, typename T_T>
using accType_t = typename accType<T_D, T_T>::type;

// IF input type-------------------------------------
template <bool T_CASC_IN, typename T_D, typename T_T>
struct T_inputIF {};
// CASC_IN_FALSE
template <typename T_D, typename T_T>
struct T_inputIF<CASC_IN_FALSE, T_D, T_T> {
    // input_buffer<T_D> &inWindow;
    T_D* __restrict inWindow;
    no_port* inCascade;
};
// CASC_IN_TRUE
template <typename T_D, typename T_T>
struct T_inputIF<CASC_IN_TRUE, T_D, T_T> {
    // input_buffer<T_D> &inWindow;
    T_D* __restrict inWindow;
    input_stream<accType_t<T_D, T_T> >* inCascade;
};
// IF output type ------------------------------------
template <bool T_CASC_OUT, typename T_D, typename T_T>
struct T_outputIF {};
// CASC_OUT_FALSE
template <typename T_D, typename T_T>
struct T_outputIF<CASC_OUT_FALSE, T_D, T_T> {
    T_D* __restrict outWindow;
    no_port* outCascade;
};
// CASC_OUT_TRUE
template <typename T_D, typename T_T>
struct T_outputIF<CASC_OUT_TRUE, T_D, T_T> {
    no_port* outWindow;
    output_stream<accType_t<T_D, T_T> >* outCascade;
};

//-----------------------------------------------------------------------------------------------------
// Base class
template <typename TT_DATA,
          typename TT_TWIDDLE,
          unsigned int TP_POINT_SIZE,
          unsigned int TP_FFT_NIFFT,
          unsigned int TP_SHIFT,
          unsigned int TP_CASC_LEN,
          unsigned int TP_NUM_FRAMES,
          unsigned int TP_KERNEL_POSITION,
          bool TP_CASC_IN = CASC_IN_FALSE,
          bool TP_CASC_OUT = CASC_OUT_FALSE>
class kernelDFTClass {
   private:
    // Parameter value defensive and legality checks
    static_assert(fnCheckDataType<TT_DATA>(), "ERROR: TT_IN_DATA is not a supported type");
    static_assert(fnCheckTwiddleType<TT_TWIDDLE>(), "ERROR: TT_TWIDDLE is not a supported type");
    static_assert(fnCheckDataTwiddleType<TT_DATA, TT_TWIDDLE>(), "ERROR: TT_TWIDDLE is incompatible with data type");
    static_assert(fnCheckPointSize<TP_POINT_SIZE>(), "ERROR: TP_POINT_SIZE is not a supported value {4-128");
    static_assert(TP_FFT_NIFFT == 0 || TP_FFT_NIFFT == 1, "ERROR: TP_FFT_NIFFT must be 0 (reverse) or 1 (forward)");
    static_assert(fnCheckShift<TP_SHIFT>(), "ERROR: TP_SHIFT is out of range (0 to 60)");
    static_assert(fnCheckShiftFloat<TT_DATA, TP_SHIFT>(),
                  "ERROR: TP_SHIFT is ignored for data type cfloat so must be set to 0");

    typedef typename std::conditional_t<
        std::is_same<TT_DATA, int16>::value,
        cint16,
        std::conditional_t<std::is_same<TT_DATA, int32>::value,
                           cint32,
                           std::conditional_t<std::is_same<TT_DATA, float>::value, cfloat, TT_DATA> > >
        T_outDataType;

    static constexpr int kSamplesInVectOutData = 256 / 8 / sizeof(T_outDataType);
    static constexpr int kSamplesInVectData = 256 / 8 / sizeof(TT_DATA);
    static constexpr int kSamplesInVectTwiddle = kSamplesInVectOutData;

    static constexpr int paddedDataSize = CEIL(TP_POINT_SIZE, kSamplesInVectOutData);
    static constexpr int paddedCoeffSize = CEIL(TP_POINT_SIZE, kSamplesInVectTwiddle);

    static constexpr int paddedFrameSize = CEIL(paddedDataSize, (kSamplesInVectOutData * TP_CASC_LEN));
    static constexpr int paddedWindowSize = TP_NUM_FRAMES * paddedFrameSize;

    static constexpr int cascWindowSize = paddedWindowSize / TP_CASC_LEN;
    static constexpr int cascFrameSize = paddedFrameSize / TP_CASC_LEN;

    static constexpr int stepSize =
        (TP_KERNEL_POSITION < (TP_POINT_SIZE % TP_CASC_LEN)) + (TP_POINT_SIZE / TP_CASC_LEN);

    // static constexpr int kTotalCoeffSize = paddedCoeffSize*cascFrameSize;
    static constexpr int kTotalCoeffSize = paddedCoeffSize * stepSize;
    static constexpr int kVecInCoeff = paddedCoeffSize / kSamplesInVectData;
    static constexpr int kVecInFrame = cascFrameSize / kSamplesInVectData;

   public:
    // Constructor
    kernelDFTClass() {}

    // DFT
    void kernelDFT(T_inputIF<TP_CASC_IN, TT_DATA, TT_TWIDDLE> inInterface,
                   TT_TWIDDLE* __restrict coeffPtr,
                   T_outputIF<TP_CASC_OUT, T_outDataType, TT_TWIDDLE> outInterface);
};

//-----------------------------------------------------------------------------------------------------
// Cascade layer class and specializations

//-----------------------------------------------------------------------------------------------------
// This is the main declaration of the matrix_mult class, and is also used for the Standalone kernel specialization with
// no cascade ports, a single input and no reload

template <typename TT_DATA,
          typename TT_TWIDDLE,
          unsigned int TP_POINT_SIZE,
          unsigned int TP_FFT_NIFFT,
          unsigned int TP_SHIFT,
          unsigned int TP_CASC_LEN,
          unsigned int TP_NUM_FRAMES,
          unsigned int TP_KERNEL_POSITION,
          bool TP_CASC_IN = CASC_IN_FALSE,
          bool TP_CASC_OUT = CASC_OUT_FALSE>
class dft {
   private:
    kernelDFTClass<TT_DATA,
                   TT_TWIDDLE,
                   TP_POINT_SIZE,
                   TP_FFT_NIFFT,
                   TP_SHIFT,
                   TP_CASC_LEN,
                   TP_NUM_FRAMES,
                   TP_CASC_IN,
                   TP_CASC_OUT>
        m_dftKernel;
    typedef typename std::conditional_t<
        std::is_same<TT_DATA, int16>::value,
        cint16,
        std::conditional_t<std::is_same<TT_DATA, int32>::value,
                           cint32,
                           std::conditional_t<std::is_same<TT_DATA, float>::value, cfloat, TT_DATA> > >
        T_outDataType;
    static constexpr int kSamplesInVectOutData = 256 / 8 / sizeof(T_outDataType);
    static constexpr int kSamplesInVectTwiddle = kSamplesInVectOutData;
    // static constexpr int paddedDataSize  = CEIL(TP_POINT_SIZE, kSamplesInVectOutData);
    static constexpr int paddedCoeffSize = CEIL(TP_POINT_SIZE, kSamplesInVectTwiddle);
    // static constexpr int paddedFrameSize = CEIL(paddedDataSize, (kSamplesInVectOutData*TP_CASC_LEN));

    // static constexpr int cascFrameSize = paddedFrameSize/TP_CASC_LEN;
    static constexpr int stepSize =
        (TP_KERNEL_POSITION < (TP_POINT_SIZE % TP_CASC_LEN)) + (TP_POINT_SIZE / TP_CASC_LEN);

    // static constexpr int kTotalCoeffSize = paddedCoeffSize*cascFrameSize;
    static constexpr int kTotalCoeffSize = paddedCoeffSize * stepSize;

   public:
    TT_TWIDDLE (&inCoeff)[kTotalCoeffSize]; // Need to change
    // Constructor

    dft(TT_TWIDDLE (&coeff)[kTotalCoeffSize]) : inCoeff(coeff) {}

    // Register Kernel Class
    static void registerKernelClass() {
        REGISTER_FUNCTION(dft::dftMain);
        REGISTER_PARAMETER(inCoeff);
    }
    // DFT
    void dftMain(input_buffer<TT_DATA>& __restrict inWindow, output_buffer<T_outDataType>& __restrict outWindow);
};

//-----------------------------------------------------------------------------------------------------
// Partially specialized classes for cascaded interface (First kernel in cascade), single input, no reload
template <typename TT_DATA,
          typename TT_TWIDDLE,
          unsigned int TP_POINT_SIZE,
          unsigned int TP_FFT_NIFFT,
          unsigned int TP_SHIFT,
          unsigned int TP_CASC_LEN,
          unsigned int TP_NUM_FRAMES,
          unsigned int TP_KERNEL_POSITION>
class dft<TT_DATA,
          TT_TWIDDLE,
          TP_POINT_SIZE,
          TP_FFT_NIFFT,
          TP_SHIFT,
          TP_CASC_LEN,
          TP_NUM_FRAMES,
          TP_KERNEL_POSITION,
          CASC_IN_FALSE,
          CASC_OUT_TRUE> {
   private:
    kernelDFTClass<TT_DATA,
                   TT_TWIDDLE,
                   TP_POINT_SIZE,
                   TP_FFT_NIFFT,
                   TP_SHIFT,
                   TP_CASC_LEN,
                   TP_NUM_FRAMES,
                   TP_KERNEL_POSITION,
                   CASC_IN_FALSE,
                   CASC_OUT_TRUE>
        m_dftKernel;
    typedef typename std::conditional_t<
        std::is_same<TT_DATA, int16>::value,
        cint16,
        std::conditional_t<std::is_same<TT_DATA, int32>::value,
                           cint32,
                           std::conditional_t<std::is_same<TT_DATA, float>::value, cfloat, TT_DATA> > >
        T_outDataType;
    static constexpr int kSamplesInVectOutData = 256 / 8 / sizeof(T_outDataType);
    static constexpr int kSamplesInVectTwiddle = kSamplesInVectOutData;
    // static constexpr int paddedDataSize  = CEIL(TP_POINT_SIZE, kSamplesInVectOutData);
    static constexpr int paddedCoeffSize = CEIL(TP_POINT_SIZE, kSamplesInVectTwiddle);
    // static constexpr int paddedFrameSize = CEIL(paddedDataSize, (kSamplesInVectOutData*TP_CASC_LEN));
    // static constexpr int cascFrameSize = paddedFrameSize/TP_CASC_LEN;

    static constexpr int stepSize =
        (TP_KERNEL_POSITION < (TP_POINT_SIZE % TP_CASC_LEN)) + (TP_POINT_SIZE / TP_CASC_LEN);
    // static constexpr int kTotalCoeffSize = paddedCoeffSize*cascFrameSize;
    static constexpr int kTotalCoeffSize = paddedCoeffSize * stepSize;

   public:
    TT_TWIDDLE (&inCoeff)[kTotalCoeffSize];
    // Constructor

    dft(TT_TWIDDLE (&coeff)[kTotalCoeffSize]) : inCoeff(coeff) {}

    // Register Kernel Class
    static void registerKernelClass() {
        REGISTER_FUNCTION(dft::dftMain);
        REGISTER_PARAMETER(inCoeff);
    }
    // DFT
    void dftMain(input_buffer<TT_DATA>& __restrict inWindow,
                 output_stream<accType_t<T_outDataType, TT_TWIDDLE> >* outCascade);
};

// Partially specialized classes for cascaded interface (middle kernels in cascade), single input, no reload
template <typename TT_DATA,
          typename TT_TWIDDLE,
          unsigned int TP_POINT_SIZE,
          unsigned int TP_FFT_NIFFT,
          unsigned int TP_SHIFT,
          unsigned int TP_CASC_LEN,
          unsigned int TP_NUM_FRAMES,
          unsigned int TP_KERNEL_POSITION>
class dft<TT_DATA,
          TT_TWIDDLE,
          TP_POINT_SIZE,
          TP_FFT_NIFFT,
          TP_SHIFT,
          TP_CASC_LEN,
          TP_NUM_FRAMES,
          TP_KERNEL_POSITION,
          CASC_IN_TRUE,
          CASC_OUT_TRUE> {
   private:
    // kernelDFTClass <TT_DATA, TT_TWIDDLE, TP_POINT_SIZE, TP_FFT_NIFFT, TP_SHIFT, CASC_IN_TRUE, CASC_OUT_TRUE>
    // m_dftKernel;
    kernelDFTClass<TT_DATA,
                   TT_TWIDDLE,
                   TP_POINT_SIZE,
                   TP_FFT_NIFFT,
                   TP_SHIFT,
                   TP_CASC_LEN,
                   TP_NUM_FRAMES,
                   TP_KERNEL_POSITION,
                   CASC_IN_TRUE,
                   CASC_OUT_TRUE>
        m_dftKernel;
    typedef typename std::conditional_t<
        std::is_same<TT_DATA, int16>::value,
        cint16,
        std::conditional_t<std::is_same<TT_DATA, int32>::value,
                           cint32,
                           std::conditional_t<std::is_same<TT_DATA, float>::value, cfloat, TT_DATA> > >
        T_outDataType;
    static constexpr int kSamplesInVectOutData = 256 / 8 / sizeof(T_outDataType);
    static constexpr int kSamplesInVectTwiddle = kSamplesInVectOutData;
    // static constexpr int paddedDataSize  = CEIL(TP_POINT_SIZE, kSamplesInVectOutData);
    static constexpr int paddedCoeffSize = CEIL(TP_POINT_SIZE, kSamplesInVectTwiddle);
    // static constexpr int paddedFrameSize = CEIL(paddedDataSize, (kSamplesInVectOutData*TP_CASC_LEN));
    // static constexpr int cascFrameSize = paddedFrameSize/TP_CASC_LEN;

    static constexpr int stepSize =
        (TP_KERNEL_POSITION < (TP_POINT_SIZE % TP_CASC_LEN)) + (TP_POINT_SIZE / TP_CASC_LEN);
    // static constexpr int kTotalCoeffSize = paddedCoeffSize*cascFrameSize;
    static constexpr int kTotalCoeffSize = paddedCoeffSize * stepSize;

   public:
    TT_TWIDDLE (&inCoeff)[kTotalCoeffSize];
    // Constructor

    dft(TT_TWIDDLE (&coeff)[kTotalCoeffSize]) : inCoeff(coeff) {}

    // Register Kernel Class
    static void registerKernelClass() {
        REGISTER_FUNCTION(dft::dftMain);
        REGISTER_PARAMETER(inCoeff);
    }
    // DFT
    void dftMain(input_buffer<TT_DATA>& __restrict inWindow,
                 input_stream<accType_t<T_outDataType, TT_TWIDDLE> >* inCascade,
                 output_stream<accType_t<T_outDataType, TT_TWIDDLE> >* outCascade);
};

// For multiple kernels, final kernel used input cascade output window
template <typename TT_DATA,
          typename TT_TWIDDLE,
          unsigned int TP_POINT_SIZE,
          unsigned int TP_FFT_NIFFT,
          unsigned int TP_SHIFT,
          unsigned int TP_CASC_LEN,
          unsigned int TP_NUM_FRAMES,
          unsigned int TP_KERNEL_POSITION>
class dft<TT_DATA,
          TT_TWIDDLE,
          TP_POINT_SIZE,
          TP_FFT_NIFFT,
          TP_SHIFT,
          TP_CASC_LEN,
          TP_NUM_FRAMES,
          TP_KERNEL_POSITION,
          CASC_IN_TRUE,
          CASC_OUT_FALSE> {
   private:
    kernelDFTClass<TT_DATA,
                   TT_TWIDDLE,
                   TP_POINT_SIZE,
                   TP_FFT_NIFFT,
                   TP_SHIFT,
                   TP_CASC_LEN,
                   TP_NUM_FRAMES,
                   TP_KERNEL_POSITION,
                   CASC_IN_TRUE,
                   CASC_OUT_FALSE>
        m_dftKernel;
    typedef typename std::conditional_t<
        std::is_same<TT_DATA, int16>::value,
        cint16,
        std::conditional_t<std::is_same<TT_DATA, int32>::value,
                           cint32,
                           std::conditional_t<std::is_same<TT_DATA, float>::value, cfloat, TT_DATA> > >
        T_outDataType;
    static constexpr int kSamplesInVectOutData = 256 / 8 / sizeof(T_outDataType);
    static constexpr int kSamplesInVectTwiddle = kSamplesInVectOutData;
    // static constexpr int paddedDataSize  = CEIL(TP_POINT_SIZE, kSamplesInVectOutData);
    static constexpr int paddedCoeffSize = CEIL(TP_POINT_SIZE, kSamplesInVectTwiddle);
    // static constexpr int paddedFrameSize = CEIL(paddedDataSize, (kSamplesInVectOutData*TP_CASC_LEN));
    // static constexpr int cascFrameSize = paddedFrameSize/TP_CASC_LEN;

    static constexpr int stepSize =
        (TP_KERNEL_POSITION < (TP_POINT_SIZE % TP_CASC_LEN)) + (TP_POINT_SIZE / TP_CASC_LEN);
    // static constexpr int kTotalCoeffSize = paddedCoeffSize*cascFrameSize;
    static constexpr int kTotalCoeffSize = paddedCoeffSize * stepSize;

   public:
    TT_TWIDDLE (&inCoeff)[kTotalCoeffSize];
    // Constructor

    dft(TT_TWIDDLE (&coeff)[kTotalCoeffSize]) : inCoeff(coeff) {}

    // Register Kernel Class
    static void registerKernelClass() {
        REGISTER_FUNCTION(dft::dftMain);
        REGISTER_PARAMETER(inCoeff);
    }
    // DFT
    void dftMain(input_buffer<TT_DATA>& __restrict inWindow,
                 input_stream<accType_t<T_outDataType, TT_TWIDDLE> >* inCascade,
                 output_buffer<T_outDataType>& __restrict outWindow);
};
}
}
}
}
}
#endif // _DSPLIB_DFT_HPP_