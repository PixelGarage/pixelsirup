<?php
/**
 * @file
 * Implements the forms needed for the Vacuspeed Layout-Tool.
 *
 * Created by PhpStorm.
 * User: ralph
 * Date: 28.07.14
 * Time: 17:44
 */

/*-------------------------------------------------------------------------------
 *        Vacuspeed initialisation page
 *-------------------------------------------------------------------------------*/

/**
 * Define the initialisation page.
 */
function vacuspeed_tool_init_page() {
  $content = array();

  drupal_set_title('');
  $content['top-region']['init_form'] = drupal_get_form('vacuspeed_tool_init_form');
  $content['top-region']['#prefix'] = '<div class="region region-top">';
  $content['top-region']['#suffix'] = '</div>';

  return $content;
}

/**
 * Creates the init form of the vacuspeed layout tool.
 *
 * @param $form
 * @param $form_state
 * @return mixed
 */
function vacuspeed_tool_init_form($form, &$form_state) {
  // add helper image for dimension input
  $img_src = drupal_get_path('theme', 'standard_theme') . '/images/surface_main.jpg';
  $img_src = file_create_url($img_src); // make url absolute

  $form['vacuspeed_init_desc'] = array(
    '#markup' => t('Define the maximal extent of the surface that should be tiled with Vacuspeed. Cut-offs can be defined during the next step.'),
    '#suffix' => '<div class="region-helper-image"><img src="' . $img_src . '"></div>',
  );
  $form['vacuspeed_init_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Side A (width in cm).'),
    '#description' => t('Define the maximal width of the surface in cm.'),
    '#default_value' => 100,
    '#element_validate' => array('element_validate_integer'),
    '#size' => 10,
    '#maxlength' => 5,
    '#required' => TRUE,
  );
  $form['vacuspeed_init_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Side B (height in cm).'),
    '#description' => t('Define the maximal height of the surface in cm.'),
    '#default_value' => 100,
    '#element_validate' => array('element_validate_integer'),
    '#size' => 10,
    '#maxlength' => 5,
    '#required' => TRUE,
  );
  $form['vacuspeed_init_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next'),
  );
  return $form;
}

/**
 * Submits the layout form of the vacuspeed layout tool.
 *
 * @param $form
 * @param $form_state
 */
function vacuspeed_tool_init_form_submit($form, &$form_state) {
  // initialize tiling page
  $session_data = &vacuspeed_tool_session_data();
  $session_data['width'] = $form_state['values']['vacuspeed_init_width'];
  $session_data['height'] = $form_state['values']['vacuspeed_init_height'];
  $session_data['timestamp'] = date('dmYHis');
  $session_data['show_double_layer'] = false;
  $session_data['cuts_serialized'] = '';
  $session_data['counts_serialized'] = '';

  // redirect to tiling page
  $form_state['redirect'] = array(
    'layout/tiling',
  );
}


/*-------------------------------------------------------------------------------
 *        Vacuspeed tiling page
 *-------------------------------------------------------------------------------*/

/**
 * Define the tiling page.
 */
function vacuspeed_tool_tiling_page() {
  // initialize page data
  $session_data = &vacuspeed_tool_session_data();
  $session_data['cuts'] = array();
  if (!empty($session_data['cuts_serialized'])) {
    $session_data['cuts'] = unserialize($session_data['cuts_serialized']);
  }
  drupal_set_title('');

  // define the css and js files and settings needed for the page
  $path = drupal_get_path('module', 'vacuspeed_tool');
  $settings = array(
    'dialog_title' => t('Define cut-off'),
  );

  // define page
  $content = array();
  $content['content']['top']['#markup'] = t('<p>Start editing the surface until it fits the original.<br><br>Add new cut-offs (near vertices, on edges, inside surface) by pressing ' .
                                             'the corresponding plus-buttons (+) on the surface, which opens a form to define the cut-off.<br>All defined cut-offs are listed in ' .
                                             'the cut-off table and can be deleted by the minus-button (-).<br><br>Vacuspeed can be layed anytime by pressing the button "Lay Vacuspeed", ' .
                                             'which additionally shows the number of needed tiles per layer in the counts table.</p>');
  $content['content']['top']['#prefix'] = '<div class="region region-two-50-top region-conditional-stack"><div class="region-inner clearfix">';
  $content['content']['top']['#suffix'] = '</div></div>';

  $content['content']['first']['images']  = _vacuspeed_tool_create_image_wrapper('surface-only');
  $content['content']['first']['#prefix'] = '<div class="region region-two-50-first"><div class="region-inner clearfix">';
  $content['content']['first']['#suffix'] = '</div></div>';

  $content['content']['second']['cuts_table']   = _vacuspeed_tool_create_cuts_table();
  $content['content']['second']['counts_table'] = _vacuspeed_tool_create_counts_table();
  $content['content']['second']['cut_form']     = drupal_get_form('vacuspeed_tool_cut_form');
  $content['content']['second']['#prefix']      = '<div class="region region-two-50-second"><div class="region-inner clearfix">';
  $content['content']['second']['#suffix']      = '</div></div>';

  $content['content']['form']['tiling_form'] = drupal_get_form('vacuspeed_tool_tiling_form');
  $content['content']['form']['#prefix'] = '<div class="region region-two-50-bottom region-conditional-stack"><div class="region-inner clearfix">';
  $content['content']['form']['#suffix'] = '</div></div>';

  $content['content']['#prefix'] = '<div class="at-panel gpanel panel-display two-50 clearfix">';
  $content['content']['#suffix'] = '</div>';

  $content['content']['#attached']['js'] = array(
    $path . '/scripts/vacuspeed_cuts.js',
    array(
      'data' => array('vacuspeed_tool' => $settings),
      'type' => 'setting',
    ),
  );

  // store rendered enabled/disabled submit button for later use
  $submit_enabled = $content['content']['form']['tiling_form']['vacuspeed_tiling_submit'];
  $submit_disabled = $submit_enabled;
  $submit_disabled['#attributes'] = array('disabled' => 'disabled');
  $session_data['submit_disabled'] = drupal_render($submit_disabled);
  $session_data['submit_enabled'] = drupal_render($submit_enabled);

  // set submit button as disabled
  $content['content']['form']['tiling_form']['vacuspeed_tiling_submit']['#attributes'] = array('disabled' => 'disabled');
  // return page content
  return $content;
}

/**
 * Creates the tiling form of the vacuspeed layout tool.
 *
 * @param $form
 * @param $form_state
 * @return mixed
 */
function vacuspeed_tool_tiling_form($form, &$form_state) {
  $form['vacuspeed_tiling_double_layer'] = array(
    '#type' => 'checkbox',
    '#default_value' => 0,
    '#title' => t('Show double layer'),
    '#weight' => -1,
  );
  $form['vacuspeed_tiling_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Request Offer'),
    '#weight' => 0,
  );
  $form['vacuspeed_tiling_preview'] = array(
    '#type' => 'button',
    '#value' => t('Lay Vacuspeed'),
    '#weight' => 1,
    '#ajax' => array(
      'callback' => 'vacuspeed_tool_preview_callback',
      'wrapper'  => 'tiled-image-wrapper',
      'effect'   => 'fade',
      'speed'    => 'slow',
      'progress' => array(
        'type' => 'throbber'
      ),
    ),
  );

  return $form;
}

