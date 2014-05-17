<?php
/**
 * @file
 * Hooks provided by Isotope.
 */

/**
 * Alter Isotope's default layout options.
 *
 * @param $options
 *   An associative array of option names and their default values.
 */
function hook_isotope_layout_default_options_alter(&$options) {
  // Add default value for easing option
  $options['isotope_animation_easing'] = 'linear';
}

/**
 * Alter the form that Isotope layout options are added to.
 *
 * @param $form
 *   A form array.
 * @param $default_values
 *   An array of default form values.
 */
function hook_isotope_layout_options_form_alter(&$form, $default_values) {
  // Add form item for easing option
  $form['isotope_animation_easing'] = array(
    '#type' => 'select',
    '#title' => t('Animation easing'),
    '#description' => t("The easing function to use for animations."),
    '#options' => array(
      'linear' => t('Linear'),
      'swing' => t('Swing'),
    ),
    '#default_value' => $default_values['isotope_animation_easing'],
    '#states' => array(
      'visible' => array(
        'input.form-checkbox[name$="[isotope_resizable]"]' => array('checked' => TRUE),
      ),
    ),
  );
}

/**
 * Alter Isotope's default sort options.
 *
 * @param $options
 *   An associative array of option names and their default values.
 */
function hook_isotope_sort_default_options_alter(&$options) {

}

/**
 * Alter the form that defines the Isotope sort options.
 *
 * @param $form
 *   A form array.
 * @param $default_values
 *   An array of default form values.
 */
function hook_isotope_sort_options_form_alter(&$form, $default_values) {

}

/**
 * Alter Isotope's default filter options.
 *
 * @param $options
 *   An associative array of option names and their default values.
 */
function hook_isotope_filter_default_options_alter(&$options) {

}

/**
 * Alter the form that defines the Isotope filter options.
 *
 * @param $form
 *   A form array.
 * @param $default_values
 *   An array of default form values.
 */
function hook_isotope_filter_options_form_alter(&$form, $default_values) {

}

/**
 * Alter the Isotope script.
 *
 * @param $js_settings
 *   An array of Isotope options to send to the script file.
 * @param $script_file
 *   A path to the javascript file that triggers Isotope.
 * @param $context
 *   An associative array of additional variables.
 *   Contains:
 *   - container: The CSS selector of the container element to apply Isotope to.
 *   - options: An associative array of Isotope options. See isotope_apply().
 */
function hook_isotope_script_alter(&$js_settings, &$script_file, $context) {
  $container = $context['container'];
  $options = $context['options'];

  // Send easing option to the script file
  $js_settings['isotope'][$container]['animation_easing'] = $options['isotope_animation_easing'];

  // Use a custom javascript file that includes easing in the animationOptions
  $script_file = drupal_get_path('module', 'isotope') . '/js/isotope.js';
}

