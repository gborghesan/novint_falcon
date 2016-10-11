#ifndef OROCOS_NOVINT_FALCON_COMPONENT_HPP
#define OROCOS_NOVINT_FALCON_COMPONENT_HPP

#include <rtt/RTT.hpp>
#include "falcon/core/FalconLogger.h"
#include "falcon/core/FalconDevice.h"
#include "falcon/firmware/FalconFirmwareNovintSDK.h"
#include "falcon/util/FalconFirmwareBinaryNvent.h"
#include "falcon/grip/FalconGripFourButton.h"
#include "falcon/kinematic/FalconKinematicStamper.h"
#include "geometry_msgs/Vector3.h"

using namespace libnifalcon;
class Novint_falcon : public RTT::TaskContext{
public:
	Novint_falcon(std::string const& name);
	bool configureHook();
	bool startHook();
	void updateHook();
	void stopHook();
	void cleanupHook();
	bool calibrateDevice();
private:
	std::shared_ptr<FalconFirmware> f;
	std::shared_ptr<FalconGrip> grip;

	FalconDevice m_falconDevice;

	int NoFalcon;
	int HowManyMiss;
	std::array<double, 3> force;
	std::array<double, 3> pos;

	RTT::InputPort<geometry_msgs::Vector3 > force_inport;
	RTT::OutputPort<geometry_msgs::Vector3 > position_outport;
	//RTT::OutputPort<int > button_outport;
	//RTT::OutputPort<std::string > event_outport;

	geometry_msgs::Vector3 force_in;
	geometry_msgs::Vector3 pos_out;
	//int buttonOld;
	//int buttonNew;
	//bool buttons[4];

	bool calibrated;


	int y;
	bool red;
	bool m_displayCalibrationMessage;

};
#endif