/**
 * Submits the tiling form of the vacuspeed layout tool.
 *
 * @param $form
 * @param $form_state
 */
function vacuspeed_tool_tiling_form_submit($form, &$form_state) {
  // initialize order page data
  $session_data = &vacuspeed_tool_session_data();
  $session_data['thickness_layer_1'] = 0;
  $session_data['thickness_layer_2'] = 0;

  // redirect to order page
  $form_state['redirect'] = array(
    'layout/order',
  );
}

/**
 * Replaces the surface image with the calculated tiled layer images and enables the submit button.
 * @param $form
 * @param $form_state
 * @return array Array with ajax commands
 */
function vacuspeed_tool_preview_callback($form, &$form_state) {
  // get double layer flag
  $session_data = &vacuspeed_tool_session_data();
  $show_double_layer = $form_state['values']['vacuspeed_tiling_double_layer'];
  $session_data['show_double_layer'] = $show_double_layer;

  // calculate the tiles of the surface according to the defined cuts and layers
  $image_wrapper = _vacuspeed_tool_create_image_wrapper('surface-tiled', $show_double_layer);
  $counts_table = _vacuspeed_tool_create_counts_table();

  // serialized counts array for use in order form
  $session_data['counts_serialized'] = serialize($session_data['counts']);


  // build ajax commands to replace image wrapper and enable submit button (once) and close dialog
  $commands = array();
  $commands[] = ajax_command_replace("#tiled-image-wrapper", drupal_render($image_wrapper));
  $commands[] = ajax_command_replace("#counts-table-wrapper", drupal_render($counts_table));

  // enable submit button (tiles and counts available)
  $commands[] = ajax_command_replace("#edit-vacuspeed-tiling-submit", $session_data['submit_enabled']);

  // return ajax commands
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}

/**
 * Creates the form to define cuts of the surface (shown in modal dialog).
 *
 * @param $form
 * @param $form_state
 */
function vacuspeed_tool_cut_form($form, &$form_state) {
  // add helper image for cut input
  $img_src = drupal_get_path('theme', 'standard_theme') . '/images/surface_cut_mm.gif';
  $img_src = file_create_url($img_src); // make url absolute

  $form['vacuspeed_cut_desc'] = array(
    '#markup' => t('Define position and extent of the cut-off.'),
    '#prefix' => '<div class="description">',
    '#suffix' => '<div class="form-helper-image"><img src="' . $img_src . '"></div></div>',
  );
  $form['vacuspeed_cut_left'] = array(
    '#type' => 'textfield',
    '#title' => t('Position x'),
    '#description' => t('Distance from left edge (cm).'),
    '#default_value' => 0,
    '#element_validate' => array('element_validate_integer'),
    '#attributes' =>  array('class' => array('text-left')),
    '#size' => 6,
    '#maxlength' => 5,
    '#required' => TRUE,
  );
  $form['vacuspeed_cut_top'] = array(
    '#type' => 'textfield',
    '#title' => t('Position y'),
    '#description' => t('Distance from top edge (cm).'),
    '#default_value' => 0,
    '#element_validate' => array('element_validate_integer'),
    '#attributes' =>  array('class' => array('text-top')),
    '#size' => 6,
    '#maxlength' => 5,
    '#required' => TRUE,
  );
  $form['vacuspeed_cut_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Width l'),
    '#description' => t('Width of cut-off (cm).'),
    '#default_value' => 0,
    '#element_validate' => array('element_validate_integer'),
    '#attributes' =>  array('class' => array('text-width')),
    '#size' => 6,
    '#maxlength' => 5,
    '#required' => TRUE,
  );
  $form['vacuspeed_cut_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Height b'),
    '#description' => t('Height of cut-off (cm).'),
    '#default_value' => 0,
    '#element_validate' => array('element_validate_integer'),
    '#attributes' =>  array('class' => array('text-height')),
    '#size' => 6,
    '#maxlength' => 5,
    '#required' => TRUE,
  );
  $form['vacuspeed_cut_add'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
    '#name' => '',
    '#attributes' =>  array('class' => array('submit-button')),
    '#ajax' => array(
      'callback' => 'vacuspeed_tool_cut_add_callback',
      'wrapper'  => 'cuts-table-wrapper',
      'effect'   => 'fade',
      'speed'    => 'slow',
    ),
  );
  $form['vacuspeed_cut_cancel'] = array(
    '#type' => 'button',
    '#value' => t('Cancel'),
    '#attributes' =>  array('class' => array('cancel-button')),
    '#ajax' => array(
      'callback' => 'vacuspeed_tool_cut_cancel_callback',
      'wrapper'  => 'cuts-table-wrapper',
      'effect'   => 'fade',
      'speed'    => 'slow',
    ),
  );

  return $form;
}


function vacuspeed_tool_cut_form_validate($form, &$form_state) {
  $session_data = &vacuspeed_tool_session_data();
  $width = $session_data['width'];
  $height = $session_data['height'];
  $cl = (int)$form_state['values']['vacuspeed_cut_left'];
  $ct = (int)$form_state['values']['vacuspeed_cut_top'];
  $cw = (int)$form_state['values']['vacuspeed_cut_width'];
  $ch = (int)$form_state['values']['vacuspeed_cut_height'];

  // prevent validating values to be calculated (value = -1)
  $cl = $cl == -1 ? 0 : $cl;
  $ct = $ct == -1 ? 0 : $ct;

  // test cut to be inside defined surface
  if ($cl < 0 || $cl > $width) {
    form_set_error('vacuspeed_cut_left', t('@field must be inside surface border. (0 <= x < width)', array('@field' => $form['vacuspeed_cut_left']['#title'])));
  }
  if ($cw <= 0 || $cw > $width) {
    form_set_error('vacuspeed_cut_width', t('@field must be positive number smaller than surface width (0 < cut width < width)', array('@field' => $form['vacuspeed_cut_width']['#title'])));
  }
  if (($cl+$cw) > $width) {
    form_set_error('vacuspeed_cut_width', t('Position of right edge must be inside surface border (0 <= xRight <= width)'));
  }
  if ($ct < 0 || $ct > $height) {
    form_set_error('vacuspeed_cut_top', t('@field  must be inside surface border (0 <= y < height)', array('@field' => $form['vacuspeed_cut_top']['#title'])));
  }
  if ($ch <= 0 || $ch > $height) {
    form_set_error('vacuspeed_cut_height', t('@field  must be positive number smaller than the surface height (0 < cut height < height)', array('@field' => $form['vacuspeed_cut_height']['#title'])));
  }
  if (($ct+$ch) > $height) {
    form_set_error('vacuspeed_cut_height', t('Position of bottom edge must be inside surface border (0 <= yBottom <= height)'));
  }

}

/**
 * Callback function to add a new cut definition.
 *
 * @param $form
 * @param $form_state
 */
