<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 14.12.13
 * Time: 16:23
 */

/**
 * This class implements the responsive proximity widget, that defines a grid of X x Y cells,
 * which can be animated by the proximity of the mouse (or finger touch).
 */
class RWProximityWidget extends ResponsiveWidget {

  public static function getType() {
    return 'proximity';
  }

  protected function defaultSettings() {
    return array(
      'nx' => 10,
      'ny' => 6,
      'start_scale' => 1.0,
      'end_scale' => 1.3,
      'start_opacity' => 0.3,
      'end_opacity' => 0.9,
      'extent' => 80,
    );
  }

  /**
   * Defines the content of all cells in the grid.
   * @var $cellDefinitions
   */
  protected $cellDefinitions;

  /**
   * Returns an array of all cell definitions.
   */
  public function getCellDefinitions() {
    return $this->cellDefinitions;
  }

  /**
   * Constructor of the RWProximityWidget class.
   */
  public function __construct($rw) {
    parent::__construct($rw);

    // set cell definitions, if not available
    if (!isset($this->settings['wrapper']['cell_definitions']) || empty($this->settings['wrapper']['cell_definitions'])) {
      $this->settings['wrapper']['cell_definitions'] = array(
        0 => array(), // add an empty cell definition for a new proximity widget
      );
    }
    $this->cellDefinitions = &$this->settings['wrapper']['cell_definitions'];
  }

  /**
   * Creates the settings form of the proximity widget and manages cell definitions (created via ajax callbacks).
   *
   * @param array $form_state The form state array.
   * @return array A renderable array defining the settings form of the proximity widget
   */
  public function settingsForm(&$form_state) {
    // get the proximity form
    $form = $this->proximityForm();

    // If the "Add cell definition" button was clicked, this form is rebuild with the current form state.
    // Increment the number of cell definitions by one.
    if (isset($form_state['cell_definitions_count']) && $form_state['cell_definitions_count'] > count($this->cellDefinitions)) {
      $this->cellDefinitions[] = array();
    }
    $form_state['cell_definitions_count'] = count($this->cellDefinitions);

    // If the "Remove" button was clicked for a cell definition, we need to remove that item
    // from the form.
    if (isset($form_state['cell_definition_to_remove'])) {
      unset($this->cellDefinitions[$form_state['cell_definition_to_remove']]);
      unset($form_state['cell_definition_to_remove']);
      $form_state['cell_definitions_count']--;
    }

    // build cell definitions form (table).
    foreach ($this->cellDefinitions as $index => $cell_definition) {
      $cell_definition['index'] = $index;
      $form['wrapper']['cell_definitions'][$index] = $this->cellDefinitionForm($cell_definition);
    }

    // If there's only one cell definition, hide remove button.
    if (count($this->cellDefinitions) == 1) $form['wrapper']['cell_definitions'][0]['remove']['#access'] = false;

    // return the complete form
    return $form;
  }


