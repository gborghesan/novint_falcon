#include "controller-component.hpp"
#include <rtt/Component.hpp>
#include <rtt/Logger.hpp>
#include <algorithm>    // std::fill
#include <iostream>

using namespace std;
using namespace RTT;
void controller::setZeroForce()
{
	std::fill(force_m_out.begin(), force_m_out.end(), 0.0);
	std::fill(force_s_out.begin(), force_s_out.end(), 0.0);
	force_m_outport.write(force_m_out);
	force_s_outport.write(force_s_out);
	gain_ratio_master=0.0;
	state_master=FORCE_GAIN_CONSTANT;
	gain_ratio_slave=0.0;
	state_slave=FORCE_GAIN_CONSTANT;
}
controller::controller(std::string const& name) : TaskContext(name)
, gain_ratio_master(0)
, gain_ratio_slave(0)
, force_m_out(3,0.0)
, force_s_out(3,0.0)
, state_master(FORCE_GAIN_CONSTANT)
, state_slave (FORCE_GAIN_CONSTANT)
{
	addProperty("pos_gain_master",pos_gain_master);
	addProperty("pos_gain_slave",pos_gain_slave);
	addProperty("delta_change_per_sample",delta_change_per_sample)
	.doc("value from zero to one, representing the fraction of gain added when enabling force");
	pos_m_in.resize(3);
	pos_s_in.resize(3);

	addPort("force_m_outport",force_m_outport);
	addPort("force_s_outport",force_s_outport);
	addPort("pos_m_inport",pos_m_inport);
	addPort("pos_s_inport",pos_s_inport);

	addOperation("masterEnable",&controller::masterEnable,this,OwnThread);
	addOperation("masterDisable",&controller::masterDisable,this,OwnThread);
	addOperation("slaveEnable",&controller::slaveEnable,this,OwnThread);
	addOperation("slaveDisable",&controller::slaveDisable,this,OwnThread);

	setZeroForce();


} 

bool controller::configureHook()
{


	setZeroForce();


	return true;
}

bool controller::startHook(){

	setZeroForce();

	return true;
}

void controller::updateHook(){

	if(pos_m_inport.read(pos_m_in)==NoData||
			pos_s_inport.read(pos_s_in)==NoData)
	{
		setZeroForce();
		Logger::In in(this->getName());
		log( Error )<<"one of the two position"
				" ports has no data."<< endlog();
		stop();
	}
	//compute value
	for (int i=0; i<3; i++)
	{
		force_m_out[i]= (pos_m_in[i]-pos_s_in[i])*pos_gain_master*gain_ratio_master;
		force_s_out[i]=-(pos_m_in[i]-pos_s_in[i])*pos_gain_slave*gain_ratio_slave;
	}

	force_m_outport.write(force_m_out);
	force_s_outport.write(force_s_out);

	//update gains
	if(state_master==FORCE_GAIN_RISING)
	{
		gain_ratio_master+=delta_change_per_sample;
		if (gain_ratio_master>1.0)
		{
			gain_ratio_master=1.0;
			state_master=FORCE_GAIN_CONSTANT;
			event_outport.write("master_force_max");
		}
	} else if (state_master==FORCE_GAIN_DROPPING)
	{
		gain_ratio_master-=delta_change_per_sample;
		if (gain_ratio_master<0.0)
		{
			gain_ratio_master=0.0;
			state_master=FORCE_GAIN_CONSTANT;
			event_outport.write("master_force_zero");
		}	}

	if(state_slave==FORCE_GAIN_RISING)
		{
		gain_ratio_slave+=delta_change_per_sample;
			if (gain_ratio_slave>1.0)
			{
				gain_ratio_slave=1.0;
				state_slave=FORCE_GAIN_CONSTANT;
				event_outport.write("slave_force_max");
			}
		} else if (state_slave==FORCE_GAIN_DROPPING)
		{
			gain_ratio_slave-=delta_change_per_sample;
			if (gain_ratio_slave<0.0)
			{
				gain_ratio_slave=0.0;
				state_slave=FORCE_GAIN_CONSTANT;
				event_outport.write("slave_force_zero");
			}
		}



}

void controller::stopHook() {
}

void controller::cleanupHook() {
}

bool controller::masterEnable(){
	if(!this->isRunning())
		return false;
	event_outport.write("master_starting");
	state_master=FORCE_GAIN_RISING;
	return true;
}
bool controller::masterDisable(){
	if(!this->isRunning())
		return false;
	event_outport.write("master_stopping");
	state_master=FORCE_GAIN_DROPPING;
	return true;
}

bool controller::slaveEnable(){
	if(!this->isRunning())
		return false;
	event_outport.write("slave_starting");
	state_slave=FORCE_GAIN_RISING;
	return true;
}
bool controller::slaveDisable(){
	if(!this->isRunning())
		return false;
	event_outport.write("slave_stopping");
	state_slave=FORCE_GAIN_DROPPING;
	return true;
}



ORO_CREATE_COMPONENT(controller)