function vacuspeed_tool_cut_add_callback($form, &$form_state) {
  $session_data = &vacuspeed_tool_session_data();
  $commands = array();

  // restore all changed field descriptions
  if (isset($session_data['form_item_restore'])) {
    foreach($session_data['form_item_restore'] as $sel => $desc){
      $commands[] = ajax_command_html($sel, $desc);
      $commands[] = ajax_command_css($sel, array('color' => 'inherit'));
    }

  }
  $session_data['form_item_restore'] = array();

  //
  // Validation error, return errors and leave dialog open
  //
  // display validation error directly at the corresponding field
  if ($errors = form_get_errors()) {
    // add error text to field description
    foreach($errors as $id => $error) {
      // store original field description
      $field_class = '.form-item-' . str_replace('_', '-', $id);
      $selector = $field_class . ' .description';
      $session_data['form_item_restore'][$selector] = $form[$id]['#description'];

      // add ajax commands to alter field description
      $commands[] = ajax_command_html($selector, $error);
      $commands[] = ajax_command_css($selector, array('color' => 'red'));
    }
    // clear form errors
    form_clear_error();
    drupal_get_messages('error');

    // return ajax commands
    return array(
      '#type' => 'ajax',
      '#commands' => $commands,
    );
  }

  //
  // cut definition successful, add it to table and image and close dialog
  //
  // get form and session data
  $width = $session_data['width'];
  $height = $session_data['height'];
  $left = (int)$form_state['values']['vacuspeed_cut_left'];
  $top = (int)$form_state['values']['vacuspeed_cut_top'];

  // evaluate cut type
  $type = 'Undefined';
  if ($top == 0 && $left == 0) {
    $type = 'TL';
  } else if ($top == 0 && $left > 0) {
    $type = 'TM';
  } else if ($top == 0 && $left == -1) {
    $type = 'TR';
  } else if ($top > 0 && $left == 0) {
    $type = 'ML';
  } else if ($top > 0 && $left > 0) {
    $type = 'MM';
  } else if ($top > 0 && $left == -1) {
    $type = 'MR';
  } else if ($top == -1 && $left == 0) {
    $type = 'BL';
  } else if ($top == -1 && $left > 0) {
    $type = 'BM';
  } else if ($top == -1 && $left == -1) {
    $type = 'BR';
  }

  // store created cut in session variable and create table
  $session_data['cuts'][] = array(
    'left' => ($left == -1) ? $width - (int)$form_state['values']['vacuspeed_cut_width'] : $left,
    'top'  => ($top == -1) ? $height - (int)$form_state['values']['vacuspeed_cut_height'] : $top,
    'width' => $form_state['values']['vacuspeed_cut_width'],
    'height' => $form_state['values']['vacuspeed_cut_height'],
    'type' => $type,
  );
  $cuts_table = _vacuspeed_tool_create_cuts_table();

  // create surface image (no tiles)
  $image_wrapper = _vacuspeed_tool_create_image_wrapper('surface-only');
  $counts_table = _vacuspeed_tool_create_counts_table();

  // build ajax commands to replace image wrapper and table wrapper and close dialog
  $commands[] = vacuspeed_tool_command_dialog_close();
  $commands[] = ajax_command_replace("#tiled-image-wrapper", drupal_render($image_wrapper));
  $commands[] = ajax_command_replace("#cuts-table-wrapper", drupal_render($cuts_table));
  $commands[] = ajax_command_replace("#counts-table-wrapper", drupal_render($counts_table));

  // disable submit button (no tiles available)
  $commands[] = ajax_command_replace("#edit-vacuspeed-tiling-submit", $session_data['submit_disabled']);

  // return ajax commands
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}

/**
 * Cancel a cut definition process. Clearing all field errors and close dialog.
 *
 * @param $form
 * @param $form_state
 * @return array The array with the ajax commands
 */
function vacuspeed_tool_cut_cancel_callback($form, &$form_state) {
  $session_data = &vacuspeed_tool_session_data();
  $commands = array();

  // close dialog
  $commands[] = vacuspeed_tool_command_dialog_close();

  // restore all changed field descriptions and
  if (isset($session_data['form_item_restore'])) {
    foreach($session_data['form_item_restore'] as $sel => $desc){
      $commands[] = ajax_command_html($sel, $desc);
      $commands[] = ajax_command_css($sel, array('color' => 'inherit'));
    }

  }
  $session_data['form_item_restore'] = array();

  // clear form errors and message stack
  form_clear_error();
  drupal_get_messages('error');

  // return ajax commands
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}

/**
 * Callback function to delete a stored cut definition.
 *
 * @param int $i_cut Index of the cut to be deleted in cuts-array.
 * @param string $type Contains either 'ajax' or 'nojs' depending on the capabilities of the client.
 * @return array Array of ajax commands to replace wrapper sections.
 */
function vacuspeed_tool_cut_delete_callback($i_cut, $type = 'ajax') {
  // delete cut at given index
  $session_data = &vacuspeed_tool_session_data();
  $cuts = &$session_data['cuts'];
  array_splice($cuts, $i_cut, 1);

  // return ajax commands or degrade gracefully to page reload
  if ($type == 'nojs') {
    // reload tiling page
    drupal_goto('layout/tiling');
    return;
  }

  // create table with cuts
  $cuts_table = _vacuspeed_tool_create_cuts_table();

  // create surface image
  $image_wrapper = _vacuspeed_tool_create_image_wrapper('surface-only');
  $counts_table = _vacuspeed_tool_create_counts_table();

  // build ajax commands to replace image wrapper and table wrapper
  $commands = array();
  $commands[] = ajax_command_replace("#tiled-image-wrapper", drupal_render($image_wrapper));
  $commands[] = ajax_command_replace("#cuts-table-wrapper", drupal_render($cuts_table));
  $commands[] = ajax_command_replace("#counts-table-wrapper", drupal_render($counts_table));

  // disable submit button (no tiles available)
  $commands[] = ajax_command_replace("#edit-vacuspeed-tiling-submit", $session_data['submit_disabled']);

  // return ajax commands
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}

/**
 * Creates the wrapper containing all created images (tiled or without tiles)
 *
 * @param string $type  One of the following values: 'surface-only', 'surface-tiled'
 * @param bool $show_double_layer Flag indicating if a second shifted layer has to be calculated
 * @return array Render array of image wrapper containing all created images.
 */
function _vacuspeed_tool_create_image_wrapper($type, $show_double_layer = false) {
  $session_data = &vacuspeed_tool_session_data();
  $width = $session_data['width'];
  $height = $session_data['height'];
  $cuts = $session_data['cuts'];

  // reset counts and create new image(s)
  $images = array();
  $session_data['counts'] = array();

  switch($type) {
    case 'surface-tiled':
      $images = vacuspeed_tool_calculate_tiled_surface($width, $height, $cuts, $show_double_layer);
      break;
    case 'surface-only':
    default:
      $images[] = vacuspeed_tool_create_surface_image($width, $height, $cuts);
      break;
  }

  $markup = '';
  if (count($images) == 0) {
    // no images could be created, return message to ui
    $markup = t('An unrecoverable error occurred during tiles calculation. Please contact your system administrator!');

  } else {
    foreach ($images as $index => $imgPath) {
      $markup .= '<div class="tiled-image tiled-image-' . $index . '" style="position:relative"><img src="' . $imgPath . '">';
      $markup .= ($index == 0) ? _vacuspeed_tool_create_add_button_markup($width, $height) . '</div>' : '</div>';
    }

  }

  // return render array
  return array(
    '#markup' => $markup,
    '#prefix' => '<div id="tiled-image-wrapper" style="color:red">',
    '#suffix' => '</div>',
  );
}


