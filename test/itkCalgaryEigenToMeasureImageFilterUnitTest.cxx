/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "itkGTest.h"
#include "itkCalgaryEigenToMeasureImageFilter.h"
#include "itkImageMaskSpatialObject.h"
#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"

namespace
{
template <typename T>
class itkCalgaryEigenToMeasureImageFilterUnitTest : public ::testing::Test
{
public:
  /* Useful typedefs */
  static const unsigned int DIMENSION = 3;
  using PixelType = T;
  using OutputPixelType = itk::Image<PixelType, DIMENSION>;
  using EigenPixelType = float;
  using EigenValueArrayType = itk::FixedArray<EigenPixelType, DIMENSION>;
  using EigenImageType = itk::Image<EigenValueArrayType, DIMENSION>;
  using MaskImageType = itk::Image<unsigned char, DIMENSION>;
  using FilterType = typename itk::CalgaryEigenToMeasureImageFilter<EigenImageType, OutputPixelType>;
  using FilterPointerType = typename FilterType::Pointer;
  using ParameterArrayType = typename FilterType::ParameterArrayType;
  using SpatialObjectType = itk::ImageMaskSpatialObject<DIMENSION>;

  itkCalgaryEigenToMeasureImageFilterUnitTest()
  {
    /* Instantiate filter */
    m_Filter = FilterType::New();

    /* Set parameter size - Calgary uses 2 parameters */
    m_Parameters.SetSize(2);

    /* Create EigenPixels */
    for (unsigned int i = 0; i < m_OneEigenPixel.Length; ++i)
    {
      m_OneEigenPixel[i] = 1;
      m_ZeroEigenPixel[i] = 0;
    }
    m_NonZeroEigenPixel[0] = 0.25;
    m_NonZeroEigenPixel[1] = 1;
    m_NonZeroEigenPixel[2] = -1;
    m_NonZeroDarkEigenPixel[0] = 0.25;
    m_NonZeroDarkEigenPixel[1] = 1;
    m_NonZeroDarkEigenPixel[2] = 1;

    /* Create ImageRegion */
    typename EigenImageType::IndexType start;
    start[0] = 0;
    start[1] = 0;
    start[2] = 0;

    typename EigenImageType::SizeType size;
    size[0] = 10;
    size[1] = 10;
    size[2] = 10;

    m_Region.SetSize(size);
    m_Region.SetIndex(start);

    /* Create EigenImage */
    m_OnesEigenImage = EigenImageType::New();
    m_OnesEigenImage->SetRegions(m_Region);
    m_OnesEigenImage->Allocate();
    m_OnesEigenImage->FillBuffer(m_OneEigenPixel);

    m_ZerosEigenImage = EigenImageType::New();
    m_ZerosEigenImage->SetRegions(m_Region);
    m_ZerosEigenImage->Allocate();
    m_ZerosEigenImage->FillBuffer(m_ZeroEigenPixel);

    m_NonZeroEigenImage = EigenImageType::New();
    m_NonZeroEigenImage->SetRegions(m_Region);
    m_NonZeroEigenImage->Allocate();
    m_NonZeroEigenImage->FillBuffer(m_NonZeroEigenPixel);

    m_NonZeroDarkEigenImage = EigenImageType::New();
    m_NonZeroDarkEigenImage->SetRegions(m_Region);
    m_NonZeroDarkEigenImage->Allocate();
    m_NonZeroDarkEigenImage->FillBuffer(m_NonZeroDarkEigenPixel);

    m_MaskingEigenImage = EigenImageType::New();
    m_MaskingEigenImage->SetRegions(m_Region);
    m_MaskingEigenImage->Allocate();
    m_MaskingEigenImage->FillBuffer(m_OneEigenPixel);

    /* Create MaskImage */
    typename EigenImageType::IndexType maskStart;
    maskStart[0] = 2;
    maskStart[1] = 2;
    maskStart[2] = 2;

    typename EigenImageType::SizeType maskSize;
    maskSize[0] = 12;
    maskSize[1] = 12;
    maskSize[2] = 12;

    m_MaskRegion.SetSize(maskSize);
    m_MaskRegion.SetIndex(maskStart);

    m_MaskImage = MaskImageType::New();
    m_MaskImage->SetRegions(m_MaskRegion);
    m_MaskImage->Allocate();
    m_MaskImage->FillBuffer(0);

    m_MaskRegion.Crop(m_Region);
    itk::ImageRegionIteratorWithIndex<MaskImageType> maskIt(m_MaskImage, m_MaskRegion);

    maskIt.GoToBegin();
    while (!maskIt.IsAtEnd())
    {
      maskIt.Set(1);
      ++maskIt;
    }

    m_SpatialObject = SpatialObjectType::New();
    m_SpatialObject->SetImage(m_MaskImage);
  }
  ~itkCalgaryEigenToMeasureImageFilterUnitTest() override = default;

protected:
  void
  SetUp() override
  {}
  void
  TearDown() override
  {}

