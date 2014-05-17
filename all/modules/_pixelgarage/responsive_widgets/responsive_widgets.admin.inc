<?php
/**
 * @file
 * Creates the administration forms to configure, edit and export responsive widgets.
 *
 * Created by PhpStorm.
 * User: ralph
 * Date: 11.12.13
 * Time: 12:39
 */

/**
 * Menu callback listing all available responsive widgets. This is the entry page
 * for the responsive widget administration.
 */
function responsive_widgets_list() {
  $header = array(
    array('data' => t('Responsive Widget')),
    array('data' => t('Type')),
    array('data' => t('Storage')),
    array('data' => t('Operations'), 'colspan' => 4),
  );
  $rows = array();

  // create rows with stored responsive widgets (ctools export based)
  foreach (responsive_widgets_load_multiple() as $rw) {
    // Determine storage
    switch ($rw->export_type) {
      case EXPORT_IN_DATABASE | EXPORT_IN_CODE:
        $storage = t('Overridden');
        $delete = l(t('Revert'), 'admin/structure/responsive_widgets/manage/'. $rw->machine_name .'/delete');
        break;
      case EXPORT_IN_DATABASE:
        $storage = t('Normal');
        $delete = l(t('Delete'), 'admin/structure/responsive_widgets/manage/'. $rw->machine_name .'/delete');
        break;
      case EXPORT_IN_CODE:
        $storage = t('Default');
        $delete = '';
        break;
      default:
        $storage = t('Undefined');
        $delete = '';
    }

    $row = array(
      array('data' => check_plain($rw->title)),
      array('data' => $rw->type),
      array('data' => $storage),
      array('data' => l(t('Edit'), 'admin/structure/responsive_widgets/manage/'. $rw->machine_name .'/edit')),
      array('data' => l(t('Export'), 'admin/structure/responsive_widgets/manage/'. $rw->machine_name .'/export')),
      array('data' => l(t('Clone'), 'admin/structure/responsive_widgets/manage/'. $rw->machine_name .'/clone')),
      array('data' => $delete),
    );
    $rows[] = $row;
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No responsive widgets available.'), 'colspan' => 6));
  }

  $build = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('id' => 'responsive-widgets'),
  );
  return $build;
}

/**
 * Builds the administration form to configure a responsive widget.
 * This form is called, if the Add or Edit button in the list form is pressed or if a local action
 * in the widget specific form is performed.
 *
 * @param $form . A renderable array.
 * @param $form_state . The form state defining the form state and rendering.
 * @param $op . The operation to be performed with the form, generally 'add' or 'edit'
 * @param null $rw . The responsive widget to be edited or NULL, if a new widget is created
 * @return array The admin form for the responsive widget.
 */
function responsive_widgets_admin_form($form, &$form_state, $op, $rw = NULL) {
  // create the responsive widget data object
  if (!isset($rw)) {
    // new default widget
    $rw = new stdClass;
    $rw->type = 'proximity';
    $rw->settings = array();
  }
  if (isset($form_state['values']['title'])) {
    // form update (local action)
    $rw = ResponsiveWidget::convertFormToWidget($form_state);
  }

  // the widget title (block title)
  $form['title'] = array(
    '#title' => t('Title'),
    '#description' => t('This will appear as the block title of the responsive widget.'),
    '#type' => 'textfield',
    '#default_value' => isset($rw->title) ? $rw->title : '',
    '#weight' => -9,
    '#required' => TRUE,
  );
  // the unique identifier of the widget
  $form['machine_name'] = array(
    '#type' => 'machine_name',
    '#maxlength' => 32,
    '#machine_name' => array(
      'exists' => 'responsive_widgets_machine_name_exists',
      'source' => array('title'),
    ),
    '#description' => t('A unique machine-readable name for this responsive widget. It must only contain lowercase letters, numbers, and underscores. The machine name will be used internally by Responsive Widgets and will be used in the CSS ID of your responsive widget block.'),
    '#weight' => -8,
  );

  if (!empty($rw->machine_name)) {
    $form['machine_name']['#disabled'] = TRUE;
    $form['machine_name']['#value'] = $rw->machine_name;
    $form['machine_name']['#default_value'] = $rw->machine_name;
  }

  // define the height/width ratio of widget
  $form['hw_ratio'] = array(
    '#type' => 'textfield',
    '#title' => t('Height-width ratio of widget'),
    '#size' => 20,
    '#description'  => t('Define the height width ratio (h/w) of the proximity widget. Default = 0.618 (golden ratio).'),
    '#default_value' => isset($rw->hw_ratio) ? $rw->hw_ratio : 0.618,
    '#required' => TRUE,
    '#weight' => -7,
  );

  // list all available widget types and display the widget form of the selected widget type
  ctools_include('plugins');
  $widget_infos = ctools_get_plugins('responsive_widgets', 'widget');
  $widget_types = array();
  foreach ($widget_infos as $type => $info) {
    // unset type specific settings when type isn't matching
    if ($type !== $rw->type) unset($rw->settings);

    // create all available widgets and add all specific settings form (only selected is visible)
    if ($widget = ResponsiveWidget::widgetFactory($type, $rw)) {
      // add a field set for each widget type containing the widget specific settings
      $form['settings'] = array(
        '#tree' => true,  // keep hierarchy of settings form
        '#type' => 'fieldset',
        '#title' => t('Widget settings'),
        '#collapsible' => true,
        '#collapsed' => false,
        '#weight' => -5,
        // set only the selected widget's settings form visible
        '#states' => array('visible' => array(':input[name="widget_types"]' => array('value' => $type))),
      );
      $form['settings'][$type] = $widget->settingsForm($form_state);

      // Add the widget type to the drop-down list of available widgets
      $widget_types[$type] = $type;
    }
  }
  ksort($widget_types);
  $form['widget_type'] = array(
    '#type' => 'select',
    '#title' => t('Widget type'),
    '#options' => $widget_types,
    '#default_value' => $rw->type,
    '#description' => t('Choose one of the available widget types.'),
    '#weight' => -6,
  );

  // submit form
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit_form'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['actions']['cancel_form'] = array(
    '#markup' => l(t('Cancel'), 'admin/structure/responsive_widgets'),
  );

  return $form;
}

