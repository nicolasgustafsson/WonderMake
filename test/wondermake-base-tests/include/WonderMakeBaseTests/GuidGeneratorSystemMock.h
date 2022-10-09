#pragma once

#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-base/GuidGeneratorSystem.h"

class GuidGeneratorSystemMock
	: public GuidGeneratorSystem
{
public:
	MOCK_METHOD(std::optional<Guid>, GenerateNew, (), (override));

	void DelegateToFake()
	{
		ON_CALL(*this, GenerateNew)
			.WillByDefault([this]()
				{
					std::array<u8, sizeof(Guid)> dataArr = { 0 };

					(void)std::memcpy(static_cast<void*>(dataArr.data()), static_cast<void*>(&myNextGuid), sizeof(myNextGuid));

					++myNextGuid;

					return Guid(std::move(dataArr));
				});
	}

private:
	u64 myNextGuid = 1;

};