  FilterPointerType                   m_Filter;
  typename MaskImageType::Pointer     m_MaskImage;
  typename EigenImageType::Pointer    m_ZerosEigenImage;
  typename EigenImageType::Pointer    m_OnesEigenImage;
  typename EigenImageType::Pointer    m_NonZeroEigenImage;
  typename EigenImageType::Pointer    m_NonZeroDarkEigenImage;
  typename EigenImageType::Pointer    m_MaskingEigenImage;
  EigenValueArrayType                 m_OneEigenPixel;
  EigenValueArrayType                 m_ZeroEigenPixel;
  EigenValueArrayType                 m_NonZeroEigenPixel;
  EigenValueArrayType                 m_NonZeroDarkEigenPixel;
  ParameterArrayType                  m_Parameters;
  typename EigenImageType::RegionType m_Region;
  typename EigenImageType::RegionType m_MaskRegion;
  typename SpatialObjectType::Pointer m_SpatialObject;
};
} // namespace

// Define the templates we would like to test
using TestingLabelTypes = ::testing::Types<double, float>;
TYPED_TEST_SUITE(itkCalgaryEigenToMeasureImageFilterUnitTest, TestingLabelTypes);

TYPED_TEST(itkCalgaryEigenToMeasureImageFilterUnitTest, InitialParameters)
{
  /* Default enhance bright structures */
  EXPECT_DOUBLE_EQ(-1.0, this->m_Filter->GetEnhanceType());

  EXPECT_EQ(2, static_cast<int>(this->m_Filter->GetEigenValueOrder()));
}

TYPED_TEST(itkCalgaryEigenToMeasureImageFilterUnitTest, TestZerosImage)
{
  this->m_Parameters[0] = 0.5;
  this->m_Parameters[1] = 1;
  this->m_Filter->SetParameters(this->m_Parameters);
  this->m_Filter->SetInput(this->m_ZerosEigenImage);
  this->m_Filter->Update();
  EXPECT_NO_THROW(this->m_Filter->Update());
  EXPECT_TRUE(this->m_Filter->GetOutput()->GetBufferedRegion() == this->m_Region);

  using ImageType = typename itk::Image<TypeParam, 3>;
  itk::ImageRegionIteratorWithIndex<ImageType> input(this->m_Filter->GetOutput(), this->m_Region);

  input.GoToBegin();
  while (!input.IsAtEnd())
  {
    ASSERT_EQ(0.0, input.Get());
    ++input;
  }
}

TYPED_TEST(itkCalgaryEigenToMeasureImageFilterUnitTest, TestRealEigenPixelBrightSheet)
{
  /* Calgary formula:
   * Rbone = (l1*l2)/(l3*l3) = (0.25*1)/(1*1) = 0.25
   * Rnoise = sqrt(0.25^2 + 1^2 + 1^2) = sqrt(2.0625) = 1.4361406616345072
   * sheetness = (m_EnhanceType*a3/l3) * exp(-(Rbone^2)/(2*alpha^2)) * (1 - exp(-(Rnoise^2)/(2*c^2)))
   * With alpha=0.5, c=0.25, m_EnhanceType=-1, a3=-1, l3=1:
   * sheetness = (-1*-1/1) * exp(-(0.25^2)/(2*0.5^2)) * (1 - exp(-(2.0625)/(2*0.25^2)))
   * sheetness = 1 * exp(-0.0625/0.5) * (1 - exp(-2.0625/0.125))
   * sheetness = 1 * exp(-0.125) * (1 - exp(-16.5))
   * sheetness = 0.8824969... * 0.999999... = 0.8824969...
   */
  this->m_Parameters[0] = 0.5;
  this->m_Parameters[1] = 0.25;
  this->m_Filter->SetParameters(this->m_Parameters);
  this->m_Filter->SetInput(this->m_NonZeroEigenImage);
  EXPECT_NO_THROW(this->m_Filter->Update());
  EXPECT_TRUE(this->m_Filter->GetOutput()->GetBufferedRegion() == this->m_Region);

  using ImageType = typename itk::Image<TypeParam, 3>;
  itk::ImageRegionIteratorWithIndex<ImageType> input(this->m_Filter->GetOutput(), this->m_Region);

  input.GoToBegin();
  while (!input.IsAtEnd())
  {
    ASSERT_NEAR((TypeParam)0.882496902585, input.Get(), 1e-6);
    ++input;
  }
}