/**
 * Helper function to create the add buttons used to define the cuts.
 *
 * @param int $width  Width of the surface.
 * @param int $height Height of the surface.
 * @return string Markup of the add buttons.
 */
function _vacuspeed_tool_create_add_button_markup($width, $height) {
  // get add button image
  $img_src = drupal_get_path('theme', 'standard_theme') . '/images/plus_green_circle_small.png';
  $img_src = file_create_url($img_src); // make url absolute

  $markup = '';
  for ($i = 0; $i < 9; $i++) {
    $markup .= '<div class="add-button add-button-' . $i . '"><img src="' . $img_src . '" height="32px" width="32px"/></div>';
  }
  return $markup;
}

/**
 * Creates a table of all defined cuts.
 *
 * @return array Render array for the cts table.
 */
function _vacuspeed_tool_create_cuts_table() {
  // serialized cuts array
  $session_data = &vacuspeed_tool_session_data();
  $cuts = $session_data['cuts'];
  $session_data['cuts_serialized'] = serialize($cuts);

  // define table header
  $header = array(
    array('data' => t('Cut-offs'), 'class' => 'column-index'),
    array('data' => t('x'), 'class' => 'column-x'),
    array('data' => t('y'), 'class' => 'column-y'),
    array('data' => t('l'), 'class' => 'column-width'),
    array('data' => t('b'), 'class' => 'column-height'),
    array('data' => t(''), 'class' => 'column-action'),
  );

  // create a row for each stored cut
  $img_src = drupal_get_path('theme', 'standard_theme') . '/images/minus_red_circle_small.png';
  $img_src = file_create_url($img_src); // make url absolute
  $rows = array();

  foreach ($cuts as $index => $cut) {
    $row = array(
      array('data' => $index, 'class' => 'column-index'),
      array('data' => $cut['left'], 'class' => 'column-x'),
      array('data' => $cut['top'], 'class' => 'column-y'),
      array('data' => $cut['width'], 'class' => 'column-width'),
      array('data' => $cut['height'], 'class' => 'column-height'),
      array('data' => l('<img src="' . $img_src . '" height="20" width="20">', 'layout/cut/delete/' . $index . '/nojs/', array('attributes' => array('class' => array('use-ajax')), 'html' => true)), 'class' => 'column-action'),
    );
    $rows[] = $row;
  }

  // set empty row
  if (empty($rows)) {
    $rows[] = array(
      array('data' => t('There are no cut-offs defined!'), 'colspan' => 6)
    );
  }

  // return the table render array
  return array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('id' => 'table-cuts'),
    '#prefix' => '<div id="cuts-table-wrapper">',
    '#suffix' => '</div>',
  );
}

/**
 * Creates the counts table containing all information about the number of tiles used for surface.
 *
 * @return array Render array of counts table
 */
function _vacuspeed_tool_create_counts_table() {
  // get counts array
  $session_data = &vacuspeed_tool_session_data();
  $counts = $session_data['counts'];

  $header = array(
    array('data' => t('Counts'), 'class' => 'column-layer'),
    array('data' => t('100x50'), 'class' => 'column-100x50'),
    array('data' => t('50x50'), 'class' => 'column-50x50'),
    array('data' => t('50x25'), 'class' => 'column-50x25'),
    array('data' => t('25x25'), 'class' => 'column-25x25'),
    array('data' => t('Cut'), 'class' => 'column-cut'),
    array('data' => t('Total'), 'class' => 'column-total'),
  );

  // create rows with stored counts
  $rows = array();
  $row_total = array();
  foreach ($counts as $layer => $tile_counts) {
    $row = array(
      array('data' => $layer == 'totals' ? t('Total') : $layer, 'class' => 'column-layer'),
      array('data' => $tile_counts['100x50'], 'class' => 'column-100x50'),
      array('data' => $tile_counts['50x50'], 'class' => 'column-50x50'),
      array('data' => $tile_counts['50x25'], 'class' => 'column-50x25'),
      array('data' => $tile_counts['25x25'], 'class' => 'column-25x25'),
      array('data' => $tile_counts['CUT'], 'class' => 'column-cut'),
      array('data' => $tile_counts['total'], 'class' => 'column-total'),
    );
    if ($layer == 'totals') {
      $row_total = array(
        'data' => $row,
        'class' => array('row-total'),
      );
      continue;
    }
    $rows[] = $row;

  }

  // set total or empty row
  if (empty($rows)) {
    $rows[] = array(
      array('data' => t('There are no Vacuspeed numbers available!'), 'colspan' => 7)
    );

  } else {
    // add total row to table
    $rows[] = $row_total;

  }

  // return the table render array
  return array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('id' => 'table-counts'),
    '#prefix' => '<div id="counts-table-wrapper">',
    '#suffix' => '</div>',
  );
}


/*-------------------------------------------------------------------------------
 *        Vacuspeed order page
 *-------------------------------------------------------------------------------*/

/**
 * Define the order page.
 */
function vacuspeed_tool_order_page() {
  drupal_set_title('');

  // define page
  $content = array();
  $content['content']['top']['packets_form'] = drupal_get_form('vacuspeed_tool_calc_packets_form');
  $content['content']['top']['packets_table'] = _vacuspeed_tool_create_packets_table();
  $content['content']['top']['#prefix'] = '<div class="region region-top">';
  $content['content']['top']['#suffix'] = '</div>';

  $content['content']['bottom']['packets_form'] = drupal_get_form('vacuspeed_tool_order_form');
  $content['content']['bottom']['#prefix'] = '<div class="region region-bottom">';
  $content['content']['bottom']['#suffix'] = '</div>';

  // return page content
  return $content;
}

/**
 * Creates the order packets form of the vacuspeed layout tool.
 *
 * @param $form
 * @param $form_state
 * @return mixed
 */
function vacuspeed_tool_calc_packets_form($form, &$form_state) {
  $session_data = &vacuspeed_tool_session_data();
  $show_double_layer = (bool)$session_data['show_double_layer'];

  $form['vacuspeed_packets_desc'] = array(
    '#markup' => t('Define the thickness of the Vacuspeed tiles for each layer separately. The number of Vacuspeed packets per format is immediately displayed in the table.'),
    '#prefix' => '<div><p>',
    '#suffix' => '</p></div>',
  );
  $form['vacuspeed_packets_select_thick_1'] = array(
    '#type' => 'select',
    '#title' => t('Thickness layer 1'),
    '#options' => array(
      0 => t('15mm'),
      1 => t('20mm'),
      2 => t('25mm'),
    ),
    '#description' => t('Define the tile thickness of layer 1.'),
    '#default_value' => 0,
    '#ajax' => array(
      'callback' => 'vacuspeed_tool_thickness_add_callback',
      'wrapper'  => 'packets-table-wrapper',
      'effect'   => 'fade',
      'speed'    => 'slow',
    ),
    '#required' => true,
    '#chosen' => false,
  );
  if ($show_double_layer) {
    $form['vacuspeed_packets_select_thick_2'] = array(
      '#type' => 'select',
      '#title' => t('Thickness layer 2'),
      '#options' => array(
        0 => t('15mm'),
        1 => t('20mm'),
        2 => t('25mm'),
      ),
      '#description' => t('Define the tile thickness of layer 2.'),
      '#default_value' => 0,
      '#ajax' => array(
        'callback' => 'vacuspeed_tool_thickness_add_callback',
        'wrapper'  => 'packets-table-wrapper',
        'effect'   => 'fade',
        'speed'    => 'slow',
      ),
      '#required' => true,
      '#chosen' => false,
    );
  }
  return $form;
}