  /**
   * Creates the settings form of the proximity widget.
   *
   * @return array A renderable array defining the settings form of the proximity widget
   */
  private function proximityForm() {
    $form = array();

    // define the cell grid
    $form['nx'] = array (
      '#type' => 'textfield',
      '#title' => t('Number of horizontal cells'),
      '#size' => 5,
      '#description'  => t('Define the number of horizontal cells in the proximity effect widget.'),
      '#default_value' => $this->settings['nx'],
      '#weight' => 1,
      '#required' => TRUE,
    );
    $form['ny'] = array (
      '#type' => 'textfield',
      '#title' => t('Number of vertical cells'),
      '#size' => 5,
      '#description'  => t('Define the number of vertical cells in the proximity effect widget.'),
      '#default_value' => $this->settings['ny'],
      '#weight' => 2,
      '#required' => TRUE,
    );
    // define proximity effect
    $form['start_scale'] = array (
      '#type' => 'textfield',
      '#title' => t('Initial scale factor'),
      '#size' => 5,
      '#description'  => t('Define the initial scale factor for the cells in the proximity effect widget.'),
      '#default_value' => $this->settings['start_scale'],
      '#weight' => 3,
      '#required' => TRUE,
    );
    $form['end_scale'] = array (
      '#type' => 'textfield',
      '#title' => t('Final scale factor'),
      '#size' => 5,
      '#description'  => t('Define the final scale factor for the cells in the proximity effect widget.'),
      '#default_value' => $this->settings['end_scale'],
      '#weight' => 4,
      '#required' => TRUE,
    );
    $form['start_opacity'] = array (
      '#type' => 'textfield',
      '#title' => t('Initial opacity'),
      '#size' => 5,
      '#description'  => t('Define the initial opacity for the cells in the proximity effect widget [0,1].'),
      '#default_value' => $this->settings['start_opacity'],
      '#weight' => 5,
      '#required' => TRUE,
    );
    $form['end_opacity'] = array (
      '#type' => 'textfield',
      '#title' => t('Final opacity'),
      '#size' => 5,
      '#description'  => t('Define the final opacity for the cells in the proximity effect widget [0,1].'),
      '#default_value' => $this->settings['end_opacity'],
      '#weight' => 6,
      '#required' => TRUE,
    );
    $form['extent'] = array (
      '#type' => 'textfield',
      '#title' => t('Extent of proximity effect'),
      '#size' => 5,
      '#description'  => t('Define the extent of the proximity effect in the widget (unit = px).'),
      '#default_value' => $this->settings['extent'],
      '#weight' => 7,
      '#required' => TRUE,
    );

    // Add a wrapper for the cell definitions and "Add Cell Definition" button.
    $form['wrapper'] = array(
      '#prefix' => '<div id="proximity-cell-definition-wrapper">',
      '#suffix' => '</div>',
      '#weight' => 8,
    );

    $form['wrapper']['desc'] = array(
      '#type' => 'item',
      '#title' => t('Cell definitions'),
      '#description' => t('Define a title and description as well as a hyperlink for each cell in the grid of the proximity widget. A cell can be identified by ' .
        'its x- and y-position in the grid. The top left cell has position (x=0, y=0). A value of -1 for the x- and/or y-position means, the cell definition is set ' .
        'for all cells in the corresponding row (x, y = -1) or column (x = -1, y). If you want to add images to a cell, you can do that in your style-sheet adding ' .
        'the following selector for a specific cell: #proximity-cell-definitions .cell-x-y { background-image: url(...); }.'),
      '#weight' => -1,
    );

    $form['wrapper']['cell_definitions'] = array(
      '#tree' => TRUE,  // keep cell definition hierarchy in form_states
      '#prefix' => '<div id="proximity-cell-definitions">',
      '#suffix' => '</div>',
      '#weight' => 0,
      '#theme' => 'proximity_form_cell_definitions',
    );

    $form['wrapper']['cell_definitions_add'] = array(
      '#type' => 'submit',
      '#prefix' => '<div id="add-cell-definition-button">',
      '#suffix' => '</div>',
      '#value' => t('Add cell definition'),
      '#attributes' => array('class' => array('cell-definition-add'), 'title' => t('Click here to add more cell definitions.')),
      '#weight' => 1,
      '#submit' => array('proximity_add_cell_definition_submit'),
      '#ajax' => array(
        'callback' => 'proximity_ajax_callback',
        'wrapper' => 'proximity-cell-definitions',
        'effect' => 'fade',
      ),
    );

    return $form;
  }

