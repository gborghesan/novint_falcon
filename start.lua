require "rttlib"
require "rfsm_rtt"
require "rttros"

----------------------
-- get the deployer --
tc=rtt.getTC()
if tc:getName() == "lua" then
  depl=tc:getPeer("Deployer") 
elseif tc:getName() == "Deployer" then
  depl=tc
end

depl:import("rtt_roscomm")
depl:import("rtt_rosnode")
depl:import("rtt_std_msgs")

depl:import("novint_falcon")


--------------------
-- Configurations --
Ts=0.0002


-------------------------------------------
-- deploy & configure the Falcon control --
depl:import("rtt_rospack")
rtt_rospack_find=rtt.provides("ros"):getOperation("find")
local path=rtt_rospack_find("novint_falcon")

depl:loadComponent("falcon_master", "Novint_falcon")
falcon_master = depl:getPeer("falcon_master")
falcon_master:getProperty("NoFalcon"):set(0)

depl:loadComponent("falcon_slave", "Novint_falcon")
falcon_slave = depl:getPeer("falcon_slave")
falcon_slave:getProperty("NoFalcon"):set(1)

depl:loadComponent("controllerPP", "controller")
controllerPP = depl:getPeer("controllerPP")

cp=rtt.Variable("ConnPolicy")
depl:connect("falcon_master.position_outport","controllerPP.pos_m_inport", cp)
depl:connect("falcon_slave.position_outport","controllerPP.pos_s_inport", cp)
depl:connect("controllerPP.force_m_outport","falcon_master.force_inport", cp)
depl:connect("controllerPP.force_s_outport","falcon_slave.force_inport", cp)

controllerPP:getProperty("pos_gain_master"):set(-300)
controllerPP:getProperty("pos_gain_slave"):set(-300)
controllerPP:getProperty("delta_change_per_sample"):set(0.001)

---------------------
-- Set up the rFSM --
---------------------

depl:loadComponent("Supervisor", "OCL::LuaComponent")
sup = depl:getPeer("Supervisor")
depl:addPeer("Supervisor","Deployer")
depl:connectPeers("Deployer","Supervisor")
sup:exec_file("lua/supervisor_comp.lua")
sup:configure()

depl:import("rtt_std_msgs")
depl:loadComponent("StringEcho", "OCL::LuaComponent")
strecho = depl:getPeer("StringEcho")
depl:addPeer("StringEcho","Deployer")
depl:connectPeers("Deployer","Supervisor")
strecho:exec_file("lua/signal_echo.lua")
strecho:configure()

cp_ros = rtt.Variable('ConnPolicy')
cp_ros.transport = 3   -- transport layer: ROS
cp_ros.name_id="/events"
--depl:stream("StingEcho.event_in", rtt.provides("ros"):topic("/events"))
depl:stream("StringEcho.event_in", cp_ros)
cp_events = rtt.Variable('ConnPolicy')
cp_events.type = 0   -- type DATA

depl:connect("StringEcho.event_out", "Supervisor.inPort_events", cp_events)

----------------------
-- Start components --
----------------------

sup:start()
strecho:start()

falcon_master:setPeriod(Ts*2)
falcon_master:configure()
falcon_master:start()

falcon_slave:setPeriod(Ts*2)
falcon_slave:configure()
falcon_slave:start()

controllerPP:setPeriod(Ts)
controllerPP:configure()
--controllerPP:start()
