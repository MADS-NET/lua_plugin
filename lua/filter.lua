--   _____ _ _ _                            _       _   
--  |  ___(_) | |_ ___ _ __   ___  ___ _ __(_)_ __ | |_ 
--  | |_  | | | __/ _ \ '__| / __|/ __| '__| | '_ \| __|
--  |  _| | | | ||  __/ |    \__ \ (__| |  | | |_) | |_ 
--  |_|   |_|_|\__\___|_|    |___/\___|_|  |_| .__/ \__|
--                                           |_|        
-- The json libary is available
-- The MADS table has:
-- - a MADS.data subtable (automaticaly filed with the JSON data)
-- - a MADS.topic string (the topic of the message)

print("Loading LUA script")

-- This is the mandatory entry point for this script. This function must 
-- return a valid JSON string.
function MADS:process()
  self.data.myary = {1, 2.3, 3, 4, 5}
  print("Topic: ", self.topic)
  print("Data in Lua: ", self.dump(self.data))
  print("JSON in Lua: ", json.encode(self.data))
  return json.encode(self.data)
end