  /**
   * Returns a form for the cell definition, that is added themed as row of a cell definition table.
   *
   * @param array $cellDefinition. The cell definition to be filled into the returned form.
   * @return array $form. The cell definition form.
   */
  private function cellDefinitionForm(array $cellDefinition) {
    $form['#tree'] = TRUE;
    $index = $cellDefinition['index'];

    // defines the weight of the row in the table (draggable)
    $form['weight'] = array(
      '#type' => 'weight',
      '#default_value' => isset($cellDefinition['weight']) ? $cellDefinition['weight'] : $index-100,
      '#delta' => 100,
      '#weight' => -10,
    );

    // defines the x-position of the cell (-1 means valid for all cells)
    $x_pos = isset($cellDefinition['x_pos']) ? $cellDefinition['x_pos'] : -1;
    $form['x_pos'] = array(
      '#type' => 'textfield',
      '#size' => 4,
      '#default_value' => $x_pos,
      '#element_validate' => array('element_validate_integer'),
      '#weight' => -8,
    );

    // defines the y-position of the cell (-1 means valid for all cells)
    $y_pos = isset($cellDefinition['y_pos']) ? $cellDefinition['y_pos'] : -1;
    $form['y_pos'] = array(
      '#type' => 'textfield',
      '#size' => 4,
      '#default_value' => $y_pos,
      '#element_validate' => array('element_validate_integer'),
      '#weight' => -7,
    );

    // defines cell name
    $name = 'cell';
    if (is_numeric($x_pos) && $x_pos >= 0) $name .= '-x' . $x_pos;
    if (is_numeric($y_pos) && $y_pos >= 0) $name .= '-y' . $y_pos;
    $form['name'] = array(
      '#type' => 'item',
      '#title' => $name,
      '#weight' => -9,
    );
    $form['cell'] = array(
      '#type' => 'hidden',
      '#value' => $name,
    );

    // defines the title of the cell description
    $form['title'] = array(
      '#type' => 'textfield',
      '#size' => 32,
      '#default_value' => isset($cellDefinition['title']) ? $cellDefinition['title'] : '',
      '#weight' => -6,
    );

    // defines the cell's description
    $form['description'] = array(
      '#type' => 'textarea',
      '#rows' => 3,
      '#resizable' => false,
      '#default_value' => isset($cellDefinition['description']) ? $cellDefinition['description'] : '',
      '#weight' => -5,
    );

    // defines the cell's link
    $form['link'] = array(
      '#type' => 'textarea',
      '#rows' => 3,
      '#resizable' => false,
      '#default_value' => isset($cellDefinition['link']) ? $cellDefinition['link'] : '',
      '#weight' => -4,
    );

    $form['remove'] = array(
      '#type' => 'submit',
      '#name' => $index,
      '#value' => t('Delete'),
      '#attributes' => array('class' => array('cell-definition-remove'), 'title' => t('Click here to delete this cell definition.')),
      '#submit' => array('proximity_remove_cell_definition_submit'),
      '#ajax' => array(
        'callback' => 'proximity_ajax_callback',
        'wrapper' => 'proximity-cell-definitions',
        'effect' => 'fade',
        'method' => 'replace',
      ),
      '#weight' => -3,
    );

    return $form;
  }


  /**
   * Validates the proximity widget settings.
   */
  public function validateSettingsForm(&$settings) {
    // x_pos/y_pos is validated by element validation

    // update cell definition names
    foreach($settings['wrapper']['cell_definitions'] as &$cell_definition) {
      $cell_definition['cell'] = 'cell';
      $x_pos = $cell_definition['x_pos'];
      $y_pos = $cell_definition['y_pos'];
      if (is_numeric($x_pos) && $x_pos >= 0) $cell_definition['cell'] .= '-x' . $x_pos;
      if (is_numeric($y_pos) && $y_pos >= 0) $cell_definition['cell'] .= '-y' . $y_pos;
    }

  }

  /**
   * Returns the theme registration array.
   * @return array|void
   */
  public function getSettingsFormThemes() {
    return array(
      'proximity_form_cell_definitions' => array(
        // defines render element key in associative array passed to theme function
        'render element' => 'cell_definitions',
        'path' => drupal_get_path('module', 'responsive_widgets') . '/widgets',
        'file' => 'RWProximityWidget.inc',
      ),
    );
  }

