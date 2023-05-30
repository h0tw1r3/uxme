-- license:BSD-3-Clause
-- copyright-holders:Jeff Clark
local exports = {}
exports.name = 'faststart'
exports.version = '0.0.1'
exports.description = 'Fast Start'
exports.license = 'The BSD 3-Clause License'
exports.author = { name = 'Jeff Clark' }

local faststart = exports

-- faststart.config = require('faststart_config')

local frame_subscription, reset_subscription

local faststart_plugin_path

function faststart.set_folder(path)
	faststart_plugin_path = path
end

function faststart.startplugin()
        local cs = nil
        local fastframes = 0
        local running = false

	local faststartdata_path = "faststart.dat"

	local function read_faststart_dat()
	  local rm_match;
	  local file = io.open( faststartdata_path, "r" );
	  if not file then
		file = io.open( faststart_plugin_path .. "/faststart.dat", "r" );
	  end

	  if emu.softname() ~= "" then
		local soft = emu.softname():match("([^:]*)$")
		rm_match = '^' .. emu.romname() .. ',' .. soft .. ':';
	  else
		rm_match = '^' .. emu.romname() .. ':';
	  end

	  local frames = {};
	  local is_match = false;

	  if file then
		repeat
		  line = file:read("*l");
		  if line then
			-- remove comments
			line = line:gsub( '[ \t\r\n]*;.+$', '' );
			-- handle lines
			if string.find(line, rm_match) then --- match this game
			  is_match = true;
                        elseif string.find(line, '^[0-9]+$') then -- data line
			  if is_match then
			    table.insert(frames, line)
                          end
			elseif string.find(line, '^[a-z0-9_]+:') then --- some game
			  if is_match and #frames > 0 then
				break; -- we're done
			  end
			else --- empty line or garbage
			  -- noop
			end
		  end
		until not line;
		file:close();
	  end

	  return frames
	end

	local function process_frame()
            if not running then return end

            if cs == nil then
                for k,v in pairs(manager.machine.screens) do
                    cs = k
                    break
                end
                -- print(string.format("width=%d height=%d type=%s", manager.machine.screens[cs].width, manager.machine.screens[cs].height, manager.machine.screens[cs].screen_type))
            end


            frameNum = manager.machine.screens[cs]:frame_number()
            if fastframes > 0 and frameNum >= fastframes then
                manager.machine.video.throttled = true
                manager.machine.video.frameskip = 0
                manager.machine.sound.system_mute = false
                fastframes = 0
            end
	end
        
	local function update()
		running = emu.romname() ~= "___empty"
		local details = running and manager.machine.system.description or nil
		if emu.softname() ~= "" then
			for name, dev in pairs(manager.machine.images) do
				if dev.software_longname then
					details = details .. " (" .. dev.software_longname .. ")"
					break
				end
			end
		end
                if not running then
                    cs = nil
                else
		    local dat = read_faststart_dat()
                    if #dat > 1 then
                        fastframes = tonumber(dat[2])
                    elseif #dat > 0 then
                        fastframes = tonumber(dat[1])
                    else
                        fastframes = 0
                    end

                    if fastframes > 0 then
                        print("fast forward " .. tostring(fastframes) .. " frames")
                        manager.machine.video.throttled = false
                        manager.machine.video.frameskip = 12
                        manager.machine.sound.system_mute = true
                    end
                end
	end

	frame_subscription = emu.add_machine_frame_notifier(process_frame)
	reset_subscription = emu.add_machine_reset_notifier(update)
end

return exports