/**
 * Ajax Callback for thickness selection elements.
 */
function vacuspeed_tool_thickness_add_callback($form, &$form_state) {
  $session_data = &vacuspeed_tool_session_data();
  $show_double_layer = (bool)$session_data['show_double_layer'];
  $commands = array();

  // store selection in session variable
  $session_data['thickness_layer_1'] = (int)$form_state['values']['vacuspeed_packets_select_thick_1'];
  $session_data['thickness_layer_2'] = $show_double_layer ? (int)$form_state['values']['vacuspeed_packets_select_thick_2'] : -1;

  // create packet table
  $packets_table = _vacuspeed_tool_create_packets_table();

  // build ajax commands to replace image wrapper and table wrapper and close dialog
  $commands[] = ajax_command_replace("#packets-table-wrapper", drupal_render($packets_table));

  // return ajax commands
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}


/**
 * Creates the table of needed vacuspeed packets for all types of vacuspeed tiles.
 *
 * @return array Render array of the packets table.
 */
function _vacuspeed_tool_create_packets_table() {
  // restore counts
  $session_data = &vacuspeed_tool_session_data();
  $show_double_layer = (bool)$session_data['show_double_layer'];
  $session_data['counts'] = unserialize($session_data['counts_serialized']);
  $options = array('15mm', '20mm', '25mm');
  $thick_1 = $options[$session_data['thickness_layer_1']];
  $thick_2 = $show_double_layer ? $options[$session_data['thickness_layer_2']] : $thick_1;
  $same_thickness_used = $session_data['thickness_layer_1'] == $session_data['thickness_layer_2'];

  $header = array(
      array('data' => t('Layer'), 'class' => 'column-layer'),
      array('data' => t('Thickness (mm)'), 'class' => 'column-thickness'),
      array('data' => t('Format (mm)'), 'class' => 'column-format'),
      array('data' => t('Nr. of tiles'), 'class' => 'column-nr-tiles'),
      array('data' => t('Nr. of packets'), 'class' => 'column-count-packets'),
  );

  // create rows with stored counts
  $rows = array();
  $formats = array(
    '100x50' => '1000x500',
    '50x50' => '500x500',
    '50x25' => '500x250',
    '25x25' => '250x250',
    'CUT' => '125x1000',
  );

  foreach ($formats as $i => $format){
    // set vacuspeed title before first format
    if ($i == '100x50') {
      $rows[] = array(
        array('data' => t('Vacuspeed (10 pieces per packet)'), 'colspan' => 5, 'class' => 'caption'),
      );
    } elseif ($i == 'CUT') {
      // set cut title
      $rows[] = array(
        array('data' => '', 'colspan' => 5),
      );
      $rows[] = array(
        array('data' => t('Vacuspeed CUT (20 pieces per packet)'), 'colspan' => 5, 'class' => 'caption'),
      );
    }

    foreach ($session_data['counts'] as $layer => $tile_counts) {
      // only list tiles format containing tiles
      if ($tile_counts[$i] == 0 && $i != 'CUT') continue;

      // set packet size
      $counts_per_packet =  ($i == 'CUT') ? 20.0 : 10.0;

      if ($same_thickness_used) {
        // add format counts evaluated over all layers
        if ($layer != 'totals') continue;
        $row = array(
          array('data' => t('Total'), 'class' => 'column-layer'),
          array('data' => $thick_1, 'class' => 'column-thickness'),
          array('data' => $format, 'class' => 'column-format'),
          array('data' => $tile_counts[$i], 'class' => 'column-nr-tiles'),
          array('data' => ceil($tile_counts[$i] / $counts_per_packet), 'class' => 'column-count-packets'),
        );

      } else {
        // add format counts for each layer separately
        if ($layer == 'totals') continue;
        $row = array(
          array('data' => $layer, 'class' => 'column-layer'),
          array('data' => strpos($layer, '1') === false ? $thick_2 : $thick_1, 'class' => 'column-thickness'),
          array('data' => $format, 'class' => 'column-format'),
          array('data' => $tile_counts[$i], 'class' => 'column-nr-tiles'),
          array('data' => ceil($tile_counts[$i] / $counts_per_packet), 'class' => 'column-count-packets'),
        );

      }

      $rows[] = $row;
    }

  }

  if (empty($rows)) {
    $rows[] = array(
      array('data' => t('There is no Vacuspeed needed!'), 'colspan' => 5),
    );

  }

  // return the table render array
  return array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('id' => 'table-packets'),
    '#prefix' => '<div id="packets-table-wrapper">',
    '#suffix' => '</div>',
  );
}


/**
 * Creates the form to get an offer or to order vacuspeed tiles.
 *
 * @param $form
 * @param $form_state
 */