  /**
   * Creates the markup for the proximity widget including the cell grid and a wrapper around it.
   *
   * @return array $renderable_widget The render array describing the markup of the proximity widget.
   */
  public function render() {
    // define the responsive grid depending on the given cell numbers and the widget side ratio
    $ratio = (float)$this->hw_ratio;
    $nx    = (int)$this->settings['nx'];
    $ny    = (int)$this->settings['ny'];
    $width = $this->floorDec(100/$nx, 7);
    $height= $this->floorDec(100/$ny, 7) * $ratio;
    $style = 'width:'. $width . '%;height:' . $height . '%;';

    // make an associative array from cell definitions
    $cell_definitions = array();
    foreach($this->cellDefinitions as $cell_definition) {
      $cell_definitions[$cell_definition['cell']] = $cell_definition;
    }

    // create a responsive cover image
    $cells[] = array(
      'data' => '<div class="cover-image" role="img" aria-label="Cover image" style="height:0;padding-bottom:' . $ratio * 100 . '%;"></div>'
    );

    // create the cell grid
    for($y = 0; $y < $ny; $y++) {
      for($x = 0; $x < $nx; $x++) {
        // check if cell definition for this cell exists (more specific definition overrides general one)
        $grid_row = 'cell-x' . $x;
        $grid_col = 'cell-y' . $y;
        $grid_cell = 'cell-x' . $x . '-y' . $y;
        $cell_definition = NULL;
        $cell_description = '';
        $link = '';
        if (isset($cell_definitions['cell'])) {
          // first: cell definition valid for all cells
          $cell_definition = $cell_definitions['cell'];
        }
        if (isset($cell_definitions[$grid_row])) {
          // second: cell definition valid for grid row
          $cell_definition = $cell_definitions[$grid_row];
        }
        if (isset($cell_definitions[$grid_col])) {
          // third: cell definition valid for grid column
          $cell_definition = $cell_definitions[$grid_col];
        }
        if (isset($cell_definitions[$grid_cell])) {
          // last: cell definition valid for cell only
          $cell_definition = $cell_definitions[$grid_cell];
        }
        if (isset($cell_definition)) {
          $title = !empty($cell_definition['title']) ? $cell_definition['title'] : '';
          $description = !empty($cell_definition['description']) ? $cell_definition['description'] : '';
          $link = !empty($cell_definition['link']) ? $cell_definition['link'] : '';

          if (!empty($title) ||!empty($description)) {
            $cell_description = '<div class="cell-description"><h3>' . $title .'</h3><p>' . $description . '</p></div>';
          }
        }

        // define a responsive cell with alternate text and image role
        $html  = '<div class="cell cell-' . $x . '-' . $y . '" role="img" aria-label="Proximity cell" ' .
          'style="height:0;padding-bottom:' . $height / $width * 100 . '%;"></div>';
        $html .= $cell_description;
        $cell  = array (
          // render array for li element (data = content, any other key is treated as attribute)
          'data'  => !empty($link) ? l($html, $link, array('html' => TRUE, 'attributes' => array('target' => '_blank'))) : $html,
          'style' => $style
        );
        $cells[] = $cell;
      }
    }

    // create the render array for the grid
    $path = drupal_get_path('module', 'responsive_widgets');
    $renderable_widget = array(
      '#theme' => 'item_list',
      '#type' => 'ul',  // The default
      '#items' => $cells,
      '#attributes' => array('id' => 'pe-cell-container', 'class' => array('pe-cell-container')),
      '#attached' => array(
        // add the css and js files needed for the widget
        'css' => array($path . '/css/proximity.css'),
        'js'  => array($path . '/js/jquery.proximity.js', $path . '/js/proximity_zoom.js'),
      )
    );

    return $renderable_widget;
  }

  /**
   * Returns an array of all proximity settings, that have to be available in JavaScript.
   * Cast the values to the correct type.
   *
   * @return array The array of all widget settings, that have to be available in JavaScript.
   */
  public function getJSSettings() {
    $pe_settings = array(
      'startScale'  => (float)$this->settings['start_scale'],
      'endScale'    => (float)$this->settings['end_scale'],
      'startOpacity'=> (float)$this->settings['start_opacity'],
      'endOpacity'  => (float)$this->settings['end_opacity'],
      'extent'      => (int)$this->settings['extent'],
    );

   return array('rwProximity' => $pe_settings);
  }


