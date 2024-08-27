# script plugin for MADS

This is a collection of plugins for [MADS](https://github.com/MADS-NET/MADS). The project creates the plugins `lua_source.plugin`, `lua_filter.plugin`, and `lua_sink.plugin`.

The plugins provide an interface to [Lua](https://lua.org) scripts, allowing you to run Lua scripts from MADS.

*Required MADS version: 1.0.3.*


## Supported platforms

Currently, the supported platforms are:

* **Linux** 
* **MacOS**
* **Windows**


## Installation

Linux and MacOS:

```bash
cmake -Bbuild -DCMAKE_INSTALL_PREFIX="$(mads -p)"
cmake --build build -j4
sudo cmake --install build
```

Windows:

```powershell
cmake -Bbuild -DCMAKE_INSTALL_PREFIX="$(mads -p)"
cmake --build build --config Release
cmake --install build --config Release
```


## INI settings

The plugin supports the following settings in the INI file:

```ini
[lua_source]
script_file = "source.lua"
search_paths = []

[lua_filter]
script_file = "filter.lua"
search_paths = []

[lua_sink]
script_file = "sink.lua"
search_paths = []
```

The `script_file` is Lua script file to run. The path can be:

* an absolute file path (e.g., `/path/to/script.lua`)
* a relative file path (e.g., `./script.lua`)
* a file name (e.g., `script.lua`)

In the last two cases, the script is searched in the following directories, in this order:

1. the folder `./lua` relative to the folder containing the current executable file
2. the folder `./scripts` relative to the folder containing the current executable file
3. the folder `../lua` relative to the folder containing the current executable file
4. the folder `../scripts` relative to the folder containing the current executable file
5. the absolute path `<INSTALL_PREFIX>/lua`
6. the absolute path `<INSTALL_PREFIX>/scripts`
7. any other path in the `search_path` list, which bust be either absolute or relative to the executable file

Note that the **executable file is typically the MADS plugin loader**.

All settings are optional; if omitted, the default values are used.


## Lua details

The lua script has the package search list set to the same directories above detailed, where lua scripts are loaded. If you need an additional library or scripts, just put it in one of those dirs and require it.

The Lua script has available the [JSON library](https://github.com/rxi/json.lua), loaded as `json`.

The Lua script defines the table `MADS` with the fields `data` and `topic`. These fields are automatically updated by the plugin upon receiving a new payload.

### Source plugin (`lua_source.plugin`)

The Lua script **must implement** the function `MADS::get_output()`, which is called by the plugin obtaining a new payload. The function must return a JSON-formatted string (use `json.encode(self.data)`).

### Filter plugin (`lua_filter.plugin`)

The Lua script **must implement** the function `MADS::process()`, which is called by the plugin for each payload. The function must return a JSON-formatted string (use `json.encode(self.data)`).

### Sink plugin (`lua_sink.plugin`)

The Lua script **must implement** the function `MADS::deal_with_data()`, which is called by the plugin for each payload. The function must return nothing.

Note that the library [ansicolors](https://github.com/kikito/ansicolors.lua) is available (but not loaded by default). If you need it, just require it in your script as `local col = require("ansicolors")`.


---