function vacuspeed_tool_order_form($form, &$form_state) {
  $form['vacuspeed_order_radios'] = array(
    '#type' => 'radios',
    '#title' => t('Möchten Sie für die berechnete Menge eine Offerte anfragen oder direkt bestellen?'),
    '#title_display' => 'before',
    '#options' => array(0 => t('Offer'), 1 => t('Order')),
    '#default_value' => 0,
    '#required' => TRUE,
  );
  // Besteller
  $form['vacuspeed_order_title'] = array(
    '#type' => 'markup',
    '#markup' => t('Customer'),
    '#prefix' => '<div class="form-region form-region-top-left"><div class="region-inner"><h6>',
    '#suffix' => '</h6>',
  );
  $form['vacuspeed_order_company'] = array(
    '#type' => 'textfield',
    '#title' => t('Company'),
    '#size' => 20,
    '#maxlength' => 30,
    '#required' => TRUE,
  );
  $form['vacuspeed_order_street'] = array(
    '#type' => 'textfield',
    '#title' => t('Street / Nr.'),
    '#size' => 20,
    '#maxlength' => 30,
    '#required' => TRUE,
  );
  $form['vacuspeed_order_city'] = array(
    '#type' => 'textfield',
    '#title' => t('ZIP / City'),
    '#size' => 20,
    '#maxlength' => 30,
    '#required' => TRUE,
  );
  $form['vacuspeed_order_contact'] = array(
    '#type' => 'textfield',
    '#title' => t('Contact'),
    '#size' => 15,
    '#maxlength' => 30,
    '#required' => TRUE,
  );
  $form['vacuspeed_order_contact_email'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail'),
    '#size' => 20,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#suffix' => '</div></div>',
  );

  // Fakturaempfänger
  $form['vacuspeed_invoice_title'] = array(
    '#type' => 'markup',
    '#markup' => t('Invoice address'),
    '#prefix' => '<div class="form-region form-region-top-right"><div class="region-inner"><h6>',
    '#suffix' => '</h6>',
  );
  $form['vacuspeed_invoice_company'] = array(
    '#type' => 'textfield',
    '#title' => t('Company'),
    '#size' => 20,
    '#maxlength' => 30,
  );
  $form['vacuspeed_invoice_street'] = array(
    '#type' => 'textfield',
    '#title' => t('Street / Nr.'),
    '#size' => 20,
    '#maxlength' => 30,
  );
  $form['vacuspeed_invoice_city'] = array(
    '#type' => 'textfield',
    '#title' => t('ZIP / City'),
    '#size' => 20,
    '#maxlength' => 30,
  );
  $form['vacuspeed_invoice_phone'] = array(
    '#type' => 'textfield',
    '#title' => t('Phone'),
    '#size' => 15,
    '#maxlength' => 30,
  );
  $form['vacuspeed_invoice_fax'] = array(
    '#type' => 'textfield',
    '#title' => t('Fax'),
    '#size' => 20,
    '#maxlength' => 50,
    '#suffix' => '</div></div>',
  );

  // Baustelle
  $form['vacuspeed_install_title'] = array(
    '#type' => 'markup',
    '#markup' => t('Installation site'),
    '#prefix' => '<div class="form-region form-region-middle-left"><div class="region-inner"><h6>',
    '#suffix' => '</h6>',
  );
  $form['vacuspeed_install_commission_nr'] = array(
    '#type' => 'textfield',
    '#title' => t('Commission / Nr.'),
    '#size' => 20,
    '#maxlength' => 30,
  );
  $form['vacuspeed_install_street'] = array(
    '#type' => 'textfield',
    '#title' => t('Street / Nr.'),
    '#size' => 20,
    '#maxlength' => 30,
  );
  $form['vacuspeed_install_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Site Identifer'),
    '#size' => 20,
    '#maxlength' => 30,
  );
  $form['vacuspeed_install_city'] = array(
    '#type' => 'textfield',
    '#title' => t('ZIP / City'),
    '#size' => 20,
    '#maxlength' => 30,
  );
  $form['vacuspeed_install_type'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Construction type'),
    '#title_display' => 'before',
    '#options' => array(t('New'), t('Renovation')),
    '#suffix' => '</div></div>',
  );

  // Lieferung
  $form['vacuspeed_delivery_title'] = array(
    '#type' => 'markup',
    '#markup' => t('Delivery to'),
    '#prefix' => '<div class="form-region form-region-middle-right"><div class="region-inner"><h6>',
    '#suffix' => '</h6>',
  );
  $form['vacuspeed_delivery_date'] = array(
    '#type' => 'textfield',
    '#title' => t('Delivery date'),
    '#size' => 20,
    '#maxlength' => 30,
  );
  $form['vacuspeed_delivery_oncall'] = array(
    '#type' => 'checkbox',
    '#title' => t('On call'),
    '#default_value' => 0,
  );
  $form['vacuspeed_delivery_avis'] = array(
    '#type' => 'textfield',
    '#title' => t('AVIS Phone/Mobile'),
    '#size' => 20,
    '#maxlength' => 30,
  );
  $form['vacuspeed_delivery_address'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Delivery address'),
    '#title_display' => 'before',
    '#options' => array(t('Customer'), t('Orderer'), t('Invoice address')),
  );
  $form['vacuspeed_delivery_time'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Delivery time'),
    '#title_display' => 'before',
    '#options' => array(t('Morning'), t('Afternoon'), t('All day')),
    '#suffix' => '</div></div>',
  );

  // Transport
  $form['vacuspeed_transport_type'] = array(
    '#type' => 'checkboxes',
    '#title' => t('<h6>Transport</h6>'),
    '#title_display' => 'before',
    '#options' => array(t('per LKW'), t('Pick up')),
    '#prefix' => '<div class="form-region form-region-bottom-left"><div class="region-inner">',
    '#suffix' => '</div></div>',
  );

  // Ablad
  $form['vacuspeed_unloading_type'] = array(
    '#type' => 'checkboxes',
    '#title' => t('<h6>Unloading</h6>'),
    '#title_display' => 'before',
    '#options' => array(t('LKW elevator'), t('LKW crane')),
    '#prefix' => '<div class="form-region form-region-bottom-middle"><div class="region-inner">',
    '#suffix' => '</div></div>',
  );

  // Zufahrt
  $form['vacuspeed_access_type'] = array(
    '#type' => 'checkboxes',
    '#title' => t('<h6>Access</h6>'),
    '#title_display' => 'before',
    '#options' => array(t('LKW w/o trailer'), t('Semitrailer')),
    '#prefix' => '<div class="form-region form-region-bottom-right"><div class="region-inner">',
    '#suffix' => '</div></div>',
  );

  // Bemerkung
  $form['vacuspeed_order_remark'] = array(
    '#type' => 'textarea',
    '#title' => t('<h6>Remark</h6>'),
    '#maxlength' => 300,
    '#prefix' => '<div class="form-region form-region-final"><div class="region-inner">',
  );
  $form['vacuspeed_order_pdf'] = array(
    '#type' => 'submit',
    '#value' => t('Build PDF'),
    '#attributes' =>  array('class' => array('submit-button')),
    '#suffix' => '</div></div>',
  );

  return $form;
}

/**
 * Order form submit handler.
 */
function vacuspeed_tool_order_form_submit($form, &$form_state) {
  // order or offer
  $session_data = &vacuspeed_tool_session_data();
  $session_data['is_order'] = (bool)$form_state['values']['vacuspeed_order_radios'];
  $session_data['order_form'] = $form;

  // redirect to pdf page
  $form_state['redirect'] = array(
    'layout/order/pdf/FI',
  );
}


/*-------------------------------------------------------------------------------
 *        Vacuspeed pdf page
 *-------------------------------------------------------------------------------*/

/**
 * The menu callback function to create the pdf file.
 *
 * @param string $dest Destination where to send the document. It can take one of the following values:
 * <ul>
 *  <li>I: send the file inline to the browser (default). The plug-in is used if available. The name given by name is used when one selects the "Save as" option on the link generating the PDF.</li>
 *  <li>D: send to the browser and force a file download with the name given by name.</li>
 *  <li>F: save to a local server file with the name given by name.</li>
 *  <li>S: return the document as a string (name is ignored).</li>
 *  <li>FI: equivalent to F + I option</li>
 *  <li>FD: equivalent to F + D option</li>
 *  <li>E: return the document as base64 mime multi-part email attachment (RFC 2045)</li>
 * </ul>
 *
 * @return string the compiled pdf string, if $dest is 'S'
 */
