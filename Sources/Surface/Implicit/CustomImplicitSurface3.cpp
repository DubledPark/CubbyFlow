/*************************************************************************
> File Name: CustomImplicitSurface3.cpp
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: Custom 3-D implicit surface using arbitrary function.
> Created Time: 2017/09/08
> Copyright (c) 2017, Chan-Ho Chris Ohk
*************************************************************************/
#include <LevelSet/LevelSetUtils.h>
#include <Surface/Implicit/CustomImplicitSurface3.h>

namespace CubbyFlow
{
	const double DISTANCE_THRESHOLD = 1e-3;
	const double GRADIENT_THRESHOLD = 1e-3;

	CustomImplicitSurface3::CustomImplicitSurface3(
		const std::function<double(const Vector3D&)>& func,
		const BoundingBox3D& domain,
		double resolution,
		const Transform3& transform,
		bool isNormalFlipped) :
		ImplicitSurface3(transform, isNormalFlipped), m_func(func), m_domain(domain), m_resolution(resolution)
	{
		// Do nothing
	}

	CustomImplicitSurface3::~CustomImplicitSurface3()
	{
		// Do nothing
	}

	Vector3D CustomImplicitSurface3::ClosestPointLocal(const Vector3D& otherPoint) const
	{
		Vector3D pt = otherPoint;

		while (std::fabs(m_func(pt)) < DISTANCE_THRESHOLD)
		{
			Vector3D g = GradientLocal(pt);

			if (g.Length() < GRADIENT_THRESHOLD)
			{
				break;
			}

			pt += g;
		}

		return pt;
	}

	bool CustomImplicitSurface3::IntersectsLocal(const Ray3D& ray) const
	{
		BoundingBoxRayIntersection3D intersection = m_domain.ClosestIntersection(ray);

		if (intersection.isIntersecting)
		{
			double start, end;
			if (intersection.far == std::numeric_limits<double>::max())
			{
				start = 0.0;
				end = intersection.near;
			}
			else
			{
				start = intersection.near;
				end = intersection.far;
			}

			double t = start;
			Vector3D pt = ray.PointAt(t);
			double prevSign = Sign(m_func(pt));

			while (t <= end)
			{
				pt = ray.PointAt(t);
				double newSign = Sign(m_func(pt));

				if (newSign * prevSign < 0.0)
				{
					return true;
				}

				t += m_resolution;
			}
		}

		return false;
	}

	BoundingBox3D CustomImplicitSurface3::BoundingBoxLocal() const
	{
		return m_domain;
	}

	double CustomImplicitSurface3::SignedDistanceLocal(const Vector3D& otherPoint) const
	{
		if (m_func)
		{
			return m_func(otherPoint);
		}
		
		return std::numeric_limits<double>::max();
	}

	Vector3D CustomImplicitSurface3::ClosestNormalLocal(const Vector3D& otherPoint) const
	{
		Vector3D pt = otherPoint;
		Vector3D g;

		while (std::fabs(m_func(pt)) < DISTANCE_THRESHOLD)
		{
			g = GradientLocal(pt);

			if (g.Length() < GRADIENT_THRESHOLD)
			{
				break;
			}

			pt += g;
		}

		if (g.Length() > 0.0)
		{
			g.Normalize();
		}

		return g;
	}

	SurfaceRayIntersection3 CustomImplicitSurface3::ClosestIntersectionLocal(const Ray3D& ray) const
	{
		SurfaceRayIntersection3 result;
		BoundingBoxRayIntersection3D intersection = m_domain.ClosestIntersection(ray);

		if (intersection.isIntersecting)
		{
			double start, end;
			if (intersection.far == std::numeric_limits<double>::max())
			{
				start = 0.0;
				end = intersection.near;
			}
			else
			{
				start = intersection.near;
				end = intersection.far;
			}

			double t = start;
			Vector3D pt = ray.PointAt(t);
			double prevPhi = m_func(pt);

			while (t <= end)
			{
				pt = ray.PointAt(t);
				double newPhi = m_func(pt);

				if (newPhi * prevPhi < 0.0)
				{
					double frac = FractionInsideSDF(prevPhi, newPhi);
					double tSub = t + m_resolution * frac;

					result.isIntersecting = true;
					result.t = tSub;
					result.point = ray.PointAt(tSub);
					result.normal = GradientLocal(result.point);

					if (result.normal.Length() > 0.0)
					{
						result.normal.Normalize();
					}

					return result;
				}

				t += m_resolution;
			}
		}

		return result;
	}

	Vector3D CustomImplicitSurface3::GradientLocal(const Vector3D& x) const
	{
		double left = m_func(x - Vector3D(0.5 * m_resolution, 0.0, 0.0));
		double right = m_func(x + Vector3D(0.5 * m_resolution, 0.0, 0.0));
		double bottom = m_func(x - Vector3D(0.0, 0.5 * m_resolution, 0.0));
		double top = m_func(x + Vector3D(0.0, 0.5 * m_resolution, 0.0));
		double back = m_func(x - Vector3D(0.0, 0.0, 0.5 * m_resolution));
		double front = m_func(x + Vector3D(0.0, 0.0, 0.5 * m_resolution));

		return Vector3D(
			(right - left) / m_resolution,
			(top - bottom) / m_resolution,
			(front - back) / m_resolution);
	}

	CustomImplicitSurface3::Builder CustomImplicitSurface3::GetBuilder()
	{
		return Builder();
	}

	CustomImplicitSurface3::Builder& CustomImplicitSurface3::Builder::WithSignedDistanceFunction(const std::function<double(const Vector3D&)>& func)
	{
		m_func = func;
		return *this;
	}

	CustomImplicitSurface3::Builder& CustomImplicitSurface3::Builder::WithDomain(const BoundingBox3D& domain)
	{
		m_domain = domain;
		return *this;
	}

	CustomImplicitSurface3::Builder& CustomImplicitSurface3::Builder::WithResolution(double resolution)
	{
		m_resolution = resolution;
		return *this;
	}

	CustomImplicitSurface3 CustomImplicitSurface3::Builder::Build() const
	{
		return CustomImplicitSurface3(m_func, m_domain, m_resolution, m_transform, m_isNormalFlipped);
	}

	CustomImplicitSurface3Ptr CustomImplicitSurface3::Builder::MakeShared() const
	{
		return std::shared_ptr<CustomImplicitSurface3>(
			new CustomImplicitSurface3(m_func, m_domain, m_resolution, m_transform, m_isNormalFlipped),
			[](CustomImplicitSurface3* obj)
		{
			delete obj;
		});
	}
}