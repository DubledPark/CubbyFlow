/*************************************************************************
> File Name: ParticleEmitter2.cpp
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: Abstract base class for 2-D particle emitter.
> Created Time: 2017/05/19
> Copyright (c) 2017, Chan-Ho Chris Ohk
*************************************************************************/
#include <Emitter/ParticleEmitter2.h>

namespace CubbyFlow
{
	ParticleEmitter2::ParticleEmitter2()
	{
		// Do nothing
	}

	ParticleEmitter2::~ParticleEmitter2()
	{
		// Do nothing
	}

	const ParticleSystemData2Ptr& ParticleEmitter2::Target() const
	{
		return m_particles;
	}

	void ParticleEmitter2::SetTarget(const ParticleSystemData2Ptr& particles)
	{
		m_particles = particles;

		OnSetTarget(particles);
	}

	void ParticleEmitter2::Update(double currentTimeInSeconds, double timeIntervalInSeconds)
	{
		if (m_onBeginUpdateCallback)
		{
			m_onBeginUpdateCallback(this, currentTimeInSeconds, timeIntervalInSeconds);
		}

		OnUpdate(currentTimeInSeconds, timeIntervalInSeconds);
	}

	// TODO: particles is unused variable.
	void ParticleEmitter2::OnSetTarget(const ParticleSystemData2Ptr& particles)
	{
		// Do nothing
	}

	void ParticleEmitter2::SetOnBeginUpdateCallback(const OnBeginUpdateCallback& callback)
	{
		m_onBeginUpdateCallback = callback;
	}
}