/*
 * Copyright 2019 by its authors. See AUTHORS.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// g++ -g `pkg-config --cflags --libs elementary-cxx efl-cxx eina-cxx eo-cxx ecore-cxx evas-cxx edje-cxx` slider_cxx_example.cc -o slider_cxx_example

#define EFL_CXXPERIMENTAL
#ifndef EFL_BETA_API_SUPPORT
#define EFL_BETA_API_SUPPORT
#endif
#include <Efl_Ui.hh>

using namespace std::placeholders;
using efl::eo::instantiate;

static efl::ui::Win win;

static void
efl_main(void *data EINA_UNUSED, const Efl_Event *ev EINA_UNUSED)
{
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

   win = efl::ui::Win(instantiate);
   win.text_set("Slider example");
   win.delete_request_event_cb_add([](){ win._delete(); });

   efl::ui::Box bx(instantiate, win);
   win.content_set(bx);

   efl::ui::Slider sl(instantiate, win);
   sl.hint_fill_set(true, false);
   bx.pack_end(sl);

   efl::ui::Slider sl2(instantiate, win);
   efl::ui::Image ic(instantiate, win);
   ic.icon_set("folder");
   ic.can_upscale_set(false);
   ic.can_downscale_set(false);
   // FIXME: C++ part API needs special reference handling! This will show ERR!
   efl::eo::downcast<efl::Content>(sl2.part_get("elm.swallow.end"))
         .content_set(ic);

   sl2.hint_fill_set(true, false);
   bx.pack_end(sl2);

   efl::ui::Slider sl3(instantiate, win);
   sl3.range_value_set(1);
   sl3.hint_size_min_set({220, 0});
   sl3.hint_fill_set(true, false);
   bx.pack_end(sl3);

   efl::ui::Slider sl4(instantiate, win);
   sl4.range_limits_set(0, 100);
   sl4.hint_fill_set(true, false);
   bx.pack_end(sl4);

   efl::ui::Slider sl5(instantiate, win);
   sl5.range_limits_set(0, 100);
   sl5.range_step_set(1);
   sl5.orientation_set((Efl_Ui_Layout_Orientation)(EFL_UI_LAYOUT_ORIENTATION_VERTICAL | EFL_UI_LAYOUT_ORIENTATION_INVERTED));
   sl5.hint_fill_set(true, false);
   sl5.hint_size_min_set({0, 120});
   bx.pack_end(sl5);

   efl::ui::Slider sl6(instantiate, win);
   sl6.orientation_set(EFL_UI_LAYOUT_ORIENTATION_HORIZONTAL);
   sl6.range_limits_set(0, 10);
   sl6.hint_fill_set(false, true);
   sl6.hint_weight_set(0, EFL_GFX_HINT_EXPAND);
   bx.pack_end(sl6);

   efl::ui::Slider sl7(instantiate, win);
   sl7.hint_fill_set(true, false);
   bx.pack_end(sl7);

   auto changed = std::bind ( [] (efl::ui::Range_Display obj)
   { std::cout << "Changed to " << obj.range_value_get() << std::endl; }
         , std::placeholders::_1);

   auto steady =  std::bind ( [] (efl::ui::Range_Interactive obj)
   { std::cout << "Steady to " << obj.range_value_get() << std::endl; }
         , std::placeholders::_1);

   efl::eo::downcast<efl::ui::Range_Display>(sl7).changed_event_cb_add(changed);
   efl::eo::downcast<efl::ui::Range_Interactive>(sl7).steady_event_cb_add(steady);
}
EFL_MAIN()
