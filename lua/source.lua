--   ____                                           _       _   
--  / ___|  ___  _   _ _ __ ___ ___   ___  ___ _ __(_)_ __ | |_ 
--  \___ \ / _ \| | | | '__/ __/ _ \ / __|/ __| '__| | '_ \| __|
--   ___) | (_) | |_| | | | (_|  __/ \__ \ (__| |  | | |_) | |_ 
--  |____/ \___/ \__,_|_|  \___\___| |___/\___|_|  |_| .__/ \__|
--                                                   |_|        
-- The json libary is available
-- The MADS table has:
-- - a MADS.data subtable (automaticaly filed with the JSON data)
-- - a MADS.topic string (the topic of the message)

print("Loading LUA script")

-- This is the mandatory entry point for this script. This function must 
-- return a valid JSON string.
function MADS:get_output()
  self.data = {}
  self.data.string = "test_string"
  self.data.myary = {1, 2.3, 3, 4, 5}
  return json.encode(self.data)
end
