/*************************************************************************
> File Name: GridFractionalBoundaryConditionSolver3.cpp
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: Fractional 3-D boundary condition solver for grids.
> Created Time: 2017/08/09
> Copyright (c) 2017, Chan-Ho Chris Ohk
*************************************************************************/
#include <Array/ArrayUtils.h>
#include <LevelSet/LevelSetUtils.h>
#include <Solver/Grid/GridFractionalBoundaryConditionSolver3.h>
#include <Surface/Implicit/ImplicitSurface3.h>
#include <Surface/Implicit/SurfaceToImplicit3.h>
#include <Utils/PhysicsHelpers.h>

namespace CubbyFlow
{
	GridFractionalBoundaryConditionSolver3::GridFractionalBoundaryConditionSolver3()
	{
		// Do nothing
	}

	GridFractionalBoundaryConditionSolver3::~GridFractionalBoundaryConditionSolver3()
	{
		// Do nothing
	}

	void GridFractionalBoundaryConditionSolver3::ConstrainVelocity(FaceCenteredGrid3* velocity, unsigned int extrapolationDepth)
	{
		Size3 size = velocity->Resolution();

		if (m_colliderSDF == nullptr || m_colliderSDF->Resolution() != size)
		{
			UpdateCollider(GetCollider(), size, velocity->GridSpacing(), velocity->Origin());
		}

		auto u = velocity->GetUAccessor();
		auto v = velocity->GetVAccessor();
		auto w = velocity->GetWAccessor();
		auto uPos = velocity->GetUPosition();
		auto vPos = velocity->GetVPosition();
		auto wPos = velocity->GetWPosition();

		Array3<double> uTemp(u.Size());
		Array3<double> vTemp(v.Size());
		Array3<double> wTemp(w.Size());
		Array3<char> uMarker(u.Size(), 1);
		Array3<char> vMarker(v.Size(), 1);
		Array3<char> wMarker(w.Size(), 1);

		Vector3D h = velocity->GridSpacing();

		// Assign collider's velocity first and initialize markers
		velocity->ParallelForEachUIndex([&](size_t i, size_t j, size_t k)
		{
			Vector3D pt = uPos(i, j, k);
			double phi0 = m_colliderSDF->Sample(pt - Vector3D(0.5 * h.x, 0.0, 0.0));
			double phi1 = m_colliderSDF->Sample(pt + Vector3D(0.5 * h.x, 0.0, 0.0));
			double frac = FractionInsideSDF(phi0, phi1);
			frac = 1.0 - std::clamp(frac, 0.0, 1.0);

			if (frac > 0.0)
			{
				uMarker(i, j, k) = 1;
			}
			else
			{
				Vector3D colliderVel = GetCollider()->VelocityAt(pt);
				u(i, j, k) = colliderVel.x;
				uMarker(i, j, k) = 0;
			}
		});

		velocity->ParallelForEachVIndex([&](size_t i, size_t j, size_t k)
		{
			Vector3D pt = vPos(i, j, k);
			double phi0 = m_colliderSDF->Sample(pt - Vector3D(0.0, 0.5 * h.y, 0.0));
			double phi1 = m_colliderSDF->Sample(pt + Vector3D(0.0, 0.5 * h.y, 0.0));
			double frac = FractionInsideSDF(phi0, phi1);
			frac = 1.0 - std::clamp(frac, 0.0, 1.0);

			if (frac > 0.0)
			{
				vMarker(i, j, k) = 1;
			}
			else
			{
				Vector3D colliderVel = GetCollider()->VelocityAt(pt);
				v(i, j, k) = colliderVel.y;
				vMarker(i, j, k) = 0;
			}
		});

		velocity->ParallelForEachWIndex([&](size_t i, size_t j, size_t k)
		{
			Vector3D pt = wPos(i, j, k);
			double phi0 = m_colliderSDF->Sample(pt - Vector3D(0.0, 0.0, 0.5 * h.z));
			double phi1 = m_colliderSDF->Sample(pt + Vector3D(0.0, 0.0, 0.5 * h.z));
			double frac = FractionInsideSDF(phi0, phi1);
			frac = 1.0 - std::clamp(frac, 0.0, 1.0);

			if (frac > 0.0)
			{
				wMarker(i, j, k) = 1;
			}
			else
			{
				Vector3D colliderVel = GetCollider()->VelocityAt(pt);
				w(i, j, k) = colliderVel.z;
				wMarker(i, j, k) = 0;
			}
		});

		// Free-slip: Extrapolate fluid velocity into the collider
		ExtrapolateToRegion(velocity->GetUConstAccessor(), uMarker, extrapolationDepth, u);
		ExtrapolateToRegion(velocity->GetVConstAccessor(), vMarker, extrapolationDepth, v);
		ExtrapolateToRegion(velocity->GetWConstAccessor(), wMarker, extrapolationDepth, w);

		// No-flux: project the extrapolated velocity to the collider's surface normal
		velocity->ParallelForEachUIndex([&](size_t i, size_t j, size_t k)
		{
			Vector3D pt = uPos(i, j, k);

			if (IsInsideSDF(m_colliderSDF->Sample(pt)))
			{
				Vector3D colliderVel = GetCollider()->VelocityAt(pt);
				Vector3D vel = velocity->Sample(pt);
				Vector3D g = m_colliderSDF->Gradient(pt);

				if (g.LengthSquared() > 0.0)
				{
					Vector3D n = g.Normalized();
					Vector3D velr = vel - colliderVel;
					Vector3D velt = ProjectAndApplyFriction(velr, n, GetCollider()->FrictionCoefficient());
					Vector3D velp = velt + colliderVel;

					uTemp(i, j, k) = velp.x;
				}
				else
				{
					uTemp(i, j, k) = colliderVel.x;
				}
			}
			else
			{
				uTemp(i, j, k) = u(i, j, k);
			}
		});

		velocity->ParallelForEachVIndex([&](size_t i, size_t j, size_t k)
		{
			Vector3D pt = vPos(i, j, k);

			if (IsInsideSDF(m_colliderSDF->Sample(pt)))
			{
				Vector3D colliderVel = GetCollider()->VelocityAt(pt);
				Vector3D vel = velocity->Sample(pt);
				Vector3D g = m_colliderSDF->Gradient(pt);

				if (g.LengthSquared() > 0.0)
				{
					Vector3D n = g.Normalized();
					Vector3D velr = vel - colliderVel;
					Vector3D velt = ProjectAndApplyFriction(velr, n, GetCollider()->FrictionCoefficient());
					Vector3D velp = velt + colliderVel;

					vTemp(i, j, k) = velp.y;
				}
				else
				{
					vTemp(i, j, k) = colliderVel.y;
				}
			}
			else
			{
				vTemp(i, j, k) = v(i, j, k);
			}
		});

		velocity->ParallelForEachWIndex([&](size_t i, size_t j, size_t k)
		{
			Vector3D pt = wPos(i, j, k);

			if (IsInsideSDF(m_colliderSDF->Sample(pt)))
			{
				Vector3D colliderVel = GetCollider()->VelocityAt(pt);
				Vector3D vel = velocity->Sample(pt);
				Vector3D g = m_colliderSDF->Gradient(pt);

				if (g.LengthSquared() > 0.0)
				{
					Vector3D n = g.Normalized();
					Vector3D velr = vel - colliderVel;
					Vector3D velt = ProjectAndApplyFriction(velr, n, GetCollider()->FrictionCoefficient());
					Vector3D velp = velt + colliderVel;

					wTemp(i, j, k) = velp.z;
				}
				else
				{
					wTemp(i, j, k) = colliderVel.z;
				}
			}
			else
			{
				wTemp(i, j, k) = w(i, j, k);
			}
		});

		// Transfer results
		u.ParallelForEachIndex([&](size_t i, size_t j, size_t k)
		{
			u(i, j, k) = uTemp(i, j, k);
		});
		v.ParallelForEachIndex([&](size_t i, size_t j, size_t k)
		{
			v(i, j, k) = vTemp(i, j, k);
		});
		w.ParallelForEachIndex([&](size_t i, size_t j, size_t k)
		{
			w(i, j, k) = wTemp(i, j, k);
		});

		// No-flux: Project velocity on the domain boundary if closed
		if (GetClosedDomainBoundaryFlag() & DIRECTION_LEFT)
		{
			for (size_t k = 0; k < u.Size().z; ++k)
			{
				for (size_t j = 0; j < u.Size().y; ++j)
				{
					u(0, j, k) = 0;
				}
			}
		}
		if (GetClosedDomainBoundaryFlag() & DIRECTION_RIGHT)
		{
			for (size_t k = 0; k < u.Size().z; ++k)
			{
				for (size_t j = 0; j < u.Size().y; ++j)
				{
					u(u.Size().x - 1, j, k) = 0;
				}
			}
		}
		if (GetClosedDomainBoundaryFlag() & DIRECTION_DOWN)
		{
			for (size_t k = 0; k < v.Size().z; ++k)
			{
				for (size_t i = 0; i < v.Size().x; ++i)
				{
					v(i, 0, k) = 0;
				}
			}
		}
		if (GetClosedDomainBoundaryFlag() & DIRECTION_UP)
		{
			for (size_t k = 0; k < v.Size().z; ++k)
			{
				for (size_t i = 0; i < v.Size().x; ++i)
				{
					v(i, v.Size().y - 1, k) = 0;
				}
			}
		}
		if (GetClosedDomainBoundaryFlag() & DIRECTION_BACK)
		{
			for (size_t j = 0; j < w.Size().y; ++j)
			{
				for (size_t i = 0; i < w.Size().x; ++i)
				{
					w(i, j, 0) = 0;
				}
			}
		}
		if (GetClosedDomainBoundaryFlag() & DIRECTION_FRONT)
		{
			for (size_t j = 0; j < w.Size().y; ++j)
			{
				for (size_t i = 0; i < w.Size().x; ++i)
				{
					w(i, j, w.Size().z - 1) = 0;
				}
			}
		}
	}

