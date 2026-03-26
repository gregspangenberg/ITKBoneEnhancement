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

#ifndef itkCalgaryEigenToMeasureImageFilter_h
#define itkCalgaryEigenToMeasureImageFilter_h

#include "itkEigenToMeasureImageFilter.h"
#include "itkMath.h"

namespace itk
{
/** \class CalgaryEigenToMeasureImageFilter
 * \brief Convert eigenvalues into a measure of sheetness according to the method of Besler et al.
 *
 * Converts a 3D fixed array of eigenvalues into a measure of sheetness according to the method
 * of Besler et al. The parameters of the filter should be set using
 * CalgaryEigenToMeasureParameterEstimationFilter.
 *
 * Computes the following equation for eigenvalues in a three dimensional fixed array:
 *  \f{eqnarray*}{
 *      R_{bone} &=&  \frac{|\lambda_1| |\lambda_2|}{|\lambda_3|^2} \\
 *      R_{noise} &=&  \sqrt{|\lambda_1|^2 + |\lambda_2|^2 + |\lambda_3|^2} \\
 *      s &=& \frac{\lambda_3}{|\lambda_3|}  \exp\left(- \frac{R_{bone}^2}{2 \alpha^2} \right) \left(1 - \exp\left(-
 * \frac{R_{noise}^2}{2 c^2} \right) \right) \f}
 *
 * Note that if \f$ \lambda_3 \approx 0 \f$, \f$ s = 0 \f$.
 *
 * \sa CalgaryEigenToMeasureParameterEstimationFilter
 * \sa EigenToMeasureImageFilter
 * \sa MultiScaleHessianEnhancementImageFilter
 *
 * \author: Bryce Besler
 * \ingroup BoneEnhancement
 */
template <typename TInputImage, typename TOutputImage>
class CalgaryEigenToMeasureImageFilter : public EigenToMeasureImageFilter<TInputImage, TOutputImage>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(CalgaryEigenToMeasureImageFilter);

  /** Standard Self typedef */
  using Self = CalgaryEigenToMeasureImageFilter;
  using Superclass = EigenToMeasureImageFilter<TInputImage, TOutputImage>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Input typedefs */
  using InputImageType = typename Superclass::InputImageType;
  using InputImagePixelType = typename Superclass::InputImagePixelType;
  using InputImagePointer = typename Superclass::InputImagePointer;
  using InputImageConstPointer = typename Superclass::InputImageConstPointer;
  using InputImageRegionType = typename Superclass::InputImageRegionType;

  /** Output typedefs */
  using OutputImageType = typename Superclass::OutputImageType;
  using OutputImagePointer = typename Superclass::OutputImagePointer;
  using OutputImageRegionType = typename Superclass::OutputImageRegionType;
  using OutputImagePixelType = typename Superclass::OutputImagePixelType;

  /** Parameter typedefs */
  using RealType = typename Superclass::RealType;
  using ParameterArrayType = typename Superclass::ParameterArrayType;
  using ParameterDecoratedType = typename Superclass::ParameterDecoratedType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkOverrideGetNameOfClassMacro(CalgaryEigenToMeasureImageFilter);

  /** Explicitely state the eigenvalues are ordered by magnitude for this filter */
  typename Superclass::EigenValueOrderEnum
  GetEigenValueOrder() const override
  {
    return Superclass::EigenValueOrderEnum::OrderByMagnitude;
  }

  /** Setter/Getter methods for setting Direction */
  itkSetMacro(EnhanceType, RealType);
  itkGetConstMacro(EnhanceType, RealType);
  void
  SetEnhanceBrightObjects()
  {
    SetEnhanceType(-1.0);
  }
  void
  SetEnhanceDarkObjects()
  {
    SetEnhanceType(1.0);
  }

#ifdef ITK_USE_CONCEPT_CHECKING
  // Begin concept checking
  itkConceptMacro(InputHaveDimension3Check, (Concept::SameDimension<TInputImage::ImageDimension, 3u>));
  itkConceptMacro(OutputHaveDimension3Check, (Concept::SameDimension<TOutputImage::ImageDimension, 3u>));
  itkConceptMacro(InputFixedArrayHasDimension3Check, (Concept::SameDimension<TInputImage::PixelType::Dimension, 3u>));
  // End concept checking
#endif
protected:
  CalgaryEigenToMeasureImageFilter();
  ~CalgaryEigenToMeasureImageFilter() override = default;

  OutputImagePixelType
  ProcessPixel(const InputImagePixelType & pixel) override;

  /** Check the input has the right number of parameters. */
  void
  BeforeThreadedGenerateData() override;

  void
  PrintSelf(std::ostream & os, Indent indent) const override;

private:
  /* Member variables */
  RealType m_EnhanceType;
}; // end class
} /* end namespace itk */

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkCalgaryEigenToMeasureImageFilter.hxx"
#endif

#endif /* itkCalgaryEigenToMeasureImageFilter_h */
