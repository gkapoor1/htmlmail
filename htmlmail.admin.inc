<?php
// $Id$

function htmlmail_admin_settings() {
  $form['htmlmail_settings']['htmlmail_test_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Send Test E-mails to'),
    '#default_value' => variable_get('htmlmail_test_email', variable_get('site_mail', '')),
    '#size' => 40,
    '#maxlength' => 128,
    '#description' => t('Enter the email address for test emails, defaults to site email. '. l('Send Test', 'admin/settings/htmlmail/test')),
  );
  $form['htmlmail_settings']['htmlmail_autop'] = array(
    '#type' => 'checkbox',
    '#title' => t('Line break converter'),
    '#default_value' => variable_get('htmlmail_autop', '1'),
    '#description' => t('Converts line breaks into HTML (i.e. &lt;br&gt; and &lt;p&gt; tags, ala filter.module).'),
  );
  $form['htmlmail_settings']['htmlmail_urlfilter'] = array(
    '#type' => 'checkbox',
    '#title' => t('URL Filter'),
    '#default_value' => variable_get('htmlmail_urlfilter', '1'),
    '#description' => t('Automatically converts text web addresses (URLs, e-mail addresses, ftp links, etc.) into hyperlinks.'),
  );
  $form['htmlmail_settings']['htmlmail_emogrifier'] = array(
    '#type' => 'checkbox',
    '#title' => t('Emogrifier'),
    '#default_value' => variable_get('htmlmail_emogrifier', '0'),
    '#description' => t('Emogrifier automagically transmogrifies your HTML by parsing your CSS and inserting your CSS definitions into tags within your HTML based on your CSS selectors.<br />Requires <a href="http://www.pelagodesign.com/sidecar/emogrifier">Emogrifier</a> residing in module directory'),
  );
  $form['htmlmail_settings']['htmlmail_preformat'] = array(
    '#type' => 'checkbox',
    '#title' => t('Preformat HTML code'),
    '#default_value' => variable_get('htmlmail_preformat', '1'),
    '#description' => t('Automagically inserts the &lt;HTML&gt; &lt;HEAD&gt; &lt;BODY&gt; tags, untick if you want control over this. You should check this if using a WYSIWYG editor.'),
  );
  return system_settings_form($form);
}

function htmlmail_template_settings() {

  $form['htmlmail_template']['htmlmail_header'] = array(
    '#type' => 'textarea',
    '#title' => t('Header HTML'),
    '#default_value' => variable_get('htmlmail_header', ''),
  );

  $form['htmlmail_template']['htmlmail_footer'] = array(
    '#type' => 'textarea',
    '#title' => t('Footer HTML'),
    '#default_value' => variable_get('htmlmail_footer', ''),
  );

  $form['htmlmail_template']['htmlmail_css'] = array(
    '#type' => 'textarea',
    '#title' => t('CSS'),
    '#default_value' => variable_get('htmlmail_css', ''),
  );

  return system_settings_form($form);
}

function htmlmail_test_form($form_values = NULL) {
  $form['test_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#size' => 40,
    '#default_value' => variable_get('htmlmail_test_subject', ''),
    '#maxlength' => 128,
  );

  $form['test_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#rows' => 20,
    '#default_value' => variable_get('htmlmail_test_body', ''),
    '#description' => t('Formatted in HTML.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

function htmlmail_test_form_submit($form, &$form_state) {
  if ($form_state['values']['test_subject']) {
    variable_set('htmlmail_test_subject', $form_state['values']['test_subject']);
  }
  if ($form_state['values']['test_body']) {
    variable_set('htmlmail_test_body', $form_state['values']['test_body']);
  }
  
  $message['id'] = 'htmlmail_test';
  $message['to'] = variable_get('htmlmail_test_email', variable_get('site_mail', ''));
  $message['subject'] = $form_state['values']['test_subject'];
  $message['body'] = $form_state['values']['test_body'];

  $to = variable_get('htmlmail_test_email', variable_get('site_mail', ''));
  $params = array(
    'body' => $form_state['values']['test_body'],
    'subject' => $form_state['values']['test_subject']
  );

  if (drupal_mail('htmlmail', 'htmlmail_test', $to, language_default(), $params, $to)) {
    drupal_set_message('HTML Mail test message sent.');
  }
  else {
    drupal_set_message('drupal_mail failed');
  }
}

