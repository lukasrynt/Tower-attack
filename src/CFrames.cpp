#include "CFrames.hpp"

CFrames::CFrames(int speed)
	: m_Speed(speed),
	  m_CurrFrame(0)
{}

bool CFrames::ActionAllowed()
{
	if (m_CurrFrame++ != MAX_FRAME_CNT / m_Speed)
		return false;
	m_CurrFrame = 0;
	return true;
}