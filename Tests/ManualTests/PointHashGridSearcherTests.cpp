#include "pch.h"

#include <ManualTests.h>

#include <Array/Array2.h>
#include <BoundingBox/BoundingBox2.h>
#include <BoundingBox/BoundingBox3.h>
#include <PointGenerator/BccLatticePointGenerator.h>
#include <PointGenerator/TrianglePointGenerator.h>
#include <Searcher/PointHashGridSearcher2.h>
#include <Searcher/PointHashGridSearcher3.h>
#include <Searcher/PointParallelHashGridSearcher2.h>
#include <Searcher/PointParallelHashGridSearcher3.h>

using namespace CubbyFlow;

CUBBYFLOW_TESTS(PointHashGridSearcher2);

CUBBYFLOW_BEGIN_TEST_F(PointHashGridSearcher2, Build)
{
	Array1<Vector2D> points;
	TrianglePointGenerator pointsGenerator;
	BoundingBox2D bbox(Vector2D(0, 0), Vector2D(1, 1));
	double spacing = 0.1;

	pointsGenerator.Generate(bbox, spacing, &points);

	PointHashGridSearcher2 pointSearcher(4, 4, 0.18);
	pointSearcher.Build(ArrayAccessor1<Vector2D>(points.Size(), points.Data()));

	Array2<double> grid(4, 4, 0.0);

	for (size_t j = 0; j < grid.Size().y; ++j)
	{
		for (size_t i = 0; i < grid.Size().x; ++i)
		{
			size_t key = pointSearcher.GetHashKeyFromBucketIndex(Point2I(static_cast<ssize_t>(i), static_cast<ssize_t>(j)));
			size_t value = pointSearcher.GetBuckets()[key].size();
			grid(i, j) += static_cast<double>(value);
		}
	}

	SaveData(grid.ConstAccessor(), "Data_#grid2.npy");
}
CUBBYFLOW_END_TEST_F

CUBBYFLOW_TESTS(PointHashGridSearcher3);

CUBBYFLOW_BEGIN_TEST_F(PointHashGridSearcher3, Build)
{
	Array1<Vector3D> points;
	BccLatticePointGenerator pointsGenerator;
	BoundingBox3D bbox(Vector3D(0, 0, 0), Vector3D(1, 1, 1));
	double spacing = 0.1;

	pointsGenerator.Generate(bbox, spacing, &points);

	PointHashGridSearcher3 pointSearcher(4, 4, 4, 0.18);
	pointSearcher.Build(ArrayAccessor1<Vector3D>(points.Size(), points.Data()));

	Array2<double> grid(4, 4, 0.0);

	for (size_t j = 0; j < grid.Size().y; ++j)
	{
		for (size_t i = 0; i < grid.Size().x; ++i)
		{
			size_t key = pointSearcher.GetHashKeyFromBucketIndex(Point3I(static_cast<ssize_t>(i), static_cast<ssize_t>(j), 0));
			size_t value = pointSearcher.GetBuckets()[key].size();
			grid(i, j) += static_cast<double>(value);
		}
	}

	SaveData(grid.ConstAccessor(), "Data_#grid2.npy");
}
CUBBYFLOW_END_TEST_F

CUBBYFLOW_TESTS(PointParallelHashGridSearcher2);

CUBBYFLOW_BEGIN_TEST_F(PointParallelHashGridSearcher2, Build)
{
	Array1<Vector2D> points;
	TrianglePointGenerator pointsGenerator;
	BoundingBox2D bbox(
		Vector2D(0, 0),
		Vector2D(1, 1));
	double spacing = 0.1;

	pointsGenerator.Generate(bbox, spacing, &points);

	PointParallelHashGridSearcher2 pointSearcher(4, 4, 0.18);
	pointSearcher.Build(ArrayAccessor1<Vector2D>(points.Size(), points.Data()));

	Array2<double> grid(4, 4, 0.0);

	for (size_t j = 0; j < grid.Size().y; ++j)
	{
		for (size_t i = 0; i < grid.Size().x; ++i)
		{
			size_t key = pointSearcher.GetHashKeyFromBucketIndex(Point2I(static_cast<ssize_t>(i), static_cast<ssize_t>(j)));
			size_t start = pointSearcher.StartIndexTable()[key];
			size_t end = pointSearcher.EndIndexTable()[key];
			size_t value = end - start;
			grid(i, j) += static_cast<double>(value);
		}
	}

	SaveData(grid.ConstAccessor(), "Data_#grid2.npy");
}
CUBBYFLOW_END_TEST_F

CUBBYFLOW_TESTS(PointParallelHashGridSearcher3);

CUBBYFLOW_BEGIN_TEST_F(PointParallelHashGridSearcher3, Build)
{
	Array1<Vector3D> points;
	BccLatticePointGenerator pointsGenerator;
	BoundingBox3D bbox(Vector3D(0, 0, 0), Vector3D(1, 1, 1));
	double spacing = 0.1;

	pointsGenerator.Generate(bbox, spacing, &points);

	PointParallelHashGridSearcher3 pointSearcher(4, 4, 4, 0.18);
	pointSearcher.Build(ArrayAccessor1<Vector3D>(points.Size(), points.Data()));

	Array2<double> grid(4, 4, 0.0);

	for (size_t j = 0; j < grid.Size().y; ++j)
	{
		for (size_t i = 0; i < grid.Size().x; ++i)
		{
			size_t key = pointSearcher.GetHashKeyFromBucketIndex(Point3I(static_cast<ssize_t>(i), static_cast<ssize_t>(j), 0));
			size_t start = pointSearcher.StartIndexTable()[key];
			size_t end = pointSearcher.EndIndexTable()[key];
			size_t value = end - start;
			grid(i, j) += static_cast<double>(value);
		}
	}

	SaveData(grid.ConstAccessor(), "Data_#grid2.npy");
}
CUBBYFLOW_END_TEST_F