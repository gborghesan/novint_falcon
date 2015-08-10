#include "novint_falcon-component.hpp"
#include <rtt/Component.hpp>
#include <rtt/Logger.hpp>
#include <algorithm>    // std::fill
#include <iostream>
using namespace libnifalcon;
using namespace std;
using namespace RTT;

Novint_falcon::Novint_falcon(std::string const& name) : TaskContext(name)
, NoFalcon(0)
{
	addProperty("NoFalcon",NoFalcon);
	force[0]=0;
	force[1]=0;
	force[2]=0;
	m_falconDevice.setFalconFirmware<FalconFirmwareNovintSDK>(); //Set Firmware
	m_falconDevice.setFalconGrip<FalconGripFourButton>(); //Set Grip
	m_falconDevice.setFalconKinematic<libnifalcon::FalconKinematicStamper>();
	f = m_falconDevice.getFalconFirmware();
	grip= m_falconDevice.getFalconGrip();
	force_in.resize(3);
	pos_out.resize(3);
	//buttonOld=0;
	//buttonNew=0;
	HowManyMiss=0;
	addPort("force_inport",force_inport);
	addPort("position_outport",position_outport);
	//addPort("button_outport",button_outport);
	//addPort("event_outport",event_outport);
	calibrated=false;
	addConstant("calibrated",calibrated);

	m_displayCalibrationMessage=true;
	red=true;y=0;
} 





bool Novint_falcon::configureHook()
{


	RTT::Logger::In in(this->getName());
	unsigned int num_falcons = 0;
	m_falconDevice.getDeviceCount(num_falcons);
	log( RTT::Info )<<"Falcons found: " << (int)num_falcons << RTT::endlog();


	if(!m_falconDevice.open(NoFalcon)) //Open falcon @ index
	{
		log( RTT::Error )<<"Falcon "<<NoFalcon<<" not found!"
				<< RTT::endlog();
		return false;
	}
	else
		log( RTT::Info )<<"Falcon "<<NoFalcon<<" found"
		<< RTT::endlog();



	//Next load the firmware to the device

	bool skip_checksum = false;
	//See if we have firmware
	bool firmware_loaded = false;
	firmware_loaded = m_falconDevice.isFirmwareLoaded();
	bool init_firmware_loaded=true;
	if(firmware_loaded)
		log( RTT::Info )<<"Falcon firmware already loaded"<< RTT::endlog();
	else
	{
		init_firmware_loaded=false;
		log( RTT::Info )<<"loading Firmware"<< RTT::endlog();

		for(int i = 1; i < 41; ++i)//try 20 times
		{
			if(!f->loadFirmware(true,
					NOVINT_FALCON_NVENT_FIRMWARE_SIZE,
					const_cast<uint8_t*>(NOVINT_FALCON_NVENT_FIRMWARE)))


				log( RTT::Info )<< "Firmware loading trial N "<< i << " of 40 failed" << RTT::endlog();
			else
			{
				firmware_loaded = true;
				break;
			}
		}

	}
	if(!init_firmware_loaded && m_falconDevice.isFirmwareLoaded())
		log( RTT::Info )<<"Firmware loaded" << RTT::endlog();// i managed to load the firmware in the 20 trials
	if(!m_falconDevice.isFirmwareLoaded())

	{
		log( RTT::Error ) << "Cannot load firmware of falcon N "<<NoFalcon << RTT::endlog();
		return false;
	}
	m_falconDevice.setForce(force);

	f->setLEDStatus(libnifalcon::FalconFirmware::RED_LED|libnifalcon::FalconFirmware::BLUE_LED);
	int iter=0;
	while(!	m_falconDevice.runIOLoop())
	{
		usleep(50000);
		log( RTT::Info ) <<"trying update: Error Code: " <<
				m_falconDevice.getErrorCode() <<
				" Device Error Code: " <<
				m_falconDevice.getFalconComm()->getDeviceErrorCode()
				<< RTT::endlog();
		iter++;
		if (iter>10)
		{
			log( RTT::Error ) <<"MISS 10 times: Error Code: " <<
					m_falconDevice.getErrorCode() <<
					" Device Error Code: " <<
					m_falconDevice.getFalconComm()->getDeviceErrorCode()
					<< RTT::endlog();
		}
	}

	return true;
}

