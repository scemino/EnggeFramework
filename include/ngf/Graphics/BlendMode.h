#pragma once

namespace ngf {
enum class BlendEquation {
  Add,
  Subtract,
  ReverseSubtract,
};

enum class BlendFactor {
  Zero,
  One,
  SrcColor,
  OneMinusSrcColor,
  DstColor,
  OneMinusDstColor,
  SrcAlpha,
  OneMinusSrcAlpha,
  DstAlpha,
  OneMinusDstAlpha
};

/// @brief Blending mode used for drawing.
struct BlendMode {
  constexpr BlendMode()
      : colorSrcFactor(BlendFactor::One), colorDstFactor(BlendFactor::Zero), colorEquation(BlendEquation::Add),
        alphaSrcFactor(BlendFactor::One), alphaDstFactor(BlendFactor::Zero), alphaEquation(BlendEquation::Add) {
  }

  constexpr BlendMode(BlendFactor sourceFactor,
                      BlendFactor destinationFactor,
                      BlendEquation equation = BlendEquation::Add)
      : colorSrcFactor(sourceFactor), colorDstFactor(destinationFactor), colorEquation(equation),
        alphaSrcFactor(sourceFactor), alphaDstFactor(destinationFactor), alphaEquation(equation) {

  }

  constexpr BlendMode(BlendFactor colorSourceFactor,
                      BlendFactor colorDestinationFactor,
                      BlendEquation colorBlendEquation,
                      BlendFactor alphaSourceFactor,
                      BlendFactor alphaDestinationFactor,
                      BlendEquation alphaBlendEquation)
      : colorSrcFactor(colorSourceFactor), colorDstFactor(colorDestinationFactor), colorEquation(colorBlendEquation),
        alphaSrcFactor(alphaSourceFactor), alphaDstFactor(alphaDestinationFactor), alphaEquation(alphaBlendEquation) {

  }

  BlendFactor colorSrcFactor;
  BlendFactor colorDstFactor;
  BlendEquation colorEquation;
  BlendFactor alphaSrcFactor;
  BlendFactor alphaDstFactor;
  BlendEquation alphaEquation;
};

constexpr BlendMode BlendAlpha{
    BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha, BlendEquation::Add,
    BlendFactor::One, BlendFactor::OneMinusSrcAlpha, BlendEquation::Add
};

constexpr BlendMode BlendAdd{
    BlendFactor::SrcAlpha, BlendFactor::One, BlendEquation::Add,
    BlendFactor::One, BlendFactor::One, BlendEquation::Add
};

constexpr BlendMode BlendMultiply{BlendFactor::DstColor, BlendFactor::Zero};

constexpr BlendMode BlendNone{BlendFactor::One, BlendFactor::Zero};

constexpr bool operator==(const BlendMode &lhs, const BlendMode &rhs) {
  return lhs.colorSrcFactor == rhs.colorSrcFactor
      && lhs.colorDstFactor == rhs.colorDstFactor
      && lhs.colorEquation == rhs.colorEquation
      && lhs.alphaSrcFactor == rhs.alphaSrcFactor
      && lhs.alphaDstFactor == rhs.alphaDstFactor
      && lhs.alphaEquation == rhs.alphaEquation;
}
}
