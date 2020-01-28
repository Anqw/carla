// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef __TRAFFICMANAGERSERVER__
#define __TRAFFICMANAGERSERVER__

#include "carla/trafficmanager/TrafficManagerBase.h"

#include <memory.h>
#include <carla/client/Actor.h>
#include <carla/Version.h>
#include <carla/rpc/Server.h>

#include <vector>

using ActorPtr = carla::SharedPtr<carla::client::Actor>;

class TrafficManagerServer
{
public:

	/// here RPCPort is the TM local instance RPC server port where
	/// it can listen to remote TM(s) and apply the changes to
	/// LOcal instance vis TrafficManagerBase *
	TrafficManagerServer
		( const uint16_t RPCPort
		, carla::traffic_manager::TrafficManagerBase *tm)
		: server(new rpc::server(RPCPort))
	{
	    // Binding a lambda function to the name "register_vehicle".
		server->bind("register_vehicle", [=](std :: vector <carla::rpc::Actor> _actor_list) {
			std::vector<ActorPtr> actor_list;
			for (auto &&actor : _actor_list) {
				actor_list.emplace_back(carla::client::detail::ActorVariant(actor).Get(tm->GetEpisodeProxy()));
			}
			tm->RegisterVehicles(actor_list);
	    });


	    // Binding a lambda function to the name "unregister_vehicle".
		server->bind("unregister_vehicle", [=](std :: vector <carla::rpc::Actor> _actor_list) {
			std::vector<ActorPtr> actor_list;
			for (auto &&actor : _actor_list) {
				actor_list.emplace_back(carla::client::detail::ActorVariant(actor).Get(tm->GetEpisodeProxy()));
			}
			tm->UnregisterVehicles(actor_list);
	    });

		/// Set target velocity specific to a vehicle.
		server->bind("set_percentage_speed_difference", [=](carla::rpc::Actor actor, const float percentage) {
			tm->SetPercentageSpeedDifference(carla::client::detail::ActorVariant(actor).Get(tm->GetEpisodeProxy()), percentage);
	    });

		/// Set global target velocity.
		server->bind("set_global_percentage_speed_difference", [=](const float percentage) {
			tm->SetGlobalPercentageSpeedDifference(percentage);
	    });

		/// Set collision detection rules between vehicles.
		server->bind("set_collision_detection", [=]
				( const carla::rpc::Actor &reference_actor
				, const carla::rpc::Actor &other_actor
				, const bool detect_collision) {
			tm->SetCollisionDetection
				( carla::client::detail::ActorVariant(reference_actor).Get(tm->GetEpisodeProxy())
				, carla::client::detail::ActorVariant(other_actor).Get(tm->GetEpisodeProxy())
				, detect_collision);
		});

		/// Method to force lane change on a vehicle.
		/// Direction flag can be set to true for left and false for right.
		server->bind("set_force_lane_change", [=](carla::rpc::Actor actor, const bool direction) {
			tm->SetForceLaneChange(carla::client::detail::ActorVariant(actor).Get(tm->GetEpisodeProxy()), direction);
	    });

		/// Enable / disable automatic lane change on a vehicle.
		server->bind("set_auto_lane_change", [=](carla::rpc::Actor actor, const bool enable) {
			tm->SetAutoLaneChange(carla::client::detail::ActorVariant(actor).Get(tm->GetEpisodeProxy()), enable);
	    });

		/// Method to specify how much distance a vehicle should maintain to
		/// the leading vehicle.
		server->bind("set_distance_to_leading_vehicle", [=](carla::rpc::Actor actor, const float distance) {
			tm->SetDistanceToLeadingVehicle(carla::client::detail::ActorVariant(actor).Get(tm->GetEpisodeProxy()), distance);
	    });

		/// Method to specify the % chance of ignoring collisions with other actors
		server->bind("set_percentage_ignore_actors", [=](carla::rpc::Actor actor, const float percentage) {
			tm->SetPercentageIgnoreActors(carla::client::detail::ActorVariant(actor).Get(tm->GetEpisodeProxy()), percentage);
	    });

		/// Method to specify the % chance of running a red light
		server->bind("set_percentage_running_light", [=](carla::rpc::Actor actor, const float percentage) {
			tm->SetPercentageRunningLight(carla::client::detail::ActorVariant(actor).Get(tm->GetEpisodeProxy()), percentage);
	    });

		/// Method to reset all traffic lights.
		server->bind("reset_all_traffic_lights", [=]() {
			tm->ResetAllTrafficLights();
		});

		/// Run TM server to respond of any user client in async mode
		server->async_run();
	}


	/// Stop running server and clear memory
	~TrafficManagerServer() {
		if(server) {
			server->stop();
			delete server;
			server = nullptr;
		}
	}

	/// Server instance to
	rpc::server *server = nullptr;
};

#endif /* __TRAFFICMANAGERSERVER__ */
