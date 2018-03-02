<?php

namespace Drupal\htmlmail\Form;

use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Url;


/**
 * Configure site information settings for this site.
 */
class HTMLMailSettingsForm extends ConfigFormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'htmlmail_settings';
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return ['htmlmail.settings'];
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $form['template'] = [
      '#type' => 'fieldset',
      '#title' => $this->t('Step 1'),
      '#collapsible' => FALSE,
    ];
    $form['template']['htmlmail_template'] = [
      '#type' => 'fieldset',
      '#prefix' => '<strong>' . $this->t('Twig file') . ':</strong><br />'
      . $this->t('A twig file is applied to your message header, subject, and body text.'),
      '#title' => $this->t('Instructions'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    ];
    $form['template']['htmlmail_template']['instructions'] = [
    '#type' => 'item',
    '#suffix' => $this->t('!Instructions
	<p>When formatting an email message with a given <code>$module</code> and <code>$key</code>, <a href="https://www.drupal.org/project/htmlmail">HTML Mail</a> will use the first template file it finds from the following list:</p>
	<ol style="list-style-type: decimal;">
  	<li><code>htmlmail--$module--$key.tpl.php</code></li>
  	<li><code>htmlmail--$module.tpl.php</code></li>
  	<li><code>htmlmail.tpl.php</code></li>
	</ol>
	<p>For each filename, <a href="https://www.drupal.org/project/htmlmail">HTML Mail</a> looks first in the chosen <em>Email theme</em> directory, then in its own module directory, before proceeding to the next filename.</p>
	<p>For example, if <code>example_module</code> sends mail with:</p>
	<pre>
	<code>drupal_mail("example_module", "outgoing_message" ...)
	</code>
	</pre>
	<p>the possible template file names would be:</p>
	<ol style="list-style-type: decimal;">
    <li><code>htmlmail--example_module--outgoing_message.tpl.php</code></li>
    <li><code>htmlmail--example_module.tpl.php</code></li>
    <li><code>htmlmail.tpl.php</code></li>
	</ol>
	<p>Template files are cached, so remember to clear the cache by visiting <u>admin/config/development/performance</u> after changing any <code>.tpl.php</code> files.</p>
	<p>The following variables available in this template:</p>
	<dl>
  	<dt><strong><code>body</code></strong></dt>
  	<dd>
    <p>The message body text.</p>
  	</dd>
  	<dt><strong><code>$module</code></strong></dt>
  	<dd>
    <p>The first argument to <a href="https://api.drupal.org/api/drupal/includes--mail.inc/function/drupal_mail/7"><code>drupal_mail()</code></a>, which is, by convention, the machine-readable name of the sending module.</p>
  	</dd>
  	<dt><strong><code>key</code></strong></dt>
  	<dd>
    <p>The second argument to <a href="https://api.drupal.org/api/drupal/includes--mail.inc/function/drupal_mail/7"><code>drupal_mail()</code></a>, which should give some indication of why this email is being sent.</p>
  	</dd>
  	<dt><strong><code>message_id</code></strong></dt>
  	<dd>
    <p>The email message id, which should be equal to <code>"{$module}_{$key}"</code>.</p>
  	</dd>
  	<dt><strong><code>headers</code></strong></dt>
  	<dd>
    <p>An array of email <code>(name =&gt; value)</code> pairs.</p>
  	</dd>
  	<dt><strong><code>from</code></strong></dt>
  	<dd>
    <p>The configured sender address.</p>
  	</dd>
  	<dt><strong><code>to</code></strong></dt>
  	<dd>
    <p>The recipient email address.</p>
  	</dd>
  	<dt><strong><code>subject</code></strong></dt>
  	<dd>
    <p>The message subject line.</p>
  	</dd>
  	<dt><strong><code>body</code></strong></dt>
  	<dd>
    <p>The formatted message body.</p>
  	</dd>
  	<dt><strong><code>language</code></strong></dt>
  	<dd>
    <p>The language code for this message.</p>
  	</dd>
  	<dt><strong><code>params</code></strong></dt>
  	<dd>
    <p>Any module-specific parameters.</p>
  	</dd>
  	<dt><strong><code>template_name</code></strong></dt>
  	<dd>
    <p>The basename of the active template.</p>
  	</dd>
  	<dt><strong><code>template_path</code></strong></dt>
  	<dd>
    <p>The relative path to the template directory.</p>
  	</dd>
  	<dt><strong><code>template_url</code></strong></dt>
  	<dd>
    <p>The absolute URL to the template directory.</p>
  	</dd>
  	<dt><strong><code>theme</code></strong></dt>
  	<dd>
    <p>The name of the <em>Email theme</em> used to hold template files. If the <a href="https://www.drupal.org/project/echo">Echo</a> module is enabled this theme will also be used to transform the message body into a fully-themed webpage.</p>
  	</dd>
  	<dt><strong><code>theme_path</code></strong></dt>
  	<dd>
    <p>The relative path to the selected <em>Email theme</em> directory.</p>
  	</dd>
  	<dt><strong><code>theme_url</code></strong></dt>
  	<dd>
    <p>The absolute URL to the selected <em>Email theme</em> directory.</p>
  	</dd>
  	<dt><strong><code>debug</code></strong></dt>
  	<dd>
    <p><code>TRUE</code> to add some useful debugging info to the bottom of the message.</p>
  	</dd>
	</dl>
	<p>Other modules may also add or modify theme variables by implementing a <code>MODULENAME_preprocess_htmlmail(&amp;$variables)</code> <a href="https://api.drupal.org/api/drupal/modules--system--theme.api.php/function/hook_preprocess_HOOK/7">hook function</a>.</p>'
    ),
  	];
  	$form['template']['htmlmail_debug'] = [
      '#type' => 'checkbox',
      '#prefix' => '<br />',
      '#title' => '<em>' . $this->t('(Optional)') . '</em> ' . $this->t('Debug'),
      '#description' => $this->t('Add debugging info (Set <code>$debug</code> to <code>TRUE</code>).'),
  	];
  	$form['theme'] = [
      '#type' => 'fieldset',
      '#title' => $this->t('Step 2'),
      '#collapsible' => FALSE,
    ];


  return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {

  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

  }

}
