/*************************************************************************
> File Name: Plane3.h
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: 3-D plane geometry.
> Created Time: 2017/06/18
> Copyright (c) 2017, Chan-Ho Chris Ohk
*************************************************************************/
#ifndef CUBBYFLOW_PLANE3_H
#define CUBBYFLOW_PLANE3_H

#include <Surface/Surface3.h>

namespace CubbyFlow
{
	//!
	//! \brief 3-D plane geometry.
	//!
	//! This class represents 3-D plane geometry which extends Surface3 by
	//! overriding surface-related queries.
	//!
	class Plane3 final : public Surface3
	{
	public:
		class Builder;

		//! Plane normal.
		Vector3D normal = Vector3D(0, 1, 0);

		//! Point that lies on the plane.
		Vector3D point;

		//! Constructs a plane that crosses (0, 0, 0) with surface normal (0, 1, 0).
		Plane3(const Transform3& transform = Transform3(), bool isNormalFlipped = false);

		//! Constructs a plane that cross \p point with surface normal \p normal.
		Plane3(const Vector3D& normal, const Vector3D& point,
			const Transform3& transform = Transform3(), bool isNormalFlipped = false);

		//! Constructs a plane with three points on the surface. The normal will be
		//! set using the counter clockwise direction.
		Plane3(const Vector3D& point0, const Vector3D& point1, const Vector3D& point2,
			const Transform3& transform = Transform3(), bool isNormalFlipped = false);

		//! Copy constructor.
		Plane3(const Plane3& other);

		//! Returns builder fox Plane3.
		static Builder GetBuilder();

	protected:
		Vector3D ClosestPointLocal(const Vector3D& otherPoint) const override;

		bool IntersectsLocal(const Ray3D& ray) const override;

		BoundingBox3D BoundingBoxLocal() const override;

		Vector3D ClosestNormalLocal(const Vector3D& otherPoint) const override;

		SurfaceRayIntersection3 ClosestIntersectionLocal(const Ray3D& ray) const override;
	};

	//! Shared pointer for the Plane3 type.
	using Plane3Ptr = std::shared_ptr<Plane3>;
	
	//!
	//! \brief Front-end to create Plane3 objects step by step.
	//!
	class Plane3::Builder final : public SurfaceBuilderBase3<Plane3::Builder>
	{
	public:
		//! Returns builder with plane normal.
		Builder& WithNormal(const Vector3D& normal);

		//! Returns builder with point on the plane.
		Builder& WithPoint(const Vector3D& point);

		//! Builds Plane3.
		Plane3 Build() const;

		//! Builds shared pointer of Plane3 instance.
		Plane3Ptr MakeShared() const;

	private:
		Vector3D m_normal{ 0, 1, 0 };
		Vector3D m_point{ 0, 0, 0 };
	};
}

#endif