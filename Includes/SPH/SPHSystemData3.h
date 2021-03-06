/*************************************************************************
> File Name: SPHSystemData3.h
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: 3-D SPH particle system data.
> Created Time: 2017/05/31
> Copyright (c) 2017, Chan-Ho Chris Ohk
*************************************************************************/
#ifndef CUBBYFLOW_SPH_SYSTEM_DATA3_H
#define CUBBYFLOW_SPH_SYSTEM_DATA3_H

#include <Particle/ParticleSystemData3.h>

namespace CubbyFlow
{
	//!
	//! \brief      3-D SPH particle system data.
	//!
	//! This class extends ParticleSystemData3 to specialize the data model for SPH.
	//! It includes density and pressure array as a default particle attribute, and
	//! it also contains SPH utilities such as interpolation operator.
	//!
	class SPHSystemData3 : public ParticleSystemData3
	{
	public:
		//! Constructs empty SPH system.
		SPHSystemData3();

		//! Constructs SPH system data with given number of particles.
		explicit SPHSystemData3(size_t numberOfParticles);

		//! Copy constructor.
		SPHSystemData3(const SPHSystemData3& other);

		//! Destructor.
		virtual ~SPHSystemData3();

		//!
		//! \brief Sets the radius.
		//!
		//! Sets the radius of the particle system. The radius will be interpreted
		//! as target spacing.
		//!
		void SetRadius(double newRadius) override;

		//!
		//! \brief      Sets the mass of a particle.
		//!
		//! Setting the mass of a particle will change the target density.
		//!
		//! \param[in]  newMass The new mass.
		//!
		void SetMass(double newMass) override;

		//! Returns the density array accessor (immutable).
		ConstArrayAccessor1<double> GetDensities() const;

		//! Returns the density array accessor (mutable).
		ArrayAccessor1<double> GetDensities();

		//! Returns the pressure array accessor (immutable).
		ConstArrayAccessor1<double> GetPressures() const;

		//! Returns the pressure array accessor (mutable).
		ArrayAccessor1<double> GetPressures();

		//! Updates the density array with the latest particle positions.
		void UpdateDensities();

		//! Sets the target density of this particle system.
		void SetTargetDensity(double targetDensity);

		//! Returns the target density of this particle system.
		double GetTargetDensity() const;

		//!
		//! \brief Sets the target particle spacing in meters.
		//!
		//! Once this function is called, hash grid and density should be
		//! updated using updateHashGrid() and updateDensities).
		//!
		void SetTargetSpacing(double spacing);

		//! Returns the target particle spacing in meters.
		double GetTargetSpacing() const;

		//!
		//! \brief Sets the relative kernel radius.
		//!
		//! Sets the relative kernel radius compared to the target particle
		//! spacing (i.e. kernel radius / target spacing).
		//! Once this function is called, hash grid and density should
		//! be updated using updateHashGrid() and updateDensities).
		//!
		void SetRelativeKernelRadius(double relativeRadius);

		//!
		//! \brief Returns the relative kernel radius.
		//!
		//! Returns the relative kernel radius compared to the target particle
		//! spacing (i.e. kernel radius / target spacing).
		//!
		double GetRelativeKernelRadius() const;

		//! Returns the kernel radius in meters unit.
		double GetKernelRadius() const;

		//! Returns sum of kernel function evaluation for each nearby particle.
		double SumOfKernelNearby(const Vector3D& position) const;

		//!
		//! \brief Returns interpolated value at given origin point.
		//!
		//! Returns interpolated scalar data from the given position using
		//! standard SPH weighted average. The data array should match the
		//! particle layout. For example, density or pressure arrays can be
		//! used.
		//!
		double Interpolate(const Vector3D& origin, const ConstArrayAccessor1<double>& values) const;

		//!
		//! \brief Returns interpolated vector value at given origin point.
		//!
		//! Returns interpolated vector data from the given position using
		//! standard SPH weighted average. The data array should match the
		//! particle layout. For example, velocity or acceleration arrays can be
		//! used.
		//!
		Vector3D Interpolate(const Vector3D& origin, const ConstArrayAccessor1<Vector3D>& values) const;

		//! Returns the gradient of the given values at i-th particle.
		Vector3D GradientAt(size_t i, const ConstArrayAccessor1<double>& values) const;

		//! Returns the Laplacian of the given values at i-th particle.
		double LaplacianAt(size_t i, const ConstArrayAccessor1<double>& values) const;

		//! Returns the Laplacian of the given values at i-th particle.
		Vector3D LaplacianAt(size_t i, const ConstArrayAccessor1<Vector3D>& values) const;

		//! Builds neighbor searcher with kernel radius.
		void BuildNeighborSearcher();

		//! Builds neighbor lists with kernel radius.
		void BuildNeighborLists();

		//! Serializes this SPH system data to the buffer.
		void Serialize(std::vector<uint8_t>* buffer) const override;

		//! Deserializes this SPH system data from the buffer.
		void Deserialize(const std::vector<uint8_t>& buffer) override;

		//! Copies from other SPH system data.
		void Set(const SPHSystemData3& other);

		//! Copies from other SPH system data.
		SPHSystemData3& operator=(const SPHSystemData3& other);

	private:
		//! Target density of this particle system in kg/m^3.
		double m_targetDensity = WATER_DENSITY;

		//! Target spacing of this particle system in meters.
		double m_targetSpacing = 0.1;

		//! Relative radius of SPH kernel.
		//! SPH kernel radius divided by target spacing.
		double m_kernelRadiusOverTargetSpacing = 1.8;

		//! SPH kernel radius in meters.
		double m_kernelRadius;

		size_t m_pressureIdx;

		size_t m_densityIdx;

		//! Computes the mass based on the target density and spacing.
		void ComputeMass();
	};

	//! Shared pointer for the SPHSystemData3 type.
	using SPHSystemData3Ptr = std::shared_ptr<SPHSystemData3>;
}

#endif