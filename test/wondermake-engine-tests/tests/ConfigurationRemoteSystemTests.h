#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-engine/ConfigurationRemoteSystem.h"

#include "WonderMakeBaseTests/ProcessSystemMock.h"

#include "wondermake-base/ConfigurationSystem.h"

inline auto MakeConfigurationRemoteSystem(std::optional<Guid> aInstanceId = std::nullopt) -> std::tuple<SharedReference<ConfigurationSystem>, SharedReference<NiceMock<ProcessSystemMock>>, SharedReference<ConfigurationRemoteSystem>>
{
	ConfigurationSystem::InjectDependencies(std::tie());

	auto configSys = MakeSharedReference<ConfigurationSystem>();
	auto processSys = MakeSharedReference<NiceMock<ProcessSystemMock>>();

	configSys->Initialize();

	if (aInstanceId)
		EXPECT_CALL(*processSys, GetCurrentProcessId)
		.WillOnce(Return(*aInstanceId))
		.WillRepeatedly(DoDefault());

	ConfigurationRemoteSystem::InjectDependencies(std::tie(*configSys, *processSys));

	auto configRemoteSys = MakeSharedReference<ConfigurationRemoteSystem>();

	configRemoteSys->Initialize();

	return { std::move(configSys), std::move(processSys), std::move(configRemoteSys) };
}
