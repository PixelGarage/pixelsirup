<?php
/**
 * @file
 * Implements the Isotope views style plugin.
 */

class views_plugin_style_isotope_views extends views_plugin_style {

  /**
   * Set default options.
   */
  function option_definition() {
    $options = parent::option_definition();

    // Get default options
    $default_options = isotope_layout_default_options();

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
   * Includes the Isotope option form into the views style form.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // Add Isotope layout options to views style form in a dedicated field set
    $form['isotope'] = array(
      '#type' => 'fieldset',
      '#title' => t('Isotope settings'),
    );

    if (isotope_installed()) {
      // add options to form filled with saved values
      isotope_add_layout_options_to_form($form, $this->options);

      // Display each option field within the Isotope fieldset
      foreach (isotope_layout_default_options() as $option => $default_value) {
        $form[$option]['#fieldset'] = 'isotope';
      }

      // display help text for filter and sort usage
      $form['isotope_help'] = array(
        '#markup' => t('<br><h2>How to enable isotope filtering and sorting</h2>' .
          '<h3>Implement Filters</h3>' .
          '<p>Isotope filtering is implemented with taxonomy vocabularies, at which each vocabulary corresponds to one filter criteria. These taxonomies are integral part of the isotope items (content type). ' .
          'The actual filtering is based on CSS classes, therefore the item taxonomy terms have to be exposed as classes in the markup.' .
          'This can be achieved with a helper div-element added to each item (row) in the following format: <br>' .
          '<em>&lt;div class="isotope-filters"&gt;filter1,filter2,...&lt;/div&gt;</em>. </p>' .
          '<h6>Best practice:</h6> <ol>' .
          '<li>Define a taxonomy vocabulary for each filter criteria you need, e.g. "Color", "Item-type" etc. with the corresponding filter terms.</li>' .
          '<li>Add a taxonomy reference field for each filter criteria to the isotope item (content type).</li>' .
          '<li>In the views fields section below, add those reference fields to the list and set each field to plain text and exclude it from the display.</li>' .
          '<li>Override one of the successor fields in the field section and add the "isotope-filters" div-element (see above) to it containing the taxonomy terms as tokens.</li>' .
          '<li>Adding the isotope-filters div-element to the items (rows) of an Isotope view enables this view automatically for filtering.</li>' .
          '<li>To create a filter button group for each taxonomy vocabulary, create a new view with the format "Isotope filters". See help there for further instructions.</li> ' .
          '</ol>' .
          '<h3>Implement Sorting</h3>' .
          '<p>Isotope sorting is implemented with a special div-element too (see below), that fully defines the item sorting. Any item field value, e.g. a taxonomy term, ' .
          'the item title, the item counter etc. can be used as sort criteria. Multi-level sorting is also possible by adding multiple values to the div-element, whereby the order of the sort values define the order of the sorting.<br>' .
          'To enable Isotope item sorting, add the sorting div-element to each item containing the sort values as comma-separated list, as the following example shows:<br>' .
          '<em>&lt;div class="isotope-sorting"&gt;sort-value-0, sort-value-1,...&lt;/div&gt;</em>. </p>' .
          '<h6>Best practice:</h6> <ol>' .
          '<li>Add those item fields to the views fields section below, which are used for sorting.' .
          '<li>Set each of these sort fields to plain text and exclude it from the display.</li>' .
          '<li>Override one of the successor field output in the field section and add the "isotope-sorting" div-element (see above) to it containing the field values as tokens.</li>' .
          '<li>Adding the isotope-sorting div-element to the items (rows) of an Isotope view enables this view automatically for sorting.</li>' .
          '<li>To create a sorting button group containing all sort criteria, create a new view with the format "Isotope sorting". See help there for further instructions.</li> ' .
          '</ol>' .
          '<em>Remark:</em> If multiple Isotope containers are present in one page, only one container should be enabled for filtering and/or sorting.'),
        '#fieldset' => 'isotope',
      );

    }
    else {
      // Disable Isotope as plugin is not installed
      $form['isotope_disabled'] = array(
        '#markup' => t('These options have been disabled as the Isotope magical layouts plugin is not installed.'),
        '#fieldset' => 'isotope',
      );
    }
  }

}