function vacuspeed_tool_order_pdf($dest) {
  // initialize data
  $session_data = &vacuspeed_tool_session_data();
  $pdf_file_path = vacuspeed_tool_result_path() . '/vacuspeed_offer.pdf';
  $pdf_page_start_height = 40;

  // create the pdf with meta data and header / footer callback functions
  $options = array(
    'title' => 'Offertenanfrage',
    'subject' => 'Vacuspeed Tool',
    'author' => 'ZZ Wancor',
    'keywords' => 'ZZWancor, Offerte, Vacuspeed, pdf, Drupal, Pixelgarage',
    'header' => array(
      'callback' => array(
        'function' => 'vacuspeed_tool_order_pdf_header',
        'context' => array(
          'linecolor' => array(167, 0, 20),
        ),
      ),
    ),
    'footer' => array(
      'callback' => array(
        'function' => 'vacuspeed_tool_order_pdf_footer',
        'context' => array(
          'linecolor' => array(167, 0, 20),
        ),
      ),
    ),
  );
  $pdf = tcpdf_document_instance($options);

  // create pdf body
  // 1. page: order form
  $title = $session_data['is_order'] ? t("<h2>Vacuspeed Order</h2>") : t("<h2>Offer application</h2>");
  $pdf->writeHTMLCell(0, 0, PDF_MARGIN_LEFT, $pdf_page_start_height, $title, 0, 1, false, true, '', true);

  // customer part
  //$hidden_elems = array('vacuspeed_order_radios','vacuspeed_order_title','vacuspeed_order_company','vacuspeed_order_street','vacuspeed_order_city','vacuspeed_order_contact','vacuspeed_order_contact_email','vacuspeed_invoice_title','vacuspeed_invoice_company','vacuspeed_invoice_street','vacuspeed_invoice_city','vacuspeed_invoice_phone','vacuspeed_invoice_fax','vacuspeed_install_title','vacuspeed_install_commission_nr','vacuspeed_install_street','vacuspeed_install_id','vacuspeed_install_city','vacuspeed_install_type','vacuspeed_delivery_title','vacuspeed_delivery_date','vacuspeed_delivery_oncall','vacuspeed_delivery_avis','vacuspeed_delivery_address','vacuspeed_delivery_time','vacuspeed_transport_type','vacuspeed_unloading_type','vacuspeed_access_type','vacuspeed_order_remark','vacuspeed_order_pdf');
  $hidden_elems = array('vacuspeed_order_radios','vacuspeed_invoice_title','vacuspeed_invoice_company','vacuspeed_invoice_street','vacuspeed_invoice_city','vacuspeed_invoice_phone','vacuspeed_invoice_fax',
                        'vacuspeed_install_title','vacuspeed_install_commission_nr','vacuspeed_install_street','vacuspeed_install_id','vacuspeed_install_city','vacuspeed_install_type',
                        'vacuspeed_delivery_title','vacuspeed_delivery_date','vacuspeed_delivery_oncall','vacuspeed_delivery_avis','vacuspeed_delivery_address','vacuspeed_delivery_time',
                        'vacuspeed_transport_type','vacuspeed_unloading_type','vacuspeed_access_type','vacuspeed_order_remark','vacuspeed_order_pdf');
  $pdf->DrupalForm($session_data['order_form'], PDF_MARGIN_LEFT, $pdf_page_start_height-5, 85, false, $hidden_elems);

  // invoice part
  $hidden_elems = array('vacuspeed_order_radios','vacuspeed_order_title','vacuspeed_order_company','vacuspeed_order_street','vacuspeed_order_city','vacuspeed_order_contact','vacuspeed_order_contact_email',
                        'vacuspeed_install_title','vacuspeed_install_commission_nr','vacuspeed_install_street','vacuspeed_install_id','vacuspeed_install_city','vacuspeed_install_type',
                        'vacuspeed_delivery_title','vacuspeed_delivery_date','vacuspeed_delivery_oncall','vacuspeed_delivery_avis','vacuspeed_delivery_address','vacuspeed_delivery_time',
                        'vacuspeed_transport_type','vacuspeed_unloading_type','vacuspeed_access_type','vacuspeed_order_remark','vacuspeed_order_pdf');
  $pdf->DrupalForm($session_data['order_form'], PDF_MARGIN_LEFT+95, $pdf_page_start_height-5, 85, false, $hidden_elems);

  // install part
  $hidden_elems = array('vacuspeed_order_radios','vacuspeed_order_title','vacuspeed_order_company','vacuspeed_order_street','vacuspeed_order_city','vacuspeed_order_contact','vacuspeed_order_contact_email',
                        'vacuspeed_invoice_title','vacuspeed_invoice_company','vacuspeed_invoice_street','vacuspeed_invoice_city','vacuspeed_invoice_phone','vacuspeed_invoice_fax',
                        'vacuspeed_delivery_title','vacuspeed_delivery_date','vacuspeed_delivery_oncall','vacuspeed_delivery_avis','vacuspeed_delivery_address','vacuspeed_delivery_time',
                        'vacuspeed_transport_type','vacuspeed_unloading_type','vacuspeed_access_type','vacuspeed_order_remark','vacuspeed_order_pdf');
  $pdf->DrupalForm($session_data['order_form'], PDF_MARGIN_LEFT, $pdf_page_start_height+45, 85, false, $hidden_elems);

  // delivery part
  $hidden_elems = array('vacuspeed_order_radios','vacuspeed_order_title','vacuspeed_order_company','vacuspeed_order_street','vacuspeed_order_city','vacuspeed_order_contact','vacuspeed_order_contact_email',
                        'vacuspeed_invoice_title','vacuspeed_invoice_company','vacuspeed_invoice_street','vacuspeed_invoice_city','vacuspeed_invoice_phone','vacuspeed_invoice_fax',
                        'vacuspeed_install_title','vacuspeed_install_commission_nr','vacuspeed_install_street','vacuspeed_install_id','vacuspeed_install_city','vacuspeed_install_type',
                        'vacuspeed_transport_type','vacuspeed_unloading_type','vacuspeed_access_type','vacuspeed_order_remark','vacuspeed_order_pdf');
  $pdf->DrupalForm($session_data['order_form'], PDF_MARGIN_LEFT+95, $pdf_page_start_height+45, 85, false, $hidden_elems);

  // transport part
  $hidden_elems = array('vacuspeed_order_radios','vacuspeed_order_title','vacuspeed_order_company','vacuspeed_order_street','vacuspeed_order_city','vacuspeed_order_contact','vacuspeed_order_contact_email',
                        'vacuspeed_invoice_title','vacuspeed_invoice_company','vacuspeed_invoice_street','vacuspeed_invoice_city','vacuspeed_invoice_phone','vacuspeed_invoice_fax',
                        'vacuspeed_install_title','vacuspeed_install_commission_nr','vacuspeed_install_street','vacuspeed_install_id','vacuspeed_install_city','vacuspeed_install_type',
                        'vacuspeed_delivery_title','vacuspeed_delivery_date','vacuspeed_delivery_oncall','vacuspeed_delivery_avis','vacuspeed_delivery_address','vacuspeed_delivery_time',
                        'vacuspeed_unloading_type','vacuspeed_access_type','vacuspeed_order_remark','vacuspeed_order_pdf');
  $pdf->DrupalForm($session_data['order_form'], PDF_MARGIN_LEFT, $pdf_page_start_height+140, 55, false, $hidden_elems);

  // unloading part
  $hidden_elems = array('vacuspeed_order_radios','vacuspeed_order_title','vacuspeed_order_company','vacuspeed_order_street','vacuspeed_order_city','vacuspeed_order_contact','vacuspeed_order_contact_email',
                        'vacuspeed_invoice_title','vacuspeed_invoice_company','vacuspeed_invoice_street','vacuspeed_invoice_city','vacuspeed_invoice_phone','vacuspeed_invoice_fax',
                        'vacuspeed_install_title','vacuspeed_install_commission_nr','vacuspeed_install_street','vacuspeed_install_id','vacuspeed_install_city','vacuspeed_install_type',
                        'vacuspeed_delivery_title','vacuspeed_delivery_date','vacuspeed_delivery_oncall','vacuspeed_delivery_avis','vacuspeed_delivery_address','vacuspeed_delivery_time',
                        'vacuspeed_transport_type','vacuspeed_access_type','vacuspeed_order_remark','vacuspeed_order_pdf');
  $pdf->DrupalForm($session_data['order_form'], PDF_MARGIN_LEFT+60, $pdf_page_start_height+140, 57, false, $hidden_elems);

  // access part
  $hidden_elems = array('vacuspeed_order_radios','vacuspeed_order_title','vacuspeed_order_company','vacuspeed_order_street','vacuspeed_order_city','vacuspeed_order_contact','vacuspeed_order_contact_email',
                        'vacuspeed_invoice_title','vacuspeed_invoice_company','vacuspeed_invoice_street','vacuspeed_invoice_city','vacuspeed_invoice_phone','vacuspeed_invoice_fax',
                        'vacuspeed_install_title','vacuspeed_install_commission_nr','vacuspeed_install_street','vacuspeed_install_id','vacuspeed_install_city','vacuspeed_install_type',
                        'vacuspeed_delivery_title','vacuspeed_delivery_date','vacuspeed_delivery_oncall','vacuspeed_delivery_avis','vacuspeed_delivery_address','vacuspeed_delivery_time',
                        'vacuspeed_transport_type','vacuspeed_unloading_type','vacuspeed_order_remark','vacuspeed_order_pdf');
  $pdf->DrupalForm($session_data['order_form'], PDF_MARGIN_LEFT+123, $pdf_page_start_height+140, 57, false, $hidden_elems);

  // remarks
  $hidden_elems = array('vacuspeed_order_radios','vacuspeed_order_title','vacuspeed_order_company','vacuspeed_order_street','vacuspeed_order_city','vacuspeed_order_contact','vacuspeed_order_contact_email',
                        'vacuspeed_invoice_title','vacuspeed_invoice_company','vacuspeed_invoice_street','vacuspeed_invoice_city','vacuspeed_invoice_phone','vacuspeed_invoice_fax',
                        'vacuspeed_install_title','vacuspeed_install_commission_nr','vacuspeed_install_street','vacuspeed_install_id','vacuspeed_install_city','vacuspeed_install_type',
                        'vacuspeed_delivery_title','vacuspeed_delivery_date','vacuspeed_delivery_oncall','vacuspeed_delivery_avis','vacuspeed_delivery_address','vacuspeed_delivery_time',
                        'vacuspeed_transport_type','vacuspeed_unloading_type','vacuspeed_access_type');
  $pdf->DrupalForm($session_data['order_form'], PDF_MARGIN_LEFT, $pdf_page_start_height+175, 0, false, $hidden_elems);

  $pdf->AddPage();

  // 2.page: table of vacuspeed packets to order
  $table = _vacuspeed_tool_create_packets_table();

  $pdf->writeHTMLCell(0, 0, PDF_MARGIN_LEFT, $pdf_page_start_height, t("<h2>Number of tiles and packets</h2>"), 0, 1, false, true, '', true);
  $pdf->DrupalTable($table, PDF_MARGIN_LEFT, $pdf_page_start_height+10);
  $pdf->AddPage();

  // 3. - last page: Layer images
  $show_double_layer = $session_data['show_double_layer'];
  $path = vacuspeed_tool_result_path();
  $pdf->writeHTMLCell(0, 0, PDF_MARGIN_LEFT, $pdf_page_start_height, t("<h2>Layer 1</h2>"), 0, 1, false, true, '', true);
  $pdf->Image($path . "/img_layer_1.jpg", PDF_MARGIN_LEFT, $pdf_page_start_height+10, 180, 0, '', '', 'N', 1, 300);

  if ($show_double_layer) {
    $pdf->AddPage();
    $pdf->writeHTMLCell(0, 0, PDF_MARGIN_LEFT, $pdf_page_start_height, t("<h2>Layer 2</h2>"), 0, 1, false, true, '', true);
    $pdf->Image($path . "/img_layer_2.jpg", PDF_MARGIN_LEFT, $pdf_page_start_height+10, 180, 0, '', '', 'N', 1, 300);
  }

  // output pdf as defined in calling url
  $dest = empty($dest) ? 'FI' : $dest;
  $pdf->lastPage();
  $pdf->Output($pdf_file_path, $dest);

}

