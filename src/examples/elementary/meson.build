examples = [
  'actionslider_example_01',
  'bg_example_01',
  'bg_example_02',
  'bg_example_03',
  'box_example_02',
  'bubble_example_01',
  'button_example_00',
  'button_example_01',
  'calendar_example_01',
  'calendar_example_02',
  'calendar_example_03',
  'calendar_example_04',
  'calendar_example_05',
  'calendar_example_06',
  'check_example_01',
  'clock_example',
  'colorselector_example_01',
  'conformant_example_01',
  'conformant_example_02',
  'combobox_example_01',
  'ctxpopup_example_01',
  'datetime_example',
  'dayselector_example',
  'diskselector_example_01',
  'diskselector_example_02',
  'entry_example',
  'fileselector_button_example',
  'fileselector_entry_example',
  'fileselector_example',
  'fileviewlist',
  'filemvc',
  'filter_example',
  'flip_example_01',
  'flipselector_example',
  'frame_example_01',
  'general_funcs_example',
  'gengrid_example',
  'genlist_example_01',
  'genlist_example_02',
  'genlist_example_03',
  'genlist_example_04',
  'genlist_example_05',
  'glview_example_01',
  'hover_example_01',
  'hoversel_example_01',
  'icon_example_01',
  'image_example_01',
  'index_example_01',
  'index_example_02',
  'inwin_example',
  'label_example_01',
  'label_example_02',
  'label_example_03',
  'layout_example_01',
  'layout_example_02',
  'layout_example_03',
  'layout_property_bind',
  'list_example_01',
  'list_example_02',
  'list_example_03',
  'map_example_01',
  'map_example_02',
  'map_example_03',
  'mapbuf_example',
  'menu_example_01',
  'naviframe_example',
  'notify_example_01',
  'panes_example',
  'panel_example_01',
  'photocam_example_01',
  'prefs_example_01',
  'prefs_example_02',
  'prefs_example_03',
  'popup_example_01',
  'popup_example_02',
  'popup_example_03',
  'progressbar_example',
  'radio_example_01',
  'radio_example_02',
  'segment_control_example',
  'separator_example_01',
  'slider_example',
  'slideshow_example',
  'spinner_example',
  'scroller_example_01',
  'table_example_01',
  'table_example_02',
  'theme_example_01',
  'theme_example_02',
  'thumb_example_01',
  'toolbar_example_01',
  'toolbar_example_02',
  'toolbar_example_03',
  'transit_example_01',
  'transit_example_02',
  'transit_example_03',
  'transit_example_04',
  'web_example_01',
  'web_example_02',
  'win_example',
  'track_example_01',
  'efl_thread_1',
  'efl_thread_2',
  'efl_thread_3',
  'efl_thread_4',
  'efl_thread_5',
  'efl_thread_6',
  'efl_ui_list_example_1',
  'efl_ui_list_view_example_1',
  'efl_ui_list_view_example_2',
  'efl_ui_list_view_example_3',
  'efl_ui_collection_view_example_1',
  'efl_canvas_layout_text',
  'efl_ui_theme_example_01',
  'efl_ui_theme_example_02',
  'efl_ui_slideshow_example',
  'efl_ui_radio_example_01',
  'efl_ui_grid_example_1',
  'efl_ui_grid_view_example_1',
  'efl_canvas_textblock_obstacles_example'
]

foreach example : examples
  executable(example, example + '.c', dependencies: [elementary, ecore, eio])
endforeach
if get_option('bindings').contains('cxx')
  cxx_examples = [
    'bg_cxx_example_01',
    'bg_cxx_example_02',
    'box_cxx_example_02',
    'button_cxx_example_00',
    'button_cxx_example_01',
    'calendar_cxx_example_01',
    'calendar_cxx_example_02',
    'calendar_cxx_example_03',
    'calendar_cxx_example_04',
    'calendar_cxx_example_05',
    'clock_cxx_example',
    'icon_cxx_example_01',
    'menu_cxx_example_01',
    'popup_cxx_example',
    'radio_cxx_example_01',
    'slider_cxx_example',
    'spinner_cxx_example',
    'table_cxx_example_01',
    'table_cxx_example_02',
    'toolbar_cxx_example_01',
  ]

  foreach example : cxx_examples
    executable(example, example + '.cc', dependencies: [elementary_cxx])
  endforeach

endif

edc_files = [
  'codegen_example.edc',
  'efl_ui_theme_example.edc'
]

themes = []

foreach edc_file : edc_files
   themes += custom_target('edje_cc_' + edc_file,
    depends : edje_depends,
    input : edc_file,
    output : '@BASENAME@.edj',
    command : edje_cc_exe + ['-beta',
              '-id', meson.current_source_dir(),
              '-fd', meson.current_source_dir(),
              '-sd', meson.current_source_dir(),
              '-vd', meson.current_source_dir(),
              '-dd', meson.current_source_dir(),
              '-md', meson.current_source_dir(),
              '-td', meson.current_source_dir(),
              '@INPUT@', '@OUTPUT@'],
   )
endforeach


codegen = custom_target('elementary_codegen_examples',
    depends : [themes, elementary_codegen],
    input : themes[0],
    output : ['codegen_example_generated.c', 'codegen_example_generated.h'],
    command : elementary_codegen_exe + ['-p=codegen_example', '@INPUT@', 'elm/example/mylayout/default', '@OUTPUT0@', '@OUTPUT1@'],
)

executable('codegen_example',
  ['codegen_example.c'] + [codegen],
  dependencies: [elementary],
  include_directories : include_directories('..')
)