  /**
   * Helper function to floor the given number to $decimals digits.
   *
   * @param float $number
   * @param int $decimals
   * @return float
   */
  private function floorDec($number, $decimals=2) {
    return floor(pow(10,$decimals)*$number)/pow(10,$decimals);
  }

}


/*
 * Implement theme functions here
 */

/**
 * Themes the table of cell definitions in the proximity form.
 * @param $variables
 * @return string
 */
function theme_proximity_form_cell_definitions($variables) {
  // get the element to be themed (see render element key in theme function definition)
  $cell_definitions = $variables['cell_definitions'];

  // make rows draggable defining value of cell definition weight
  drupal_add_tabledrag('proximity-cell-definition-table', 'order', 'sibling', 'cell-definition-weight');
  $rows = array();
  $header = array(
    t('Cell'),
    t('Weight'),
    t('X-Pos'),
    t('Y-Pos'),
    t('Title'),
    t('Description'),
    t('Link'),
    t('Operations'),
  );

  foreach (element_children($cell_definitions) as $key) {
    $cell_definition = &$cell_definitions[$key];
    $cell_definition['weight']['#attributes']['class'] = array('cell-definitions-weight');

    // cell definition fields
    $cell_definition_fields = array(
      array('data' => drupal_render($cell_definition['name']), 'class' => array('cell-definition-name')),
      array('data' => drupal_render($cell_definition['weight']), 'class' => array('cell-definition-weight')),
      array('data' => drupal_render($cell_definition['x_pos']), 'class' => array('cell-definition-xpos')),
      array('data' => drupal_render($cell_definition['y_pos']), 'class' => array('cell-definition-ypos')),
      array('data' => drupal_render($cell_definition['title']), 'class' => array('cell-definition-title')),
      array('data' => drupal_render($cell_definition['description']), 'class' => array('cell-definition-description')),
      array('data' => drupal_render($cell_definition['link']), 'class' => array('cell-definition-link')),
      array('data' => drupal_render($cell_definition['remove']), 'class' => array('cell-definition-remove')),
    );

    // Build the table row.
    $row = array(
      'data' => $cell_definition_fields,
      'class' => array('draggable'),
    );

    // Add additional attributes to the row, such as a class for this row.
    if (isset($cell_definition['#attributes'])) {
      $row = array_merge($row, $cell_definition['#attributes']);
    }
    $rows[] = $row;
  }

  $build['cell_definitions'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('id' => 'proximity-cell-definition-table'),
    '#weight' => -1,
  );
  $build['#attached']['css'][] = drupal_get_path('module', 'responsive_widgets') . '/css/cell-definitions-admin.css';

  $output = drupal_render($build);
  return $output;
}


/*
 * Implement alternative submit and ajax callbacks
 */

/**
 * Ajax callback for the add and remove cell definition buttons.
 */
function proximity_ajax_callback($form, $form_state) {
  // return the updated cell definitions form
  $form_cell_definitions = $form['settings']['proximity']['wrapper']['cell_definitions'];
  return $form_cell_definitions;
}

/**
 * Handles the addition of a cell definition.
 * @param $form
 * @param $form_state
 */
function proximity_add_cell_definition_submit($form, &$form_state) {
  // Increment the number of cell definitions to be rendered.
  $form_state['cell_definitions_count']++;
  $form_state['rebuild'] = true;  // rebuilds form and sends it back
}

/**
 * Handles the removal of a cell definition.
 * @param $form
 * @param $form_state
 */
function proximity_remove_cell_definition_submit($form, &$form_state) {
  // get the clicked button's name as index
  $index = $form_state['clicked_button']['#name'];
  $form_state['cell_definition_to_remove'] = $index;
  $form_state['rebuild'] = true;  // rebuilds form and sends it back
}


