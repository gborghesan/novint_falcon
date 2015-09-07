#ifndef OROCOS_controller_teleop_COMPONENT_HPP
#define OROCOS_controller_teleop_COMPONENT_HPP

#include <rtt/RTT.hpp>



class controller : public RTT::TaskContext{
public:
	controller(std::string const& name);
	bool configureHook();
	bool startHook();
	void updateHook();
	void stopHook();
	void cleanupHook();

void masterEnable();
void masterDisable();
void slaveEnable();
void slaveDisable();

private:

	void setZeroForce();

	enum{FORCE_GAIN_CONSTANT, FORCE_GAIN_RISING, FORCE_GAIN_DROPPING} state_master, state_slave;

	RTT::InputPort<std::vector<double> > pos_m_inport;
	RTT::InputPort<std::vector<double> > pos_s_inport;
	RTT::OutputPort<std::vector<double> > force_m_outport;
	RTT::OutputPort<std::vector<double> > force_s_outport;
	RTT::OutputPort<std::string > event_outport;
//properties
	double pos_gain_master;
	double pos_gain_slave;
	double delta_change_per_sample;

	double gain_ratio_master;//current gain
	double gain_ratio_slave;//current gain

	std::vector<double> force_m_out;
	std::vector<double> force_s_out;
	std::vector<double> pos_m_in;
	std::vector<double> pos_s_in;





};
#endif
