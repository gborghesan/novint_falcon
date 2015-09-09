require "rttlib"
require "rfsm"
require "rfsm_rtt"
require "rfsmpp"
require "rfsm_timeevent"

--require "rfsm_proto"

tc=rtt.getTC();
--print(tc)
dep = tc:getPeer("Deployer")
sup = dep:getPeer("Supervisor")

gettime = rtt.getTime
rfsm_timeevent.set_gettime_hook(gettime)


function configureHook()
   -- load state machine
   --rfsm_proto.install{host="localhost", port=44044, allow_send=true}
   
   events_in = rtt.InputPort("string")
   sup:addEventPort(events_in, "inPort_events", "rFSM event input port")
   
   fsm = rfsm.init(rfsm.load("lua/main_fsm.lua"))
 
   -- enable state entry and exit dbg output
   fsm.dbg=rfsmpp.gen_dbgcolor("falcon-novint", 
                   { STATE_ENTER=true, STATE_EXIT=true}, 
                   false)
 
   -- redirect rFSM output to rtt log
   fsm.info=function(...) rtt.logl('Info', table.concat({...}, ' ')) end
   fsm.warn=function(...) rtt.logl('Warning', table.concat({...}, ' ')) end
   fsm.err=function(...) rtt.logl('Error', table.concat({...}, ' ')) end
 
   -- the following creates a string input port, adds it as a event
   -- driven port to the Taskcontext. The third line generates a
   -- getevents function which returns all data on the current port as
   -- events. This function is called by the rFSM core to check for
   -- new events.
   fsm.getevents = rfsm_rtt.gen_read_str_events(events_in)
 
   -- optional: create a string port to which the currently active
   -- state of the FSM will be written. gen_write_fqn generates a
   -- function suitable to be added to the rFSM step hook to do this.
   fqn_out = rtt.OutputPort("string")
   sup:addPort(fqn_out, "outPort_current_rFSM_state", "current active rFSM state")
   rfsm.post_step_hook_add(fsm, rfsm_rtt.gen_write_fqn(fqn_out))

   -- Aditional port to send events
   sup_event_port = rtt.OutputPort("string")
   sup:addPort(sup_event_port, "outPort_events", "Events sent by supervisor")

   return true
end
 
function updateHook()
	
	rfsm.run(fsm)
--	events_in:read(oneEvent)
--	print(oneEvent)
end
 
function cleanupHook()
   -- cleanup the created ports.
   sup_event_port = sup:getPort("outPort_events")
   sup_event_port:write("e_end_application")
   rttlib.tc_cleanup()
end
