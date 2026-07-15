#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/Core/Base.h>
#include <type_traits>

#define pimm_typeid(Class) \
public:\
using type_id_tag = Class;\
static size_t getTypeId()\
{\
	static const auto id = typeid(Class).hash_code();\
	return id;\
}\
size_t GetTypeID() const noexcept override\
{\
	return getTypeId();\
}

namespace pimm
{
	class Identifier : public Base
	{
		public:
			explicit Identifier(const BaseDescriptor& descriptor) :
				Base(descriptor) { }

			virtual size_t GetTypeID() const noexcept = 0;
	};

	template <typename Type>
	concept HasTypeID = std::is_same_v<typename Type::type_id_tag, Type>;

	template <typename Base, typename Derived>
	//Checks the object if it is registered if it is derived from base and has a type ID
	concept IsRegistered = std::is_base_of_v<Base, Derived>&& HasTypeID<Derived>;
}