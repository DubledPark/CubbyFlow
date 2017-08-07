/*************************************************************************
> File Name: GridPointGenerator2.cpp
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: 2-D regular-grid point generator.
> Created Time: 2017/08/07
> Copyright (c) 2017, Chan-Ho Chris Ohk
*************************************************************************/
#include <PointGenerator/GridPointGenerator2.h>

namespace CubbyFlow
{
	void GridPointGenerator2::ForEachPoint(
		const BoundingBox2D& boundingBox,
		double spacing,
		const std::function<bool(const Vector2D&)>& callback) const
	{
		Vector2D position;
		double boxWidth = boundingBox.Width();
		double boxHeight = boundingBox.Height();
		bool shouldQuit = false;
		
		for (int j = 0; j * spacing <= boxHeight && !shouldQuit; ++j)
		{
			position.y = j * spacing + boundingBox.lowerCorner.y;

			for (int i = 0; i * spacing <= boxWidth && !shouldQuit; ++i)
			{
				position.x = i * spacing + boundingBox.lowerCorner.x;
			
				if (!callback(position))
				{
					shouldQuit = true;
					break;
				}
			}
		}
	}
}