bool Novint_falcon::startHook(){
	f->setLEDStatus(libnifalcon::FalconFirmware::GREEN_LED);
	m_falconDevice.runIOLoop();
	return true;
}

void Novint_falcon::updateHook(){

	if(!calibrated)
	{
		calibrated=calibrateDevice();
		return;
	}
	if(force_inport.read(force_in)==RTT::NoData)
		std::fill(force_in.begin(),force_in.end(),0.0);
	force[0]=force_in[0];	force[1]=force_in[1];	force[2]=force_in[2];
	m_falconDevice.setForce(force);
	bool ok=m_falconDevice.runIOLoop();
	if (!ok)
	{
		HowManyMiss++;
		if(HowManyMiss==100)
		{
			RTT::Logger::In in(this->getName());
			log( RTT::Error )<<this->getName()<<" :runIOLoop failed 100 in a row\n"
					<<" Error Code: " <<
					m_falconDevice.getErrorCode() <<
					" Device Error Code: " <<
					m_falconDevice.getFalconComm()->getDeviceErrorCode()
					<< RTT::endlog();
			this->stop();
		}
		return;
	}
	else HowManyMiss=0;

	pos = m_falconDevice.getPosition();
	pos_out[0]=pos[0];	pos_out[1]=pos[1];	pos_out[2]=pos[2];

	position_outport.write(pos_out);

/* buttons works with too much jitter...
 *
	bool new_button_value;
	new_button_value=grip->getDigitalInput(0);
	if (new_button_value!=buttons[0])
	{

		cout<< "button plus changed from "<< buttons[0]<<" to "<< new_button_value<< endl;
		buttons[0]=new_button_value;
	}
	//buttonOld=buttonNew;

	buttonNew= grip->getDigitalInputs();
	bool new_button_value;
	if(buttonNew!=buttonOld)
	{
		button_outport.write(buttonNew);
		//cout<< "button  changed"<< endl;
		new_button_value=grip->getDigitalInput(0);
		//=buttonNew & libnifalcon::FalconGripFourButton::FORWARD_BUTTON;
		if (new_button_value!=buttons[0])
		{

			cout<< "button plus changed from "<< buttons[0]<<" to "<< new_button_value<< endl;
			buttons[0]=new_button_value;
		}
		buttonOld=buttonNew;
	}
*/


}

void Novint_falcon::stopHook() {
	f->setLEDStatus(0);
	calibrated=false;
	m_falconDevice.runIOLoop();

}

void Novint_falcon::cleanupHook() {


	m_falconDevice.close();

}












bool Novint_falcon::calibrateDevice()
{
	if(f->isHomed())
	{

		return true;
	}



	f->setHomingMode(true);
	m_falconDevice.runIOLoop();
	bool ok=false;
	f->setLEDStatus(libnifalcon::FalconFirmware::RED_LED);

	y++;
	if (y*getPeriod()>0.5)
	{
		y=0;
		if (red) f->setLEDStatus(libnifalcon::FalconFirmware::RED_LED);
		else     f->setLEDStatus(libnifalcon::FalconFirmware::BLUE_LED);
		red=!red;
	}


	pos = m_falconDevice.getPosition();

	if(!f->isHomed())
	{

		if(m_displayCalibrationMessage)
		{
			log( RTT::Warning ) << "Falcon "<<this->getName()<<" not currently calibrated.\n "
					"Move control all the way out then push straight all the way in."
					<< RTT::endlog();
			m_displayCalibrationMessage = false;
		}
		ok= false;
	}
	else
	{
		log( RTT::Warning ) << "Falcon "<<this->getName()<<" currently calibrated.\n "
				<< RTT::endlog();
		f->setLEDStatus(libnifalcon::FalconFirmware::GREEN_LED);

		ok= true;
	}
	return ok;
}
/*
 * Using this macro, only one component may live
 * in one library *and* you may *not* link this library
 * with another component library. Use
 * ORO_CREATE_COMPONENT_TYPE()
 * ORO_LIST_COMPONENT_TYPE(Novint_falcon)
 * In case you want to link with another library that
 * already contains components.
 *
 * If you have put your component class
 * in a namespace, don't forget to add it here too:
 */
ORO_CREATE_COMPONENT(Novint_falcon)
