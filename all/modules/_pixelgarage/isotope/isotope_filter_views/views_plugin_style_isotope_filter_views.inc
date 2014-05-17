<?php
/**
 * @file
 * Implements the Isotope filter views style plugin.
 */

class views_plugin_style_isotope_filter_views extends views_plugin_style {

  /**
   * Set default filter options.
   */
  function option_definition() {
    $options = parent::option_definition();

    // Get default options
    $default_options = isotope_filter_default_options();

    // Set default values for Isotope
    foreach ($default_options as $option => $default_value) {
      $options[$option] = array(
        'default' => $default_value,
      );
      if (is_int($default_value)) {
        $options[$option]['bool'] = TRUE;
      }
    }

    return $options;
  }

  /**
   * Includes the Isotope filter option form into the views style form.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // add Isotope filter options to views style form in a dedicated field set
    $form['isotope_filter'] = array(
      '#type' => 'fieldset',
      '#title' => t('Isotope filters'),
    );
    if (isotope_installed()) {
      // add filter options to form filled with saved values
      isotope_add_filter_options_to_form($form, $this->options);

      // Display each option field within the "Isotope filters" fieldset
      foreach (isotope_filter_default_options() as $option => $default_value) {
        $form[$option]['#fieldset'] = 'isotope_filter';
      }

      // display help text for filter creation
      $form['isotope_filter_help'] = array(
        '#markup' => t('<h2>How to create isotope filter buttons</h2>' .
          '<p>With this style you can create a filter button group for each taxonomy vocabulary, that is used for item filtering. This button group is automatically connected ' .
          'to the Isotope container on the same page, if that container is enabled for filtering (meaning contains filter classes for each item).</p>' .
          '<h4>How to create filter buttons for a taxonomy vocabulary?</h4> <ol>' .
          '<li>Create a new view (block) and add a taxonomy vocabulary as filter criteria. Because you are reading this, you have obviously already selected the "Isotope filters" format.</li> ' .
          '<li>Choose Field row style. In its settings dialog, disable the flag "Provide default field wrapper elements". This assures that the plain term value is listed without any wrapper.</li>' .
          '<li>Add the name of the taxonomy term as field and make sure that the flag "Link this field to its taxonomy term page" is disabled, as well as the label flag.</li>' .
          '<li>Choose some sort criteria if you want control over the order of the buttons.</li>' .
          '<li>See help text in the "Isotope layout" format for a guide, how to enable filtering for the Isotope items.</li>' .
          '</ol><br>' .
          '<em>Remark:</em> If multiple Isotope containers are present in one page, only one container should be enabled for filtering.'),
        '#fieldset' => 'isotope_filter',
      );

    }
    else {
      // Disable Isotope as plugin is not installed
      $form['isotope_filter_disabled'] = array(
        '#markup' => t('These options have been disabled as the Isotope magical layouts plugin is not installed.'),
        '#fieldset' => 'isotope_filter',
      );
    }

  }

}

