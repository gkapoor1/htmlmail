<?php

/**
 * @file
 * Admin forms for HTML Mail
 */

/**
 * Implements hook_admin_settings().
 */
function htmlmail_admin_settings() {
  $form['#submit'] = array('htmlmail_admin_settings_submit');
  $form['htmlmail_theme'] = array(
    '#type' => 'select',
    '#title' => t('Theme to use for emails'),
    '#default_value' => variable_get('htmlmail_theme', ''),
    '#options' => htmlmail_get_allowed_themes(),
    '#description' => t(
      'You may use a template or a website theme to format your emails. See !readme for details.',
      array('!readme' => l('README.txt', drupal_get_path('module', 'htmlmail') . '/README.txt'))
    ),
  );
  $formats = array('0' => 'Unfiltered');
  foreach (filter_formats() as $id => $filter) {
    $formats[$id] = $filter->name;
  }
  $form['htmlmail_prefilter'] = array(
    '#type' => 'select',
    '#title' => t('Text format pre-filter'),
    '#default_value' => variable_get('htmlmail_prefilter'),
    '#options' => $formats,
    '#description' => t('Choose a <a href="/admin/config/content/formats">text format</a> to be used for filtering email messages <em>before</em> the selected theme or template is applied.'),
  );
  $form['htmlmail_postfilter'] = array(
    '#type' => 'select',
    '#title' => t('Text format post-filter'),
    '#default_value' => variable_get('htmlmail_postfilter'),
    '#options' => $formats,
    '#description' => t('Choose a <a href="/admin/config/content/formats">text format</a> to be used for filtering email messages <em>after</em> the selected theme or template is applied.'),
  );
  // @todo Move to a separate filter module.
  $disabled = module_load_include('inc', 'htmlmail', 'emogrifier/emogrifier')
    && (_htmlmail_emogrifier() === FALSE);
  $form['htmlmail_emogrifier'] = array(
    '#type' => 'checkbox',
    '#title' => t('Emogrifier'),
    '#default_value' => $disabled ? 0 : variable_get('htmlmail_emogrifier', '0'),
    '#description' => t('Insert your CSS definitions as inline styles into HTML tags for Outlook 2007 and Google Gmail.<br /><em>Checkbox will be disabled if the !e library is not available.</em>', array('!e' => 'emogrifier.php')),
    '#disabled' => $disabled,
  );
  $form['htmlmail_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Debug'),
    '#default_value' => variable_get('htmlmail_debug', '0'),
    '#description' => t('Add module name to bottom of email. Useful for theme template suggestion or exclusion settings.'),
  );
  return system_settings_form($form);
}

// @todo Move to a separate UI module.
function htmlmail_admin_settings_submit($form, &$form_state) {
  $mail_system = array(
    'default-system' => (
      empty($form_state['values']['default-system'])
      ? 'DefaultMailSystem'
      : $form_state['values']['default-system']
    )
  );
  foreach (system_rebuild_module_data() as $key => $module) {
    if ($module->status && isset($form_state['values'][$key])) {
      if (!empty($form_state['values'][$key])) {
        $mail_system[$key] = $form_state['values'][$key];
      }
      unset($form_state['values'][$key]);
    }
  }
  variable_set('mail_system', $mail_system);
}

/**
 * Builds a form for setting Header, Footer, and CSS content to be included in
 * sent emails.
 */
function htmlmail_template_settings() {
  $defaults = array('value' => '', 'format' => filter_default_format());
  $header = variable_get('htmlmail_header', $defaults);
  $form['htmlmail_header'] = array(
    '#type' => 'text_format',
    '#title' => t('Header HTML'),
    '#rows' => 6,
    '#default_value' => $header['value'],
    '#format' => $header['format'],
    '#description' => 'Extra HTML markup to add at the top of emails.',
  );
  $footer = variable_get('htmlmail_footer', $defaults);
  $form['htmlmail_footer'] = array(
    '#type' => 'text_format',
    '#title' => t('Footer HTML'),
    '#rows' => 6,
    '#default_value' => $footer['value'],
    '#format' => $footer['format'],
    '#description' => 'Extra HTML markup to add at the bottom of emails.',
  );
  $form['htmlmail_css'] = array(
    '#type' => 'textarea',
    '#title' => t('CSS'),
    '#default_value' => variable_get('htmlmail_css', ''),
    '#description' => 'Extra CSS rules to be added to emails.',
  );
  return system_settings_form($form);
}

/**
 * Builds a form for sending a test message.
 */
function htmlmail_test_form($form_values = NULL) {
  // set up some defaults for test mail
  $defaults = variable_get(
    'htmlmail_test',
    array(
      'to' => variable_get('site_mail'),
      'subject' => 'test',
      'body' => array(
        'value' => 'test',
        'format' => filter_fallback_format(),
      ),
    )
  );

  $form['to'] = array(
    '#type' => 'textfield',
    '#title' => t('To'),
    '#default_value' => $defaults['to'],
    '#maxlength' => 128,
    '#required' => TRUE,
  );
  $form['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => $defaults['subject'],
    '#maxlength' => 128,
    '#required' => TRUE,
  );
  $form['body'] = array(
    '#type' => 'text_format',
    '#title' => t('Body'),
    '#rows' => 20,
    '#default_value' => $defaults['body']['value'],
    '#format' => $defaults['body']['format'],
    '#required' => TRUE,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Send test message'),
  );
  return $form;
}

/**
 * Sends the test messsage and saves the contents for re-use.
 */
function htmlmail_test_form_submit($form, &$form_state) {
  // save form values
  $defaults = array(
    'to' => $form_state['values']['to'],
    'subject' => $form_state['values']['subject'],
    'body' => $form_state['values']['body'],
  );
  // set the defaults for reuse
  variable_set('htmlmail_test', $defaults);
  // send email
  $params = array(
    'subject' => $form_state['values']['subject'],
    'body' => $form_state['values']['body']['value'],
  );
  if (drupal_mail('htmlmail', 'test', $form_state['values']['to'], language_default(), $params)) {
    drupal_set_message(t('HTML Mail test message sent.'));
  }
}

/**
 * Displays the email message body passed via POST as a themed page.
 */
function htmlmail_show_email() {
  if ($css = variable_get('htmlmail_css', '')) {
    drupal_add_css($css, array('type' => 'inline'));
  }
  return isset($_POST['body']) ? $_POST['body'] : '(Empty)';
}

/**
 * Returns the email message subject passed via POST for a title callback.
 */
function htmlmail_get_post_subject($subject) {
  return check_plain(isset($_POST['subject']) ? $_POST['subject'] :  t($subject));
}
