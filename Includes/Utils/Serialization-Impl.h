/*************************************************************************
> File Name: Serialization-Impl.h
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: Abstract base class for any serializable class.
> Created Time: 2017/04/28
> Copyright (c) 2017, Chan-Ho Chris Ohk
*************************************************************************/
#ifndef CUBBYFLOW_SERIALIZATION_IMPL_H
#define CUBBYFLOW_SERIALIZATION_IMPL_H

namespace CubbyFlow
{
	template <typename T>
	void Serialize(const Array1<T>& array, std::vector<uint8_t>* buffer)
	{
		size_t size = sizeof(T) * array.Size();
		Serialize(reinterpret_cast<const uint8_t*>(array.Data()), size, buffer);
	}

	template <typename T>
	void Deserialize(const std::vector<uint8_t>& buffer, Array1<T>* array)
	{
		std::vector<uint8_t> data;
		Deserialize(buffer, &data);
		array->Resize(data.size() / sizeof(T));
		memcpy(array->Data(), data.data(), data.size());
	}
}

#endif