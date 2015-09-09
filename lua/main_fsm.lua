
-- ----------------------- --
-- Load actions of the FSM --
-- ----------------------- --

dofile("lua/fsm_actions.lua")

-- --------------------- --
-- Make global variables --
-- --------------------- --

entrySecs = 0
entryNanosecs = 0

-- -------------------------- --
-- Main definition of the FSM --
-- -------------------------- --

return rfsm.state {

  entry = function ()
    print ("Program start")
  end,
  
  exit = function()
    print("Program finished")
  end,

  -- Global configuration of the system is done here
  configuration = rfsm.state {
    entry = function()
      print("Starting configuration")
    end,
    exit = function()
      print("Configuration finished")
    end,
  },

  operative = rfsm.state {
    entry = function()
      enableController()
      print("Controller Enabled")
    end,
    exit = function()
      disableController()
      print("Controller Disabled")
    end,
    hapticsOff = rfsm.state {
    
    },
    hapticsOn = rfsm.state {
      entry = function()
        enableHaptics()
        print("Haptics Enabled")
      end,
      exit = function()
        disableHaptics()
        print("Haptics Disabled")
      end,
    },
    rfsm.trans {src="initial", tgt="hapticsOff"},
    rfsm.trans {src="hapticsOff", tgt="hapticsOn", events={"e_haptics_enable"}},
    rfsm.trans {src="hapticsOn", tgt="hapticsOff", events={"e_haptics_disable"}},
  }, -- end of operative fsm
  
  cleanup = rfsm.state {
    entry = function ()
      print("Start sweeping")
      shutdownApplication()
    end,
    
    exit = function ()
      print("Configuration cleaned-up")
      print("Exiting...")
    end,
  },
  
  rfsm.trans {src="initial", tgt="configuration" },
  rfsm.trans {src="configuration", tgt="operative", events={"e_configured","e_switch_teleop_on"}},
  rfsm.trans {src="operative", tgt="configuration", events={"e_configuration_required","e_switch_teleop_off"}},
  rfsm.trans {src="configuration", tgt="cleanup", events={"e_quit"}},
  rfsm.trans {src="operative", tgt="cleanup", events={"e_quit"}},
  --rfsm.trans {src="", tgt="", events={""}},
  --rfsm.trans {src="", tgt="", events={""}},
}
