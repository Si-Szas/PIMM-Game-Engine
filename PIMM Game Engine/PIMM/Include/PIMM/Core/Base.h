#pragma once
#include <PIMM/Core/Common.h>

namespace pimm 
{
	class Base
	{
		pimm_disable_copy_and_move(Base)
		public:
			//CONSTRUCTOR
				//Explicitly declare that descriptor is used in read only mode
			explicit Base(const BaseDescriptor& descriptor);

			//DESTRUCTOR
			virtual ~Base();
			virtual Logger& GetLogger() const noexcept final;
	
		protected:
			//Dependency injection, not singleton for logger
				//Pass the injection is a design pattern where you pass or inject an object dependencies from outside
			Logger& m_logger;
	};
}