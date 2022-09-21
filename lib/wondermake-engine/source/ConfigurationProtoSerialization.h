#pragma once

namespace ProtoConfigurationRemote
{
	class InstanceConfig;
}

class Configuration;

void SerializeConfigurationToProto(const Configuration& aConfiguration, ProtoConfigurationRemote::InstanceConfig& aOutProtoConfig);
void DeserializeConfigurationFromProto(const ProtoConfigurationRemote::InstanceConfig& aProtoConfig, Configuration& aOutConfiguration);
