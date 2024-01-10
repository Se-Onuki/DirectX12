#include "Component.hpp"
//#include "Component.hpp"
////
////
////Component::Component() {}
////
////Component::~Component() {}
////
////
////
////Transform::Transform() {}
////
////Transform::~Transform() {}

SoLib::Color::RGB4 ECS::ColorLarp::EaseColor(float t) {

	return SoLib::Lerp(start_, end_, easeFunc_(t));
}