/**
 * The header callback function to define a specific pdf header.
 *
 * @param $pdf_document int A reference to the pdf document.
 * @param $context array Additional data for the header creation.
 */
function vacuspeed_tool_order_pdf_header(&$pdf_document, $context) {
  // Set title
  $pdf_document->SetFont('helvetica', 'B', 14);
  $pdf_document->SetXY(PDF_MARGIN_LEFT, 15);
  $pdf_document->Cell(0, 0, t('Vacuspeed Layout-Tool'), 0, 0, 'L', false, '', 0, false, 'C', 'C');

  // draw Logo
  $image_file = K_PATH_IMAGES . PDF_HEADER_LOGO;
  $pdf_document->Image($image_file, 5, 5, 30, '', '', '', 'T', false, 300, 'R');

  // draw bottom line
  $linestyle = array('width' => 0.7, 'cap' => 'square', 'color' => $context['linecolor']);
  $pdf_document->Line(PDF_MARGIN_LEFT, 26, $pdf_document->getPageWidth()-PDF_MARGIN_RIGHT, 26, $linestyle);
}

/**
 * The footer callback function to define a specific pdf footer.
 *
 * @param $pdf_document int A reference to the pdf document.
 * @param $context array Additional data for the footer creation.
 */
function vacuspeed_tool_order_pdf_footer(&$pdf_document, $context) {
  $footer_height = 15;
  $pdf_document->SetFont('helvetica', 'I', 6);

  // address on left side
  $pdf_document->SetY(-$footer_height + 2);
  $address = t("ZZ Wancor AG, Althardstrasse 5, CH-8105 Regensdorf\nTel +41 (0)44 871 32 32\nFax +41 (0)44 871 32 90");
  $pdf_document->MultiCell(0, $footer_height, $address, 0, 'L');

  // MWST / email / website in the middle
  $pdf_document->SetY(-$footer_height + 2);
  $address2 = t("zzw@zzwancor.ch\nwww.zzwancor.ch\nMWSt Nr./ TVA Nr. 131 560");
  $pdf_document->MultiCell(0, $footer_height, $address2, 0, 'C');

  // Page number / total right aligned
  $pdf_document->SetY(-$footer_height + 2);
  $page_number = t('Page ').$pdf_document->getAliasNumPage().'/'.$pdf_document->getAliasNbPages();
  $pdf_document->Cell(0, 10, $page_number, 0, 0, 'R', false, '', 0, false, 'T', 'C');

  // draw top line
  $linestyle = array('width' => 0.7, 'cap' => 'square', 'color' => $context['linecolor']);
  $height = $pdf_document->getPageHeight() - $footer_height;
  $pdf_document->Line(PDF_MARGIN_LEFT, $height, $pdf_document->getPageWidth()-PDF_MARGIN_RIGHT, $height, $linestyle);
}