	ScalarField3Ptr GridFractionalBoundaryConditionSolver3::ColliderSDF() const
	{
		return m_colliderSDF;
	}

	VectorField3Ptr GridFractionalBoundaryConditionSolver3::ColliderVelocityField() const
	{
		return m_colliderVel;
	}

	void GridFractionalBoundaryConditionSolver3::OnColliderUpdated(
		const Size3& gridSize,
		const Vector3D& GridSpacing,
		const Vector3D& gridOrigin)
	{
		if (m_colliderSDF == nullptr)
		{
			m_colliderSDF = std::make_shared<CellCenteredScalarGrid3>();
		}

		m_colliderSDF->Resize(gridSize, GridSpacing, gridOrigin);

		if (GetCollider() != nullptr)
		{
			Surface3Ptr surface = GetCollider()->Surface();
			ImplicitSurface3Ptr implicitSurface = std::dynamic_pointer_cast<ImplicitSurface3>(surface);
			if (implicitSurface == nullptr)
			{
				implicitSurface = std::make_shared<SurfaceToImplicit3>(surface);
			}

			m_colliderSDF->Fill([&](const Vector3D& pt)
			{
				return implicitSurface->SignedDistance(pt);
			});

			m_colliderVel = CustomVectorField3::Builder()
				.WithFunction([&](const Vector3D& x)
			{
				return GetCollider()->VelocityAt(x);
			})
				.WithDerivativeResolution(GridSpacing.x)
				.MakeShared();
		}
		else
		{
			m_colliderSDF->Fill(std::numeric_limits<double>::max());

			m_colliderVel = CustomVectorField3::Builder()
				.WithFunction([](const Vector3D& x)
			{
				return Vector3D();
			})
				.WithDerivativeResolution(GridSpacing.x)
				.MakeShared();
		}
	}
}