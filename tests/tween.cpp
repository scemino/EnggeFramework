#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include <ngf/Graphics/Tween.h>

TEST_CASE("Tweening", "[tween]") {
  auto lastValue = -1;
  auto tween = ngf::Tweening::make(0, 10)
      .onValueChanged([&lastValue](int value) { lastValue = value; });
  REQUIRE(tween.isDone() == true);
  REQUIRE(lastValue == -1);

  SECTION("updating the tween has no effect if there is no duration") {
    tween.update(ngf::TimeSpan::seconds(10));
    REQUIRE(tween.isDone());
    REQUIRE(lastValue == -1);
  }

  SECTION("setting duration changes the status of the tween") {
    tween.setDuration(ngf::TimeSpan::seconds(10));
    REQUIRE(tween.isDone() == false);

    tween.update(ngf::TimeSpan::seconds(0));
    REQUIRE(lastValue == 0);
    REQUIRE(tween.isDone() == false);

    tween.update(ngf::TimeSpan::seconds(5));
    REQUIRE(lastValue == 5);
    REQUIRE(tween.isDone() == false);

    tween.update(ngf::TimeSpan::seconds(5));
    REQUIRE(lastValue == 10);
    REQUIRE(tween.isDone());
  }

  SECTION("setting a repeat changes the behavior of the tween when the interpolation is done") {
    tween.setDuration(ngf::TimeSpan::seconds(10))
        .onValueChanged([&lastValue](int value) { lastValue = value; })
        .repeat(1);
    REQUIRE(tween.isDone() == false);

    tween.update(ngf::TimeSpan::seconds(4));
    REQUIRE(lastValue == 4);
    REQUIRE(tween.isDone() == false);

    tween.update(ngf::TimeSpan::seconds(4));
    REQUIRE(lastValue == 8);
    REQUIRE(tween.isDone() == false);

    tween.update(ngf::TimeSpan::seconds(4));
    REQUIRE(lastValue == 2);
    REQUIRE(tween.isDone() == false);

    tween.update(ngf::TimeSpan::seconds(8));
    REQUIRE(lastValue == 10);
    REQUIRE(tween.isDone());
  }

  SECTION("setting a two way repeat changes the behavior of the tween when the interpolation is done") {
    tween.setDuration(ngf::TimeSpan::seconds(10))
        .onValueChanged([&lastValue](int value) { lastValue = value; })
        .repeat(1, ngf::RepeatBehavior::TwoWay);
    REQUIRE(tween.isDone() == false);

    tween.update(ngf::TimeSpan::seconds(4));
    REQUIRE(lastValue == 4);
    REQUIRE(tween.isDone() == false);

    tween.update(ngf::TimeSpan::seconds(4));
    REQUIRE(lastValue == 8);
    REQUIRE(tween.isDone() == false);

    tween.update(ngf::TimeSpan::seconds(4));
    REQUIRE(lastValue == 8);
    REQUIRE(tween.isDone() == false);

    tween.update(ngf::TimeSpan::seconds(8));
    REQUIRE(lastValue == 0);
    REQUIRE(tween.isDone());
  }
}