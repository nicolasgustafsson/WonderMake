#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-engine/ConfigurationRemoteSystem.h"

#include "WonderMakeBaseTests/GuidGeneratorSystemMock.h"

#include "wondermake-base/ConfigurationSystem.h"

inline auto MakeConfigurationRemoteSystem(std::optional<Guid> aInstanceId = std::nullopt) -> std::tuple<SharedReference<ConfigurationSystem>, SharedReference<NiceMock<GuidGeneratorSystemMock>>, SharedReference<ConfigurationRemoteSystem>>
{
	ConfigurationSystem::InjectDependencies(std::tie());

	auto configSys = MakeSharedReference<ConfigurationSystem>();
	auto guidSys = MakeSharedReference<NiceMock<GuidGeneratorSystemMock>>();

	configSys->Initialize();
	guidSys->DelegateToFake();

	if (aInstanceId)
		EXPECT_CALL(*guidSys, GenerateNew)
		.WillOnce(Return(*aInstanceId))
		.WillRepeatedly(DoDefault());

	ConfigurationRemoteSystem::InjectDependencies(std::tie(*configSys, *guidSys));

	auto configRemoteSys = MakeSharedReference<ConfigurationRemoteSystem>();

	configRemoteSys->Initialize();

	return { std::move(configSys), std::move(guidSys), std::move(configRemoteSys) };
}