TYPED_TEST(itkCalgaryEigenToMeasureImageFilterUnitTest, TestRealEigenPixelDarkSheet)
{
  /* With dark objects (m_EnhanceType=1) and a3=-1:
   * sheetness = (1*-1/1) = -1 times positive terms = negative
   * For bright eigenvalues (lambda3 < 0), dark enhancement should give negative result
   */
  this->m_Parameters[0] = 0.5;
  this->m_Parameters[1] = 0.25;
  this->m_Filter->SetParameters(this->m_Parameters);
  this->m_Filter->SetInput(this->m_NonZeroEigenImage);
  this->m_Filter->SetEnhanceDarkObjects();
  EXPECT_NO_THROW(this->m_Filter->Update());
  EXPECT_TRUE(this->m_Filter->GetOutput()->GetBufferedRegion() == this->m_Region);

  using ImageType = typename itk::Image<TypeParam, 3>;
  itk::ImageRegionIteratorWithIndex<ImageType> input(this->m_Filter->GetOutput(), this->m_Region);

  input.GoToBegin();
  while (!input.IsAtEnd())
  {
    ASSERT_NEAR((TypeParam)-0.882496902585, input.Get(), 1e-6);
    ++input;
  }
}

TYPED_TEST(itkCalgaryEigenToMeasureImageFilterUnitTest, TestDarkRealEigenPixelBrightSheet)
{
  /* With bright enhancement (m_EnhanceType=-1) and a3=1 (dark eigenvalue):
   * sheetness = (-1*1/1) = -1 times positive terms = negative
   */
  this->m_Parameters[0] = 0.5;
  this->m_Parameters[1] = 0.25;
  this->m_Filter->SetParameters(this->m_Parameters);
  this->m_Filter->SetInput(this->m_NonZeroDarkEigenImage);
  EXPECT_NO_THROW(this->m_Filter->Update());
  EXPECT_TRUE(this->m_Filter->GetOutput()->GetBufferedRegion() == this->m_Region);

  using ImageType = typename itk::Image<TypeParam, 3>;
  itk::ImageRegionIteratorWithIndex<ImageType> input(this->m_Filter->GetOutput(), this->m_Region);

  input.GoToBegin();
  while (!input.IsAtEnd())
  {
    ASSERT_NEAR((TypeParam)-0.882496902585, input.Get(), 1e-6);
    ++input;
  }
}

TYPED_TEST(itkCalgaryEigenToMeasureImageFilterUnitTest, TestDarkRealEigenPixelDarkSheet)
{
  /* With dark enhancement (m_EnhanceType=1) and a3=1:
   * sheetness = (1*1/1) = 1 times positive terms = positive
   */
  this->m_Parameters[0] = 0.5;
  this->m_Parameters[1] = 0.25;
  this->m_Filter->SetParameters(this->m_Parameters);
  this->m_Filter->SetInput(this->m_NonZeroDarkEigenImage);
  this->m_Filter->SetEnhanceDarkObjects();
  EXPECT_NO_THROW(this->m_Filter->Update());
  EXPECT_TRUE(this->m_Filter->GetOutput()->GetBufferedRegion() == this->m_Region);

  using ImageType = typename itk::Image<TypeParam, 3>;
  itk::ImageRegionIteratorWithIndex<ImageType> input(this->m_Filter->GetOutput(), this->m_Region);

  input.GoToBegin();
  while (!input.IsAtEnd())
  {
    ASSERT_NEAR((TypeParam)0.882496902585, input.Get(), 1e-6);
    ++input;
  }
}
