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


depl:import("novint_falcon")

--------------------------------------
-- deploy & configure the lwr agent --
depl:import("rtt_rospack")
rtt_rospack_find=rtt.provides("ros"):getOperation("find")
local path=rtt_rospack_find("novint_falcon")

depl:loadComponent("falcon_master", "Novint_falcon")
falcon_master = depl:getPeer("falcon_master")
falcon_master:getProperty("NoFalcon"):set(0)

falcon_master:setPeriod(0.001)
falcon_master:configure()
falcon_master:start()
