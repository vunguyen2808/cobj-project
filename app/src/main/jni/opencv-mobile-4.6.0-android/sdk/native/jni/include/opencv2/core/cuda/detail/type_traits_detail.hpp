#ifndef OPENCV_CUDA_TYPE_TRAITS_DETAIL_HPP
#define OPENCV_CUDA_TYPE_TRAITS_DETAIL_HPP

#include "../common.hpp"
#include "../vec_traits.hpp"

namespace cv { namespace cuda { namespace device
{
    namespace type_traits_detail
    {
        template <bool, typename T1, typename T2> struct Select { typedef T1 type; };
        template <typename T1, typename T2> struct Select<false, T1, T2> { typedef T2 type; };

        template <typename T> struct IsSignedIntergral { enum {value = 0}; };
        template <> struct IsSignedIntergral<schar> { enum {value = 1}; };
        template <> struct IsSignedIntergral<char1> { enum {value = 1}; };
        template <> struct IsSignedIntergral<short> { enum {value = 1}; };
        template <> struct IsSignedIntergral<short1> { enum {value = 1}; };
        template <> struct IsSignedIntergral<int> { enum {value = 1}; };
        template <> struct IsSignedIntergral<int1> { enum {value = 1}; };

        template <typename T> struct IsUnsignedIntegral { enum {value = 0}; };
        template <> struct IsUnsignedIntegral<uchar> { enum {value = 1}; };
        template <> struct IsUnsignedIntegral<uchar1> { enum {value = 1}; };
        template <> struct IsUnsignedIntegral<ushort> { enum {value = 1}; };
        template <> struct IsUnsignedIntegral<ushort1> { enum {value = 1}; };
        template <> struct IsUnsignedIntegral<uint> { enum {value = 1}; };
        template <> struct IsUnsignedIntegral<uint1> { enum {value = 1}; };

        template <typename T> struct IsIntegral { enum {value = IsSignedIntergral<T>::value || IsUnsignedIntegral<T>::value}; };
        template <> struct IsIntegral<char> { enum {value = 1}; };
        template <> struct IsIntegral<bool> { enum {value = 1}; };

        template <typename T> struct IsFloat { enum {value = 0}; };
        template <> struct IsFloat<float> { enum {value = 1}; };
        template <> struct IsFloat<double> { enum {value = 1}; };

        template <typename T> struct IsVec { enum {value = 0}; };
        template <> struct IsVec<uchar1> { enum {value = 1}; };
        template <> struct IsVec<uchar2> { enum {value = 1}; };
        template <> struct IsVec<uchar3> { enum {value = 1}; };
        template <> struct IsVec<uchar4> { enum {value = 1}; };
        template <> struct IsVec<uchar8> { enum {value = 1}; };
        template <> struct IsVec<char1> { enum {value = 1}; };
        template <> struct IsVec<char2> { enum {value = 1}; };
        template <> struct IsVec<char3> { enum {value = 1}; };
        template <> struct IsVec<char4> { enum {value = 1}; };
        template <> struct IsVec<char8> { enum {value = 1}; };
        template <> struct IsVec<ushort1> { enum {value = 1}; };
        template <> struct IsVec<ushort2> { enum {value = 1}; };
        template <> struct IsVec<ushort3> { enum {value = 1}; };
        template <> struct IsVec<ushort4> { enum {value = 1}; };
        template <> struct IsVec<ushort8> { enum {value = 1}; };
        template <> struct IsVec<short1> { enum {value = 1}; };
        template <> struct IsVec<short2> { enum {value = 1}; };
        template <> struct IsVec<short3> { enum {value = 1}; };
        template <> struct IsVec<short4> { enum {value = 1}; };
        template <> struct IsVec<short8> { enum {value = 1}; };
        template <> struct IsVec<uint1> { enum {value = 1}; };
        template <> struct IsVec<uint2> { enum {value = 1}; };
        template <> struct IsVec<uint3> { enum {value = 1}; };
        template <> struct IsVec<uint4> { enum {value = 1}; };
        template <> struct IsVec<uint8> { enum {value = 1}; };
        template <> struct IsVec<int1> { enum {value = 1}; };
        template <> struct IsVec<int2> { enum {value = 1}; };
        template <> struct IsVec<int3> { enum {value = 1}; };
        template <> struct IsVec<int4> { enum {value = 1}; };
        template <> struct IsVec<int8> { enum {value = 1}; };
        template <> struct IsVec<float1> { enum {value = 1}; };
        template <> struct IsVec<float2> { enum {value = 1}; };
        template <> struct IsVec<float3> { enum {value = 1}; };
        template <> struct IsVec<float4> { enum {value = 1}; };
        template <> struct IsVec<float8> { enum {value = 1}; };
        template <> struct IsVec<double1> { enum {value = 1}; };
        template <> struct IsVec<double2> { enum {value = 1}; };
        template <> struct IsVec<double3> { enum {value = 1}; };
        template <> struct IsVec<double4> { enum {value = 1}; };
        template <> struct IsVec<double8> { enum {value = 1}; };

        template <class U> struct AddParameterType { typedef const U& type; };
        template <class U> struct AddParameterType<U&> { typedef U& type; };
        template <> struct AddParameterType<void> { typedef void type; };

        template <class U> struct ReferenceTraits
        {
            enum { value = false };
            typedef U type;
        };
        template <class U> struct ReferenceTraits<U&>
        {
            enum { value = true };
            typedef U type;
        };

        template <class U> struct PointerTraits
        {
            enum { value = false };
            typedef void type;
        };
        template <class U> struct PointerTraits<U*>
        {
            enum { value = true };
            typedef U type;
        };
        template <class U> struct PointerTraits<U*&>
        {
            enum { value = true };
            typedef U type;
        };

        template <class U> struct UnConst
        {
            typedef U type;
            enum { value = 0 };
        };
        template <class U> struct UnConst<const U>
        {
            typedef U type;
            enum { value = 1 };
        };
        template <class U> struct UnConst<const U&>
        {
            typedef U& type;
            enum { value = 1 };
        };

        template <class U> struct UnVolatile
        {
            typedef U type;
            enum { value = 0 };
        };
        template <class U> struct UnVolatile<volatile U>
        {
            typedef U type;
            enum { value = 1 };
        };
        template <class U> struct UnVolatile<volatile U&>
        {
            typedef U& type;
            enum { value = 1 };
        };
    } // namespace type_traits_detail
}}} // namespace cv { namespace cuda { namespace cudev

//! @endcond

#endif // OPENCV_CUDA_TYPE_TRAITS_DETAIL_HPP
