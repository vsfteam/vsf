/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "vsf.h"

#if APP_USE_LUA_DEMO == ENABLED && APP_USE_LINUX_DEMO == ENABLED

#include "lovevsf.h"

#include "lualib.h"
#include "lauxlib.h"

#include "../common/usrapp_common.h"
#include <SDL.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const char __config_love[] = VSF_STR(
LOVE_CFG = {}
LOVE_CFG.DEFAULT_FONT = "/memfs/font/Roboto-Regular.ttf"
LOVE_CFG.DEFAULT_FONT_SIZE = 16
);

static const char __boot_love[] = VSF_STR(
function love.nogame()

  function love.load()
    love.graphics.setBackgroundColor(0, 138, 225)
  end

  function love.keypressed(key)
    if key == "escape" then
      os.exit()
    end
  end

  local function drawText(str, y)
    local screenw = love.graphics.getWidth()
    local font = love.graphics.getFont()
    love.graphics.print(str, (screenw - font:getWidth(str)) / 2, y)
  end

  function love.draw()
    love.graphics.setColor(255, 255, 255)
    drawText("LoveVSF (inspired by LoveDOS)" .. love.getVersion(), 90)
    drawText("No game", 106)
    love.graphics.setColor(95, 181, 255)
    drawText("Press ESCAPE to quit", 186)
  end

end

function love.graphics.getFont()
  return love.graphics.font
end

function love.graphics.setFont(font)
  love.graphics.font = font
end

function love.boot()
  package.path = "?.lua;?/init.lua"

  love.graphics.setFont(love.graphics.newFont(LOVE_CFG.DEFAULT_FONT, LOVE_CFG.DEFAULT_FONT_SIZE))

  if love.filesystem.isFile("main.lua") then
    require("main")
  else
    love.nogame()
  end
  love.run();
end

function love.run()
  if love.load then love.load(args) end
  love.timer.step()
  while true do
    love.timer.step()
    local dt = love.timer.getDelta()
    if love.update then love.update(dt) end
    love.graphics.clear()
    if love.draw then love.draw() end
    love.graphics.present()
  end
end

function love.errhand(msg)
  print(msg)
end

xpcall(love.boot, love.errhand)
);

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

int love_main(int argc, char *argv[])
{
    if (NULL == usrapp_ui_common.disp) {
        printf("no display device found\n");
        return -1;
    }

    vsf_sdl2_cfg_t cfg = {
        .disp_dev = usrapp_ui_common.disp,
    };
    vsf_sdl2_init(&cfg);

    // TODO: process on_exit to call lua_close(L);
    lua_State *L = lua_newstate(NULL, NULL);
    luaL_openlibs(L);
    luaL_requiref(L, "love", luaopen_love, 1);

    if (    (LUA_OK != luaL_dostring(L, __config_love))
        ||  (LUA_OK != luaL_dostring(L, __boot_love))) {
        goto lua_fatal;
    }

    lua_close(L);
    return 0;

lua_fatal:
    printf("boot.lua failed with : %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    lua_close(L);
    return -1;
}

#endif
