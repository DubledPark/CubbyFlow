/*************************************************************************
> File Name: FDMGaussSeidelSolver2.h
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: 2-D finite difference-type linear system solver using Gauss-Seidel method.
> Created Time: 2017/08/17
> Copyright (c) 2017, Chan-Ho Chris Ohk
*************************************************************************/
#ifndef CUBBYFLOW_FDM_GAUSS_SEIDEL_SOLVER2_H
#define CUBBYFLOW_FDM_GAUSS_SEIDEL_SOLVER2_H

#include <Solver/FDM/FDMLinearSystemSolver2.h>

namespace CubbyFlow
{
	//! \brief 2-D finite difference-type linear system solver using Gauss-Seidel method.
	class FDMGaussSeidelSolver2 final : public FDMLinearSystemSolver2
	{
	public:
		//! Constructs the solver with given parameters.
		FDMGaussSeidelSolver2(
			unsigned int maxNumberOfIterations,
			unsigned int residualCheckInterval,
			double tolerance);

		//! Solves the given linear system.
		bool Solve(FDMLinearSystem2* system) override;

		//! Returns the max number of Gauss-Seidel iterations.
		unsigned int GetMaxNumberOfIterations() const;

		//! Returns the last number of Gauss-Seidel iterations the solver made.
		unsigned int GetLastNumberOfIterations() const;

		//! Returns the max residual tolerance for the Gauss-Seidel method.
		double GetTolerance() const;

		//! Returns the last residual after the Gauss-Seidel iterations.
		double GetLastResidual() const;

	private:
		unsigned int m_maxNumberOfIterations;
		unsigned int m_lastNumberOfIterations;
		unsigned int m_residualCheckInterval;
		double m_tolerance;
		double m_lastResidual;

		FDMVector2 m_residual;

		void Relax(FDMLinearSystem2* system);
	};

	//! Shared pointer type for the FDMGaussSeidelSolver2.
	using FDMGaussSeidelSolver2Ptr = std::shared_ptr<FDMGaussSeidelSolver2>;
}

#endif