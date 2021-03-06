/*************************************************************************
> File Name: FLIPSolver3.h
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: 3-D Fluid-Implicit Particle (FLIP) implementation.
> Created Time: 2017/09/13
> Copyright (c) 2017, Chan-Ho Chris Ohk
*************************************************************************/
#ifndef CUBBYFLOW_FLIP_SOLVER3_H
#define CUBBYFLOW_FLIP_SOLVER3_H

#include <Solver/PIC/PICSolver3.h>

namespace CubbyFlow
{
	//!
	//! \brief 3-D Fluid-Implicit Particle (FLIP) implementation.
	//!
	//! This class implements 3-D Fluid-Implicit Particle (FLIP) solver from the
	//! SIGGRAPH paper, Zhu and Bridson 2005. By transferring delta-velocity field
	//! from grid to particles, the FLIP solver achieves less viscous fluid flow
	//! compared to the original PIC method.
	//!
	//! \see Zhu, Yongning, and Robert Bridson. "Animating sand as a fluid."
	//!     ACM Transactions on Graphics (TOG). Vol. 24. No. 3. ACM, 2005.
	//!
	class FLIPSolver3 : public PICSolver3
	{
	public:
		class Builder;

		//! Default constructor.
		FLIPSolver3();

		//! Constructs solver with initial grid size.
		FLIPSolver3(
			const Size3& resolution,
			const Vector3D& gridSpacing,
			const Vector3D& gridOrigin);

		//! Default destructor.
		virtual ~FLIPSolver3();

		//! Returns builder fox FLIPSolver3.
		static Builder GetBuilder();

	protected:
		//! Transfers velocity field from particles to grids.
		void TransferFromParticlesToGrids() override;

		//! Transfers velocity field from grids to particles.
		void TransferFromGridsToParticles() override;

	private:
		Array3<float> m_uDelta;
		Array3<float> m_vDelta;
		Array3<float> m_wDelta;
	};

	//! Shared pointer type for the FLIPSolver3.
	using FLIPSolver3Ptr = std::shared_ptr<FLIPSolver3>;
	
	//!
	//! \brief Front-end to create FLIPSolver3 objects step by step.
	//!
	class FLIPSolver3::Builder final : public GridFluidSolverBuilderBase3<FLIPSolver3::Builder>
	{
	public:
		//! Builds FLIPSolver3.
		FLIPSolver3 Build() const;

		//! Builds shared pointer of FLIPSolver3 instance.
		FLIPSolver3Ptr MakeShared() const;
	};
}

#endif