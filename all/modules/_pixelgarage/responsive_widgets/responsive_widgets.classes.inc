<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 13.12.13
 * Time: 17:58
 */

interface RenderableResponsiveWidget {

  /**
   * Returns the short type name of the widget plugin, e.g. 'proximity'.
   */
  public static function getType();

  /**
   * Returns the widget title.
   */
  public function getTitle();

  /**
   * Renders the widget markup.
   */
  public function render();

  /**
   * Gets an associative array containing all widget settings that have to be accessible in JavaScript (Drupal.settings).
   * The key of the widget settings array has to be unique in Drupal.settings and should therefore start with rw...
   *
   * An array to be returned could look like this:
   * @code
   * array('rwProximity' => array(...));
   * @endcode
   *
   * @return array
   */
  public function getJSSettings();

  /**
   * Returns an array of all theme function, that has to be registered by Drupal.
   * This method is called by the responsive_widgets_theme() method.
   *
   * Implement the registered theme function in your plugins.inc file and don't forget to
   * add this information in the registration array of the theme function, as follows:
   * @code
   * array('your_theme_function' => array(
   *   'render element' => 'element',
   *   'path' => drupal_get_path('module', 'responsive_widgets') . '/widgets',
   *   'file' => 'your_plugin_file.inc',
   *   ),
   * );
   * @endcode
   */
  public function getSettingsFormThemes();

}


/**
 * Abstract base class for a renderable responsive widget.
 */
abstract class ResponsiveWidget implements RenderableResponsiveWidget {

  /**
   * Used as the block title of the widget.
   * @var string
   */
  protected $title;

  /**
   * Defines the height to width ratio of the rendered responsive widget.
   * @var float
   */
  protected $hw_ratio;

  /**
   * An array containing all configuration settings that defines the widget, specific
   * to its type.
   * @var array
   */
  protected $settings;

  /**
   * Returns the widget title.
   */
  public function getTitle() {
    return $this->title;
  }

  /**
   * Returns the widget's height to width ratio.
   */
  public function getHWRatio() {
    return $this->hw_ratio;
  }

  /**
   * Returns an array of the widget specific settings. The value of a specific
   * form element can be accessed by the form element key.
   */
  public function getSettings() {
    return $this->settings;
  }

  /**
   * Constructor of the ResponsiveWidget class.
   */
  public function __construct($rw) {
    $this->title = isset($rw->title) ? $rw->title : '';
    $this->hw_ratio = isset($rw->hw_ratio) ? $rw->hw_ratio : 0.618;
    $this->settings = array_merge($this->defaultSettings(), isset($rw->settings) ? $rw->settings : array());
  }


  /**
   * Instantiate a responsive widget of the given type and initialize it with the given data object.
   *
   * @param $type string The type of the widget to be created
   * @param $rw object The responsive widget data object.
   * @return object The responsive widget instance of the given type initialized with the data object.
   */
  public static function widgetFactory($type, $rw) {
    ctools_include('plugins');
    if ($class = ctools_plugin_load_class('responsive_widgets', 'widget', $type, 'handler')) {
      // We now need to check the plugin's dependencies, to make sure they're installed.
      $plugin = ctools_get_plugins('responsive_widgets', 'widget', $type);
      if (isset($plugin['dependencies'])) {
        foreach ($plugin['dependencies'] as $dependency) {
          // If any dependency is missing we cannot instantiate our class.
          if (!module_exists($dependency)) return NULL;
        }
      }

      // create widget instance
      return new $class($rw);
    }
    return NULL;
  }

  /**
   * Instantiate a responsive widget from the actual state of the settings form.
   * @param $form_state
   * @return object The responsive widget instance of the given type initialized with the data object.
   */
  public static function formWidgetFactory($form_state) {
    $type = $form_state['values']['widget_type'];
    $rw = ResponsiveWidget::convertFormToWidget($form_state);
    return ResponsiveWidget::widgetFactory($type, $rw);
  }

  /**
   * Converts the widget settings form state to a data object representing the widget.
   * This method defines the data structure of the widget and must match the database schema for responsive widgets.
   *
   * @param $form_state The form state array.
   * @return object The widget data object.
   */
  public static function convertFormToWidget($form_state) {
    $values = $form_state['values'];
    $type = $values['widget_type'];

    // create the responsive widget data object
    $rw = new stdClass();
    $rw->title = $values['title'];
    $rw->type = $type;
    $rw->hw_ratio = $values['hw_ratio'];
    $rw->settings = isset($values['settings'][$type]) ? $values['settings'][$type] : array();

    if (isset($values['machine_name'])) {
      $rw->machine_name = $values['machine_name'];
    }

    return $rw;
  }

  /**
   * Gets an associative array containing all widget settings that have to be accessible in JavaScript (Drupal.settings).
   * The key of the widget settings array has to be unique in Drupal.settings and should start with rw...
   *
   * CAUTION! Make sure, that the settings have the right type, e.g. cast a number value in the settings array to a float or int.
   * This is necessary, because the settings array contains only strings.
   * @code
   * array('rwProximity' => array(...));
   * @endcode
   * @return array
   */
  public function getJSSettings() {
    return array();
  }

  /**
   * Returns an array of all theme function, that has to be registered by Drupal.
   * This method is called by the responsive_widgets_theme() method.
   *
   * Implement the registered theme function in your plugins.inc file and don't forget to
   * add this information in the registration array of the theme function, for example:
   * @code
   * array('your_theme_function' => array(
   *   'render element' => 'element',
   *   'path' => drupal_get_path('module', 'responsive_widgets') . '/widgets',
   *   'file' => 'your_plugin_file.inc',
   *   ),
   * );
   * @endcode
   */
  public function getSettingsFormThemes() {
    return array();
  }

  /**
   * Method for returning the admin form elements to display for this widget type (specific widget settings).
   * Default: empty array.
   *
   * The form_state is passed by reference, so that the state can be altered. This allows to control callbacks from AJAX
   * and to define local form actions (returning to this form).
   *
   * Don't use '#tree' => false inside the settings form elements (except you know what you do), otherwise those values
   * (and its children) will not be accessible inside your specific widget class (via settings property) and will not be
   * stored in the database.
   *
   *
   * @param $form_state The form state array.
   * @return array A renderable settings form.
   */
  public function settingsForm(&$form_state) {
    return array();
  }

  /**
   * Implement the settings form validation in this method. It will be called during
   * the admin form validation of the widget.
   */
  abstract public function validateSettingsForm(&$settings);

  /**
   * Returns an array of the default settings for the specific widget.
   *
   * @return array
   */
  abstract protected function defaultSettings();

  /**
   * @return array A renderable array defining the widget markup.
   */
  abstract public function render();


}

