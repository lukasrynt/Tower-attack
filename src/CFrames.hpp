#pragma once

class CFrames
{
public:
	explicit CFrames(int speed);
	bool ActionAllowed();
private:
	int m_Speed;
	int m_CurrFrame;
	static const int MAX_FRAME_CNT = 60;
};