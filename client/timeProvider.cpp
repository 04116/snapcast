/***
    This file is part of snapcast
    Copyright (C) 2014-2017  Johannes Pohl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***/

#include "timeProvider.h"
#include "common/log.h"
#include "common/timeDefs.h"

TimeProvider::TimeProvider() : diffToServer_(0)
{
	diffBuffer_.setSize(200);
}


void TimeProvider::setDiff(const tv& c2s, const tv& s2c)
{
	tv latency = c2s - s2c;
	double diff = latency.sec * 1000. + latency.usec / 1000.;
	setDiffToServer(diff / 2.);
}


void TimeProvider::setDiffToServer(double ms)
{
	static int32_t lastTimeSync = 0;
	auto nowSeconds = std::chrono::duration_cast<chronos::sec>(chronos::clk::now().time_since_epoch()).count();

	/// clear diffBuffer if last update is older than a minute
	if (!diffBuffer_.empty() && (std::abs(nowSeconds - lastTimeSync) > 60))
	{
		LOG(INFO) << "Last time sync older than a minute. Clearing time buffer\n";
		diffToServer_ = ms*1000;
		diffBuffer_.clear();
	}
	lastTimeSync = nowSeconds;

	diffBuffer_.add(ms*1000);
	diffToServer_ = diffBuffer_.median(3);
//	LOG(INFO) << "setDiffToServer: " << ms << ", diff: " << diffToServer_ / 1000.f << "\n";
}

/*
long TimeProvider::getPercentileDiffToServer(size_t percentile)
{
	return diffBuffer.percentile(percentile);
}
*/