/**
 * Validation handler for responsive widget admin form.
 */
function responsive_widgets_admin_form_validate($form, &$form_state) {
  // validate machine name
  if (empty( $form_state['values']['machine_name'])) {
    // check if machine name is set
    form_set_error('machine_name', t('The widget machine name is required.'));

  } elseif (!preg_match('!^[a-z0-9_]+$!',  $form_state['values']['machine_name'])) {
    // check if machine name is valid
    form_set_error('machine_name', t('The widget machine name must contain only lowercase letters, numbers, and underscores.'));
  }

  // validate widget specific settings form
  $type =  $form_state['values']['widget_type'];
  if ($widget = ResponsiveWidget::formWidgetFactory($form_state)) {
    $widget->validateSettingsForm($form_state['values']['settings'][$type]);
  }

}

/**
 * Submit handler for responsive widget admin form.
 */
function responsive_widgets_admin_form_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#type'] == 'submit') {
    $rw = ResponsiveWidget::convertFormToWidget($form_state);

    $exists = responsive_widgets_load($rw->machine_name);
    if ($exists && empty($exists->in_code_only)) {
      // update the existing widget
      if (responsive_widgets_update($rw)) {
        drupal_set_message(t('The Responsive Widget has been updated.'));
      }
    } else {
      // create a new widget
      if (responsive_widgets_save($rw)) {
        drupal_set_message(t('The Responsive Widget has been created.'));
      }
    }
    $form_state['redirect'] = 'admin/structure/responsive_widgets';
  }
}

/**
 * Clones a responsive widget.
 *
 * @param $rw . The loaded responsive widget (@see responsive_widgets_load).
 * @return array|mixed. Form filled with cloned widget.
 */
function responsive_widgets_clone($rw) {
  unset($rw->machine_name);
  $rw->title = '';
  return drupal_get_form('responsive_widgets_admin_form', 'clone', $rw);}

/**
 * Deletes a responsive widget from the database, if deletion form is confirmed.
 *
 * @param $form
 * @param $form_state
 * @param $rw. The loaded responsive widget (see responsive_widgets_load)
 * @return array|mixed. Deletion confirmation form.
 */
function responsive_widgets_delete_form($form, $form_state, $rw) {
  // create a simple confirmation form with hidden fields
  $form['machine_name'] = array('#type' => 'hidden', '#value' => $rw->machine_name);
  $form['title'] = array('#type' => 'hidden', '#value' => $rw->title);
  return confirm_form($form, t('Are you sure you want to delete the responsive widget "%title"?', array('%title' => $rw->title)), 'admin/structure/responsive_widgets', '', t('Delete'), t('Cancel'));
}

/**
 * Submit handler for the widget deletion.
 */
function responsive_widgets_delete_form_submit($form, &$form_state) {
  // delete the widget definitely
  responsive_widgets_delete($form_state['values']['machine_name']);
  drupal_set_message(t('The responsive widget "%name" has been removed.', array('%name' => $form_state['values']['title'])));
  cache_clear_all();
  $form_state['redirect'] = 'admin/structure/responsive_widgets';
};

/**
 * Exports a responsive widget.
 *
 * @param $form. The form
 * @param $form_state. The form state
 * @param $rw . The loaded responsive widget (@see responsive_widgets_load)
 * @return array|mixed. Export form filled with exported widget.
 */
function responsive_widgets_export_form($form, $form_state, $rw) {
  ctools_include('export');
  // Generate export code
  $code = ctools_export_object('responsive_widgets', $rw, '');

  // Create form
  $form = array();
  $form['export'] = array(
    '#type' => 'textarea',
    '#default_value' => $code,
    '#rows' => substr_count($code, "\n") + 1,
    '#resizable' => FALSE,
    '#description' => t('Place this code in your module\'s implementation of <code>hook_default_responsive_widgets_widget()</code> to provide it as a default responsive widget.'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Done'),
  );
  $form['#redirect'] = 'admin/structure/responsive_widgets';

  return $form;
}

