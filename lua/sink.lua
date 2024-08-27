--   ____  _       _                    _       _   
--  / ___|(_)_ __ | | __  ___  ___ _ __(_)_ __ | |_ 
--  \___ \| | '_ \| |/ / / __|/ __| '__| | '_ \| __|
--   ___) | | | | |   <  \__ \ (__| |  | | |_) | |_ 
--  |____/|_|_| |_|_|\_\ |___/\___|_|  |_| .__/ \__|
--                                       |_|        
-- The json libary is available
-- The MADS table has:
-- - a MADS.data subtable (automaticaly filed with the JSON data)
-- - a MADS.topic string (the topic of the message)
local col = require("ansicolors")
print("Loading LUA script")

-- This is the mandatory entry point for this script. This function must 
-- return a valid JSON string.
function MADS:deal_with_data()
  print("Topic:",  col("%{red bright}" .. self.topic))
  print("Data in Lua: ", col("%{red bright}" .. self.dump(self.data)))
end
