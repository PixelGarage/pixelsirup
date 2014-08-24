<?php

/**
 * @file
 * Process theme data.
 *
 * Use this file to run your theme specific implimentations of theme functions,
 * such preprocess, process, alters, and theme function overrides.
 *
 * Preprocess and process functions are used to modify or create variables for
 * templates and theme functions. They are a common theming tool in Drupal, often
 * used as an alternative to directly editing or adding code to templates. Its
 * worth spending some time to learn more about these functions - they are a
 * powerful way to easily modify the output of any template variable.
 * 
 * Preprocess and Process Functions SEE: http://drupal.org/node/254940#variables-processor
 * 1. Rename each function and instance of "adaptivetheme_subtheme" to match
 *    your subthemes name, e.g. if your theme name is "footheme" then the function
 *    name will be "footheme_preprocess_hook". Tip - you can search/replace
 *    on "xy_theme".
 * 2. Uncomment the required function to use.
 */


/**
 * Preprocess variables for the html template.
 */
function standard_theme_preprocess_html(&$vars) {
  global $theme_key;

  // add font awesome bootstrap
  drupal_add_html_head_link(array('href' => '//netdna.bootstrapcdn.com/font-awesome/4.1.0/css/font-awesome.min.css', 'rel' => 'stylesheet'));

  // make sure jQuery UI and effects is loaded for anonymous users
  drupal_add_library('system', 'ui');
  drupal_add_library('system', 'effects');

  // Browser/platform sniff - adds body classes such as ipad, webkit, chrome etc.
  $vars['classes_array'][] = css_browser_selector();

  // IE9 and greater gradient support
  $vars['polyfills']['gte IE 9'] = array(
    '#type' => 'markup',
    '#markup' => "<style type='text/css'> .gradient {filter: none;} </style>",
    '#prefix' => "<!--[if gte IE 9]>\n",
    '#suffix' => "\n<![endif]-->\n"
  );

}
// */


/* =============================================================================
 *
 *      User login / register / password form alter
 *
 * ========================================================================== */

/**
 * Alters the menu entries.
 * @param $items
 */
function standard_theme_menu_alter(&$items) {
  // remove the tabs on the login / register form page
  $items['user/login']['type'] = MENU_CALLBACK;
  $items['user/register']['type'] = MENU_CALLBACK;
  $items['user/password']['type'] = MENU_CALLBACK;
}

/**
 * Alter the user login form.
 * @param $form
 * @param $form_state
 * @param $form_id
 */
function standard_theme_form_user_login_alter(&$form, &$form_state, $form_id) {
  $form['name']['#prefix']  = '<div id="' . $form_id . '_form">';
  $form['name']['#prefix'] .= '<h1>' . t('Login') . '</h1>';
  $form['pass']['#suffix']  = '<div class="form-actions-wrapper">';
  $form['pass']['#suffix'] .= l(t('Forgot your password?'), 'user/password', array('attributes' => array('class' => array('login-password'), 'title' => t('Get a new password'))));
  $form['actions']['#suffix']  = '</div></div>';
  if (variable_get('user_register', USER_REGISTER_VISITORS_ADMINISTRATIVE_APPROVAL) != USER_REGISTER_ADMINISTRATORS_ONLY) {
    $form['actions']['#suffix'] .= '<div class="create-account clearfix">';
    $form['actions']['#suffix'] .= "\r<h2>" . t('I don\'t have an account') . "</h2>";
    $form['actions']['#suffix'] .= "\r<div class='create-account-description'><p>" . t("To use this website you need to register.\r Press the button below to apply for an account.") . "</p>";
    $form['actions']['#suffix'] .= "\r<p>" . t("After the processing of your application you will receive an email with detailed information about the login.") . "</p></div>";
    $form['actions']['#suffix'] .= "\r" . l(t('Create an account'), 'user/register', array('attributes' => array('class' => array('login-register'), 'title' => t('Create a new account'))));
    $form['actions']['#suffix'] .= '</div>';
  }
}


/**
 * Alter the user registration form.
 * @param $form
 * @param $form_state
 * @param $form_id
 */
function standard_theme_form_user_register_form_alter (&$form, &$form_state, $form_id) {
  $form['account']['name']['#prefix'] = '<div id="' . $form_id . '">';
  $form['account']['name']['#prefix'] .= '<h1>' . t('Register') . '</h1>';
  $form['actions']['submit']['#suffix'] = '<div class="back-to-login clearfix">' . l(t('Back to login'), 'user/login', array('attributes' => array('class' => array('login-account'), 'title' => t('Sign in')))) . '</div>';
  $form['actions']['submit']['#suffix'] .= '</div>';
}

/**
 * Alter the user password form.
 * @param $form
 * @param $form_state
 * @param $form_id
 */
function standard_theme_form_user_pass_alter (&$form, &$form_state, $form_id) {
  $form['name']['#prefix'] = '<div id="' . $form_id . '_form">';
  $form['name']['#prefix'] .= '<h1>' . t('Request a new password') . '</h1>';
  $form['actions']['#suffix'] = '<div class="back-to-login clearfix">' . l(t('Back to login'), 'user/login', array('attributes' => array('class' => array('login-account'), 'title' => t('Sign in')))) . '</div>';
  $form['actions']['#suffix'] .= '</div>';
}

/**
 * Override or insert variables for the page templates.
 */
function standard_theme_preprocess_page(&$vars) {
  // hide title for user registration / login
  switch (current_path()) {
    case 'user':
    case 'user/login':
    case 'user/register':
    case 'user/password':
      $vars['title'] = '';
  }
}


