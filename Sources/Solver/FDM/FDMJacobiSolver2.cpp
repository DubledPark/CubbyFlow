/*************************************************************************
> File Name: FDMJacobiSolver2.cpp
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: 2-D finite difference-type linear system solver using Jacobi method.
> Created Time: 2017/08/17
> Copyright (c) 2017, Chan-Ho Chris Ohk
*************************************************************************/
#include <Solver/FDM/FDMJacobiSolver2.h>

namespace CubbyFlow
{
	FDMJacobiSolver2::FDMJacobiSolver2(
		unsigned int maxNumberOfIterations,
		unsigned int residualCheckInterval,
		double tolerance) :
		m_maxNumberOfIterations(maxNumberOfIterations),
		m_lastNumberOfIterations(0),
		m_residualCheckInterval(residualCheckInterval),
		m_tolerance(tolerance),
		m_lastResidual(std::numeric_limits<double>::max())
	{
		// Do nothing
	}

	bool FDMJacobiSolver2::Solve(FDMLinearSystem2* system)
	{
		m_xTemp.Resize(system->x.Size());
		m_residual.Resize(system->x.Size());

		m_lastNumberOfIterations = m_maxNumberOfIterations;

		for (unsigned int iter = 0; iter < m_maxNumberOfIterations; ++iter)
		{
			Relax(system, &m_xTemp);
			m_xTemp.Swap(system->x);

			if (iter != 0 && iter % m_residualCheckInterval == 0)
			{
				FDMBlas2::Residual(system->A, system->x, system->b, &m_residual);

				if (FDMBlas2::L2Norm(m_residual) < m_tolerance)
				{
					m_lastNumberOfIterations = iter + 1;
					break;
				}
			}
		}

		FDMBlas2::Residual(system->A, system->x, system->b, &m_residual);
		m_lastResidual = FDMBlas2::L2Norm(m_residual);

		return m_lastResidual < m_tolerance;
	}

	unsigned int FDMJacobiSolver2::GetMaxNumberOfIterations() const 
	{
		return m_maxNumberOfIterations;
	}

	unsigned int FDMJacobiSolver2::GetLastNumberOfIterations() const
	{
		return m_lastNumberOfIterations;
	}

	double FDMJacobiSolver2::GetTolerance() const
	{
		return m_tolerance;
	}

	double FDMJacobiSolver2::GetLastResidual() const
	{
		return m_lastResidual;
	}

	void FDMJacobiSolver2::Relax(FDMLinearSystem2* system, FDMVector2* xTemp)
	{
		Size2 size = system->x.Size();
		FDMMatrix2& A = system->A;
		FDMVector2& x = system->x;
		FDMVector2& b = system->b;

		A.ParallelForEachIndex([&](size_t i, size_t j)
		{
			double r =
				((i > 0) ? A(i - 1, j).right * x(i - 1, j) : 0.0) +
				((i + 1 < size.x) ? A(i, j).right * x(i + 1, j) : 0.0) +
				((j > 0) ? A(i, j - 1).up * x(i, j - 1) : 0.0) +
				((j + 1 < size.y) ? A(i, j).up * x(i, j + 1) : 0.0);

			(*xTemp)(i, j) = (b(i, j) - r) / A(i, j).center;
		});
	}
}