

-- ------------------------------- --
-- Functions to use during the FSM --
-- ------------------------------- --

local tc=rtt.getTC();

----------------------
-- get the deployer --
tc=rtt.getTC()
depl=tc:getPeer("Deployer")

sup = depl:getPeer("Supervisor")
falcon_master = depl:getPeer("falcon_master")
falcon_slave = depl:getPeer("falcon_slave")
controllerPP = depl:getPeer("controllerPP")

function enableController()
  controllerPP:start()
  controllerPP:slaveEnable()
end

function disableController()
  controllerPP:slaveDisable()
  controllerPP:stop()
end

function enableHaptics()
  controllerPP:masterEnable()
end

function disableHaptics()
  controllerPP:masterDisable()
end

function shutdownApplication()
  depl:kickOutAll()
end

--function set_path_viz_limit(newLimit)
--  visualizer_path_limit = visualizer:getProperty("Maximum_samples")
--  visualizer_path_limit:set(newLimit)
--end
--
--function set_report_file_name(newName)
--  report_file_prop = reporter:getProperty("ReportFile")
--  report_file_prop:set(newName)
--end

