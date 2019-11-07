#pragma once
#include "Level.h"
/* Putting all this object-oriented trash in one file so i can ignore it easily */

using Leap::Controller;
using Leap::Frame;

class SampleListener : public Leap::Listener
{
public:
	virtual void onConnect(const Controller&);
	virtual void onFrame(const Controller&);
};

void SampleListener::onConnect(const Controller& controller)
{
	return;
}

void SampleListener::onFrame(const Controller& controller)
{
	return;
}